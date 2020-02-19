#include "pch.h"

using namespace winrt;
using namespace Windows::UI;
using namespace Windows::UI::Composition;
using namespace Windows::UI::Xaml::Hosting;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Numerics;

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);

// This Hwnd will be the window handler for the Xaml Island: A child window that contains Xaml.  
HWND _hWndXamlIsland = nullptr;
HWND _hWnd;
HWND _childhWnd;
HINSTANCE _hInstance;

int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	_hInstance = hInstance;

	// The main window class name.
	const wchar_t szWindowClass[] = L"WinDesk";
	WNDCLASSEX windowClass = { };

	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.lpfnWndProc = WindowProc;
	windowClass.hInstance = hInstance;
	windowClass.lpszClassName = szWindowClass;
	windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

	windowClass.hIconSm = LoadIcon(windowClass.hInstance, IDI_APPLICATION);

	if (RegisterClassEx(&windowClass) == NULL)
	{
		MessageBox(NULL, L"Windows registration failed!", L"Error", NULL);
		return 0;
	}

	auto width = 720;
	auto height = 1280;

	_hWnd = CreateWindow(
		szWindowClass,
		L"Windows c++ Win32 Desktop App",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT, width, height,
		NULL,
		NULL,
		hInstance,
		NULL
	);
	if (_hWnd == NULL)
	{
		MessageBox(NULL, L"Call to CreateWindow failed!", L"Error", NULL);
		return 0;
	}


	//XAML Island section

	// The call to winrt::init_apartment initializes COM; by default, in a multithreaded apartment.
	winrt::init_apartment(apartment_type::single_threaded);

	// Initialize the Xaml Framework's corewindow for current thread
	WindowsXamlManager winxamlmanager = WindowsXamlManager::InitializeForCurrentThread();

	// This DesktopWindowXamlSource is the object that enables a non-UWP desktop application 
	// to host UWP controls in any UI element that is associated with a window handle (HWND).
	DesktopWindowXamlSource desktopSource;
	// Get handle to corewindow
	auto interop = desktopSource.as<IDesktopWindowXamlSourceNative>();
	// Parent the DesktopWindowXamlSource object to current window
	check_hresult(interop->AttachToWindow(_hWnd));


	// Get the new child window's hwnd 
	interop->get_WindowHandle(&_hWndXamlIsland);
	// Update the xaml island window size becuase initially is 0,0
	SetWindowPos(_hWndXamlIsland, 0, 0, 0, width, height, SWP_SHOWWINDOW);

	//Creating the Xaml content
	Windows::UI::Xaml::Controls::StackPanel xamlContainer;
	xamlContainer.Background(Windows::UI::Xaml::Media::SolidColorBrush{ Windows::UI::Colors::LightGray() });

	//UWPApp::ReactNativeWinRT rn;
	//auto reactRootView = rn.ReactRootView();
	//reactRootView.VerticalAlignment(Windows::UI::Xaml::VerticalAlignment::Center);
	//reactRootView.HorizontalAlignment(Windows::UI::Xaml::HorizontalAlignment::Center);
	//reactRootView.Width(width);
	//reactRootView.Height(height);
	//xamlContainer.Children().Append(reactRootView);

	xamlContainer.UpdateLayout();
	desktopSource.Content(xamlContainer);

	//End XAML Island section

	ShowWindow(_hWnd, nCmdShow);
	UpdateWindow(_hWnd);

	//Message loop:
	MSG msg = { };
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT messageCode, WPARAM wParam, LPARAM lParam)
{

	switch (messageCode)
	{
	case WM_PAINT:
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

		//creating main window  
	case WM_CREATE:
		_childhWnd = CreateWindowEx(0, L"ChildWClass", NULL, WS_CHILD | WS_BORDER, 0, 0, 0, 0, hWnd, NULL, _hInstance, NULL);
		return 0;
		// main window changed size 
	case WM_SIZE:
		// Get the dimensions of the main window's client 
		// area, and enumerate the child windows. Pass the 
		// dimensions to the child windows during enumeration. 

		RECT rcClient;
		GetClientRect(hWnd, &rcClient);
		MoveWindow(_childhWnd, 200, 200, 400, 500, TRUE);
		ShowWindow(_childhWnd, SW_SHOW);
		SetWindowPos(_hWndXamlIsland, 0, 0, 0, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top, SWP_SHOWWINDOW);


		return 0;

		// Process other messages. 


	default:
		return DefWindowProc(hWnd, messageCode, wParam, lParam);
		break;
	}

	return 0;
}