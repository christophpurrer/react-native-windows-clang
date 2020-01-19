# react-native-windows-clang
Sample using react-native-windows inside a plain old win32 window, using the Clang compiler (WIP)

## Start

To clone and run this repository you'll need [Git](https://git-scm.com), [Node.js](https://nodejs.org/en/download/) (which comes with [npm](http://npmjs.com)) installed on your computer. 

### From your command line:

```bash
# Clone this repository
git clone https://github.com/christophpurrer/react-native-windows-clang.git
# Change to the repository
cd react-native-desktop
# Install dependencies
yarn install
# start Metro
yarn start
```
### Building using Visual Studio 2019 ...

1.) Set the startup project to **win32**

2.) Choose *Build / Build Solution*

3.) If you encounter compilation errors, select **UWPApp** and choose *Build / Build UWP*, then repeat 2.)

4.) Run **win32** project

## How this project was setup
This sneaky examples leverage a set of technologies
- [Xaml Islands](https://docs.microsoft.com/en-us/windows/apps/desktop/modernize/using-the-xaml-hosting-api) >> [Sample](https://github.com/marb2000/XamlIslands/tree/master/1903_Samples/CppWinRT_Win32_SimpleApp/Win32DesktopApp)
- [react-native-windows as a Windows Runtime Component](https://techcommunity.microsoft.com/t5/windows-dev-appconsult/getting-started-with-react-native-for-windows/ba-p/912093) >> [C# Sample](https://github.com/microsoft/react-native-windows-samples/tree/master/samples/TodosFeed/TodosFeed/windows)
- [Reg-Free WinRT](https://blogs.windows.com/windowsdeveloper/2019/04/30/enhancing-non-packaged-desktop-apps-using-windows-runtime-components/) >> [Sample](https://github.com/Microsoft/RegFree_WinRT/tree/master/Cpp)
- [UWP Store App exported as .DLL with dummy.exe trick](https://github.com/marb2000/XamlIslands/tree/master/1903_Samples/CppWinRT_Win32_SingleIsland) >> Study the entire [README.MD](https://github.com/marb2000/XamlIslands/blob/master/1903_Samples/CppWinRT_Win32_SingleIsland/ReadMe.md)
- [VCRT Forwarding](https://www.nuget.org/packages/Microsoft.VCRTForwarders.140)

See [WinRT documentation](https://docs.microsoft.com/en-us/windows/uwp/cpp-and-winrt-apis/get-started) in case you are unfamiliar with it
