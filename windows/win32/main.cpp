#include "pch.h"
#include "NativeModules.h"

using namespace winrt;
using namespace Windows::UI::Composition;
using namespace Windows::UI::Xaml::Hosting;
using namespace Microsoft::ReactNative;

#ifdef BUNDLE
const wchar_t* JSFILENAME = L"index.windows";
#else
const wchar_t* JSFILENAME = L"index";
#endif
const wchar_t* JSCOMPONENTNAME = L"App";

struct ReactPackageProvider : winrt::implements<ReactPackageProvider, IReactPackageProvider>
{
public:
	void CreatePackage(IReactPackageBuilder const& packageBuilder) noexcept {
		AddAttributedModules(packageBuilder);
	}
};

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

struct SimpleRedBoxHandler : winrt::implements<SimpleRedBoxHandler, winrt::Microsoft::ReactNative::IRedBoxHandler> {
	SimpleRedBoxHandler() noexcept {}

	void ShowNewError(
		winrt::Microsoft::ReactNative::IRedBoxErrorInfo const& info,
		winrt::Microsoft::ReactNative::RedBoxErrorType type) noexcept {
		OutputDebugStringA("----- Begin RedBox -----\n");

		switch (type) {
		case winrt::Microsoft::ReactNative::RedBoxErrorType::JavaScriptFatal:
			OutputDebugStringA("Fatal Error: ");
			break;
		case winrt::Microsoft::ReactNative::RedBoxErrorType::JavaScriptSoft:
			OutputDebugStringA("JavaScript Error: ");
			break;
		case winrt::Microsoft::ReactNative::RedBoxErrorType::Native:
			OutputDebugStringA("Native Error: ");
			break;
		}

		OutputDebugString(info.Message().c_str());
		OutputDebugStringA("\n-----  End RedBox  -----\n");
	}

	bool IsDevSupportEnabled() noexcept {
		return true;
	}

	void UpdateError(winrt::Microsoft::ReactNative::IRedBoxErrorInfo const& info) noexcept {
		// noop
	}

	void DismissRedBox() noexcept {
		// noop
	}
};


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
		ReactInstanceSettings settings;
		settings.MainComponentName(JSCOMPONENTNAME);
		settings.JavaScriptBundleFile(JSFILENAME);
		settings.UseDirectDebugger(false);
		settings.DebuggerBreakOnNextLine(false);

#ifdef BUNDLE
		settings.UseFastRefresh(false);
		settings.UseWebDebugger(false);
#else
		settings.UseFastRefresh(true);
		settings.UseWebDebugger(true);
#endif
		settings.RedBoxHandler(winrt::make<SimpleRedBoxHandler>());
		settings.Properties().Set(
			winrt::Microsoft::ReactNative::ReactDispatcherHelper::UIDispatcherProperty(),
			winrt::Microsoft::ReactNative::ReactDispatcherHelper::UIThreadDispatcher());

		reactNativeHost_ = Microsoft::ReactNative::ReactNativeHost();
		reactNativeHost_.InstanceSettings(settings);
		reactNativeHost_.PackageProviders(single_threaded_vector<IReactPackageProvider>());
		reactNativeHost_.PackageProviders().Append(make<ReactPackageProvider>());
		reactNativeHost_.ReloadInstance();

		reactRootView_ = Microsoft::ReactNative::ReactRootView();
		reactRootView_.ComponentName(reactNativeHost_.InstanceSettings().MainComponentName());
		reactRootView_.ReactNativeHost(reactNativeHost_);
		reactRootView_.VerticalAlignment(Windows::UI::Xaml::VerticalAlignment::Stretch);
		reactRootView_.HorizontalAlignment(Windows::UI::Xaml::HorizontalAlignment::Stretch);
		reactRootView_.UpdateLayout();
		desktopSource_.Content(reactRootView_);
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
		desktopSource_.Close();

		// Close window
		DestroyWindow(hWnd_);
	}

private:
	HWND hWnd_ = nullptr;
	// This Hwnd will be the window handler for the Xaml Island: A child window that contains Xaml.
	HWND hWndXamlIsland_ = nullptr;
	WindowsXamlManager windowsXamlManager_ = nullptr;
	DesktopWindowXamlSource desktopSource_ = nullptr;
	Microsoft::ReactNative::ReactNativeHost reactNativeHost_ = nullptr;
	Microsoft::ReactNative::ReactRootView reactRootView_ = nullptr;
	bool isRunning_ = false;
};

int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine, _In_ int nCmdShow) {
	NativeWindow nativeWindow{ hInstance };
	nativeWindow.start();
	return 0;
}