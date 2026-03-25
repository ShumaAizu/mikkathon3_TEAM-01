//=============================================================================
//
//	Mikkathon [main.cpp]
//	Author : SHUMA AIZU
// 
//=============================================================================

#include "main.h"
#include "input.h"
#include "debugproc.h"
#include "sound.h"
#include "title.h"
#include "game.h"
#include "result.h"
#include "fade.h"
#include "fog.h"
//#include "loadscript.h"
#include "camera.h"
#include "light.h"

#include "field.h"
#include "wall.h"
#include "skybox.h"

#include "modeldata.h"
#include "texture.h"
//#include "effect.h"
//#include "particle.h"
//#include "motion.h"
//#include "collision.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define CLASS_NAME			"WindowClass"					// ウインドウクラスの名前
#define WINDOW_NAME			"Mikkathon"						// ウインドウの名前

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
void Uninit(void);
void Update(void);
void Draw(void);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
LPDIRECT3D9 g_pD3D = NULL;					// Direct3Dオブジェクトへのポインタ
LPDIRECT3DDEVICE9 g_pD3DDevice = NULL;		// Direct3Dデバイスへのポインタ
MODE g_mode = MODE_TITLE;					// モード情報
int g_nCountFPS = 0;						// FPSカウンタ
int g_nFPS = 60;							// 現在の最大FPS
bool g_isFullscreen = false;				// フルスクリーンの使用状況
RECT g_windowRect;							// ウィンドウサイズ
bool g_bDebug = false;						// デバッグ表示の状態

//=============================================================================
// メイン関数
//=============================================================================
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hInstancePrev, LPSTR lpCmdLine, int nCmdShow)
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF);			// メモリリーク検知用のフラグ
#endif // _DEBUG

	WNDCLASSEX wcex =									// ウインドウクラスの構造体
	{
		sizeof(WNDCLASSEX),								// WNDCLASSEXのメモリサイズ
		CS_CLASSDC,										// ウインドウのスタイル
		WindowProc,										// ウインドウプロシージャ
		0,												// 0にする(通常は使用しない)
		0,												// 0にする(通常は使用しない)
		hInstance,										// インスタンスハンドル
		LoadIcon(NULL, IDI_APPLICATION),				// タスクバーのアイコン
		LoadCursor(NULL, IDC_ARROW),					// マウスカーソル
		(HBRUSH)(COLOR_WINDOW + 1),						// クライアント領域の背景色
		NULL,											// メニューバー
		CLASS_NAME,										// ウインドウクラスの名前
		LoadIcon(NULL, IDI_APPLICATION)					// ファイルのアイコン
	};

	HWND hWnd;				// ウインドウハンドル(識別子)
	MSG msg;				// メッセージを格納する変数
	DWORD dwCurrentTime;	// 現在時刻
	DWORD dwExecLastTime;	// 最後に処理した時刻
	DWORD dwFrameCount;		// フレームカウント
	DWORD dwFPSLastTime;	// 最後にFPSを計測した時刻
	g_nFPS = 60;

	RECT rect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };	// 画面サイズの構造体

	// ウインドウクラスの登録
	RegisterClassEx(&wcex);

	// クライアント領域を指定のサイズに調整
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

	// ウインドウを生成
	hWnd = CreateWindowEx(0,							// 拡張ウインドウスタイル
						  CLASS_NAME,					// ウインドウクラスの名前
						  WINDOW_NAME,					// ウインドウの名前
						  WS_OVERLAPPEDWINDOW,			// ウインドウスタイル
						  CW_USEDEFAULT,				// ウインドウの左上X座標
						  CW_USEDEFAULT,				// ウインドウの左上Y座標
						  (rect.right - rect.left),		// ウインドウの幅
						  (rect.bottom - rect.top),		// ウインドウの高さ
						  NULL,							// 親ウインドウのハンドル
						  NULL,							// メニューハンドルまたは子ウインドウID
						  hInstance,					// インスタンスハンドル
						  NULL);						// ウインドウ作成データ

	// 初期化処理
	if (FAILED(Init(hInstance, hWnd, TRUE)))
	{// 初期化処理が失敗した場合
		return - 1;
	}

	// 分解能を設定
	timeBeginPeriod(1);
	dwCurrentTime = 0;						// 初期化する
	dwExecLastTime = timeGetTime();			// 現在時刻を取得(保存)

	dwFrameCount = 0;						// 初期化する
	dwFPSLastTime = timeGetTime();			// 現在時刻を取得(保存)

	// ウインドウを表示
	ShowWindow(hWnd, nCmdShow);				// ウインドウの表示状態を設定
	UpdateWindow(hWnd);						// クライアント領域を更新

	// メッセージループ
	while (1)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) != 0)
		{// windowsの処理
			if (msg.message == WM_QUIT)
			{// WM_QUITメッセージを受け取ったらループを抜ける
				break;
			}
			else
			{
				// メッセージの設定
				TranslateMessage(&msg);				// 仮想メッセージを文字メッセージへ変換
				DispatchMessage(&msg);				// ウインドウプロシージャへメッセージを送出
			}
		}
		else
		{// DirectXの処理
			dwCurrentTime = timeGetTime();		// 現在時刻を取得

			if ((dwCurrentTime - dwFPSLastTime) >= 500)
			{// 0.5秒経過
				// FPSを計測
				g_nCountFPS = (dwFrameCount * 1000) / (dwCurrentTime - dwFPSLastTime);

				dwFPSLastTime = dwCurrentTime;	// FPSを計測した時間を保存
				dwFrameCount = 0;				// フレームカウントをクリア
			}

			if ((dwCurrentTime - dwExecLastTime) >= (1000 / g_nFPS))
			{// 60分の1秒経過
				dwExecLastTime = dwCurrentTime;	// 処理開始の時刻[現在時刻]を保存

				// 更新処理
				Update();

				// 描画処理
				Draw();

				dwFrameCount++;					// フレームカウントを加算
			}
		}
	}

	// 終了処理
	Uninit();

	// ウインドウクラスの登録を解除
	UnregisterClass(CLASS_NAME, wcex.hInstance);

	return (int)msg.wParam;
}

