:: Remember current path
set CURRENT_PATH=%cd%
cd %~dp0

:: Patch ReactRootView2.cpp
"C:\Program Files\Git\usr\bin\patch.exe" -i ReactRootControl.cpp.patch ..\node_modules\react-native-windows\Microsoft.ReactNative\Views\ReactRootControl.cpp

:: Creating missing pch.h
echo #pragma once > ..\node_modules\react-native-windows\Microsoft.ReactNative.Cxx\pch.h

cd %CURRENT_PATH%