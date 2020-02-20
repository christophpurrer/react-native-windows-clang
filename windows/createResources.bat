:: Remember current path
set CURRENT_PATH=%cd%
cd %~dp0

:: Create resources folder
if not exist resources mkdir resources

:: https://docs.microsoft.com/en-us/windows/uwp/app-resources/makepri-exe-command-options
:: Create priconfig
MakePri.exe createconfig /cf priconfig.xml /dq en-US /pv 10.0.0 /o
:: Create resources.pri
MakePri.exe new /pr resources /cf priconfig.xml /in win32 /of x64/Debug/resources.pri /o

cd %CURRENT_PATH%