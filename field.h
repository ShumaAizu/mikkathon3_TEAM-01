//=============================================================================
//
//	メッシュフィールド処理 [meshfield.h]
//	Author : SHUMA AIZU
// 
//=============================================================================

#ifndef _FIELD_H_
#define _FIELD_H_

#include "main.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define FIELD_WIDTH				(100.0f)			// メッシュフィールドの幅
#define FIELD_DEPTH				(100.0f)			// メッシュフィールドの奥行き

//*****************************************************************************
// メッシュフィールドの種類
//*****************************************************************************
typedef enum FIELDTYPE
{
	FIELDTYPE_000 = 0,
	FIELDTYPE_MAX
}FIELDTYPE;

//*****************************************************************************
// メッシュフィールドの構造体定義
//*****************************************************************************
typedef struct Field
{
	D3DXVECTOR3 pos;							// 位置
	D3DXVECTOR3 rot;							// 向き
	D3DXVECTOR2 tex;							// テクスチャ座標
	FIELDTYPE type;							// 種類
	int nSplitWidth;							// 幅の分割数
	int nSplitDepth;							// 奥行きの分割数
	float fWidth;								// 幅
	float fDepth;								// 奥行き
	bool bUse;									// 使用状態
	bool bMesh;									// メッシュかどうか
	LPDIRECT3DVERTEXBUFFER9 pVtxBuff;			// 頂点バッファへのポインタ
	LPDIRECT3DINDEXBUFFER9 pIdxBuff;			// インデックスバッファへのポインタ
	D3DXMATRIX mtxWorld;						// ワールドマトリックス
	D3DXMATRIX *mtxParent;						// 親マトリックス
}Field;

typedef struct Field* PFIELD;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void InitField(void);
void UninitField(void);
void UpdateField(void);
void DrawField(void);
void SetField(D3DXVECTOR3 pos, D3DXVECTOR3 rot, float fWidth, float fDepth, int nSplitWidth, int nSplitDepth, FIELDTYPE type);
bool CollisionField(D3DXVECTOR3* pPos, D3DXVECTOR3* pPosOld, D3DXVECTOR3* pMove);
void SetMesh(D3DXVECTOR3 pos, D3DXVECTOR3 rot, int nVtx, D3DXVECTOR3* VtxPos, int nSplitWidth, int nSplitDepth, FIELDTYPE type, D3DXMATRIX* pmtxParent);

#endif