//===============================================================================
// ウインドウプロシージャ
//===============================================================================
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

	int nID;

	switch (uMsg)
	{
	case WM_DESTROY:
		// WM_QUITメッセージを送る
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:				// キー押下のメッセージ
		switch (wParam)
		{
		case VK_ESCAPE:				// [ESC]が押された
			nID = MessageBox(hWnd, "終了しますか？", "終了メッセージ", MB_YESNO);
			if (nID == IDYES)
			{
				// ウインドウを破棄する
				DestroyWindow(hWnd);
			}
			break;

		case VK_F11:
			ToggleFullscreen(hWnd);// F11でフルスクリーン
			break;
		}
		break;

	case WM_CLOSE:								// 閉じるボタン押下のメッセージ
		nID = MessageBox(hWnd, "終了しますか？", "終了メッセージ", MB_YESNO);
		if (nID == IDYES)
		{
			// ウインドウを破棄する
			DestroyWindow(hWnd);
		}
		else
		{
			return 0;							// 0を返さないと終了してしまう
		}
		break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

//===============================================================================
// 初期化処理
//===============================================================================
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
	D3DDISPLAYMODE d3ddm;			// ディスプレイモード
	D3DPRESENT_PARAMETERS d3dpp;	// プレゼンテーションパラメータ

	// DirectXオブジェクトの生成
	g_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if (g_pD3D == NULL)
	{
		return E_FAIL;
	}
	
	// 現在のディスプレイモードを取得
	if (FAILED(g_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm)))
	{
		return E_FAIL;
	}

	// デバイスのプレゼンテーションパラメータの設定
	ZeroMemory(&d3dpp, sizeof(d3dpp));								// パラメータのゼロクリア

	d3dpp.BackBufferWidth = SCREEN_WIDTH;							// ゲーム画面サイズ(幅)

	d3dpp.BackBufferHeight = SCREEN_HEIGHT;							// ゲーム画面サイズ(高さ)

	d3dpp.BackBufferFormat = d3ddm.Format;							// バックバッファの形成

	d3dpp.BackBufferCount = 1;										// バックバッファの数

	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;						// ダブルバッファの切り替え(映像信号に同期)

	d3dpp.EnableAutoDepthStencil = TRUE;							// デブスバッファとステンシルバッファを作成

	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;						// デブスバッファとして16bitを使う

	d3dpp.Windowed = bWindow;										// ウインドウモード

	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;		// リフレッシュレート

	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;		// インターバル

	// Direct3Dデバイスの生成
	// 描画処理と頂点処理をハードウェアで行う
	if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&d3dpp,
		&g_pD3DDevice)))
	{
		// 頂点処理のみCPUで行う
		if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			hWnd,
			D3DCREATE_SOFTWARE_VERTEXPROCESSING,
			&d3dpp,
			&g_pD3DDevice)))
		{
			// 両方CPUで行う
			if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
				D3DDEVTYPE_REF,
				hWnd,
				D3DCREATE_SOFTWARE_VERTEXPROCESSING,
				&d3dpp,
				&g_pD3DDevice)))
			{
				return E_FAIL;
			}
		}
	}

	// レンダーステートの設定
	g_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);				// カリングの設定
	g_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);				//
	g_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);		// アルファブレンドの設定
	g_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);	//

	// サンプラーステートの設定
	g_pD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);	// テクスチャの拡縮補間の設定
	g_pD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

	g_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);	// テクスチャの繰り返し設定
	g_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

	// テクスチャステージステートの設定
	g_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	g_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	g_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);

	// 乱数シードセット
	srand((unsigned)timeGetTime());

	g_bDebug = false;
	
	// キーボードの初期化処理
	if (FAILED(InitKeyboard(hInstance, hWnd)))
	{
		return E_FAIL;
	}

	// ジョイパッドの初期化処理
	if(FAILED(InitJoypad()))
	{
		return E_FAIL;
	}

	// マウスの初期化処理
	if (FAILED(InitMouse(hWnd)))
	{
		return E_FAIL;
	}

	// カメラの初期化
	InitCamera();

	// ライトの初期化
	InitLight();

	// サウンドの初期化処理
	InitSound(hWnd);

	// テクスチャの初期化
	InitTexture();

	// モーションの初期化処理
	//InitMotion();

	// モードの設定
	InitFade(g_mode, COLOR_WHITE);

	// デバッグ表示の初期化処理
	InitDebugProc();

	// フィールドの初期化
	InitField();

	// 壁の初期化
	InitWall();

	// 空の初期化
	InitSkyBox();
	
	// エフェクトの初期化処理
	//InitEffect();

	// パーティクルの初期化処理
	//InitParticle();

	// 当たり判定の初期化処理
	//InitCollision();

