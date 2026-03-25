//=============================================================================
//
//	トラップ処理 [trap.h]
//	Author : SHUMA AIZU
// 
//=============================================================================

#ifndef _TRAP_H_
#define _TRAP_H_

#include "main.h"
#include "object.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************

//*****************************************************************************
// トラップの種類
//*****************************************************************************
typedef enum TRAPTYPE
{
	TRAPTYPE_NORMAL = 0,
	TRAPTYPE_MINI,
	TRAPTYPE_002,
	TRAPTYPE_MAX
}TRAPTYPE;

//*****************************************************************************
// トラップの構造体定義
//*****************************************************************************
typedef struct Trap
{
	D3DXVECTOR3 pos;							// 位置
	D3DXVECTOR3 rot;							// 向き
	D3DXVECTOR3 scale;							// 大きさ
	D3DXMATRIX mtxWorld;						// ワールドマトリックス
	TRAPTYPE traptype;							// トラップの種類
	Object_Normal NormalObjectData;				// 通常オブジェクトのデータ
	int nCollisionIdx;							// 当たり判定のインデックス
	bool bUse;									// 使用状態
}Trap;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void InitTrap(void);
void UninitTrap(void);
void UpdateTrap(void);
void DrawTrap(void);
bool CollisionTrap(D3DXVECTOR3 pos, float fRadius);
void SetTrap(D3DXVECTOR3 pos, D3DXVECTOR3 rot, TRAPTYPE traptype, MODELTYPE ModelType);

#endif