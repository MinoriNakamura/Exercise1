
#include <windows.h>
#include <d3d9.h>

LPDIRECT3D9 pD3d;
LPDIRECT3DDEVICE9 pD3Device;
D3DPRESENT_PARAMETERS D3dPresentParameters;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);//�E�B���h�E�v���V�[�W���[�֐��̃v���g�^�C�v�錾
HRESULT BuildDxDevice(HWND, const TCHAR*);//�v���g�^�C�v�錾
void InitPresentParameters(HWND hWnd);
HRESULT InitD3d(HWND, const TCHAR*);//Direct3D�̏������֐��̃v���g�^�C�v�錾

struct CUSTOMVERTEX {
	float	x, y, z;	// ���_���W
	float	rhw;	// ���Z��
	DWORD	Color;	// ���_�̐F
};

CUSTOMVERTEX v[3] =
{
{10, 10, 0.0f, 1.0f, 0xffff00ff},
{200, 10, 0.0f, 1.0f, 0xffffffff},
{200, 200, 0.0f, 1.0f, 0x0ff00fff}
};

int window_width = 640;
int window_hight = 480;

//�A�v���P�[�V�����̃G���g���[�֐�
INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR szStr, INT iCmdShow)
{
	DWORD SyncPrev = timeGetTime();
	DWORD SyncCurr;
	
	HWND hWnd = NULL;
	MSG msg;
	//�E�B���h�E�̏�����
	static char szAppName[] = "STEP2";
	WNDCLASSEX wndclass;

	wndclass.cbSize = sizeof(wndclass);
	wndclass.style = CS_HREDRAW | CS_VREDRAW;//H | V ��Horizontal���� | Vertical����
	//CS ��class style
	wndclass.lpfnWndProc = WndProc;//���ׂ�
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInst;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szAppName;
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	RegisterClassEx(&wndclass);//wndclass�̃A�h���X

	hWnd = CreateWindow(szAppName, szAppName, WS_OVERLAPPEDWINDOW,
		0, 0, window_width, window_hight, NULL, NULL, hInst, NULL);

	ShowWindow(hWnd, SW_SHOW);//�\������
	UpdateWindow(hWnd);

	BuildDxDevice(hWnd, "Blank.jpg");

		//���b�Z�[�W���[�v
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
			//�Q�[���̏���
			SyncCurr = timeGetTime();
			//1�b�Ԃ�60�񂱂̒��ɓ���
			if (SyncCurr - SyncPrev >= 1000 / 60)
			{
				//�E�B���h�E�����F�ŃN���A
				pD3Device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0x00, 0x00, 0x00), 1.0, 0);
				
				pD3Device->BeginScene();

				//����ꂽ�ꏊ�ɁA����ꂽ�傫���ŏ����n�߂�
				pD3Device->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 1, v, sizeof(CUSTOMVERTEX));

				pD3Device->EndScene();
				
				//�E�B���h�E�ɕ\��
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

	//�A�v���P�[�V�������I������
	return(INT)msg.wParam;
}

//�E�B���h�v���V�[�W���֐�
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
	//return�֐��i�j�G�֐����Ăяo���A���̖߂�l��return����
}

//�f�o�C�X�쐬
HRESULT BuildDxDevice(HWND hWnd, const TCHAR* filepath) {
	if (FAILED(InitD3d(hWnd, filepath))) {
		return E_FAIL;
	}

	pD3d = Direct3DCreate9(D3D_SDK_VERSION);

	if (pD3d == NULL) {
		MessageBox(0, "Direct3D�̍쐬�Ɏ��s���܂���", "", MB_OK);
		return E_FAIL;
	}
	pD3Device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	pD3Device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
	return S_OK;
}

//�_�C���N�g�RD�������֐�
HRESULT InitD3d(HWND hWnd, const TCHAR* filepath)
{
	//Direct�RD�I�u�W�F�N�g�̍쐬
	if (NULL == (pD3d = Direct3DCreate9(D3D_SDK_VERSION)))
	{
		MessageBox(0, "Direct3D�̍쐬�Ɏ��s���܂���","", MB_OK);
		return E_FAIL;
	}

	InitPresentParameters(hWnd);

	if (FAILED(pD3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_MIXED_VERTEXPROCESSING,
		&D3dPresentParameters, &pD3Device/*�_�u���|�C���^*/)))
	{
		MessageBox(0, "HAL���[�h��DIRECT3D�f�o�C�X���쐬�ł��܂���\nREF���[�h���ōĎ��s���܂�", NULL, MB_OK);
		if (FAILED(pD3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, hWnd,
			D3DCREATE_MIXED_VERTEXPROCESSING,
			&D3dPresentParameters, &pD3Device)))
		{
			MessageBox(0, "DIRECT3D�f�o�C�X�̍쐬�Ɏ��s���܂���", NULL, MB_OK);
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
