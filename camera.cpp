 //=============================================================================
//
//	カメラ処理 [camera.cpp]
//	Author : SHUMA AIZU
// 
//=============================================================================

#include "camera.h"
#include "input.h"
#include "debugproc.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define CAMERA_MOVE					(0.0075f)	// カメラの移動速度
#define CAMERAROT_RESTRICTION		(0.125f)	// カメラの角度制限

//*****************************************************************************
// グローバル変数
//*****************************************************************************
Camera g_acamera[MAX_CAMERA];	// カメラの情報
int g_nNumCamera = 0;			// カメラの数

// カメラの情報格納用
CameraInfo g_acameraInfo[] =
{
	{INIT_1PCAMERAPOSV, INIT_1PCAMERAPOSR, INIT_VECU, INIT_1PCAMERAROT, INIT_1PVEIWPORT },
	{INIT_2PCAMERAPOSV, INIT_2PCAMERAPOSR, INIT_VECU, INIT_2PCAMERAROT, INIT_2PVEIWPORT },
	{INIT_1PCAMERAPOSV, INIT_1PCAMERAPOSR, INIT_VECU, INIT_1PCAMERAROT, INIT_3PVEIWPORT },
	{INIT_1PCAMERAPOSV, INIT_1PCAMERAPOSR, INIT_VECU, INIT_1PCAMERAROT, INIT_4PVEIWPORT },
};

//*****************************************************************************
// 関数ポインタ配列の定義
//*****************************************************************************
void (*UpdateModeCamera[MODE_MAX])(void) =
{
	UpdateTitleCamera,
	UpdateGameCamera,
	UpdateResultCamera,
};

//=============================================================================
//	カメラの初期化処理
//=============================================================================
void InitCamera(void)
{
	Camera* pCamera = &g_acamera[0];
	CameraInfo* pCameraInfo = &g_acameraInfo[0];

	// 初期化
	memset(pCamera, NULL, sizeof(Camera) * MAX_CAMERA);

	for (int nCntCamera = 0; nCntCamera < MAX_CAMERA; nCntCamera++, pCamera++, pCameraInfo++)
	{// カメラに予め用意した初期値を代入
		// カメラの位置設定
		pCamera->posV = pCameraInfo->posV;
		pCamera->posR = pCameraInfo->posR;
		pCamera->vecU = pCameraInfo->vecU;
		pCamera->rot = pCameraInfo->rot;
		pCamera->fSpeed = INIT_CAMERASPEED;
		pCamera->fRadiusVertical = INIT_RADIUS;

		pCamera->viewport = DEFAULT_VEIWPORT;		// ビューポート設定

		// 目的位置
		pCamera->posVDest = pCameraInfo->posV;
		pCamera->posRDest = pCameraInfo->posR;

		// 距離を離す
		pCamera->posVDest.z = pCamera->posRDest.z + cosf(pCamera->rot.y) * CAMERAPOSR_DIS;
		pCamera->posVDest.x = pCamera->posRDest.x + sinf(pCamera->rot.y) * CAMERAPOSR_DIS;
	}

	g_nNumCamera = INIT_NUMCAMERA;
}

//=============================================================================
//	カメラの終了処理
//=============================================================================
void UninitCamera(void)
{

}

