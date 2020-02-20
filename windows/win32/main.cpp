#include "pch.h"

using namespace winrt;
using namespace Windows::UI::Composition;
using namespace Windows::UI::Xaml::Hosting;

static BOOL CALLBACK UpdateChildSizeProc(HWND childHwnd, LPARAM lParam) {
	RECT rcClient;
	GetClientRect((HWND)lParam, &rcClient);
	SetWindowPos(childHwnd, 0, 0, 0, rcClient.right - rcClient.left,
		rcClient.bottom - rcClient.top, SWP_SHOWWINDOW);
	return FALSE;
}

static LRESULT CALLBACK WindowProc(HWND hWnd, UINT messageCode, WPARAM wParam,
	LPARAM lParam) {
	switch (messageCode) {
	case WM_PAINT:
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_SIZE:
		// Pass main window dimension to (XamlIsland) child windows
		EnumChildWindows(hWnd, UpdateChildSizeProc, (LPARAM)hWnd);
		break;
	default:
		return DefWindowProc(hWnd, messageCode, wParam, lParam);
		break;
	}
	return 0;
}

struct NativeWindow {
	NativeWindow(HINSTANCE hInstance) noexcept {
		// The main window class name.
		const wchar_t szWindowClass[] = L"WinDesk";
		WNDCLASSEX windowClass = {};
		windowClass.cbSize = sizeof(WNDCLASSEX);
		windowClass.lpfnWndProc = WindowProc;
		windowClass.hInstance = hInstance;
		windowClass.lpszClassName = szWindowClass;
		windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		windowClass.hIconSm = LoadIcon(windowClass.hInstance, IDI_APPLICATION);

		if (RegisterClassEx(&windowClass) == NULL) {
			MessageBox(NULL, L"Windows registration failed!", L"Error", NULL);
			return;
		}

		auto width = 720;
		auto height = 1280;
		hWnd_ =
			CreateWindow(szWindowClass, L"Win32 Desktop App",
				WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT,
				CW_USEDEFAULT, width, height, NULL, NULL, hInstance, NULL);
		if (hWnd_ == NULL) {
			MessageBox(NULL, L"Call to CreateWindow failed!", L"Error", NULL);
			return;
		}

		// XAML Island section
		winrt::init_apartment(apartment_type::single_threaded);
		windowsXamlManager_ = WindowsXamlManager::InitializeForCurrentThread();
		desktopSource_ = DesktopWindowXamlSource{};
		auto interop = desktopSource_.as<IDesktopWindowXamlSourceNative>();
		check_hresult(interop->AttachToWindow(hWnd_));
		interop->get_WindowHandle(&hWndXamlIsland_);
		SetWindowPos(hWndXamlIsland_, 0, 0, 0, width, height, SWP_SHOWWINDOW);

		// Creating the Xaml content
		Windows::UI::Xaml::Controls::StackPanel xamlContainer;
		xamlContainer.Background(Windows::UI::Xaml::Media::SolidColorBrush{
			Windows::UI::Colors::LightGray() });

		rn_ = UWPApp::ReactNativeWinRT{};
		reactRootView_ = rn_.ReactRootView();
		reactRootView_.VerticalAlignment(Windows::UI::Xaml::VerticalAlignment::Center);
		reactRootView_.HorizontalAlignment(Windows::UI::Xaml::HorizontalAlignment::Center);
		reactRootView_.Width(width);
		reactRootView_.Height(height);
		xamlContainer.Children().Append(reactRootView_);

		xamlContainer.UpdateLayout();
		desktopSource_.Content(xamlContainer);
		// End XAML Island section

		ShowWindow(hWnd_, SW_SHOW);
		UpdateWindow(hWnd_);
	}

	~NativeWindow() = default;
	NativeWindow(const NativeWindow&) = delete;
	NativeWindow& operator=(const NativeWindow&) = delete;

	void start() {
		if (isRunning_) {
			return;
		}
		isRunning_ = true;

		// Message loop:
		MSG msg = {};
		while (GetMessage(&msg, NULL, 0, 0)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// This works around a cpp/winrt bug with composable/aggregable types tracked by Microsoft via 22116519
		rn_.as<::IUnknown>()->Release();
		reactRootView_.as<::IUnknown>()->Release();

		// Close window
		DestroyWindow(hWnd_);
	}

private:
	HWND hWnd_ = nullptr;
	// This Hwnd will be the window handler for the Xaml Island: A child window that contains Xaml.
	HWND hWndXamlIsland_ = nullptr;
	WindowsXamlManager windowsXamlManager_ = nullptr;
	DesktopWindowXamlSource desktopSource_ = nullptr;
	Windows::UI::Xaml::Controls::Grid reactRootView_ = nullptr;
	UWPApp::ReactNativeWinRT rn_ = nullptr;
	bool isRunning_ = false;
};

int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine, _In_ int nCmdShow) {
	NativeWindow nativeWindow{ hInstance };
	nativeWindow.start();
	return 0;
}