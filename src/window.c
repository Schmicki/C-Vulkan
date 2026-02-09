#include <Windows.h>
#include "containers.h"
#include "io.h"
#include "rendering.h"
#include "window.h"



/**************************************************************************************************/
/*	Functions  */

LRESULT CALLBACK main_window_proc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (msg)
	{
	case WM_CREATE:
		// Initialize the window.
		return 0;

	case WM_PAINT:
		// Paint the window's client area.
		hdc = BeginPaint(hwnd, &ps);

		// Painting
		FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
		EndPaint(hwnd, &ps);
		return 0;

	case WM_SIZE:
		// Set the size and position of the window.
		return 0;

	case WM_DESTROY:
		// Clean up window-specific data objects.
		PostQuitMessage(0);
		return 0;

		// 
		// Process other messages.
		// 

	default:
		return DefWindowProc(hwnd, msg, wp, lp);
	}
	return 0;
}

ATOM get_main_window_class()
{
	static ATOM window_class_id = 0;

	if (window_class_id == 0)
	{
		WNDCLASS window_class;
		HMODULE instance = GetModuleHandle(NULL);

		window_class.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
		window_class.lpfnWndProc = &main_window_proc;
		window_class.cbClsExtra = 0;
		window_class.cbWndExtra = sizeof(void*);
		window_class.hInstance = instance;
		window_class.hIcon = NULL;
		window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
		window_class.hbrBackground = (HBRUSH)(COLOR_WINDOW);
		window_class.lpszMenuName = NULL;
		window_class.lpszClassName = L"sge_main_window_class";
		window_class_id = RegisterClass(&window_class);

		if (window_class_id == 0)
		{
			printf("%s %s: %i", "RegisterClass failed!", __FILE__, __LINE__);
			exit(101);
			return 0;
		}
	}

	return window_class_id;
}

static void get_fullscreen_rect(HWND opt_window, int* out_x, int* out_y, int* out_width, int* out_height)
{
	HMONITOR monitor;
	MONITORINFO monitor_info = { sizeof(MONITORINFO) };

	if (opt_window == NULL)
	{
		const POINT zero = { 0, 0 };
		monitor = MonitorFromPoint(zero, MONITOR_DEFAULTTOPRIMARY);
	}
	else
	{
		monitor = MonitorFromWindow(opt_window, MONITOR_DEFAULTTONEAREST);
	}

	if (!GetMonitorInfo(monitor, &monitor_info))
	{
		printf("%s %s: %i", "GetMonitorInfo failed!", __FILE__, __LINE__);
		exit(101);
		return;
	}

	*out_x = monitor_info.rcMonitor.left;
	*out_y = monitor_info.rcMonitor.top;
	*out_width = monitor_info.rcMonitor.right - monitor_info.rcMonitor.left;
	*out_height = monitor_info.rcMonitor.bottom - monitor_info.rcMonitor.top;
}

window create_window(const char* name, sint width, sint height, sint mode)
{
	window window;
	HMODULE instance;
	LPCWSTR wtitle;
	ATOM wclass;
	DWORD style;
	int x, y;
	int w, h;

	instance = GetModuleHandle(NULL);

	switch (mode)
	{
	default:
	case WINDOW_MODE_DEFAULT:
		style = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
		x = CW_USEDEFAULT;
		y = CW_USEDEFAULT;
		w = width;
		h = height;
		break;
	case WINDOW_MODE_BORDERLESS:
	case WINDOW_MODE_FULLSCREEN:
		style = WS_POPUP | WS_VISIBLE;
		get_fullscreen_rect(NULL, &x, &y, &w, &h);
		break;
	}

	wclass = get_main_window_class();
	wtitle = utf8_to_utf16(name, NULL);
	window.handle = CreateWindow(
		(LPCWSTR)wclass,
		wtitle,
		style,
		x,
		y,
		w,
		h,
		NULL,
		NULL,
		instance,
		NULL);
	free((void*)wtitle);
	return window;
}

void destroy_window(window* window)
{
	DestroyWindow((HWND)window->handle);
	window->handle = NULL;
}

sint update_windows()
{
	MSG msg;
	sint quit = 0;

	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
			quit = 1;

		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}

	return !quit;
}


