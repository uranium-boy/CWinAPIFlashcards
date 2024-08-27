#ifndef UNICODE
#define UNICODE
#endif


#include <Windows.h>
#include <dwmapi.h>

#pragma comment (lib, "Dwmapi")

HWND hMenuScreen;
HWND hQuizScreen;
HWND hMainButton;
HWND hLabel;
HWND hAnswerButtons[4];

int width = 640;
int height = 480;

LRESULT WINAPI WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void centerWindow(HWND hwnd);
void createMenuScreen(HWND hwnd);
void createQuizScreen(HWND hwnd);
void showMenuScreen(HWND hwnd);
void showQuizScreen(HWND hwnd);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	// register window class

	const wchar_t CLASS_NAME[] = L"Main Window";

	WNDCLASS wc = {0};

	wc.lpfnWndProc	 = WindowProc;
	wc.hInstance	 = hInstance;
	wc.lpszClassName = CLASS_NAME;
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

	RegisterClass(&wc);

	// Create window

	HWND hwnd = CreateWindowEx(
		0,						// optional window styles
		CLASS_NAME,				// window class
		L"Main Window",			// window title
		WS_OVERLAPPEDWINDOW,	// window style
		// position
		CW_USEDEFAULT, CW_USEDEFAULT, 
		// size
		width, height,
		NULL,					// parent window
		NULL,					// menu
		hInstance,				// instance handle
		NULL					// additional data
	);

	if (hwnd == NULL)
	{
		return 0;
	}

	BOOL pvAttribute = TRUE;
	DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &pvAttribute, sizeof(pvAttribute));
	ShowWindow(hwnd, nCmdShow);

	// app loop

	MSG msg = {0};
	while (GetMessage(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
		centerWindow(hwnd);
		// create the main screen
		createMenuScreen(hwnd);
		createQuizScreen(hwnd);
		showMenuScreen(hwnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);
			//HBRUSH hBrush = CreateSolidBrush(RGB(30, 30, 30));
			//FillRect(hdc, &ps.rcPaint, hBrush);

			FillRect(hdc, &ps.rcPaint, GetSysColorBrush(COLOR_WINDOWTEXT));

			EndPaint(hwnd, &ps);
		}
		return 0;
	case WM_CTLCOLORSTATIC:
		{
			HDC hdcStatic = (HDC)wParam;
			SetBkMode(hdcStatic, TRANSPARENT);
			HBRUSH hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
			return (INT_PTR)hbrBackground;
			//return (LRESULT)GetStockObject(NULL_BRUSH);
		}
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void centerWindow(HWND hwnd)
{
	// center window on screen
	RECT rect;
	GetClientRect(hwnd, &rect);
	int xPos = (GetSystemMetrics(SM_CXSCREEN) - rect.right) / 2;
	int yPos = (GetSystemMetrics(SM_CYSCREEN) - rect.bottom) / 2;
	SetWindowPos(hwnd, HWND_TOP, xPos, yPos, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

void createMenuScreen(HWND hwnd)
{
	// crete wrapper for buttons
	hMenuScreen = CreateWindowEx(
		WS_EX_TRANSPARENT,		// optional window styles
		L"STATIC",				// window class
		NULL,					// window title
		WS_CHILD | WS_VISIBLE,	// window styles
		// position and size
		0, 0, width, height,
		hwnd,					// parent window
		NULL,					// menu
		// instance handle
		(HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
		NULL					// additional application data
	);
	// make the child window transparent
	//SetLayeredWindowAttributes(hMenuScreen, 0, 255, LWA_ALPHA);
	//SetWindowLongPtr(hMenuScreen, GWLP_WNDPROC, (LONG_PTR)DefWindowProc);
	
    hMainButton = CreateWindowEx(
		0,					// optional window styles
        L"BUTTON",			// window class
        L"Start Quiz",		// window text
        // window styles
        //WS_TABSTOP | WS_CHILD | BS_DEFPUSHBUTTON,
        WS_TABSTOP | WS_CHILD | BS_PUSHBUTTON | BS_FLAT | WS_VISIBLE,
        10,					// x position
        10,					// y position
        100,				// width
        20,					// height
        hMenuScreen,		// parent window
        (HMENU)0,			// ID = 0
        // instance handle
        (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
        NULL				// additional application data
        );
}

void createQuizScreen(HWND hwnd)
{
	// wrapper
	hQuizScreen = CreateWindowEx(
		WS_EX_TRANSPARENT,	// extended window styles
		L"STATIC",			// window class
		NULL,				// window text
		WS_CHILD,			// window styles
		0, 0, width, height,// size and position
		hwnd,				// parent window
		NULL,				// menu
		(HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
		NULL				// additional application data
	);

	// label
	hLabel = CreateWindowEx(
		0,
		L"STATIC",
		L"Question text goes here",
		WS_CHILD | ES_CENTER,
		50, 50, 100, 100,
		hQuizScreen,
		NULL,
		(HINSTANCE)GetWindowLongPtr(hQuizScreen, GWLP_HINSTANCE),
		NULL
	);

	// answer buttons
	for (int i = 0; i < 4; i++)
	{
		hAnswerButtons[i] = CreateWindowEx(
			0,
			L"BUTTON",
			L"Answer",
			WS_CHILD | WS_TABSTOP | BS_PUSHBUTTON | BS_FLAT,
			50, 120 + (i * 60), 400, 50,
			hQuizScreen,
			(HMENU)(2 + i),
			(HINSTANCE)GetWindowLongPtr(hQuizScreen, GWLP_HINSTANCE),
			NULL
		);
	}
}

void showMenuScreen(HWND hwnd)
{
	ShowWindow(hMenuScreen, SW_SHOW);
	ShowWindow(hQuizScreen, SW_HIDE);
}

void showQuizScreen(HWND hwnd)
{
	ShowWindow(hMenuScreen, SW_HIDE);
	ShowWindow(hQuizScreen, SW_SHOW);
}
