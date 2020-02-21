#include "pch.h"
#include "NativeModules.h"
#include "main.h"

using namespace winrt;
using namespace Windows::UI::Composition;
using namespace Windows::UI::Xaml::Hosting;
using namespace Microsoft::ReactNative;

#ifdef BUNDLE
const wchar_t* JSFILENAME = L"index.bundle";
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

struct NativeWindow {
	NativeWindow(HINSTANCE hInstance) noexcept {
		// register the windows class with unique className to allow multiWindow support
		std::wstring windowClassName = L"Messenger" + std::to_wstring(windowCounter++);
		WNDCLASSEX windowClass = {};
		windowClass.cbSize = sizeof(WNDCLASSEX);
		windowClass.lpfnWndProc = WindowProc;
		windowClass.hInstance = hInstance;
		windowClass.lpszClassName = windowClassName.c_str();
		windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		windowClass.hIconSm = LoadIcon(windowClass.hInstance, IDI_APPLICATION);

		if (RegisterClassEx(&windowClass) == NULL) {
			MessageBox(NULL, L"Windows registration failed!", L"Error", NULL);
			return;
		}

		auto width = 720;
		auto height = 1280;
		hWnd_ =
			CreateWindow(windowClassName.c_str(), L"Win32 Desktop App",
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

		reactNativeHost_ = Microsoft::ReactNative::ReactNativeHost{};
		reactNativeHost_.JavaScriptBundleFile(JSFILENAME);
		ReactInstanceSettings settings;
#ifdef BUNDLE
		settings.UseLiveReload(false);
		settings.UseWebDebugger(false);
		settings.EnableDeveloperMenu(false);
#else
		settings.UseLiveReload(true);
		settings.UseWebDebugger(true);
		settings.EnableDeveloperMenu(true);
		reactNativeHost_.UseDeveloperSupport(true);
#endif
		settings.UseJsi(true);
		reactNativeHost_.InstanceSettings(settings);
		reactNativeHost_.PackageProviders(single_threaded_vector<IReactPackageProvider>());
		reactNativeHost_.PackageProviders().Append(make<ReactPackageProvider>());
		reactNativeHost_.MainComponentName(JSCOMPONENTNAME);

		auto reactRootView = reactNativeHost_.GetOrCreateRootView(nullptr).as<Windows::UI::Xaml::Controls::Grid>();
		reactRootView.VerticalAlignment(Windows::UI::Xaml::VerticalAlignment::Center);
		reactRootView.HorizontalAlignment(Windows::UI::Xaml::HorizontalAlignment::Center);
		reactRootView.Width(width);
		reactRootView.Height(height);
		xamlContainer.Children().Append(reactRootView);

		xamlContainer.UpdateLayout();
		desktopSource_.Content(xamlContainer);
		// End XAML Island section

		ShowWindow(hWnd_, SW_SHOW);
		UpdateWindow(hWnd_);
	}

	~NativeWindow() noexcept {
		// This works around a cpp/winrt bug with composable/aggregable types tracked by Microsoft via 22116519
		reactNativeHost_.GetOrCreateRootView(nullptr).as<::IUnknown>()->Release();
	}

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

		// Close window
		DestroyWindow(hWnd_);
	}

private:
	static std::atomic<int> windowCounter;
	HWND hWnd_ = nullptr;
	// This Hwnd will be the window handler for the Xaml Island: A child window that contains Xaml.
	HWND hWndXamlIsland_ = nullptr;
	WindowsXamlManager windowsXamlManager_ = nullptr;
	DesktopWindowXamlSource desktopSource_ = nullptr;
	Microsoft::ReactNative::ReactNativeHost reactNativeHost_ = nullptr;
	bool isRunning_ = false;
};

std::atomic<int> NativeWindow::windowCounter{ 0 };

void openNativeWindow() {
	NativeWindow nativeWindow{ (HINSTANCE)GetModuleHandle(NULL) };
	nativeWindow.start();
}