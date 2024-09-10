#ifndef UNICODE
#define UNICODE
#endif


#include <Windows.h>
#include <dwmapi.h>

#pragma comment (lib, "Dwmapi")
//#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture = '*' publicKeyToken = '6595b64144ccf1df' language = '*'\"");

// constant values
#define ID_STARTQUIZ 100
#define ID_ANSWER1	101
#define ID_ANSWER2	102
#define ID_ANSWER3	103
#define ID_ANSWER4	104

const COLORREF BACKGROUND_COLOR = RGB(30, 30, 30);
const COLORREF TEXT_COLOR = RGB(255, 255, 255);
const COLORREF BUTTON_BACKGROUND = RGB(50, 50, 50);
const COLORREF BUTTON_SELECTED_BACKGROUND = RGB(40, 40, 40);

// global variables

HWND hMenuScreen;
HWND hQuizScreen;
HWND hMainButton;
HWND hLabel;
HWND hAnswerButtons[4];

WNDPROC defaultMenuScreenProc;
WNDPROC defaultQuizScreenProc;

int windowWidth = 640;
int windowHeight = 480;

int menuWidth;
int menuHeight;
int quizWidth;
int quizHeight;

// function prototypes

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK MenuWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK QuizWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

BOOL CreateMenuScreen(HWND hwnd);
BOOL CreateQuizScreen(HWND hwnd);
void CenterWindow(HWND hwnd, int width, int height);
void ShowMenuScreen(HWND hwnd);
void ShowQuizScreen(HWND hwnd);
void PaintBackground(HWND hwnd);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	// register window class

	const wchar_t CLASS_NAME[] = L"Main Window";

	WNDCLASS wc = {0};

	wc.lpfnWndProc	 = WindowProc;
	wc.hInstance	 = hInstance;
	wc.lpszClassName = CLASS_NAME;
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

	if (!RegisterClass(&wc))
	{
		MessageBox(NULL, L"Failed to register class.", L"Error", MB_ICONERROR);
		return 0;
	}

	// Create window
	HWND hwnd = CreateWindowEx(
		0,						// optional window styles
		CLASS_NAME,				// window class
		L"Main Window",			// window title
		WS_OVERLAPPEDWINDOW,	// window style
		// position
		CW_USEDEFAULT, CW_USEDEFAULT, 
		// size
		windowWidth, windowHeight,
		NULL,					// parent window
		NULL,					// menu
		hInstance,				// instance handle
		NULL					// additional data
	);

	if (!hwnd)
	{
		MessageBox(NULL, L"Failed to create main window.", L"Error", MB_ICONERROR);
		return 0;
	}

	// dark title bar
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
		{
			// center window on screen
			RECT rect;
			GetClientRect(hwnd, &rect);
			int xPos = (GetSystemMetrics(SM_CXSCREEN) - rect.right) / 2;
			int yPos = (GetSystemMetrics(SM_CYSCREEN) - rect.bottom) / 2;
			SetWindowPos(hwnd, NULL, xPos, yPos, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
			
			// create the main screen
			if (!CreateMenuScreen(hwnd))
			{
				return -1;
			}
			if (!CreateQuizScreen(hwnd))
			{
				return -1;
			}
			ShowMenuScreen(hwnd);
			break;
		}

	case WM_COMMAND:
		{
			// handling button logic
			if (LOWORD(wParam) == ID_STARTQUIZ)
			{
				ShowQuizScreen(hwnd);
			}
			break;
		}

	case WM_PAINT:
		{
			HBRUSH hbrBackground = CreateSolidBrush(BACKGROUND_COLOR);
			// pain the window background
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);
			FillRect(hdc, &ps.rcPaint, hbrBackground);
			EndPaint(hwnd, &ps);
			ReleaseDC(hwnd, hdc);
			DeleteObject(hbrBackground);
			break;
		}

	case WM_SIZE:
		{
			// calculate new window size and center children
			RECT rect;
			if (GetClientRect(hwnd, &rect))
			{
				//windowWidth = rect.right - rect.left;
				//windowHeight = rect.bottom - rect.top;

				windowWidth = LOWORD(lParam);
				windowHeight = HIWORD(lParam);
				CenterWindow(hQuizScreen, quizWidth, quizHeight);
				CenterWindow(hMenuScreen, menuWidth, menuHeight);
			}
			break;
		}

	case WM_GETMINMAXINFO:
		{
			// define minimal window size
			MINMAXINFO* minMax = (MINMAXINFO*)lParam;
			minMax->ptMinTrackSize.x = 600;
			minMax->ptMinTrackSize.y = 300;
			break;
		}

	case WM_DRAWITEM:
		{
			LPDRAWITEMSTRUCT lpdis = (LPDRAWITEMSTRUCT)lParam;

			//if (lpdis->CtlID == ID_STARTQUIZ)
			//{
				HDC hdc = lpdis->hDC;
				RECT rc = lpdis->rcItem;
				BOOL isSelected = lpdis->itemState & ODS_FOCUS;
				HBRUSH hbrButton = CreateSolidBrush(isSelected ? BUTTON_SELECTED_BACKGROUND : BUTTON_BACKGROUND);
				
				FillRect(hdc, &rc, hbrButton);
				DeleteObject(hbrButton);
				DrawEdge(hdc, &rc, EDGE_BUMP, BF_RECT);

				SetBkMode(hdc, TRANSPARENT);
				SetTextColor(hdc, TEXT_COLOR);

				int textLength = GetWindowTextLength(lpdis->hwndItem);
				if (textLength > 0)
				{
					wchar_t* buttonText = (wchar_t*)malloc((textLength + 1) * sizeof(wchar_t));
					if (buttonText != NULL)
					{
						GetWindowText(lpdis->hwndItem, buttonText, textLength + 1);
						DrawText(hdc, buttonText, -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
					}
					free(buttonText);
				}

				return TRUE;
			//}

			break;
		}

	case WM_CTLCOLORSTATIC:
		{
			HBRUSH hbrBackground = CreateSolidBrush(BACKGROUND_COLOR);
			HDC hdcStatic = (HDC)wParam;

			SetTextColor(hdcStatic, TEXT_COLOR);
			SetBkColor(hdcStatic, BACKGROUND_COLOR);

			return (INT_PTR)hbrBackground;
		}

	case WM_DESTROY:
		if (hwnd)
		{
			DestroyWindow(hwnd);
		}
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK MenuWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_COMMAND:
		SendMessage(GetParent(hwnd), uMsg, wParam, lParam);
		return 0;
	case WM_DRAWITEM:
		SendMessage(GetParent(hwnd), uMsg, wParam, lParam);
		return 0;
	default:
		return CallWindowProc(defaultMenuScreenProc, hwnd, uMsg, wParam, lParam);
	}
}

