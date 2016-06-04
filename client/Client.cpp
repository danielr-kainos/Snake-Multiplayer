#include "ClientGame.h"
#include "Drawing.h"
#include "DirectionEnum.h"
#include "ClientNetwork.h"
#include <stdlib.h>

//poczatek - nowy kod
#define ID_IP 1
#define ID_PORT 2
#define ID_SAVE 3

const int len = 20;
char user_ip[len], user_port[len];
HWND origin;

LRESULT CALLBACK windowprocessforwindow2(HWND handleforwindow2, UINT message, WPARAM wParam, LPARAM lParam);

bool window2open = false;
bool windowclass2registeredbefore = false;

enum windowtoopenenumt { none, window2 };

windowtoopenenumt windowtoopenenum = none;

void createwindow2(WNDCLASSW& wc2, HWND& hwnd, HINSTANCE hInst, int nShowCmd);
// koniec - nowy kod

typedef std::basic_string<TCHAR> ustring;
ClientGame *client;
DirectionEnum Direction;
TileTypeEnum board[MAX_X][MAX_Y];

void RestartGame(HWND hWnd)
{
	Drawing::RedrawWindow(hWnd, board);
	Direction = RIGHT;
	// clear the board array
	memset(board, 0, sizeof(board[0][0]) * MAX_X * MAX_Y);
	// get server's address from user
	char *ip = user_ip;
	char *port = user_port;
	client = new ClientGame(ip, port);
}


void HandleMenuSelection(HWND hWnd, WPARAM param)
{
	switch (LOWORD(param)) {
	case ID_FILE_NEWGAME:
		windowtoopenenum = window2;
		break;
	case ID_FILE_EXIT:
		PostQuitMessage(0);
		break;
	}
}


