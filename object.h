//=============================================================================
//
//	イベント用オブジェクトの処理 [eventobject.h]
//	Author : SHUMA AIZU
// 
//=============================================================================

#ifndef _EVECTOBJECT_H_
#define _EVECTOBJECT_H_

#include "main.h"
#include "modeldata.h"
#include "motion.h"
#include "collision.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************

//*****************************************************************************
// 使用するオブジェクトの種類
//*****************************************************************************
typedef enum EVENTOBJECTTYPE
{
	EVENTOBJECTTYPE_NONE = -1,	// 何もない
	EVENTOBJECTTYPE_NORMAL,		// 通常
	EVENTOBJECTTYPE_PARENT,		// 階層構造
	EVENTOBJECTTYPE_MAX
}EVENTOBJECTTYPE;

//*****************************************************************************
// 通常オブジェクトの構造体定義
//*****************************************************************************
typedef struct Object_Normal
{
	MODELTYPE type;				// モデルの種類
	ModelData* pModelData;		// モデルの情報
}Object_Normal;

//*****************************************************************************
// 階層構造オブジェクトの構造体定義
//*****************************************************************************
typedef struct Object_Parent
{
	PARENTMODELTYPE	type;				// 階層構造オブジェクトの種類
	Motion motion;						// モーション情報
	ParentModelData* pParentModelData;	// モデルの情報
	OffSetData OffSetData;				// モーション中の階層構造情報
}Object_Parent;

//*****************************************************************************
// イベント用オブジェクトの共用体定義
//*****************************************************************************
union ObjectInfo
{
	Object_Normal NormalObject;	// 通常モデル
	Object_Parent ParentObject;	// 階層構造モデル
};

//*****************************************************************************
// イベント用オブジェクトの構造体定義
//*****************************************************************************
typedef struct Object
{
	D3DXMATRIX mtxWorld;		// ワールドマトリックス
	D3DXVECTOR3 pos;			// オブジェクトの位置
	D3DXVECTOR3 rot;			// オブジェクトの向き
	ObjectInfo ObjectInfo;		// オブジェクト情報
	EVENTOBJECTTYPE ObjectType;	// オブジェクトの種類
	float fAlpha;				// アルファ値
	int nCollisionIdx;			// 当たり判定のインデックス
	bool bUse;					// 使用状態
}Object;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void InitObject(void);
void UninitObject(void);
void UpdateObject(void);
void DrawObject(void);
bool CollisionObject(D3DXVECTOR3* pPos, D3DXVECTOR3* pPosOld, D3DXVECTOR3* pMove, int nCollisionIdx);
void SetObjectNormal(D3DXVECTOR3 pos, D3DXVECTOR3 rot, MODELTYPE type, ColliderInfo* pColliderInfo, int nNumCollider, bool isCollision, bool isCollider);
void SetObjectParent(D3DXVECTOR3 pos, D3DXVECTOR3 rot, PARENTMODELTYPE parentmodeltype, MOTIONDATATYPE motiondatatype, ColliderInfo* pColliderInfo, int nNumCollider, bool isCollision, bool isCollider);

#endif // !_EVENTOBJECT_H_