#ifndef _DEBUG
	// マウスカーソルを非表示
	ShowCursor(false);
#endif

	return S_OK;
}

//===============================================================================
// 終了処理
//===============================================================================
void Uninit(void)
{
	// キーボードの終了処理
	UninitKeyboard();

	// ジョイパッドの終了処理
	UninitJoypad();

	// マウスの終了処理
	UninitMouse();

	// デバッグ表示の終了処理
	UninitDebugProc();

	// カメラの終了処理
	UninitCamera();

	// ライトの終了処理
	UninitLight();

	// タイトル画面の終了処理
	UninitTitle();

	// ゲーム画面の終了処理
	UninitGame();

	// リザルト画面の終了処理
	UninitResult();

	// フェードの終了処理
	UninitFade();

	// フィールドの終了
	UninitField();

	// 壁の終了
	UninitWall();

	// 空の終了
	UninitSkyBox();

	// エフェクトの終了処理
	//UninitEffect();

	// パーティクルの終了処理
	//UninitParticle();

	// サウンドを止める
	StopSound();

	// サウンドの終了処理
	UninitSound();

	// テクスチャの終了
	UninitTexture();

	// 当たり判定の終了処理
	//UninitCollision();

	// Direct3Dデバイスの破棄
	if (g_pD3DDevice != NULL)
	{
		g_pD3DDevice->Release();
		g_pD3DDevice = NULL;
	}

	// Direct3Dオブジェクトの破棄
	if (g_pD3D != NULL)
	{
		g_pD3D->Release();
		g_pD3D = NULL;
	}
}

