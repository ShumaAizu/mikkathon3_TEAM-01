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
#define MAX_ONEPATTERNTRAP		(16)		// 一つのパターンに置けるトラップの最大
#define MAX_GRID				(22)		// グリッドの最大数
#define INIT_GRIDPOSX			(-2500.0f)	// グリッドの開始位置

//*****************************************************************************
// トラップの種類
//*****************************************************************************
typedef enum TRAPTYPE
{
	TRAPTYPE_NORMAL = 0,
	TRAPTYPE_MINI,
	TRAPTYPE_MAX
}TRAPTYPE;

//*****************************************************************************
// トラップの構造体定義
//*****************************************************************************
typedef struct Trap
{
	D3DXVECTOR3 pos;							// 位置
	D3DXVECTOR3 rot;							// 向き
	D3DXMATRIX mtxWorld;						// ワールドマトリックス
	TRAPTYPE traptype;							// トラップの種類
	Object_Normal NormalObjectData;				// 通常オブジェクトのデータ
	int nShadowIdx;								// 影のインデックス
	bool bUse;									// 使用状態
}Trap;

//*****************************************************************************
// トラップ情報の構造体定義
//*****************************************************************************
typedef struct TrapInfo
{
	D3DXVECTOR3 pos;							// 位置
	D3DXVECTOR3 rot;							// 向き
	TRAPTYPE traptype;							// トラップの種類
}TrapInfo;

//*****************************************************************************
// トラップパターンの構造体定義
//*****************************************************************************
typedef struct TrapPattern
{
	TrapInfo aTrapInfo[MAX_ONEPATTERNTRAP];		// トラップの情報
	int nNumTrap;								// トラップ数
}TrapPattern;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void InitTrap(void);
void UninitTrap(void);
void UpdateTrap(void);
void DrawTrap(void);
bool CollisionTrap(D3DXVECTOR3 pos, float fRadius);
void SetTrap(void);
void SetTrapPattern(TrapPattern TrapPattern);
void ResetTrap(void);
int* GetGridUsePattern(void);

#endif