//=============================================================================
//
//	アイテム処理 [item.h]
//	Author : SHUMA AIZU
// 
//=============================================================================

#ifndef _ITEM_H_
#define _ITEM_H_

#include "main.h"
#include "object.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************

//*****************************************************************************
// アイテムの種類
//*****************************************************************************
typedef enum ITEMTYPE
{
	ITEMTYPE_000 = 0,
	ITEMTYPE_001,
	ITEMTYPE_002,
	ITEMTYPE_MAX
}ITEMTYPE;

//*****************************************************************************
// アイテムの構造体定義
//*****************************************************************************
typedef struct Item
{
	D3DXVECTOR3 pos;							// 位置
	D3DXVECTOR3 rot;							// 向き
	D3DXVECTOR3 scale;							// 大きさ
	D3DXMATRIX mtxWorld;						// ワールドマトリックス
	ITEMTYPE itemtype;							// アイテムの種類
	Object_Normal NormalObjectData;				// 通常オブジェクトのデータ
	int nCollisionIdx;							// 当たり判定のインデックス
	bool bUse;									// 使用状態
}Item;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void InitItem(void);
void UninitItem(void);
void UpdateItem(void);
void DrawItem(void);
void CollisionItem(D3DXVECTOR3* pPos, float fRadius);
void SetItem(D3DXVECTOR3 pos, D3DXVECTOR3 rot, ITEMTYPE itemtype, MODELTYPE ModelType);

#endif