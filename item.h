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
#define MAX_ONEPATTERNITEM			(16)

//*****************************************************************************
// アイテムの種類
//*****************************************************************************
typedef enum ITEMTYPE
{
	ITEMTYPE_000 = 0,
	ITEMTYPE_MAX
}ITEMTYPE;

//*****************************************************************************
// アイテムの構造体定義
//*****************************************************************************
typedef struct Item
{
	D3DXVECTOR3 pos;							// 位置
	D3DXVECTOR3 CollOffset;						// 当たり判定のオフセット
	D3DXVECTOR3 rot;							// 向き
	D3DXMATRIX mtxWorld;						// ワールドマトリックス
	ITEMTYPE itemtype;							// アイテムの種類
	Object_Normal NormalObjectData;				// 通常オブジェクトのデータ
	float fRadius;								// 半径
	int nShadowIdx;								// 影のインデックス
	int nCollisionIdx;							// 当たり判定のインデックス
	bool bUse;									// 使用状態
}Item;

//*****************************************************************************
// アイテム情報の構造体定義
//*****************************************************************************
typedef struct ItemInfo
{
	D3DXVECTOR3 pos;							// 位置
	D3DXVECTOR3 rot;							// 向き
	ITEMTYPE itemtype;							// アイテムの種類
}ItemInfo;

//*****************************************************************************
// アイテムパターンの構造体定義
//*****************************************************************************
typedef struct ItemPattern
{
	ItemInfo aItemInfo[MAX_ONEPATTERNITEM];		// アイテムの情報
	int nNumItem;								// アイテム数
}ItemPattern;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void InitItem(void);
void UninitItem(void);
void UpdateItem(void);
void DrawItem(void);
int CollisionItem(D3DXVECTOR3 pos, float fRadius);
void SetItem(void);
void SetItemPattern(ItemPattern ItemPattern);
void ResetItem(void);

#endif