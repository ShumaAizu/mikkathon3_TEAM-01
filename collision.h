//=============================================================================
//
//	当たり判定の管理 [collision.h]
//	Author : SHUMA AIZU
// 
//=============================================================================

#ifndef _COLLISION_H_
#define _COLLISION_H_

#include "main.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_ONECOLLIDER			(10)	// ひとつのオブジェクトが持てるコライダーの最大
#define MAX_TRIANGLE			(256)	// メッシュの三角形の最大数

//*****************************************************************************
// タグの種類
//*****************************************************************************
typedef enum TAGTYPE
{
	TAGTYPE_PLAYER = 0,			// プレイヤー
	TAGTYPE_OBJECT,				// オブジェクト
	TAGTYPE_MAX
}TAGTYPE;

//*****************************************************************************
// コライダーの種類
//*****************************************************************************
typedef enum COLLIDERTYPE
{
	COLLIDERTYPE_BOX = 0,		// 矩形
	COLLIDERTYPE_CYLINDER,		// 筒
	COLLIDERTYPE_SPHERE,		// 球
	COLLIDERTYPE_CAPSULE,		// カプセル
	COLLIDERTYPE_MAX
}COLLIDERTYPE;

//*****************************************************************************
// 矩形のコライダーの構造体定義
//*****************************************************************************
typedef struct BoxCollider
{
	D3DXVECTOR3 pos;	// 原点
	D3DXVECTOR3 posOld;	// 過去の原点
	D3DXVECTOR3 rot;	// 向き
	float fWidth;		// 幅
	float fHeight;		// 高さ
	float fDepth;		// 奥行き
}BoxCollider;

//*****************************************************************************
// 筒のコライダーの構造体定義
//*****************************************************************************
typedef struct CylinderCollider
{
	D3DXVECTOR3 pos;	// 原点
	D3DXVECTOR3 posOld;	// 過去の原点
	D3DXVECTOR3 rot;	// 向き
	float fRadius;		// 半径
}CylinderCollider;

//*****************************************************************************
// 球のコライダーの構造体定義
//*****************************************************************************
typedef struct SphereCollider
{
	D3DXVECTOR3 pos;	// 原点
	D3DXVECTOR3 posOld;	// 過去の原点
	float fRadius;		// 半径
}SphereCollider;

//*****************************************************************************
// カプセルのコライダーの構造体定義
//*****************************************************************************
typedef struct CapsuleCollider
{
	D3DXVECTOR3 pos;	// 原点
	D3DXVECTOR3 posOld;	// 過去の原点
	D3DXVECTOR3 rot;	// 向き
	float fRadius;		// 半径
	float fHeight;		// 高さ
}CapsuleCollider;

//*****************************************************************************
// 三角形の構造体定義
//*****************************************************************************
typedef struct Triangle
{
	D3DXVECTOR3 posA;
	D3DXVECTOR3 posB;
	D3DXVECTOR3 posC;
	D3DXVECTOR3 vecNor;
}Triangle;

//*****************************************************************************
// メッシュコライダーの構造体定義
//*****************************************************************************
typedef struct MeshCollider
{
	Triangle aTriangle[MAX_TRIANGLE];
	int nNumTriangle;
	bool bUse;
}MeshCollider;

//*****************************************************************************
// コライダーの共用体定義
//*****************************************************************************
union ColliderType
{
	BoxCollider box;			// 矩形
	CylinderCollider cylinder;	// 筒
	SphereCollider sphere;		// 球
	CapsuleCollider capsule;	// カプセル
	struct
	{
		D3DXVECTOR3 pos;		// 位置だけ設定したいときのため
		D3DXVECTOR3 posOld;		// 位置だけ設定したいときのため
		D3DXVECTOR3 rot;		// 向きだけ設定したいときのため
	};
};

//*****************************************************************************
// コライダーの構造体定義
//*****************************************************************************
typedef struct Collider
{
	COLLIDERTYPE type;				// 種類
	ColliderType Collidertype;		// 種類指定
	bool bUse;						// 使用状態
}Collider;

