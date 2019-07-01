
#include <windows.h>
#include <d3d9.h>

LPDIRECT3D9 pD3d;
LPDIRECT3DDEVICE9 pD3Device;
D3DPRESENT_PARAMETERS D3dPresentParameters;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);//ウィンドウプロシージャー関数のプロトタイプ宣言
HRESULT BuildDxDevice(HWND, const TCHAR*);//プロトタイプ宣言
void InitPresentParameters(HWND hWnd);
HRESULT InitD3d(HWND, const TCHAR*);//Direct3Dの初期化関数のプロトタイプ宣言

struct CUSTOMVERTEX {
	float	x, y, z;	// 頂点座標
	float	rhw;	// 除算数
	DWORD	Color;	// 頂点の色
};

CUSTOMVERTEX v[3] =
{
{10, 10, 0.0f, 1.0f, 0xffff00ff},
{200, 10, 0.0f, 1.0f, 0xffffffff},
{200, 200, 0.0f, 1.0f, 0x0ff00fff}
};

int window_width = 640;
int window_hight = 480;

//アプリケーションのエントリー関数
INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR szStr, INT iCmdShow)
{
	DWORD SyncPrev = timeGetTime();
	DWORD SyncCurr;
	
	HWND hWnd = NULL;
	MSG msg;
	//ウィンドウの初期化
	static char szAppName[] = "STEP2";
	WNDCLASSEX wndclass;

	wndclass.cbSize = sizeof(wndclass);
	wndclass.style = CS_HREDRAW | CS_VREDRAW;//H | V はHorizontal水平 | Vertical垂直
	//CS はclass style
	wndclass.lpfnWndProc = WndProc;//調べる
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInst;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szAppName;
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	RegisterClassEx(&wndclass);//wndclassのアドレス

	hWnd = CreateWindow(szAppName, szAppName, WS_OVERLAPPEDWINDOW,
		0, 0, window_width, window_hight, NULL, NULL, hInst, NULL);

	ShowWindow(hWnd, SW_SHOW);//表示する
	UpdateWindow(hWnd);

	BuildDxDevice(hWnd, "Blank.jpg");

		//メッセージループ
	timeBeginPeriod(1);
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
			//ゲームの処理
			SyncCurr = timeGetTime();
			//1秒間に60回この中に入る
			if (SyncCurr - SyncPrev >= 1000 / 60)
			{
				//ウィンドウを黒色でクリア
				pD3Device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0x00, 0x00, 0x00), 1.0, 0);
				
				pD3Device->BeginScene();

				//いわれた場所に、言われた大きさで書き始める
				pD3Device->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 1, v, sizeof(CUSTOMVERTEX));

				pD3Device->EndScene();
				
				//ウィンドウに表示
				pD3Device->Present(0, 0, 0, 0);

				SyncPrev = SyncCurr;
			}
		}
		Sleep(1);
	}
	timeEndPeriod(1);

	pD3Device->Release();
	pD3Device = nullptr;
	pD3d->Release();
	pD3d = nullptr;

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

//デバイス作成
HRESULT BuildDxDevice(HWND hWnd, const TCHAR* filepath) {
	if (FAILED(InitD3d(hWnd, filepath))) {
		return E_FAIL;
	}

	pD3d = Direct3DCreate9(D3D_SDK_VERSION);

	if (pD3d == NULL) {
		MessageBox(0, "Direct3Dの作成に失敗しました", "", MB_OK);
		return E_FAIL;
	}
	pD3Device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	pD3Device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
	return S_OK;
}

//ダイレクト３D初期化関数
HRESULT InitD3d(HWND hWnd, const TCHAR* filepath)
{
	//Direct３Dオブジェクトの作成
	if (NULL == (pD3d = Direct3DCreate9(D3D_SDK_VERSION)))
	{
		MessageBox(0, "Direct3Dの作成に失敗しました","", MB_OK);
		return E_FAIL;
	}

	InitPresentParameters(hWnd);

	if (FAILED(pD3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_MIXED_VERTEXPROCESSING,
		&D3dPresentParameters, &pD3Device/*ダブルポインタ*/)))
	{
		MessageBox(0, "HALモードでDIRECT3Dデバイスを作成できません\nREFモードｄで再試行します", NULL, MB_OK);
		if (FAILED(pD3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, hWnd,
			D3DCREATE_MIXED_VERTEXPROCESSING,
			&D3dPresentParameters, &pD3Device)))
		{
			MessageBox(0, "DIRECT3Dデバイスの作成に失敗しました", NULL, MB_OK);
			return E_FAIL;
		}
	}
	return S_OK;
}

void InitPresentParameters(HWND hWnd) {
	//WindowMode
	ZeroMemory(&D3dPresentParameters, sizeof(D3DPRESENT_PARAMETERS));
	D3dPresentParameters.BackBufferWidth = 0;
	D3dPresentParameters.BackBufferHeight = 0;
	D3dPresentParameters.BackBufferFormat = D3DFMT_UNKNOWN;
	D3dPresentParameters.BackBufferCount = 1;
	D3dPresentParameters.MultiSampleType = D3DMULTISAMPLE_NONE;
	D3dPresentParameters.MultiSampleQuality = 0;
	D3dPresentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
	D3dPresentParameters.hDeviceWindow = hWnd;
	D3dPresentParameters.Windowed = TRUE;
	D3dPresentParameters.EnableAutoDepthStencil = FALSE;
	D3dPresentParameters.AutoDepthStencilFormat = D3DFMT_A1R5G5B5;
	D3dPresentParameters.Flags = 0;
	D3dPresentParameters.FullScreen_RefreshRateInHz = 0;
	D3dPresentParameters.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
}
