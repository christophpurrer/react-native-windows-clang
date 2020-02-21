#include <windows.h>

HMODULE lib = LoadLibrary(L"win32.dll");
#ifdef __cplusplus
extern "C" {
#endif
typedef void (*openNativeWindowFunc)();
openNativeWindowFunc openNativeWindow = (openNativeWindowFunc)GetProcAddress(lib, "openNativeWindow");
#ifdef __cplusplus
}
#endif

int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine, _In_ int nCmdShow) {
	openNativeWindow();
	return 0;
}