//===============================================================================
// 更新処理
//===============================================================================
void Update(void)
{

#ifdef _DEBUG
	if (GetKeyboardTrigger(DIK_F5) == true)
	{
		g_bDebug = g_bDebug ? false : true;
	}
#endif

	PrintDebugProc("FPS : %d\n", g_nCountFPS);

	// キーボードの更新処理
	UpdateKeyboard();

	// ジョイパッドの更新処理
	UpdateJoypad();

	// マウスの更新処理
	UpdateMouse();

	// デバッグ表示の更新処理
	UpdateDebugProc();

	// カメラの更新処理
	UpdateCamera();

	switch (g_mode)
	{
		// タイトルモード
	case MODE_TITLE:
		UpdateTitle();
		break;

		// ゲームモード
	case MODE_GAME:
		UpdateGame();
		break;

		// リザルトモード
	case MODE_RESULT:
		UpdateResult();
		break;
	}

	// フェードの更新処理
	UpdateFade();

	// エフェクトの更新処理
	//UpdateEffect();

	// パーティクルの更新処理
	//UpdateParticle();
}

//===============================================================================
// 描画処理
//===============================================================================
void Draw(void)
{
	D3DVIEWPORT9 viewportDef;	// ビューポート保管変数
	// 画面クリア(バックバッファとZバッファのクリア)
	g_pD3DDevice->Clear(0, NULL,
		(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER),
		D3DCOLOR_RGBA(0, 0, 0, 0), 1.0f, 0);

	// 描画開始
	if (SUCCEEDED(g_pD3DDevice->BeginScene()))
	{// 描画開始が成功した場合
		//for (int nCntCamera = 0; nCntCamera < GetNumCamera(); nCntCamera++)
		//{
			// 現在のビューポートの取得
			g_pD3DDevice->GetViewport(&viewportDef);

			// カメラの設定処理
			SetCamera();

			switch (g_mode)
			{
				// タイトルモード
			case MODE_TITLE:
				DrawTitle();
				break;

				// ゲームモード
			case MODE_GAME:
				DrawGame();
				break;

				// リザルトモード
			case MODE_RESULT:
				DrawResult();
				break;
			}

			// ビューポートを元に戻す
			g_pD3DDevice->SetViewport(&viewportDef);
		//}

		// フェードの描画処理
		DrawFade();

#ifdef _DEBUG

		// デバッグ表示の描画処理
		DrawDebugProc();

#endif // _DEBUG

		// 描画終了
		g_pD3DDevice->EndScene();
	}


	// バックバッファとフロントバッファの入れ替え
	g_pD3DDevice->Present(NULL, NULL, NULL, NULL);
}

//==================================================================================
//	デバイスの取得
//==================================================================================
LPDIRECT3DDEVICE9 GetDevice(void)
{
	return g_pD3DDevice;
}

//==================================================================================
//	モードの設定
//==================================================================================
void SetMode(MODE mode)
{
	// 現在のモードの終了処理
	switch (g_mode)
	{
		// タイトルモード
	case MODE_TITLE:
		UninitTitle();
		break;

		// ゲームモード
	case MODE_GAME:
		UninitGame();
		break;

		// リザルトモード
	case MODE_RESULT:
		UninitResult();
		break;
	}

	// 新しいモードの初期化処理
	switch (mode)
	{
		// タイトルモード
	case MODE_TITLE:
		InitTitle();
		break;

		// ゲームモード
	case MODE_GAME:
		InitGame();
		break;

		// リザルトモード
	case MODE_RESULT:
		InitResult();
		break;
	}

	// カメラをモードによって切り替える
	//SetModeCamera(mode);

	// モードの更新
	g_mode = mode;
}

//==================================================================================
//	モードの取得
//==================================================================================
MODE GetMode(void)
{
	return g_mode;		// 現在のモードを返す
}

//==================================================================================
//	角度の正規化
//==================================================================================
float AngleNormalize(float fAngle)
{
	if (fAngle < -D3DX_PI)
	{
		fAngle += D3DX_PI * 2;
	}
	else if (fAngle > D3DX_PI)
	{
		fAngle -= D3DX_PI * 2;
	}

	return fAngle;
}

//==================================================================================
//	度数法から弧度法への変換
//==================================================================================
float DegreeToRadian(float fDegree)
{
	return fDegree * D3DX_PI / 180;
}

//==================================================================================
//	弧度法から度数法への変換
//==================================================================================
int RadianToDegree(float fRadian)
{
	return fRadian * 180 / D3DX_PI;
}

