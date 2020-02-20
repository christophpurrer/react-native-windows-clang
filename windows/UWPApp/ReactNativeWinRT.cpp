#include "pch.h"
#include "ReactNativeWinRT.h"
#include "ReactNativeWinRT.g.cpp"
#include "NativeModules.h"

using namespace winrt;
using namespace Windows::UI::Xaml;
using namespace Microsoft::ReactNative;

namespace winrt::UWPApp::implementation
{
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

	ReactNativeWinRT::ReactNativeWinRT()
	{
		ReactInstanceSettings settings;
#ifdef BUNDLE
		reactNativeHost.JavaScriptBundleFile(JSFILENAME);
		settings.UseLiveReload(false);
		settings.UseWebDebugger(false);
		settings.EnableDeveloperMenu(false);
#else
		reactNativeHost_.JavaScriptMainModuleName(JSFILENAME);
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
	}

	Windows::UI::Xaml::Controls::Grid ReactNativeWinRT::ReactRootView() {
		return reactNativeHost_.GetOrCreateRootView(nullptr).as<Windows::UI::Xaml::Controls::Grid>();
	}
}
