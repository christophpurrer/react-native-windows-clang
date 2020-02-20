#pragma once

#include "winrt/Windows.UI.Xaml.Controls.h"
#include "ReactNativeWinRT.g.h"

namespace winrt::UWPApp::implementation
{
	struct ReactNativeWinRT : ReactNativeWinRTT<ReactNativeWinRT>
	{
		ReactNativeWinRT();
		Windows::UI::Xaml::Controls::Grid ReactRootView();
	private:
		Microsoft::ReactNative::ReactNativeHost reactNativeHost_;
	};
}

namespace winrt::UWPApp::factory_implementation
{
	struct ReactNativeWinRT : ReactNativeWinRTT<ReactNativeWinRT, implementation::ReactNativeWinRT>
	{
	};
}
