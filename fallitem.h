//=============================================================================
//
//	アイテム処理 [fallitem.h]
//	Author : SHUMA AIZU
// 
//=============================================================================

#ifndef _FALLITEM_H_
#define _FALLITEM_H_

#include "main.h"
#include "object.h"
#include "item.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************AX

//*****************************************************************************
// アイテムの構造体定義
//*****************************************************************************
typedef struct FallItem
{
	D3DXVECTOR3 pos;							// 位置
	D3DXVECTOR3 move;							// 移動量
	D3DXVECTOR3 CollOffset;						// 当たり判定のオフセット
	D3DXVECTOR3 rot;							// 向き
	D3DXMATRIX mtxWorld;						// ワールドマトリックス
	ITEMTYPE itemtype;							// アイテムの種類
	Object_Normal NormalObjectData;				// 通常オブジェクトのデータ
	float fRadius;								// 半径
	int nCollisionIdx;							// 当たり判定のインデックス
	bool bUse;									// 使用状態
}FallItem;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void InitFallItem(void);
void UninitFallItem(void);
void UpdateFallItem(void);
void DrawFallItem(void);
void SetFallItem(D3DXVECTOR3 pos, D3DXVECTOR3 rot, ITEMTYPE itemtype);

#endif