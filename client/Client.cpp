#include "ClientGame.h"
#include "Drawing.h"
#include "DirectionEnum.h"


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
	// TODO: prompt user to get that info
	char *ip = "localhost";
	char *port = "27015";
	client = new ClientGame(ip, port);
}


void HandleMenuSelection(HWND hWnd, WPARAM param)
{
	switch (LOWORD(param)) {
	case ID_FILE_NEWGAME:
		RestartGame(hWnd);
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
	}

	UnregisterClass("wndClass", wc.hInstance);
	return 0;
}
