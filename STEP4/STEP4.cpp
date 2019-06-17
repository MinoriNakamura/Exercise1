
#include <windows.h>
#include <d3dx9.h>
#include <d3d9.h>
#include <dinput.h>
//
#pragma comment (lib,"d3d9.lib")
#pragma comment (lib,"d3dx9.lib")
#pragma comment (lib,"dinput8.lib")
#pragma comment (lib,"dxguid.lib")
#pragma comment (lib,"winmm.lib")


LPDIRECT3D9 pD3d;
LPDIRECT3DDEVICE9 pD3Device;
LPDIRECT3DTEXTURE9 pTexture;
LPD3DXFONT m_pFont;//フォント（描画ブラシ）のオブジェクト
LPDIRECTINPUT8 pDinput;
LPDIRECTINPUTDEVICE8 pKeyDevice = NULL;//DirectInputデバイスオブジェクトのポインタ
D3DPRESENT_PARAMETERS D3dPresentParameters;
// fPosX = 270, fPosY = 180;//左上を（０，０）とし、270,180の点に描画



LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);//ウィンドウプロシージャー関数のプロトタイプ宣言
HRESULT BuildDxDevice(HWND, const TCHAR*);//プロトタイプ宣言
void InitPresentParameters(HWND);
HRESULT InitD3d(HWND, const TCHAR*);//Direct3Dの初期化関数のプロトタイプ宣言]
HRESULT InitDinput(HWND hWnd);
VOID FreeDx();//解放するための関数



struct CUSTOMVERTEX {
	float	x, y, z;	// 頂点座標
	float	rhw;	// 除算数
	DWORD	Color;	// 頂点の色
	float	tu, tv;	//　テクスチャ座標
};

#define FVF_CUSTOM ( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 )

CUSTOMVERTEX v[4] =
{
{10, 10, 0.0f, 1.0f, 0xffffffff, 0.0f, 0.0f},
{110, 10, 0.0f, 1.0f, 0xffffffff, 1.0f, 0},
{110, 110, 0.0f, 1.0f, 0x0fffffff, 1.0f, 1.0f},
{10,110,0.0f,1.0f,0xffffffff,0.0f,1.0f}
};

//アプリケーションのエントリー関数
INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR szStr, INT iCmdShow)
{
	DWORD SyncPrev = timeGetTime();
	DWORD SyncCurr;
	/*D3DXCreateTextureFromFile(
		pD3Device,
		"Blank.jpg",
		&dx9.pTexture[_T("test")]);*/
	HWND hWnd = NULL;
	MSG msg;
	//ウィンドウの初期化
	static char szAppName[] = "STEP4";
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
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szAppName;
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	RegisterClassEx(&wndclass);//wndclassのアドレス

	hWnd = CreateWindow(szAppName, szAppName, WS_OVERLAPPEDWINDOW,
		0, 0, 640, 480, NULL, NULL, hInst, NULL);

	ShowWindow(hWnd, SW_SHOW);//表示する
	UpdateWindow(hWnd);

	BuildDxDevice(hWnd, "Blank.jpg");

	D3DXCreateTextureFromFile(
		pD3Device,
		"jump.png",
		&pTexture);
	//ダイレクトインプットの初期化関数
	if (FAILED(InitDinput(hWnd)))
	{
		return 0;
	}

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
			SyncCurr = timeGetTime();
			//1秒間に60回この中に入る
			if (SyncCurr - SyncPrev >= 1000 / 60)
			{
				//ウィンドウを黒色でクリア
				pD3Device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0x00, 0x00, 0x00), 1.0, 0);
				//テクスチャ貼り付け開始
				pD3Device->BeginScene();

				//テクスチャの貼り付け
				HRESULT hr = pKeyDevice->Acquire();
				if ((hr == DI_OK) || (hr == S_FALSE))
				{
					BYTE diks[256];
					pKeyDevice->GetDeviceState(sizeof(diks), &diks);
					if (diks[DIK_LEFT] & 0x80)
					{
						v[0].x -= 4;
						v[1].x -= 4;
						v[2].x -= 4;
						v[3].x -= 4;
					}
					if (diks[DIK_RIGHT] & 0x80)
					{
						v[0].x += 4;
						v[1].x += 4;
						v[2].x += 4;
						v[3].x += 4;
					}
					if (diks[DIK_UP] & 0x80)
					{
						v[0].y -= 4;
						v[1].y -= 4;
						v[2].y -= 4;
						v[3].y -= 4;
					}
					if (diks[DIK_DOWN] & 0x80)
					{
						v[0].y += 4;
						v[1].y += 4;
						v[2].y += 4;
						v[3].y += 4;
					}
				}
				//ピカチュウをかけといわれる
				pD3Device->SetTexture(0, pTexture);
				//いわれた場所に、言われた大きさで書き始める
				pD3Device->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, v, sizeof(CUSTOMVERTEX));

				//テクスチャの貼り付け終了
				//書き終わって、チョークを置く
				pD3Device->EndScene();

				//ウィンドウに表示
				//書いたからみて！！
				pD3Device->Present(0, 0, 0, 0);

				SyncPrev = SyncCurr;//ゲームの処理

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
	pD3Device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
	return S_OK;
}

//ダイレクト３D初期化関数
HRESULT InitD3d(HWND hWnd, const TCHAR * filepath)
{
	//Direct３Dオブジェクトの作成
	if (NULL == (pD3d = Direct3DCreate9(D3D_SDK_VERSION)))
	{
		MessageBox(0, "Direct3Dの作成に失敗しました", "", MB_OK);
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

	//テクスチャオブジェクトの作成
	if (FAILED(D3DXCreateTextureFromFileEx(pD3Device, filepath, 100, 100, 0, 0, D3DFMT_UNKNOWN,
		D3DPOOL_DEFAULT, D3DX_FILTER_NONE, D3DX_DEFAULT,
		0xff000000, NULL, NULL, &pTexture)))
	{
		MessageBox(0, "テクスチャの作成に失敗しました", "", MB_OK);
		return E_FAIL;
	}
	return S_OK;
}
HRESULT InitDinput(HWND hWnd)
{
	HRESULT hr;
	//DirectInputオブジェクトの作成
	if (FAILED(hr = DirectInput8Create(GetModuleHandle(NULL),
		DIRECTINPUT_VERSION, IID_IDirectInput8, (VOID * *)& pDinput, NULL)))
	{
		return hr;
	}
	//Directデバイスオブジェクトの作成
	if (FAILED(hr = pDinput->CreateDevice(GUID_SysKeyboard,
		&pKeyDevice, NULL)))
	{
		return hr;
	}
	//デバイスをキーボードに設定
	if (FAILED(hr = pKeyDevice->SetDataFormat(&c_dfDIKeyboard)))
	{
		return hr;
	}
	//協調レベルの設定
	if (FAILED(hr = pKeyDevice->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_BACKGROUND)))
		//デバイスを取得する
		pKeyDevice->Acquire();
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
VOID FreeDx()
{
	if (pKeyDevice)
	{
		pKeyDevice->Unacquire();
	}
	pKeyDevice->Release();
	pKeyDevice = nullptr;
	pDinput->Release();
	pDinput = nullptr;
	pD3Device->Release();
	pD3Device = nullptr;
	pD3d->Release();
	pD3d = nullptr;
}