//*****************************************************************************
// コライダー情報の構造体定義
//*****************************************************************************
typedef struct ColliderInfo
{
	COLLIDERTYPE type;				// 種類
	ColliderType Collidertype;		// 種類指定
}ColliderInfo;

//*****************************************************************************
// 当たり判定の構造体定義
//*****************************************************************************
typedef struct Collision
{
	D3DXVECTOR3 pos;						// 位置
	D3DXVECTOR3 rot;						// 向き
	int nColliderIdx[MAX_ONECOLLIDER];		// 使用するコライダーの番号
	int nNumCollider;						// 使用しているコライダーの数
	bool bUse;								// 使用状態
	unsigned int tag;						// タグ
}Collision;

//*****************************************************************************
// 当たり判定情報の構造体定義
//*****************************************************************************
typedef struct CollisionInfo
{
	D3DXVECTOR3 Intersection;		// 交点
	bool isCollision;				// 衝突したかどうか
	bool isRand;					// 地面についてるかどうか
}CollisionInfo;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void InitCollision(void);
void UninitCollision(void);
CollisionInfo UpdateCollision(int nMyIdx, int nTargetIdx, bool isTrigger = false);
void UpdateCollider(int nIdx, D3DXVECTOR3 pos);
void DrawCollision(void);
int SetCollision(D3DXVECTOR3 pos, D3DXVECTOR3 rot);
void ResetCollision(int nIdx);
void SetCollider(int nIdx, ColliderInfo ColliderInfo);
int GetNumCollider(void);

//*****************************************************************************
// 総当たりの当たり判定 (最適化の余裕なし)
//*****************************************************************************

//=============================================================================
//	矩形の当たり判定
//=============================================================================
bool CollisionBoxToBox(CollisionInfo& _CollisionInfo, ColliderType MyCollider, ColliderType TargetCollider, bool isTrigger);
bool CollisionBoxToCylinder(CollisionInfo& _CollisionInfo, ColliderType MyCollider, ColliderType TargetCollider, bool isTrigger);
bool CollisionBoxToSphere(CollisionInfo& _CollisionInfo, ColliderType MyCollider, ColliderType TargetCollider, bool isTrigger);
bool CollisionBoxToCapsule(CollisionInfo& _CollisionInfo, ColliderType MyCollider, ColliderType TargetCollider, bool isTrigger);

//=============================================================================
//	筒の当たり判定
//=============================================================================
bool CollisionCylinderToBox(CollisionInfo& _CollisionInfo, ColliderType MyCollider, ColliderType TargetCollider, bool isTrigger);
bool CollisionCylinderToCylinder(CollisionInfo& _CollisionInfo, ColliderType MyCollider, ColliderType TargetCollider, bool isTrigger);
bool CollisionCylinderToSphere(CollisionInfo& _CollisionInfo, ColliderType MyCollider, ColliderType TargetCollider, bool isTrigger);
bool CollisionCylinderToCapsule(CollisionInfo& _CollisionInfo, ColliderType MyCollider, ColliderType TargetCollider, bool isTrigger);

//=============================================================================
//	球の当たり判定
//=============================================================================
bool CollisionSphereToBox(CollisionInfo& _CollisionInfo, ColliderType MyCollider, ColliderType TargetCollider, bool isTrigger);
bool CollisionSphereToCylinder(CollisionInfo& _CollisionInfo, ColliderType MyCollider, ColliderType TargetCollider, bool isTrigger);
bool CollisionSphereToSphere(CollisionInfo& _CollisionInfo, ColliderType MyCollider, ColliderType TargetCollider, bool isTrigger);
bool CollisionSphereToCapsule(CollisionInfo& _CollisionInfo, ColliderType MyCollider, ColliderType TargetCollider, bool isTrigger);

//=============================================================================
//	カプセルの当たり判定 (ない)
//=============================================================================

//=============================================================================
//	メッシュの当たり判定
//=============================================================================
bool CollisionMeshCollider(CollisionInfo& _CollisionInfo, D3DXVECTOR3 pos, D3DXVECTOR3 posOld);
void LoadMeshColldier(const char* pColliderScript, D3DXVECTOR3 pos, D3DXVECTOR3 rot);

#endif // !_COLLISION_H_