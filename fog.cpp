//=============================================================================
//
//	霧の処理 [fog.cpp]
//	Author : SHUMA AIZU
// 
//=============================================================================

#include "fog.h"

//=============================================================================
//	霧の有効無効切り替え
//=============================================================================
bool SetFogEnable(bool isFog)
{
	static bool isFogCurrent = false;	// 新しい霧状況
	bool isFogOld = false;				// 古い霧状況

	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// デバイスの取得

	// 霧状況を切り替える
	pDevice->SetRenderState(D3DRS_FOGENABLE, isFog);

	isFogOld = isFogCurrent;
	isFogCurrent = isFog;

	return isFogOld;
}

//=============================================================================
//	霧の設定処理 [色 / 開始位置 / 終了位置 ]
//=============================================================================
void SetFog(D3DXCOLOR col, float fStart, float fEnd)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// デバイスの取得

	// 霧の設定
	pDevice->SetRenderState(D3DRS_FOGCOLOR, col);
	pDevice->SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR);

	pDevice->SetRenderState(D3DRS_FOGSTART, *(LPDWORD)(&fStart));
	pDevice->SetRenderState(D3DRS_FOGEND, *(LPDWORD)(&fEnd));
}