LRESULT CALLBACK QuizWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_PAINT:
		PaintBackground(hwnd);
		return 0;

	case WM_CTLCOLORSTATIC:
		return SendMessage(GetParent(hwnd), uMsg, wParam, lParam);

	case WM_DRAWITEM:
		return SendMessage(GetParent(hwnd), uMsg, wParam, lParam);

	default:
		return CallWindowProc(defaultQuizScreenProc, hwnd, uMsg, wParam, lParam);
	}
}

void CenterWindow(HWND hwnd, int childWidth, int childHeight)
{
	int left = (windowWidth - childWidth) / 2;
	int top = (windowHeight - childHeight) / 2;

	SetWindowPos(hwnd, NULL, left, top, childWidth, childHeight, SWP_NOZORDER | SWP_NOACTIVATE);
}

BOOL CreateMenuScreen(HWND hwnd)
{
	int buttonWidth = 100;
	int buttonHeight = 20;

	menuWidth = buttonWidth;
	menuHeight = buttonHeight;

	// crete wrapper for buttons
	hMenuScreen = CreateWindowEx(
		//WS_EX_TRANSPARENT,
		0,		// optional window styles
		L"STATIC",				// window class
		NULL,					// window title
		WS_CHILD | WS_VISIBLE,	// window styles
		// position and size
		0, 0, buttonWidth, buttonHeight,
		hwnd,					// parent window
		NULL,					// menu
		// instance handle
		(HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
		NULL					// additional application data
	);
	
	if (!hMenuScreen)
	{
		return FALSE;
	}

	defaultMenuScreenProc = (WNDPROC)SetWindowLongPtr(hMenuScreen, GWLP_WNDPROC, (LONG_PTR)MenuWindowProc);

    hMainButton = CreateWindowEx(
		0,					// optional window styles
        L"BUTTON",			// window class
        L"Start Quiz",		// window text
        // window styles
        //WS_TABSTOP | WS_CHILD | BS_DEFPUSHBUTTON,
        WS_TABSTOP | WS_CHILD | BS_PUSHBUTTON | BS_FLAT | BS_OWNERDRAW | WS_VISIBLE,
        0,					// x position
        0,					// y position
        buttonWidth,		// width
        buttonHeight,		// height
        hMenuScreen,		// parent window
        (HMENU)ID_STARTQUIZ,// ID = 100
        // instance handle
        (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
        NULL				// additional application data
        );

	if (!hMainButton)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL CreateQuizScreen(HWND hwnd)
{
//	+--------------------------+ ^
//  |          label           | |
//  +--------------------------+ |
//	+---------+      +---------+ |
//	|   btn   |      |   btn   | |
//  +---------+      +---------+ | 170
//	+---------+      +---------+ |
//	|   btn   |      |   btn   | |
//  +---------+      +---------+ V
//  <---250---><-10-><---250---> 
//  <-----------510------------> 
//
	int buttonWidth = 250;
	int buttonHeight = 50;
	
	quizWidth = 510;
	quizHeight = 170;

	// wrapper
	hQuizScreen = CreateWindowEx(
		0,	// extended window styles
		L"STATIC",			// window class
		NULL,				// window text
		WS_CHILD | WS_VISIBLE,			// window styles
		0, 0, windowWidth, windowHeight,// size and position
		hwnd,				// parent window
		NULL,				// menu
		(HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
		NULL				// additional application data
	);

	if (!hQuizScreen)
	{
		return FALSE;
	}

	defaultQuizScreenProc = (WNDPROC)SetWindowLongPtr(hQuizScreen, GWLP_WNDPROC, (LONG_PTR)QuizWindowProc);

	// label
	hLabel = CreateWindowEx(
		0,
		L"STATIC",
		L"Question text goes here",
		WS_CHILD | WS_VISIBLE | SS_CENTER | SS_CENTERIMAGE,
		// SS_CENTERIMAGE center text vertically
		0, 0, (2 * buttonWidth) + 10, 50,
		hQuizScreen,
		NULL,
		(HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
		NULL
	);

	if (!hLabel)
	{
		return FALSE;
	}
	//HDC hdc = GetDC(hLabel);
	//SetTextAlign(hdc, TA_CENTER | VTA_CENTER);
	//ReleaseDC(hLabel, hdc);

	// answer buttons
	hAnswerButtons[0] = CreateWindowEx(
		0,
		L"Button",
		L"Answer 1",
		WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON | BS_FLAT | BS_OWNERDRAW,
		0, buttonHeight + 10, buttonWidth, buttonHeight,
		hQuizScreen,
		(HMENU)ID_ANSWER1,
		(HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
		NULL
	);
	hAnswerButtons[1] = CreateWindowEx(
		0,
		L"Button",
		L"Answer 2",
		WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON | BS_FLAT | BS_OWNERDRAW,
		buttonWidth + 10, buttonHeight + 10, buttonWidth, buttonHeight,
		hQuizScreen,
		(HMENU)ID_ANSWER2,
		(HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
		NULL
	);
	hAnswerButtons[2] = CreateWindowEx(
		0,
		L"Button",
		L"Answer 3",
		WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON | BS_FLAT | BS_OWNERDRAW,
		0, (2 * buttonHeight) + 20, buttonWidth, buttonHeight,
		hQuizScreen,
		(HMENU)ID_ANSWER3,
		(HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
		NULL
	);
	hAnswerButtons[3] = CreateWindowEx(
		0,
		L"Button",
		L"Answer 4",
		WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON | BS_FLAT | BS_OWNERDRAW,
		buttonWidth + 10, (2 * buttonHeight) + 20, buttonWidth, buttonHeight,
		hQuizScreen,
		(HMENU)ID_ANSWER4,
		(HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
		NULL
	);

	for (int i = 0; i < 4; i++)
	{
		if (!hAnswerButtons[i])
		{
			return FALSE;
		}
	}

	return TRUE;
	//for (int i = 0; i < 4; i++)
	//{
	//	hAnswerButtons[i] = CreateWindowEx(
	//		0,
	//		L"BUTTON",
	//		L"Answer",
	//		WS_CHILD | WS_TABSTOP | BS_PUSHBUTTON | BS_FLAT | WS_VISIBLE,
	//		50, 120 + (i * 60), 400, 50,
	//		hQuizScreen,
	//		(HMENU)(2 + i),
	//		(HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
	//		NULL
	//	);
	//}
}

void ShowMenuScreen(HWND hwnd)
{
	ShowWindow(hMenuScreen, SW_SHOW);
	ShowWindow(hQuizScreen, SW_HIDE);
}

void ShowQuizScreen(HWND hwnd)
{
	ShowWindow(hMenuScreen, SW_HIDE);
	ShowWindow(hQuizScreen, SW_SHOW);
}

void PaintBackground(HWND hwnd)
{
	PAINTSTRUCT ps;
	HBRUSH hbrBackground = CreateSolidBrush(BACKGROUND_COLOR);
	HDC hdc = BeginPaint(hwnd, &ps);
	FillRect(hdc, &ps.rcPaint, hbrBackground);
	EndPaint(hwnd, &ps);
	ReleaseDC(hwnd, hdc);
}
