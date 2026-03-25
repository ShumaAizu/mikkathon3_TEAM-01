//==================================================================================================================================
//
//			カメラ処理 [camera.cpp]
//			Author : ENDO HIDETO
// 
//==================================================================================================================================
//**************************************************************
// インクルード
#include "camera.h"
#include "debugproc.h"
#include "input.h"
#include "player.h"

//**************************************************************
// グローバル変数宣言
Camera g_aCamera[MAX_CAMERA];
bool g_bCameraMove = false;

//**************************************************************
// プロトタイプ宣言
void CameraFollow(P_CAMERA pCamera);		// プレイヤーに追従移動
void CameraMove(P_CAMERA pCamera);			// カメラ移動処理
void CameraRotation(P_CAMERA pCamera);		// カメラ回転処理

//=========================================================================================
// カメラ初期化
//=========================================================================================
void InitCamera(void)
{
	//**************************************************************
	// 変数宣言
	P_CAMERA pCamera = GetCamera();

	//**************************************************************
	// 各値の初期化
	for (int nCntCamera = 0; nCntCamera < MAX_CAMERA; nCntCamera++, pCamera++)
	{
		pCamera->posV = CAMERA_V_DEFPOS;										// 視点
		pCamera->posR = CAMERA_R_DEFPOS;										// 注視点
		pCamera->posRDest = CAMERA_R_DEFPOS;									// 目的の注視点
		pCamera->rot = CAMERA_ROT;		// カメラの角度
		pCamera->rotDest = CAMERA_ROT;	// カメラの角度
		pCamera->vecU = D3DXVECTOR3(0.0f, 1.0f, 0.0f);							// 上方向ベクトル
		pCamera->fDist = CAMERA_DISTANS;										// 視点と注視点の距離
		pCamera->fViewMin = VIEW_MINDEPTH;										// 最短描画距離
		pCamera->fViewMax = VIEW_MAXDEPTH;										// 最長描画距離
		pCamera->fViewRadian = VIEW_RADIAN;										// 視野角
		pCamera->viewport.X = 0.0f;												// 画面左上 X 座標
		pCamera->viewport.Y = 0.0f;												// 画面左上 Y 座標
		if (nCntCamera == CAMERATYPE_PLAYER)
		{
			pCamera->viewport.Width = SCREEN_WIDTH;								// 表示画面の横幅
			pCamera->viewport.Height = SCREEN_HEIGHT;							// 表示画面の高さ
		}
		else if (nCntCamera == CAMERATYPE_MINIMAP)
		{
			pCamera->viewport.Width = SCREEN_WIDTH;								// 表示画面の横幅
			pCamera->viewport.Height = (float)SCREEN_HEIGHT / 6;				// 表示画面の高さ
		}
		pCamera->viewport.MinZ = 0.0f;
		pCamera->viewport.MaxZ = 1.0f;
		pCamera->bUse = true;
	}
}

//=========================================================================================
// カメラ終了
//=========================================================================================
void UninitCamera(void)
{

}

//=========================================================================================
// カメラ更新
//=========================================================================================
void UpdateCamera(MODE mode)
{
	//**************************************************************
	// 変数宣言
	P_CAMERA pCamera = GetCamera();

	// カメラを動かすモード
	if (g_bCameraMove)
	{
		// 移動
		CameraMove(pCamera);

		// 回転
		CameraRotation(pCamera);
	}
	else
	{
		// 追従
		CameraFollow(pCamera);
	}

	pCamera->posV.x = pCamera->posR.x - cosf(D3DX_PI - pCamera->rot.y) * pCamera->fDist;
	if (pCamera->posR.y <= 260.0f)
		pCamera->posV.y = pCamera->posR.y - cosf(D3DX_PI - pCamera->rot.x) * pCamera->fDist;
	pCamera->posV.z = pCamera->posR.z - sinf(D3DX_PI - pCamera->rot.y) * pCamera->fDist;
	
	PrintDebugProc("\nCameraRot:%f:%f:%f\n", pCamera->rot.x, pCamera->rot.y, pCamera->rot.z);

	if (GetKeyboardTrigger(DIK_F1))
		g_bCameraMove = g_bCameraMove ^ 1;
}

