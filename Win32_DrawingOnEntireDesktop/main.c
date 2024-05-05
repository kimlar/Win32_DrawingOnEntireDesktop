/*
*	Drawing on the entire desktop
* 
*	NOTE #1: The background behind the text is only drawn once hence the application must be manually started without any other windows overlapping it.
*	NOTE #2: Need to refresh the background at intervals in order to avoid old background. See #1 for more info.
*/


#ifndef UNICODE
#define UNICODE
#endif 

#include <windows.h>
#include <stdbool.h>

#include "AlphaText.h"

#define IDT_TIMER1 101
#define IDT_TIMER2 102

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

bool isRunning = false;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	// Register the window class.
	const wchar_t CLASS_NAME[] = L"DrawOnDesktopWindowClass";

	WNDCLASS wc = { 0 };

	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;

	RegisterClass(&wc);

	// Create the window.
	HWND hwnd = CreateWindowEx(
		WS_EX_TRANSPARENT,			// Optional window styles.
		CLASS_NAME,					// Window class
		L"Draw on desktop",			// Window text
		0,							// Window style

		// Size and position
		200, 400, 500, 500,

		NULL,       // Parent window    
		NULL,       // Menu
		hInstance,  // Instance handle
		NULL        // Additional application data
	);

	if (hwnd == NULL)
		return 0;
	
	isRunning = true;

	// Remove the title bar, border, minimize box, maximize box, X-button, and menu.
	LONG_PTR lStyle = GetWindowLongPtr(hwnd, GWL_STYLE);
	lStyle &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU);
	SetWindowLongPtr(hwnd, GWL_STYLE, lStyle);

	// Remove extended window styles.
	LONG_PTR lExStyle = GetWindowLongPtr(hwnd, GWL_EXSTYLE);
	lExStyle &= ~(WS_EX_DLGMODALFRAME | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE);
	lExStyle |= WS_EX_TOOLWINDOW;
	SetWindowLongPtr(hwnd, GWL_EXSTYLE, lExStyle);

	// And finally, to get your window to redraw with the changed styles
	SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);
	
	ShowWindow(hwnd, nCmdShow);

	// Set timers
	SetTimer(hwnd, IDT_TIMER1, 20, (TIMERPROC)NULL);
	SetTimer(hwnd, IDT_TIMER2, 5000, (TIMERPROC)NULL);

	// Run the message loop.
	MSG msg = { 0 };
	while (isRunning)
	{
		//while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) > 0)
		while (GetMessage(&msg, NULL, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		//Sleep(20);
	}

	return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_WINDOWPOSCHANGING:
		{
			WINDOWPOS* pos = (WINDOWPOS*)lParam;
			// Show desktop (Windows + D) results in the window moved to -32000, -32000 and size changed
			if (pos->x == -32000)
			{
				// Set the flags to prevent this and "survive" to the desktop toggle
				pos->flags |= SWP_NOMOVE | SWP_NOSIZE;
			}
			// Also force the z order to ensure the window is always on bottom
			pos->hwndInsertAfter = HWND_BOTTOM;
			return 0;
		}
		case WM_CLOSE:
		{
			DestroyWindow(hwnd);
			break;
		}
		case WM_DESTROY:
		{
			// Destroy the timers
			KillTimer(hwnd, IDT_TIMER1);
			KillTimer(hwnd, IDT_TIMER2);

			PostQuitMessage(0);
			isRunning = false;
			return 0;
		}
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);

			TestAlphaText(hdc, 0, 0);

			EndPaint(hwnd, &ps);
			break;
		}
		case WM_TIMER:
		{
			switch (wParam)
			{
				case IDT_TIMER1:
				{
					InvalidateRect(hwnd, NULL, TRUE);
					return 0;
				}
				case IDT_TIMER2:
				{
					// Destroy the timers
					KillTimer(hwnd, IDT_TIMER1);
					KillTimer(hwnd, IDT_TIMER2);

					PostQuitMessage(0);
					isRunning = false;
					return 0;
				}
				break;
			}
		}
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
