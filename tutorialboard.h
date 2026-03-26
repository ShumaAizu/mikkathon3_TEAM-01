//=============================================================================
//
//	イベント用オブジェクトの処理 [tutorialboard.h]
//	Author : SHUMA AIZU
// 
//=============================================================================

#ifndef _TUTRORIALBOARD_H_
#define _TUTRORIALBOARD_H_

#include "main.h"
#include "modeldata.h"
#include "motion.h"
#include "collision.h"
#include "object.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************

//*****************************************************************************
// チュートリアルレベルの種類
//*****************************************************************************
typedef enum TUTORIALLEVEL
{
	TUTORIALLEVEL_001 = 0,
	TUTORIALLEVEL_002,
	TUTORIALLEVEL_MAX
}TUTORIALLEVEL;

//*****************************************************************************
// イベント用オブジェクトの構造体定義
//*****************************************************************************
typedef struct Tutorialboard
{
	D3DXMATRIX mtxWorld;		// ワールドマトリックス
	D3DXVECTOR3 pos;			// オブジェクトの位置
	D3DXVECTOR3 *OffPos;		// 
	D3DXVECTOR3 move;			// 移動量
	D3DXVECTOR3 rot;			// オブジェクトの向き
	ObjectInfo ObjectInfo;		// オブジェクト情報
	OBJECTTYPE ObjectType;		// オブジェクトの種類
	float fAlpha;				// アルファ値
	bool bUse;					// 使用状態
}Tutorialboard;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void InitTutorialboard(void);
void UninitTutorialboard(void);
void UpdateTutorialboard(void);
void DrawTutorialboard(void);
bool CollisionTutorialboard(D3DXVECTOR3* pPos, D3DXVECTOR3* pPosOld, D3DXVECTOR3* pMove, int nCollisionIdx);
void SetTutorialboardNormal(D3DXVECTOR3 pos, D3DXVECTOR3 rot, MODELTYPE type);
void SetTutorialboardParent(D3DXVECTOR3 pos, D3DXVECTOR3 rot, PARENTMODELTYPE parentmodeltype, MOTIONDATATYPE motiondatatype, ColliderInfo* pColliderInfo, int nNumCollider, bool isCollision, bool isCollider);

TUTORIALLEVEL GetTutorialLevel(void);
void NextTutorialLevel(int nCounter);
bool IsNextTutorialLevel(void);

#endif // !_EVENTOBJECT_H_