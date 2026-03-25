//=============================================================================
//
//	空処理 [skybox.h]
//	Author : SHUMA AIZU
// 
//=============================================================================

#ifndef _SKYBOX_H_
#define _SKYBOX_H_

#include "main.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define SKYBOX_RADIUS			(3000.0f)			// 空の半径
#define MAX_SKYBOX				(4)					// 空の最大数

//*****************************************************************************
// 空の構造体定義
//*****************************************************************************
typedef struct SkyBox
{
	LPDIRECT3DVERTEXBUFFER9 pVtxBuff;			// 頂点バッファへのポインタ
	LPDIRECT3DINDEXBUFFER9 pIdxBuff;			// インデックスバッファへのポインタ
	D3DXVECTOR3 pos;							// 位置
	D3DXVECTOR3 rot;							// 向き
	D3DXVECTOR2 tex;							// テクスチャ座標
	D3DXMATRIX mtxWorld;						// ワールドマトリックス
	float fRadius;								// 半径
	int nVertical;								// 縦軸の分割数
	int nHorizontal;							// 横軸の分割数
	bool bUse;									// 使用状態
}SkyBox;

typedef struct SkyBox* PSKYBOX;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void InitSkyBox(void);
void UninitSkyBox(void);
void UpdateSkyBox(void);
void DrawSkyBox(void);
void SetSkyBox(D3DXVECTOR3 pos, D3DXVECTOR3 rot, float fRadius, int nVertical, int nHorizontal);

#endif