﻿#pragma once

#include "winrt/Windows.UI.Xaml.h"
#include "winrt/Windows.UI.Xaml.Markup.h"
#include "winrt/Windows.UI.Xaml.Interop.h"
#include "winrt/Windows.UI.Xaml.Controls.Primitives.h"
#include "ReactNativeWinRT.g.h"

namespace winrt::UWPApp::implementation
{
    struct ReactNativeWinRT : ReactNativeWinRTT<ReactNativeWinRT>
    {
        ReactNativeWinRT();
    private:
        void LoadReact();
    };
}

namespace winrt::UWPApp::factory_implementation
{
    struct ReactNativeWinRT : ReactNativeWinRTT<ReactNativeWinRT, implementation::ReactNativeWinRT>
    {
    };
}
