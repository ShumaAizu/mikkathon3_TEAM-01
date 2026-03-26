//=============================================================================
//
//	目標地点処理 [meshring.h]
//	Author : SHUMA AIZU
// 
//=============================================================================

#ifndef _FALLPOINT_H_
#define _FALLPOINT_H_

#include "main.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_FALLPOINT			(128)				// 目標地点の最大数
#define FALLPOINT_INRADIUS		(25.0f)				// 内径
#define FALLPOINT_OUTRADIUS		(35.0f)				// 外径

//*****************************************************************************
// 目標地点の構造体定義
//*****************************************************************************
typedef struct
{
	LPDIRECT3DVERTEXBUFFER9 pVtxBuff;			// 頂点バッファへのポインタ
	LPDIRECT3DINDEXBUFFER9 pIdxBuff;			// インデックスバッファへのポインタ
	D3DXVECTOR3 pos;							// 位置
	D3DXVECTOR3 rot;							// 向き
	D3DXMATRIX mtxWorld;						// ワールドマトリックス
	float fInRadius;							// 内側の半径
	float fOutRadius;							// 外側の半径
	bool bUse;									// 使用状態
}FallPoint;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void InitFallPoint(void);
void UninitFallPoint(void);
void UpdateFallPoint(void);
void DrawFallPoint(void);
void SetFallPoint(void);
void CollisionFallPoint(D3DXVECTOR3 pos);

#endif