//==================================================================================
//	外積の当たり判定 (引数で判定だけか戻すか選択)
//==================================================================================
bool CrossCollision(D3DXVECTOR3* pPos, D3DXVECTOR3* pPosOld, D3DXVECTOR3 posStart, D3DXVECTOR3 posEnd, bool bRetrun, bool bIn)
{
	D3DXVECTOR3 vecLine, vecToPos, vecToPosOld, vecMove, vecNor;	// 各種ベクトル
	D3DXVECTOR3 Intersection = {};									// 交点
	float fRate;													// 比率
	float fAngleVecLine;											// 境界線ベクトルの角度
	float fLenght = 0.0f;											// 壁ずりの長さ
	float fPosOldCross = 0.0f;
	bool bCollision = false;

	if (bIn == true)
	{
		vecLine = posStart - posEnd;		// 境界線ベクトル
		vecToPos = *pPos - posEnd;			// 
		vecToPosOld = *pPosOld - posEnd;	// 
	}
	else
	{
		vecLine = posEnd - posStart;		// 境界線ベクトル
		vecToPos = *pPos - posStart;		// 
		vecToPosOld = *pPosOld - posStart;	// 
	}

	vecMove = *pPos - *pPosOld;			// 移動ベクトル

	fAngleVecLine = atan2f(vecLine.x, vecLine.z);	// 境界線ベクトルの角度

	// 法線ベクトルを算出
	vecNor = D3DXVECTOR3(sinf((0.5f * D3DX_PI) + fAngleVecLine), 0.0f, cosf((0.5f * D3DX_PI) + fAngleVecLine));

	// 比率を算出
	fRate = ((vecToPos.z * vecMove.x) - (vecToPos.x * vecMove.z)) / ((vecLine.z * vecMove.x) - (vecLine.x * vecMove.z));

	// 壁から過去の位置へのベクトル
	fPosOldCross = (vecLine.z * vecToPosOld.x) - (vecLine.x * vecToPosOld.z);

	if (fPosOldCross < -0.0000001f && fPosOldCross > -1.0f)
	{// 誤差は切り捨て
		fPosOldCross = ceilf(fPosOldCross);		// 負の数は切り上げ
	}

	if ((vecLine.z * vecToPos.x) - (vecLine.x * vecToPos.z) < 0.0f &&
		fPosOldCross >= 0.0f &&
		fRate <= 1.0f && fRate >= 0.0f)
	{
		if (bRetrun == false)
		{
			bCollision = true;
			return bCollision;
		}

		Intersection = D3DXVECTOR3(vecLine.x * fRate, vecLine.y * fRate, vecLine.z * fRate);		// 交点

		D3DXVec3Normalize(&vecLine, &vecLine);		// 正規化

		// 交点を代入
		if (bIn == true)
		{
			pPos->x = posEnd.x + Intersection.x;
			pPos->z = posEnd.z + Intersection.z;
		}
		else
		{
			pPos->x = posStart.x + Intersection.x;
			pPos->z = posStart.z + Intersection.z;
		}

		fLenght = (vecNor.z * vecMove.x) - (vecNor.x * vecMove.z);		// 壁ずりの長さ算出

#if 0
		fLenght = (vecLine.x * vecMove.x) + (vecLine.z * vecMove.z);	// これ結果同じらしい(上が外積で下が内積)
#endif

		fLenght = fabsf(fLenght);		// 絶対値化

		if ((vecNor.z * vecMove.x) - (vecNor.x * vecMove.z) < 0)
		{// 外側なら加算
			pPos->x += vecLine.x * fLenght;
			pPos->z += vecLine.z * fLenght;
		}
		else if ((vecNor.z * vecMove.x) - (vecNor.x * vecMove.z) > 0)
		{// 内側なら減算
			pPos->x += -vecLine.x * fLenght;
			pPos->z += -vecLine.z * fLenght;
		}

		bCollision = true;
		return bCollision;
	}

	return bCollision;
}

