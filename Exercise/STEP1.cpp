#include <windows.h>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);//ウィンドウプロシージャー関数のプロトタイプ宣言

//アプリケーションのエントリー関数
INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR szStr, INT iCmdShow)
{
	HWND hWnd = NULL;
	MSG msg;
	//ウィンドウの初期化
	static char szAppName[] = "window";
	WNDCLASSEX wndclass;

	wndclass.cbSize = sizeof(wndclass);
	wndclass.style = CS_HREDRAW | CS_VREDRAW;//H | V はHorizontarl水平 | Vertical垂直
	//CS はclass style
	wndclass.lpfnWndProc = WndProc;//調べる
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInst;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.lpszMenuName = NULL;//調べる
	wndclass.lpszClassName = szAppName;
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	RegisterClassEx(&wndclass);//wndclassのアドレス

	hWnd = CreateWindow(szAppName, szAppName, WS_OVERLAPPEDWINDOW,
		0, 0, 640, 480, NULL, NULL, hInst, NULL);
	ShowWindow(hWnd, SW_SHOW);//表示する
	UpdateWindow(hWnd);

	//メッセージループ
	ZeroMemory(&msg, sizeof(msg));
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0u, 0u, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{

		}
	}
	//アプリケーションを終了する
	return(INT)msg.wParam;
}
//ウィンドプロシージャ関数
LRESULT CALLBACK WndProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam) 
{
	switch (iMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_KEYDOWN:
		switch ((CHAR)wParam) 
		{
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
		}
		break;
	}
	return DefWindowProc(hWnd, iMsg, wParam, lParam);
	//return関数（）；関数を呼び出し、その戻り値をreturnする
}