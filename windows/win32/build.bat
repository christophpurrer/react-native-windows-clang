set WINDOWS_SDK=C:\Program Files (x86)\Windows Kits\10\Lib\10.0.18362.0

@echo Generate C++/WinRT header files
cppwinrt.exe @cppwinrt_plat.rsp
cppwinrt.exe @cppwinrt_ref.rsp

@echo Create output folder
if not exist build mkdir build

@echo Compile with Clang >= 9.0.0
clang -v

@echo Create binary
clang -I "Generated Files" -I "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Tools\MSVC\14.24.28314\include" -D_WINDOWS -DUNICODE -D_UNICODE main.cpp -Xclang -std=c++2a -Wno-delete-non-virtual-dtor -Wno-c++11-narrowing -Wno-defaulted-function-deleted -o build\win32.exe -L "%WINDOWS_SDK%\um\x64" -Wl,advapi32.lib,comdlg32.lib,gdi32.lib,kernel32.lib,ole32.lib,odbc32.lib,odbccp32.lib,oleaut32.lib,user32.lib,shell32.lib,uuid.lib,windowsapp.lib,winspool.lib

@echo Embed manifest
:: https://docs.microsoft.com/en-us/cpp/build/how-to-embed-a-manifest-inside-a-c-cpp-application?view=vs-2019
"C:\Program Files (x86)\Windows Kits\10\bin\10.0.18362.0\x64\mt.exe" -manifest win32.exe.full.manifest -outputresource:build\win32.exe;1