//==============================================================
//	プレイヤーに追従
void CameraFollow(P_CAMERA pCamera)
{
	//**************************************************************
	// 変数宣言
	Player* pPlayer = GetPlayer();				// プレイヤー情報
	static float fPlayerMoveRot = atan2f(-pPlayer->move.x, -pPlayer->move.z);
	float	fCameraRDest = pPlayer->pos.y;		// プレイヤーの起点より上
	bool	bRise = false;

	//**************************************************************
	// プレイヤーに追従
	pCamera->posRDest.x = pPlayer->pos.x + 60.0f;		// プレイヤーの起点より前

	// 地面見える範囲
	if (fCameraRDest <= 240.0f)
		pCamera->posRDest.y = 180.0f;
	// 見上げる範囲
	else
		pCamera->posRDest.y = fCameraRDest - 60.0f;

	//**************************************************************
	// カメラの位置を補正
	pCamera->posR.x += (pCamera->posRDest.x - pCamera->posR.x) * CAMERA_FOLLOW_FACTOR;
	pCamera->posR.y += (pCamera->posRDest.y - pCamera->posR.y) * CAMERA_FOLLOW_FACTOR;
	pCamera->posR.z += (pCamera->posRDest.z - pCamera->posR.z) * CAMERA_FOLLOW_FACTOR;
}

//==============================================================
// 移動
void CameraMove(P_CAMERA pCamera)
{
	vec3 stickLeft = vec3_ZORO;
	if (GetJoypadStickLeft(&stickLeft.x, &stickLeft.y))
	{
		//**************************************************************
		//移動
		pCamera->posR.z += sinf(pCamera->rot.y) * stickLeft.y + cosf(pCamera->rot.y) * stickLeft.x;
		pCamera->posR.x += sinf(pCamera->rot.y) * stickLeft.x - cosf(pCamera->rot.y) * stickLeft.y;
	}

	if (GetJoypadPress(JOYKEY_LEFT_TRIGGER))
		pCamera->posR.y += 1.0f;
	if (GetJoypadPress(JOYKEY_RIGHT_TRIGGER))
		pCamera->posR.y -= 1.0f;
}

//==============================================================
// 回転
void CameraRotation(P_CAMERA pCamera)
{
	//**************************************************************
	// 変数宣言
	bool bUse = false;
	vec3 stickRight;

	//**************************************************************
	// 注視点のまわりを旋回
	if (GetJoypadStickRight(&stickRight.x, &stickRight.y))
	{
		pCamera->rot.x += stickRight.y * CAMERA_SPIN_Y;
		pCamera->rot.y += stickRight.x * CAMERA_SPIN_X;
		bUse = true;
	}

	//**************************************************************
	// その場で回転
	if (GetJoypadPress(JOYKEY_LEFT_SHOULDER))
	{
		pCamera->rot.y += CAMERA_REV * 0.5f;
		bUse = true;
	}
	if (GetJoypadPress(JOYKEY_RIGHT_SHOULDER))
	{
		pCamera->rot.y -= CAMERA_REV * 0.5f;
		bUse = true;
	}

	//**************************************************************
	// -πからπまでにする	
	if (pCamera->rot.x < -CAMERA_LOWLIM)
		pCamera->rot.x = -CAMERA_LOWLIM;
	else if (CAMERA_UPLIM < pCamera->rot.x)
		pCamera->rot.x = CAMERA_UPLIM;

	if (pCamera->rot.y < -D3DX_PI)
		pCamera->rot.y += D3DX_PI * 2;
	else if (D3DX_PI < pCamera->rot.y)
		pCamera->rot.y += -D3DX_PI * 2;

	if (pCamera->rot.z < -D3DX_PI)
		pCamera->rot.z += D3DX_PI * 2;
	else if (D3DX_PI < pCamera->rot.z)
		pCamera->rot.z += -D3DX_PI * 2;

	//**************************************************************
	// 視点から注視点を求める	
	if (bUse)
	{
		pCamera->posR.x = pCamera->posV.x + cosf(D3DX_PI - pCamera->rot.y) * pCamera->fDist;
		pCamera->posR.y = pCamera->posV.y + cosf(D3DX_PI - pCamera->rot.x) * pCamera->fDist;
		pCamera->posR.z = pCamera->posV.z + sinf(D3DX_PI - pCamera->rot.y) * pCamera->fDist;
	}
}

