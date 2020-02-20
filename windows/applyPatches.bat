:: Remember current path
set CURRENT_PATH=%cd%
cd %~dp0

:: Patch ReactRootView2.cpp
"C:\Program Files\Git\usr\bin\patch.exe" -i ReactRootView2.cpp.patch ..\node_modules\react-native-windows\Microsoft.ReactNative\ReactRootView2.cpp

:: Creating missing pch.h
echo #pragma once > ..\node_modules\react-native-windows\Microsoft.ReactNative.Cxx\pch.h

cd %CURRENT_PATH%