//==================================================================================
//	モデルの描画処理 (階層構造)
//==================================================================================
#if 0
void DrawParentModel(D3DXVECTOR3 *pPos, D3DXVECTOR3 *pRot, D3DXMATRIX *pMtxWorld, ModelData *pModelData, OffSetData *pOffSetData)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// デバイスの取得
	D3DXMATRIX mtxRot, mtxTrans;				// 計算用マトリックス
	D3DMATERIAL9 matDef;						// 現在のマテリアル保存用
	D3DXMATERIAL* pMat;							// マテリアルデータへのポインタ

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(pMtxWorld);

	// 向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, pRot->y, pRot->x, pRot->z);
	D3DXMatrixMultiply(pMtxWorld, pMtxWorld, &mtxRot);

	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans, pPos->x, pPos->y, pPos->z);
	D3DXMatrixMultiply(pMtxWorld, pMtxWorld, &mtxTrans);

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, pMtxWorld);

	// 現在のマテリアルを取得
	pDevice->GetMaterial(&matDef);

	// 全モデル(パーツ)の描画
	for (int nCntParentModel = 0; nCntParentModel < pModelData->nNumParts; nCntParentModel++)
	{
		D3DXMATRIX mtxRotOffSetModel, mtxTransOffSetModel;	// 計算用マトリックス
		D3DXMATRIX mtxParent;								// 親のマトリックス
		D3DXVECTOR3 pos = pOffSetData->pos[nCntParentModel];
		D3DXVECTOR3 rot = pOffSetData->rot[nCntParentModel];

		// パーツのワールドマトリックスを初期化
		D3DXMatrixIdentity(&pModelData->aModel[nCntParentModel].mtxWorld);

		// パーツの向きを反映
		D3DXMatrixRotationYawPitchRoll(&mtxRotOffSetModel, rot.y, rot.x, rot.z);
		D3DXMatrixMultiply(&pModelData->aModel[nCntParentModel].mtxWorld, &pModelData->aModel[nCntParentModel].mtxWorld, &mtxRotOffSetModel);

		// パーツの位置を反映(オフセット)
		D3DXMatrixTranslation(&mtxTransOffSetModel, pos.x, pos.y, pos.z);
		D3DXMatrixMultiply(&pModelData->aModel[nCntParentModel].mtxWorld, &pModelData->aModel[nCntParentModel].mtxWorld, &mtxTransOffSetModel);

		// パーツの「親のマトリックス」を設定
		if (pModelData->aModel[nCntParentModel].nIdxModelParent != -1)
		{// 親モデルがある場合
			mtxParent = pModelData->aModel[pModelData->aModel[nCntParentModel].nIdxModelParent].mtxWorld;
		}
		else
		{// 親モデルがない場合
			mtxParent = *pMtxWorld;
		}

		// 算出した「パーツのワールドマトリックス」と「親のマトリックス」を掛け合わせる
		D3DXMatrixMultiply(&pModelData->aModel[nCntParentModel].mtxWorld,
			&pModelData->aModel[nCntParentModel].mtxWorld,
			&mtxParent);

		// パーツのワールドマトリックスを設定
		pDevice->SetTransform(D3DTS_WORLD, &pModelData->aModel[nCntParentModel].mtxWorld);

		// マテリアルデータへのポインタを取得
		pMat = (D3DXMATERIAL*)pModelData->aModel[pModelData->aModel[nCntParentModel].nIdxModel].pBuffMat->GetBufferPointer();


		for (int nCntMat = 0; nCntMat < (int)pModelData->aModel[pModelData->aModel[nCntParentModel].nIdxModel].dwNumMat; nCntMat++)
		{
			// マテリアルの設定
			pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

			// テクスチャの設定
			pDevice->SetTexture(0, pModelData->aModel[pModelData->aModel[nCntParentModel].nIdxModel].apTexture[nCntMat]);

			// パーツの描画
			pModelData->aModel[pModelData->aModel[nCntParentModel].nIdxModel].pMesh->DrawSubset(nCntMat);
		}
	}

	// 保存していたマテリアルを戻す
	pDevice->SetMaterial(&matDef);
}
#endif