//=========================================================================================
// カメラ設置
//=========================================================================================
void SetCamera(void)
{
	//**************************************************************
	// 変数宣言
	LPDIRECT3DDEVICE9	pDevice = GetDevice();			// デバイスへのポインタ
	P_CAMERA			pCamera = &g_aCamera[0];		// カメラポインタ

	//**************************************************************
	// ビューポートの設定
	pDevice->SetViewport(&pCamera->viewport);

	//**************************************************************
	// プロジェクションマトリックスの初期化
	D3DXMatrixIdentity(&pCamera->mtxProjection);

	// プロジェクションマトリックスを作成
	D3DXMatrixPerspectiveFovLH(&pCamera->mtxProjection,
		D3DXToRadian(pCamera->fViewRadian),					// 視野角
		(float)pCamera->viewport.Width / (float)pCamera->viewport.Height,	// アスペクト比
		pCamera->fViewMin,								// 最短描画距離
		pCamera->fViewMax);								// 最大描画距離

// プロジェクションマトリックスを設定
	pDevice->SetTransform(D3DTS_PROJECTION, &pCamera->mtxProjection);

	//**************************************************************
	// ビューマトリックスの初期化
	D3DXMatrixIdentity(&pCamera->mtxView);

	// ビューマトリックスの作成
	D3DXMatrixLookAtLH(&pCamera->mtxView, &pCamera->posV, &pCamera->posR, &pCamera->vecU);

	// ビューマトリックスの設定
	pDevice->SetTransform(D3DTS_VIEW, &pCamera->mtxView);
}

//=========================================================================================
// カメラ設置
//=========================================================================================
void SetUICamera(vec3 viewTopLeft, D3DXVECTOR2 size)
{
	LPDIRECT3DDEVICE9	pDevice = GetDevice();		// デバイスへのポインタ
	D3DXMATRIX mtxProjection, mtxView;
	D3DVIEWPORT9 viewport;
	viewport.X = viewTopLeft.x;
	viewport.Y = viewTopLeft.y;
	viewport.Width = size.x;
	viewport.Height = size.y;
	viewport.MinZ = 0.0f;
	viewport.MaxZ = 1.0f;

	vec3 vecU = vec3(0.0f, 1.0f, 0.0f),
		posV = UICAMERA_POSV,
		posR = UICAMERA_POSR;

	float fViewRadian = 45.0f,
		fViewMin = 10.0f,
		fViewMax = 1000.0f;

	//**************************************************************
	// ビューポートの設定
	pDevice->SetViewport(&viewport);

	//**************************************************************
	// プロジェクションマトリックスの初期化
	D3DXMatrixIdentity(&mtxProjection);

	// プロジェクションマトリックスを作成
	D3DXMatrixPerspectiveFovLH(&mtxProjection,
		D3DXToRadian(fViewRadian),						// 視野角
		(float)viewport.Width / (float)viewport.Height,	// アスペクト比
		fViewMin,										// 最短描画距離
		fViewMax);										// 最大描画距離

	// プロジェクションマトリックスを設定
	pDevice->SetTransform(D3DTS_PROJECTION, &mtxProjection);

	//**************************************************************
	// ビューマトリックスの初期化
	D3DXMatrixIdentity(&mtxView);

	// ビューマトリックスの作成
	D3DXMatrixLookAtLH(&mtxView, &posV, &posR, &vecU);

	// ビューマトリックスの設定
	pDevice->SetTransform(D3DTS_VIEW, &mtxView);

	////Zテストを無効にする
	//pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);
	//pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	//**************************************************************
	// 画面クリア(バックバッファとZバッファのクリア)
	pDevice->Clear(0, NULL,
		(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER),
		D3DCOLOR_RGBA(0, 0, 0, 0), 1.0f, 0);
}

//=========================================================================================
// カメラの情報を取得
//=========================================================================================
P_CAMERA GetCamera(void)
{
	return &g_aCamera[0];
}

//=========================================================================================
// カメラリセット
//=========================================================================================
void CameraReset(P_CAMERA pCamera)
{
	//**************************************************************
	// 注視点
	pCamera->posR = GetPlayer()->pos;

	//**************************************************************
	// 注視点から視点を求める
	pCamera->fDist = CAMERA_DISTANS;				// 視点と注視点の距離
	pCamera->rot = CAMERA_ROT;						// カメラの角度
	pCamera->posV.x = pCamera->posR.x - cosf(D3DX_PI - pCamera->rot.y) * pCamera->fDist;
	pCamera->posV.y = pCamera->posR.y - cosf(D3DX_PI - pCamera->rot.x) * pCamera->fDist;
	pCamera->posV.z = pCamera->posR.z - sinf(D3DX_PI - pCamera->rot.y) * pCamera->fDist;
}

//=========================================================================================
// カメラを動かすかのbool値を取得
//=========================================================================================
bool IsEnableCameraEdit(void)
{
	return g_bCameraMove;
}