//=============================================================================
//	カメラの更新処理
//=============================================================================
void UpdateCamera(MODE mode)
{
	Camera* pCamera = &g_acamera[0];

	if (GetKeyboardPress(DIK_W) == true)
	{
		pCamera->posRDest.x += sinf(pCamera->rot.y) * -5.0f;
		pCamera->posRDest.z += cosf(pCamera->rot.y) * -5.0f;
	}

	if (GetKeyboardPress(DIK_A) == true)
	{
		pCamera->posRDest.x += sinf(pCamera->rot.y + (D3DX_PI * -0.5f)) * -5.0f;
		pCamera->posRDest.z += cosf(pCamera->rot.y + (D3DX_PI * -0.5f)) * -5.0f;
	}

	if (GetKeyboardPress(DIK_S) == true)
	{
		pCamera->posRDest.x += sinf(pCamera->rot.y + (D3DX_PI)) * -5.0f;
		pCamera->posRDest.z += cosf(pCamera->rot.y + (D3DX_PI)) * -5.0f;
	}

	if (GetKeyboardPress(DIK_D) == true)
	{
		pCamera->posRDest.x += sinf(pCamera->rot.y + (D3DX_PI * 0.5f)) * -5.0f;
		pCamera->posRDest.z += cosf(pCamera->rot.y + (D3DX_PI * 0.5f)) * -5.0f;
	}

	if (GetKeyboardPress(DIK_Q) == true)
	{
		pCamera->posRDest.y += 1.0f;
	}

	if (GetKeyboardPress(DIK_E) == true)
	{
		pCamera->posRDest.y += -1.0f;
	}

	if (GetKeyboardPress(DIK_I) == true)
	{
		pCamera->posVDest.y += 1.0f;
	}

	if (GetKeyboardPress(DIK_K) == true)
	{
		pCamera->posVDest.y += -1.0f;
	}

	if (GetKeyboardPress(DIK_Z) == true)
	{
		pCamera->rot.y += CAMERA_MOVE;
	}

	if (GetKeyboardPress(DIK_C) == true)
	{
		pCamera->rot.y += -CAMERA_MOVE;
	}

	// 目的視点を目的注視点から離す
	pCamera->posVDest.x = pCamera->posRDest.x + sinf(pCamera->rot.y) * pCamera->fRadiusHorizonttal;
	pCamera->posVDest.y = pCamera->posRDest.y + sinf(pCamera->rot.x) * pCamera->fRadiusVertical;
	pCamera->posVDest.z = pCamera->posRDest.z + cosf(pCamera->rot.y) * pCamera->fRadiusHorizonttal;

	// 注視点を目的注視点に移動
	pCamera->posR.x += (pCamera->posRDest.x - pCamera->posR.x) * CAMERA_INERTIA;
	pCamera->posR.y += (pCamera->posRDest.y - pCamera->posR.y) * CAMERA_INERTIA;
	pCamera->posR.z += (pCamera->posRDest.z - pCamera->posR.z) * CAMERA_INERTIA;

	// 視点を目的視点に移動
	pCamera->posV.x += (pCamera->posVDest.x - pCamera->posV.x) * CAMERA_INERTIA;
	pCamera->posV.y += (pCamera->posVDest.y - pCamera->posV.y) * CAMERA_INERTIA;
	pCamera->posV.z += (pCamera->posVDest.z - pCamera->posV.z) * CAMERA_INERTIA;

	PrintDebugProc("視点 = { %.2f %.2f %.2f }\n", pCamera->posV.x, pCamera->posV.y, pCamera->posV.z);
	PrintDebugProc("注視点 = { %.2f %.2f %.2f }\n", pCamera->posR.x, pCamera->posR.y, pCamera->posR.z);
	PrintDebugProc("カメラの向き = { %.2f %.2f %.2f }\n", pCamera->rot.x, pCamera->rot.y, pCamera->rot.z);

#if 0
	// モードに合わせたカメラ更新処理
	if (UpdateModeCamera[mode] != NULL)
	{// 関数が存在したら
		UpdateModeCamera[mode]();
	}
#endif

}

//=============================================================================
//	カメラの更新処理 (タイトル)
//=============================================================================
void UpdateTitleCamera(void)
{

}

//=============================================================================
//	カメラの更新処理 (ゲーム)
//=============================================================================
void UpdateGameCamera(void)
{

}

//=============================================================================
//	カメラの更新処理 (リザルト)
//=============================================================================
void UpdateResultCamera(void)
{

}

//=============================================================================
//	カメラの設定処理 [カメラの番号]
//=============================================================================
void SetCamera(int nIdx)
{
	Camera* pCamera = &g_acamera[nIdx];		// アドレス

	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	// ビューポートの設定
	pDevice->SetViewport(&pCamera->viewport);

	// プロジェクションマトリックスを初期化
	D3DXMatrixIdentity(&pCamera->mtxProjection);

	// プロジェクションマトリックスを作成
	D3DXMatrixPerspectiveFovLH(&pCamera->mtxProjection,
							D3DXToRadian(45.0f),
							(float)pCamera->viewport.Width / (float)pCamera->viewport.Height,
							10.0f,
							15000.0f);

	// プロジェクションマトリックスの設定
	pDevice->SetTransform(D3DTS_PROJECTION, &pCamera->mtxProjection);

	// ビューマトリックスを初期化
	D3DXMatrixIdentity(&pCamera->mtxView);

	// ビューマトリックスを作成
	D3DXMatrixLookAtLH(&pCamera->mtxView,
					   &pCamera->posV,
					   &pCamera->posR,
					   &pCamera->vecU);
						
	// ビューマトリックスの設定
	pDevice->SetTransform(D3DTS_VIEW, &pCamera->mtxView);
}

