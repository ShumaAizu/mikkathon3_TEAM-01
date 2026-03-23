//=============================================================================
//
//	ライト処理 [light.cpp]
//	Author : SHUMA AIZU
// 
//=============================================================================

#include "light.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define NUM_LIGHT			(3)			// ライトの数

//*****************************************************************************
// グローバル変数
//*****************************************************************************
D3DLIGHT9 g_aLight[NUM_LIGHT];			// ライトの情報

//=============================================================================
//	ライトの初期化処理
//=============================================================================
void InitLight(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	D3DXVECTOR3 vecDir;		// ライトの方向ベクトル

	int nCntLight;			// カウント用変数

	// ライトの情報をクリアする
	for (nCntLight = 0; nCntLight < NUM_LIGHT; nCntLight++)
	{
		ZeroMemory(&g_aLight[nCntLight], sizeof(D3DLIGHT9));

		// ライトの種類を設定
		g_aLight[nCntLight].Type = D3DLIGHT_DIRECTIONAL;
	}

	// ライトの拡散光を設定
	g_aLight[0].Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	// ライトの拡散光を設定
	g_aLight[1].Diffuse = D3DXCOLOR(0.65f, 0.65f, 0.65f, 1.0f);

	// ライトの拡散光を設定
	g_aLight[2].Diffuse = D3DXCOLOR(0.15f, 0.15f, 0.15f, 1.0f);

	// ライトの方向を設定
	vecDir = D3DXVECTOR3(0.2f, -0.8f, 0.4f);
	D3DXVec3Normalize(&vecDir, &vecDir);
	g_aLight[0].Direction = vecDir;

	vecDir = D3DXVECTOR3(-0.2f, 0.8f, -0.4f);
	D3DXVec3Normalize(&vecDir, &vecDir);
	g_aLight[1].Direction = vecDir;

	vecDir = D3DXVECTOR3(0.9f, 0.1f, 0.4f);
	D3DXVec3Normalize(&vecDir, &vecDir);
	g_aLight[2].Direction = vecDir;

	// ライトを設定,有効にする
	for (nCntLight = 0; nCntLight < NUM_LIGHT; nCntLight++)
	{
		pDevice->SetLight(nCntLight, &g_aLight[nCntLight]);

		pDevice->LightEnable(nCntLight, TRUE);
	}
}

//=============================================================================
//	ライトの終了処理
//=============================================================================
void UninitLight(void)
{

}

//=============================================================================
//	ライトの更新処理
//=============================================================================
void UpdateLight(void)
{

}