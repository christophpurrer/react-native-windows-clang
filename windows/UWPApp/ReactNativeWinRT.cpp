#include "pch.h"
#include "ReactNativeWinRT.h"
#if __has_include("ReactNativeWinRT.g.cpp")
#include "ReactNativeWinRT.g.cpp"
#endif

using namespace winrt;
using namespace Windows::UI::Xaml;
using namespace react::uwp;

namespace winrt::UWPApp::implementation
{
#ifdef BUNDLE  
    const wchar_t* JSFILENAME = L"index.windows";
#else  
    const wchar_t* JSFILENAME = L"index";
#endif
    const wchar_t* JSCOMPONENTNAME = L"rnw";
    
    ReactNativeWinRT::ReactNativeWinRT()
    {
        InitializeComponent();
        LoadReact();
    }

    void ReactNativeWinRT::LoadReact()
    {
        react::uwp::ReactControl RootElement;
        RootGrid().Children().Append(RootElement);

        InstanceSettings settings;

#ifdef BUNDLE
        settings.UseLiveReload = false;
        settings.UseWebDebugger = false;
#else    
        settings.UseLiveReload = true;
        settings.UseWebDebugger = true;
#endif

        auto instance = Instance::Create(winrt::hstring(JSFILENAME));
        instance.Start(settings);

        RootElement.Instance(instance);

        const wchar_t* initialProps = L"{ "
            L"\"one\":\"1\""
            L", \"two\":\"2\""
            L"}";

        RootElement.InitialProps(winrt::hstring(initialProps));
        RootElement.JsComponentName(JSCOMPONENTNAME);
        RootElement.StartRender();
    }
}