void HandleKeyboardInput(HWND hWnd, WPARAM input)
{
	switch (input) {
	case VK_RIGHT:
		if (Direction != LEFT && Direction != RIGHT)
		{
			Direction = RIGHT;
			client->sendActionPackets(RIGHT);
		}
		break;
	case VK_LEFT:
		if (Direction != LEFT && Direction != RIGHT)
		{
			Direction = LEFT;
			client->sendActionPackets(LEFT);
		}
		break;
	case VK_UP:
		if (Direction != UP && Direction != DOWN)
		{
			Direction = UP;
			client->sendActionPackets(UP);
		}
		break;
	case VK_DOWN:
		if (Direction != UP && Direction != DOWN)
		{
			Direction = DOWN;
			client->sendActionPackets(DOWN);
		}
		break;
	case VK_ESCAPE:
		PostQuitMessage(0);
		break;
	}
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_KEYDOWN: // handle keyboard input
		HandleKeyboardInput(hWnd, wParam);
		break;
	case WM_PAINT: // redraw window
		Drawing::RedrawWindow(hWnd, board);
		break;
	case WM_COMMAND: // handle menu selection 
		HandleMenuSelection(hWnd, wParam);
		break;
	case WM_CREATE: // create window
		Drawing::Init(hWnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT nShowCmd)
{
	WNDCLASSW windowclassforwindow2; // nowe
	HWND handleforwindow2;  // nowe

	// create window1
	MSG msg;
	client = nullptr;

	WNDCLASSEX wc = { sizeof(WNDCLASSEX), 0, WndProc, 0, 0, hInstance, nullptr,
		nullptr, HBRUSH(COLOR_WINDOW + 1), nullptr, "wndClass", nullptr };

	if (!RegisterClassEx(&wc))
	{
		MessageBox(nullptr, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	// create the window
	HWND hWnd = CreateWindowEx(WS_EX_CLIENTEDGE, "wndClass", "Multiplayer Snake!", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, WINDOWSIZE_X, WINDOWSIZE_Y, nullptr, nullptr, hInstance, nullptr);
	origin = hWnd; // nowe

	if (hWnd == nullptr)
	{
		MessageBox(nullptr, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	ShowWindow(hWnd, nShowCmd);
	UpdateWindow(hWnd);
	// enter the message loop - listen for events and then send them to WndProc()
	while (GetMessage(&msg, nullptr, 0, 0) > 0)
	{
		if (client != nullptr) client->update();
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		if (windowtoopenenum != none) {
			switch (windowtoopenenum) {
			case window2:
				if (window2open == false) {
					createwindow2(windowclassforwindow2, handleforwindow2, hInstance, nShowCmd);
				}
				break;
			}
			windowtoopenenum = none;
		}
	}

	UnregisterClass("wndClass", wc.hInstance);
	return 0;
}

void createwindow2(WNDCLASSW& wc, HWND& hWnd, HINSTANCE hInst, int nShowCmd) {
	if (windowclass2registeredbefore == false) {
		ZeroMemory(&wc, sizeof(WNDCLASSW));
		wc.cbClsExtra = NULL;
		wc.cbWndExtra = NULL;
		wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hIcon = NULL;
		wc.hInstance = hInst;
		wc.lpfnWndProc = (WNDPROC)windowprocessforwindow2;
		wc.lpszClassName = L"wc2";
		wc.lpszMenuName = NULL;
		wc.style = CS_HREDRAW | CS_VREDRAW;

		if (!RegisterClassW(&wc))
		{
			int nResult = GetLastError();
			MessageBox(NULL,
				"Window class creation failed",
				"Window Class Failed",
				MB_ICONERROR);
		}
		else
			windowclass2registeredbefore = true;
	}
	HWND hwnd = CreateWindowW(wc.lpszClassName, L"Network settings", 
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		220, 220, 280, 200, 0, 0, hInst, 0);


	if (!hwnd)
	{
		int nResult = GetLastError();

		MessageBox(NULL,
			"Window creation failed",
			"Window Creation Failed",
			MB_ICONERROR);
	}

	ShowWindow(hwnd, nShowCmd);
}

LRESULT CALLBACK windowprocessforwindow2(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	static HWND hwndIP, ipLabel;
	static HWND hwndPort, portLabel;
	HWND hwndButton;

	switch (msg) {

	case WM_CREATE:
		ipLabel = CreateWindowW(L"static", L"IP:",
			WS_CHILD | WS_VISIBLE | WS_TABSTOP,
			30, 20, 30, 20, hwnd, (HMENU)ID_IP,
			NULL, NULL);

		portLabel = CreateWindowW(L"static", L"Port:",
			WS_CHILD | WS_VISIBLE | WS_TABSTOP,
			20, 50, 30, 20, hwnd, (HMENU)ID_IP,
			NULL, NULL);

		hwndIP = CreateWindowW(L"Edit", NULL,
			WS_CHILD | WS_VISIBLE | WS_BORDER,
			55, 20, 150, 20, hwnd, (HMENU)ID_IP,
			NULL, NULL);

		hwndPort = CreateWindowW(L"Edit", NULL,
			WS_CHILD | WS_VISIBLE | WS_BORDER,
			55, 50, 150, 20, hwnd, (HMENU)ID_PORT,
			NULL, NULL);

		hwndButton = CreateWindowW(L"button", L"Connect",
			WS_VISIBLE | WS_CHILD, 90, 100, 80, 25,
			hwnd, (HMENU)ID_SAVE, NULL, NULL);

		break;

	case WM_COMMAND:

		if (HIWORD(wParam) == BN_CLICKED) {
			wchar_t ip[len], port[len];

			GetWindowTextW(hwndIP, ip, len);
			GetWindowTextW(hwndPort, port, len);
			memset(user_ip, 0, 20+1);
			wcstombs(user_ip, ip, 20);
			memset(user_port, 0, 20 + 1);
			wcstombs(user_port, port, 20);
			RestartGame(origin);
			DestroyWindow(hwnd);
		}

		break;

	case WM_DESTROY:
		window2open=false;
		break;
	}

	return DefWindowProcW(hwnd, msg, wParam, lParam);
}