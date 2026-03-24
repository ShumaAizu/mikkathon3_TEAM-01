//=============================================================================
//
//	落下地点目印処理 [meshring.h]
//	Author : SHUMA AIZU
// 
//=============================================================================

#ifndef _FALLPOINT_H_
#define _FALLPOINT_H_

#include "main.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define FALLPOINT_WIDTH			(100.0f)			// 落下地点目印の幅
#define FALLPOINT_HEIGHT		(100.0f)			// 落下地点目印の高さ
#define MAX_FALLPOINT			(4)					// 落下地点目印の最大数

//*****************************************************************************
// 落下地点目印の構造体定義
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
void SetFallPoint(D3DXVECTOR3 pos, D3DXVECTOR3 rot, float fInRadius, float fOutRadius);

#endif