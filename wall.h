//=============================================================================
//
//	メッシュウォール処理 [meshwall.h]
//	Author : SHUMA AIZU
// 
//=============================================================================

#ifndef _WALL_H_
#define _WALL_H_

#include "main.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define WALL_WIDTH			(100.0f)			// 壁の幅
#define WALL_HEIGHT			(100.0f)			// 壁の高さ
#define MAX_WALL			(64)				// 壁の最大数

//*****************************************************************************
// 壁の構造体定義
//*****************************************************************************
typedef struct
{
	LPDIRECT3DVERTEXBUFFER9 pVtxBuff;			// 頂点バッファへのポインタ
	LPDIRECT3DINDEXBUFFER9 pIdxBuff;			// インデックスバッファへのポインタ
	D3DXVECTOR3 pos;							// 位置
	D3DXVECTOR3 rot;							// 向き
	D3DXMATRIX mtxWorld;						// ワールドマトリックス
	int nSplitWidth;							// 横の分割数
	int nSplitHeight;							// 縦の分割数
	bool bUse;									// 使用状態
}Wall;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void InitWall(void);
void UninitWall(void);
void UpdateWall(void);
void DrawWall(void);
void SetWall(D3DXVECTOR3 pos, D3DXVECTOR3 rot, float fWidth, float fHeight, int nSplitWidth, int nSplitHeight);
void CollisionWall(D3DXVECTOR3* pPos, D3DXVECTOR3* pPosOld, D3DXVECTOR3* pMove);
bool CollisionDot(D3DXVECTOR3* pPos, D3DXVECTOR3* pPosOld, D3DXVECTOR3* pMove, D3DXVECTOR3* pRot);

#endif