//================================================
// ウィンドウフルスクリーン処理
//================================================
void ToggleFullscreen(HWND hWnd)
{
	// 現在のウィンドウスタイルを取得
	DWORD dwStyle = GetWindowLong(hWnd, GWL_STYLE);
	if (g_isFullscreen)
	{
		// ウィンドウモードに切り替え
		SetWindowLong(hWnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPos(hWnd, HWND_TOP, g_windowRect.left, g_windowRect.top,
			g_windowRect.right - g_windowRect.left, g_windowRect.bottom - g_windowRect.top,
			SWP_FRAMECHANGED | SWP_NOACTIVATE);
		ShowWindow(hWnd, SW_NORMAL);
	}
	else
	{
		// フルスクリーンモードに切り替え
		GetWindowRect(hWnd, &g_windowRect);
		SetWindowLong(hWnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
		SetWindowPos(hWnd, HWND_TOP, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
			SWP_FRAMECHANGED | SWP_NOACTIVATE);
		ShowWindow(hWnd, SW_MAXIMIZE);
	}

	g_isFullscreen = !g_isFullscreen;
}

//==================================================================================
//	テクスチャステージステートのアルファの設定
//==================================================================================
void SetTextureStageStateAlpha(int nIdx, DWORD Stage, D3DTEXTUREOP Type, DWORD Value)
{
	// テクスチャステージステートの設定
	g_pD3DDevice->SetTextureStageState(nIdx, D3DTSS_ALPHAARG1, Stage);
	g_pD3DDevice->SetTextureStageState(nIdx, D3DTSS_ALPHAOP, Type);
	g_pD3DDevice->SetTextureStageState(nIdx, D3DTSS_ALPHAARG2, Value);
}

//==================================================================================
//	テクスチャステージステートのアルファのリセット
//==================================================================================
void ResetTextureStageStateAlpha(int nNumIdx)
{
	// テクスチャステージステートの設定
	g_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	g_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	g_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);

	for (int nCntIdx = 1; nCntIdx < nNumIdx; nCntIdx++)
	{
		g_pD3DDevice->SetTextureStageState(nCntIdx, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		g_pD3DDevice->SetTextureStageState(nCntIdx, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
		g_pD3DDevice->SetTextureStageState(nCntIdx, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
	}
}

//==================================================================================
//	テクスチャステージステートのカラーの設定
//  nIdx  : 設定するテクスチャステージのインデックス
//  Stage : 合成に使用する色 (何で)
//  Type  : どういう合成方法か
//  Value : 合成に使用する色 (何に)
//==================================================================================
void SetTextureStageStateColor(int nIdx, DWORD Stage, D3DTEXTUREOP Type, DWORD Value)
{
	// テクスチャステージステートの設定
	g_pD3DDevice->SetTextureStageState(nIdx, D3DTSS_COLORARG1, Stage);
	g_pD3DDevice->SetTextureStageState(nIdx, D3DTSS_COLOROP, Type);
	g_pD3DDevice->SetTextureStageState(nIdx, D3DTSS_COLORARG2, Value);
}

//==================================================================================
//	テクスチャステージステートのカラーのリセット
//==================================================================================
void ResetTextureStageStateColor(int nNumIdx)
{
	// テクスチャステージステートの設定
	g_pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	g_pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	g_pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);

	for (int nCntIdx = 1; nCntIdx < nNumIdx; nCntIdx++)
	{
		g_pD3DDevice->SetTextureStageState(nCntIdx, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		g_pD3DDevice->SetTextureStageState(nCntIdx, D3DTSS_COLOROP, D3DTOP_DISABLE);
		g_pD3DDevice->SetTextureStageState(nCntIdx, D3DTSS_COLORARG2, D3DTA_CURRENT);
	}
}

//==================================================================================
//	処理速度測定処理
//==================================================================================
void ProcessingSpeed(bool isStart, const char* ptext)
{
	static DWORD dwCurrentTime;		// 現在時刻
	static DWORD dwExecLastTime;	// 最後に処理した時刻

	using namespace std;

	static chrono::high_resolution_clock::time_point start, end;

	if (isStart)
	{
		dwExecLastTime = timeGetTime();
		start = chrono::high_resolution_clock::now();
	}
	else
	{
		dwCurrentTime = timeGetTime();
		end = chrono::high_resolution_clock::now();

		double time = static_cast<double>(chrono::duration_cast<chrono::microseconds>(end - start).count() / 1000.0f);

		char alog[256];
		sprintf_s(alog, "%s : %lfms\n", ptext, time);

#ifdef _DEBUG
		PrintDebugProc("%s : %lfms\n", ptext, time);
#endif
		OutputDebugStringA(alog);
	}
}