//=============================================================================
//	カメラの取得処理
//=============================================================================
Camera *GetCamera(void)
{
	return &g_acamera[0];
}

//=============================================================================
//	モードごとのカメラ設定処理
//=============================================================================
#if 0
void SetModeCamera(MODE mode)
{
	Camera* pCamera = &g_acamera[0];
	CameraInfo* pCameraInfo = &g_acameraInfo[0];

	switch (mode)
	{
	case MODE_START:
		pCamera->posV		= INIT_LOGOCAMERA_POSV;
		pCamera->posVDest	= INIT_LOGOCAMERA_POSV;
		pCamera->posR		= INIT_LOGOCAMERA_POSR;
		pCamera->posRDest	= INIT_LOGOCAMERA_POSR;
		pCamera->vecU		= INIT_VECU;
		pCamera->rot		= INIT_LOGOCAMERA_ROT;
		pCamera->rotDest	= INIT_LOGOCAMERA_ROT;
		break;

	case MODE_TITLE:
		SetNumCamera(mode);
		pCamera->posV		= INIT_TITLECAMERA_POSV;
		pCamera->posVDest	= INIT_TITLECAMERA_POSV;
		pCamera->posR		= INIT_TITLECAMERA_POSR;
		pCamera->posRDest	= INIT_TITLECAMERA_POSR;
		pCamera->vecU		= INIT_VECU;
		pCamera->rot		= INIT_TITLECAMERA_ROT;
		pCamera->rotDest	= INIT_TITLECAMERA_ROT;
		break;

	case MODE_TUTORIAL:
		SetNumCamera(mode);
		break;

	case MODE_GAME:
		SetNumCamera(mode);
		break;

	case MODE_RESULT:
		SetNumCamera(mode);
		pCamera->posV		= INIT_RESULTCAMERA_POSV;
		pCamera->posVDest	= INIT_RESULTCAMERA_POSV;
		pCamera->posR		= INIT_RESULTCAMERA_POSR;
		pCamera->posRDest	= INIT_RESULTCAMERA_POSR;
		pCamera->vecU		= INIT_VECU;
		pCamera->rot		= INIT_RESULTCAMERA_ROT;
		pCamera->rotDest	= INIT_RESULTCAMERA_ROT;
		break;

	case MODE_DIAGNOSIS:
		pCamera->posV = INIT_TITLECAMERA_POSV;
		pCamera->posVDest = INIT_TITLECAMERA_POSV;
		pCamera->posR = INIT_TITLECAMERA_POSR;
		pCamera->posRDest = INIT_TITLECAMERA_POSR;
		pCamera->vecU = INIT_VECU;
		pCamera->rot = INIT_TITLECAMERA_ROT;
		pCamera->rotDest = INIT_TITLECAMERA_ROT;
		break;
	}
}
#endif

//=============================================================================
//	カメラの数取得処理
//=============================================================================
int GetNumCamera(void)
{
	return g_nNumCamera;
}

//=============================================================================
//	ビューポート生成処理
//=============================================================================
D3DVIEWPORT9 SetViewPort(DWORD X, DWORD Y, DWORD Width, DWORD Height)
{
	D3DVIEWPORT9 ViwePort = {};		// 返り値用変数

	ViwePort.X = X;					// 左上X座標
	ViwePort.Y = Y;					// 左上Y座標
	ViwePort.Width = Width;			// 幅
	ViwePort.Height = Height;		// 高さ
	ViwePort.MinZ = 0.0f;			// 固定
	ViwePort.MaxZ = 1.0f;			// 固定

	return ViwePort;
}