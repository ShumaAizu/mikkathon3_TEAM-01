//=============================================================================
//
//	当たり判定の管理 [collision.cpp]
//	Author : SHUMA AIZU
// 
//=============================================================================

#include "collision.h"
#include "debugproc.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_COLLISION			(256)									// 当たり判定の最大数
#define MAX_COLLIDER			(MAX_COLLISION * MAX_ONECOLLIDER)		// コライダーの最大数
#define MAX_MESHCOLLIDER		(32)									// メッシュコライダーの最大数

//*****************************************************************************
// グローバル変数
//*****************************************************************************
Collider g_aCollider[MAX_COLLIDER] = {};				// コライダーの情報
MeshCollider g_aMeshCollider[MAX_MESHCOLLIDER] = {};	// メッシュコライダーの情報
Collision g_aCollision[MAX_COLLISION] = {};				// 当たり判定の情報
int g_aUseColliderIdx[MAX_COLLIDER] = {};				// 使用しているコライダーの識別番号
int g_aUseCollisionIdx[MAX_COLLISION] = {};				// 使用している当たり判定の識別番号
int g_nNumCollider;										// 使用しているコライダーの数
int g_nNumCollision;									// 使用している当たり判定の数

//*****************************************************************************
// 関数ポインタの定義
//*****************************************************************************

//=============================================================================
//	矩形の当たり判定
//=============================================================================
bool(*BoxCollision[COLLIDERTYPE_MAX])(CollisionInfo& _CollisionInfo, ColliderType MyCollider, ColliderType TargetCollider, bool isTrigger) =
{
	CollisionBoxToBox,
	CollisionBoxToCylinder,
	CollisionBoxToSphere,
	CollisionBoxToCapsule,
};

//=============================================================================
//	筒の当たり判定
//=============================================================================
bool(*CylinderCollision[COLLIDERTYPE_MAX])(CollisionInfo& _CollisionInfo, ColliderType MyCollider, ColliderType TargetCollider, bool isTrigger) =
{
	CollisionCylinderToBox,
	CollisionCylinderToCylinder,
	CollisionCylinderToSphere,
	CollisionCylinderToCapsule,
};

//=============================================================================
//	球の当たり判定
//=============================================================================
bool(*SphereCollision[COLLIDERTYPE_MAX])(CollisionInfo& _CollisionInfo, ColliderType MyCollider, ColliderType TargetCollider, bool isTrigger) =
{
	CollisionSphereToBox,
	CollisionSphereToCylinder,
	CollisionSphereToSphere,
	CollisionSphereToCapsule,
};

//=============================================================================
//	カプセルの当たり判定
//=============================================================================
#if 0	// 未実装
bool(*CapsuleCollision[COLLIDERTYPE_MAX])(CollisionInfo& _CollisionInfo, ColliderType MyCollider, ColliderType TargetCollider, bool isTrigger) =
{

};
#endif

//=============================================================================
//	当たり判定の初期化処理
//=============================================================================
void InitCollision(void)
{
	// 各種ポインタ
	Collider* pCollider = &g_aCollider[0];
	Collision* pCollision = &g_aCollision[0];
	MeshCollider* pMeshCollider = &g_aMeshCollider[0];
	
	// 初期化
	memset(pCollider, NULL, sizeof(Collider) * MAX_COLLIDER);
	memset(pCollision, NULL, sizeof(Collision) * MAX_COLLISION);
	memset(pMeshCollider, NULL, sizeof(MeshCollider) * MAX_MESHCOLLIDER);
	memset(&g_aUseColliderIdx[0], -1, sizeof(int) * MAX_COLLIDER);
	memset(&g_aUseCollisionIdx[0], -1, sizeof(int) * MAX_COLLISION);

	g_nNumCollider = 0;
	g_nNumCollision = 0;

	// メッシュコライダーの設定
	LoadMeshColldier("data\\SCRIPTS\\MESH\\bridge001.bin", D3DXVECTOR3(-122.5f, -6.0f, -2400.0f), INIT_D3DXVEC3);
	LoadMeshColldier("data\\SCRIPTS\\MESH\\bridge001.bin", D3DXVECTOR3(2200.0f, -6.0f, -833.0f), D3DXVECTOR3(0.0f, DegreeToRadian(110.0f), 0.0f));
	LoadMeshColldier("data\\SCRIPTS\\MESH\\bridge001.bin", D3DXVECTOR3(2000.0f, -6.0f, 940.0f), D3DXVECTOR3(0.0f, DegreeToRadian(90.0f), 0.0f));
	LoadMeshColldier("data\\SCRIPTS\\MESH\\bridge001.bin", D3DXVECTOR3(-2844.0f, -6.0f, -2600.0f), INIT_D3DXVEC3);
	LoadMeshColldier("data\\SCRIPTS\\MESH\\riverwallcollision.bin", D3DXVECTOR3(1800.0f, -50.0f, 3500.0f), INIT_D3DXVEC3);
}

//=============================================================================
//	当たり判定の終了処理
//=============================================================================
void UninitCollision(void)
{

}

//=============================================================================
//	当たり判定の更新処理
//=============================================================================
CollisionInfo UpdateCollision(int nMyIdx, int nTargetIdx, bool isTrigger)
{
	Collision* pMyCollision = &g_aCollision[nMyIdx];			// 自分の当たり判定へのポインタ
	Collision* pTargetCollision = &g_aCollision[nTargetIdx];	// 相手の当たり判定へのポインタ
	CollisionInfo CollisionInfo = {};							// 衝突情報
	bool isCollision = false;									// 衝突したかどうか

	// 自分のコライダーから
	for (int nCntMyCollider = 0; nCntMyCollider < pMyCollision->nNumCollider; nCntMyCollider++)
	{
		// 相手のコライダー
		for (int nCntTargetCollider = 0; nCntTargetCollider < pTargetCollision->nNumCollider; nCntTargetCollider++)
		{
			// 対応する当たり判定を呼ぶ
			switch (g_aCollider[pMyCollision->nColliderIdx[nCntMyCollider]].type)
			{
				// 矩形の当たり判定
			case COLLIDERTYPE_BOX:
				isCollision = BoxCollision[g_aCollider[pTargetCollision->nColliderIdx[nCntTargetCollider]].type]
				(CollisionInfo, 
				g_aCollider[pMyCollision->nColliderIdx[nCntMyCollider]].Collidertype, 
				g_aCollider[pTargetCollision->nColliderIdx[nCntTargetCollider]].Collidertype,
				isTrigger);
				break;

				// 筒の当たり判定
			case COLLIDERTYPE_CYLINDER:
				isCollision = CylinderCollision[g_aCollider[pTargetCollision->nColliderIdx[nCntTargetCollider]].type]
				(CollisionInfo,
				g_aCollider[pMyCollision->nColliderIdx[nCntMyCollider]].Collidertype,
				g_aCollider[pTargetCollision->nColliderIdx[nCntTargetCollider]].Collidertype,
				isTrigger);
				break;

				// 球の当たり判定
			case COLLIDERTYPE_SPHERE:
				isCollision = SphereCollision[g_aCollider[pTargetCollision->nColliderIdx[nCntTargetCollider]].type]
				(CollisionInfo, 
				g_aCollider[pMyCollision->nColliderIdx[nCntMyCollider]].Collidertype, 
				g_aCollider[pTargetCollision->nColliderIdx[nCntTargetCollider]].Collidertype,
				isTrigger);
				break;

				// カプセルの当たり判定
			case COLLIDERTYPE_CAPSULE:
				// 未実装
				break;
			}

			if (isCollision)
			{
				break;
			}
		}

		if (isCollision)
		{
			break;
		}
	}

	// 衝突していたかどうか
	CollisionInfo.isCollision = isCollision;

	// 衝突情報を返す
	return CollisionInfo;
}

//=============================================================================
//	コライダーの更新処理 (仮)
//=============================================================================
void UpdateCollider(int nIdx, D3DXVECTOR3 pos)
{
	// 位置を合わせる
	g_aCollider[g_aCollision[nIdx].nColliderIdx[0]].Collidertype.posOld = g_aCollider[g_aCollision[nIdx].nColliderIdx[0]].Collidertype.pos;
	g_aCollider[g_aCollision[nIdx].nColliderIdx[0]].Collidertype.pos = pos;
}

//=============================================================================
//	当たり判定の描画処理 (仮)
//=============================================================================
void DrawCollision(void)
{

}

//=============================================================================
//	当たり判定の設定処理
//=============================================================================
int SetCollision(D3DXVECTOR3 pos, D3DXVECTOR3 rot)
{
	Collision* pCollision = &g_aCollision[0];
	int nCntCollision = 0;

	for (nCntCollision = 0; nCntCollision < MAX_COLLISION; nCntCollision++, pCollision++)
	{
		if (pCollision->bUse == true)
		{// 使ってたら弾く
			continue;
		}

		// 位置と向きを設定
		pCollision->pos = pos;
		pCollision->rot = rot;

		pCollision->bUse = true;	// 使用状態に
		g_nNumCollision++;			// 全体の数を増やす
		break;
	}

	return nCntCollision;			// 使用状態にした当たり判定の番号を返す
}

//=============================================================================
//	当たり判定の解除処理
//=============================================================================
void ResetCollision(int nIdx)
{
	// 指定したインデックスのコライダーをすべてリセット
	for (int nCntCollider = 0; nCntCollider < g_aCollision[nIdx].nNumCollider; nCntCollider++)
	{
		memset(&g_aCollider[g_aCollision[nIdx].nColliderIdx[nCntCollider]], NULL, sizeof(Collider));
		g_nNumCollider--;
	}

	// 指定したインデックスの当たり判定をリセット
	memset(&g_aCollision[nIdx], NULL, sizeof(Collision));
	g_nNumCollision--;
}

//=============================================================================
//	コライダーの設定処理
//=============================================================================
void SetCollider(int nIdx, ColliderInfo ColliderInfo)
{
	Collider* pCollider = &g_aCollider[0];	// コライダーへのポインタ
	int nCntCollider = 0;					// カウント用変数

	for (nCntCollider = 0; nCntCollider < MAX_COLLIDER; nCntCollider++, pCollider++)
	{
		if (pCollider->bUse == true)
		{// 使ってたら弾く
			continue;
		}

		// 計算用マトリックス
		D3DXMATRIX mtxRot;

		// マトリックスの初期化
		D3DXMatrixIdentity(&mtxRot);

		// 向きを算出
		D3DXMatrixRotationYawPitchRoll(&mtxRot, g_aCollision[nIdx].rot.y + pCollider->Collidertype.rot.y, 
			g_aCollision[nIdx].rot.x + pCollider->Collidertype.rot.x, 
			g_aCollision[nIdx].rot.z + pCollider->Collidertype.rot.z);

		// 各方面ベクトルを回転行列から抜き取る
		D3DXVECTOR3 AxisXNor = { mtxRot._11, mtxRot._12, mtxRot._13 };
		D3DXVECTOR3 AxisYNor = { mtxRot._21, mtxRot._22, mtxRot._23 };
		D3DXVECTOR3 AxisZNor = { mtxRot._31, mtxRot._32, mtxRot._33 };

		// オフセット位置を算出
		D3DXVECTOR3 OffSet = (ColliderInfo.Collidertype.pos.x * AxisXNor) +
			(ColliderInfo.Collidertype.pos.y * AxisYNor) +
			(ColliderInfo.Collidertype.pos.z * AxisZNor);

		// 指定された当たり判定にコライダーのインデックスを追加
		g_aCollision[nIdx].nColliderIdx[g_aCollision[nIdx].nNumCollider] = nCntCollider;

		// 各値代入
		pCollider->Collidertype = ColliderInfo.Collidertype;

		// 原点からのオフセット位置を設定
		pCollider->Collidertype.pos = g_aCollision[nIdx].pos + OffSet;

		if (ColliderInfo.type != COLLIDERTYPE_SPHERE)
		{// 球以外のコライダーだったら
			// 向きを設定
			pCollider->Collidertype.rot = g_aCollision[nIdx].rot + pCollider->Collidertype.rot;
		}

		pCollider->type = ColliderInfo.type;		// 種類を設定
		pCollider->bUse = true;						// 使用状態に

		// コライダー使用数を当たり判定でも全体でも増やす
		g_aCollision[nIdx].nNumCollider++;
		g_nNumCollider++;

		break;
	}
}

//=============================================================================
//	矩形と矩形との当たり判定処理
//=============================================================================
bool CollisionBoxToBox(CollisionInfo& _CollisionInfo, ColliderType MyCollider, ColliderType TargetCollider, bool isTrigger)
{
	// 計算用マトリックス
	D3DXMATRIX MymtxRot, TargetmtxRot;

	// マトリックスの初期化
	D3DXMatrixIdentity(&MymtxRot);
	D3DXMatrixIdentity(&TargetmtxRot);

	// 向きを算出
	D3DXMatrixRotationYawPitchRoll(&MymtxRot, MyCollider.box.rot.y, MyCollider.box.rot.x, MyCollider.box.rot.z);
	D3DXMatrixRotationYawPitchRoll(&TargetmtxRot, TargetCollider.box.rot.y, TargetCollider.box.rot.x, TargetCollider.box.rot.z);

	//----------------------------------------------------
	// 回転行列から各軸ベクトルの確保 (正規化Norと長さ)
	//----------------------------------------------------

	// 自分の矩形の各軸ベクトル
	D3DXVECTOR3 MyAxisXNor = { MymtxRot._11, MymtxRot._12, MymtxRot._13 },	// X軸
		MyAxisX = MyAxisXNor * MyCollider.box.fWidth;
	D3DXVECTOR3 MyAxisYNor = { MymtxRot._21, MymtxRot._22, MymtxRot._23 },	// Y軸
		MyAxisY = MyAxisYNor * MyCollider.box.fHeight;
	D3DXVECTOR3 MyAxisZNor = { MymtxRot._31, MymtxRot._32, MymtxRot._33 },	// Z軸
		MyAxisZ = MyAxisZNor * MyCollider.box.fDepth;

	// 対象の矩形の各軸ベクトル
	D3DXVECTOR3 TargetAxisXNor = { TargetmtxRot._11, TargetmtxRot._12, TargetmtxRot._13 },	// X軸
		TargetAxisX = TargetAxisXNor * TargetCollider.box.fWidth;
	D3DXVECTOR3 TargetAxisYNor = { TargetmtxRot._21, TargetmtxRot._22, TargetmtxRot._23 },	// Y軸
		TargetAxisY = TargetAxisYNor * TargetCollider.box.fHeight;
	D3DXVECTOR3 TargetAxisZNor = { TargetmtxRot._31, TargetmtxRot._32, TargetmtxRot._33 },	// Z軸
		TargetAxisZ = TargetAxisZNor * TargetCollider.box.fDepth;

	// 自分と対象の中心点の距離
	D3DXVECTOR3 Distance = MyCollider.box.pos - TargetCollider.box.pos;

	D3DXVECTOR3 Sep = {};

	// 自分のX方向の分離軸
	Sep = D3DXVECTOR3(fabsf(D3DXVec3Dot(&MyAxisXNor, &TargetAxisX)), fabsf(D3DXVec3Dot(&MyAxisXNor, &TargetAxisY)), fabsf(D3DXVec3Dot(&MyAxisXNor, &TargetAxisZ)));

	float rA = D3DXVec3Length(&MyAxisX);
	float rB = Sep.x + Sep.y + Sep.z;
	float L = fabsf(D3DXVec3Dot(&Distance, &MyAxisXNor));
	if (L > rA + rB)
	{
		PrintDebugProc("AX %f\n", L);
		return false; // 衝突していない
	}

	// 自分のY方向の分離軸
	Sep = D3DXVECTOR3(fabsf(D3DXVec3Dot(&MyAxisYNor, &TargetAxisX)), fabsf(D3DXVec3Dot(&MyAxisYNor, &TargetAxisY)), fabsf(D3DXVec3Dot(&MyAxisYNor, &TargetAxisZ)));

	rA = D3DXVec3Length(&MyAxisY);
	rB = Sep.x + Sep.y + Sep.z;
	L = fabsf(D3DXVec3Dot(&Distance, &MyAxisYNor));
	if (L > rA + rB)
	{
		PrintDebugProc("AY %f\n", L);
		return false; // 衝突していない
	}

	// 自分のZ方向の分離軸
	Sep = D3DXVECTOR3(fabsf(D3DXVec3Dot(&MyAxisZNor, &TargetAxisX)), fabsf(D3DXVec3Dot(&MyAxisZNor, &TargetAxisY)), fabsf(D3DXVec3Dot(&MyAxisZNor, &TargetAxisZ)));

	rA = D3DXVec3Length(&MyAxisZ);
	rB = Sep.x + Sep.y + Sep.z;
	L = fabsf(D3DXVec3Dot(&Distance, &MyAxisZNor));
	if (L > rA + rB)
	{
		PrintDebugProc("AZ %f\n", L);
		return false; // 衝突していない
	}

	// 対象のX方向の分離軸
	Sep = D3DXVECTOR3(fabsf(D3DXVec3Dot(&MyAxisX, &TargetAxisXNor)), fabsf(D3DXVec3Dot(&MyAxisY, &TargetAxisXNor)), fabsf(D3DXVec3Dot(&MyAxisZ, &TargetAxisXNor)));

	rA = Sep.x + Sep.y + Sep.z;
	rB = D3DXVec3Length(&TargetAxisX);
	L = fabsf(D3DXVec3Dot(&Distance, &TargetAxisXNor));
	if (L > rA + rB)
	{
		PrintDebugProc("BX %f\n", L);
		return false; // 衝突していない
	}

	// 対象のY方向の分離軸
	Sep = D3DXVECTOR3(fabsf(D3DXVec3Dot(&MyAxisX, &TargetAxisYNor)), fabsf(D3DXVec3Dot(&MyAxisY, &TargetAxisYNor)), fabsf(D3DXVec3Dot(&MyAxisZ, &TargetAxisYNor)));

	rA = Sep.x + Sep.y + Sep.z;
	rB = D3DXVec3Length(&TargetAxisY);
	L = fabsf(D3DXVec3Dot(&Distance, &TargetAxisYNor));
	if (L > rA + rB)
	{
		PrintDebugProc("BY %f\n", L);
		return false; // 衝突していない
	}

	// 対象のZ方向の分離軸
	Sep = D3DXVECTOR3(fabsf(D3DXVec3Dot(&MyAxisX, &TargetAxisZNor)), fabsf(D3DXVec3Dot(&MyAxisY, &TargetAxisZNor)), fabsf(D3DXVec3Dot(&MyAxisZ, &TargetAxisZNor)));

	rA = Sep.x + Sep.y + Sep.z;
	rB = D3DXVec3Length(&TargetAxisZ);
	L = fabsf(D3DXVec3Dot(&Distance, &TargetAxisZNor));
	if (L > rA + rB)
	{
		PrintDebugProc("BZ %f\n", L);
		return false; // 衝突していない
	}

	// 自分のX方向と対象のX方向との外積の分離軸
	D3DXVECTOR3 Cross;
	D3DXVec3Cross(&Cross, &MyAxisXNor, &TargetAxisXNor);

	Sep = D3DXVECTOR3(fabsf(D3DXVec3Dot(&MyAxisY, &Cross)), fabsf(D3DXVec3Dot(&MyAxisZ, &Cross)), 0.0f);
	D3DXVECTOR3 Sep2 = D3DXVECTOR3(fabsf(D3DXVec3Dot(&TargetAxisY, &Cross)), fabsf(D3DXVec3Dot(&TargetAxisZ, &Cross)), 0.0f);

	rA = Sep.x + Sep.y;
	rB = Sep2.x + Sep2.y;
	L = fabsf(D3DXVec3Dot(&Distance, &Cross));
	if (L > rA + rB)
	{
		return false;	// 衝突していない
	}

	// 自分のX方向と対象のY方向との外積の分離軸
	D3DXVec3Cross(&Cross, &MyAxisXNor, &TargetAxisYNor);

	Sep = D3DXVECTOR3(fabsf(D3DXVec3Dot(&MyAxisY, &Cross)), fabsf(D3DXVec3Dot(&MyAxisZ, &Cross)), 0.0f);
	Sep2 = D3DXVECTOR3(fabsf(D3DXVec3Dot(&TargetAxisX, &Cross)), fabsf(D3DXVec3Dot(&TargetAxisZ, &Cross)), 0.0f);

	rA = Sep.x + Sep.y;
	rB = Sep2.x + Sep2.y;
	L = fabsf(D3DXVec3Dot(&Distance, &Cross));
	if (L > rA + rB)
	{
		return false;	// 衝突していない
	}

	// 自分のX方向と対象のZ方向との外積の分離軸
	D3DXVec3Cross(&Cross, &MyAxisXNor, &TargetAxisZNor);

	Sep = D3DXVECTOR3(fabsf(D3DXVec3Dot(&MyAxisY, &Cross)), fabsf(D3DXVec3Dot(&MyAxisZ, &Cross)), 0.0f);
	Sep2 = D3DXVECTOR3(fabsf(D3DXVec3Dot(&TargetAxisX, &Cross)), fabsf(D3DXVec3Dot(&TargetAxisY, &Cross)), 0.0f);

	rA = Sep.x + Sep.y;
	rB = Sep2.x + Sep2.y;
	L = fabsf(D3DXVec3Dot(&Distance, &Cross));
	if (L > rA + rB)
	{
		return false;	// 衝突していない
	}

	// 自分のY方向と対象のX方向との外積の分離軸
	D3DXVec3Cross(&Cross, &MyAxisYNor, &TargetAxisXNor);

	Sep = D3DXVECTOR3(fabsf(D3DXVec3Dot(&MyAxisX, &Cross)), fabsf(D3DXVec3Dot(&MyAxisZ, &Cross)), 0.0f);
	Sep2 = D3DXVECTOR3(fabsf(D3DXVec3Dot(&TargetAxisY, &Cross)), fabsf(D3DXVec3Dot(&TargetAxisZ, &Cross)), 0.0f);

	rA = Sep.x + Sep.y;
	rB = Sep2.x + Sep2.y;
	L = fabsf(D3DXVec3Dot(&Distance, &Cross));
	if (L > rA + rB)
	{
		return false;	// 衝突していない
	}

	// 自分のY方向と対象のY方向との外積の分離軸
	D3DXVec3Cross(&Cross, &MyAxisYNor, &TargetAxisYNor);

	Sep = D3DXVECTOR3(fabsf(D3DXVec3Dot(&MyAxisX, &Cross)), fabsf(D3DXVec3Dot(&MyAxisZ, &Cross)), 0.0f);
	Sep2 = D3DXVECTOR3(fabsf(D3DXVec3Dot(&TargetAxisX, &Cross)), fabsf(D3DXVec3Dot(&TargetAxisZ, &Cross)), 0.0f);

	rA = Sep.x + Sep.y;
	rB = Sep2.x + Sep2.y;
	L = fabsf(D3DXVec3Dot(&Distance, &Cross));
	if (L > rA + rB)
	{
		return false;	// 衝突していない
	}

	// 自分のY方向と対象のZ方向との外積の分離軸
	D3DXVec3Cross(&Cross, &MyAxisYNor, &TargetAxisZNor);

	Sep = D3DXVECTOR3(fabsf(D3DXVec3Dot(&MyAxisX, &Cross)), fabsf(D3DXVec3Dot(&MyAxisZ, &Cross)), 0.0f);
	Sep2 = D3DXVECTOR3(fabsf(D3DXVec3Dot(&TargetAxisX, &Cross)), fabsf(D3DXVec3Dot(&TargetAxisY, &Cross)), 0.0f);

	rA = Sep.x + Sep.y;
	rB = Sep2.x + Sep2.y;
	L = fabsf(D3DXVec3Dot(&Distance, &Cross));
	if (L > rA + rB)
	{
		return false;	// 衝突していない
	}

	// 自分のZ方向と対象のX方向との外積の分離軸
	D3DXVec3Cross(&Cross, &MyAxisZNor, &TargetAxisXNor);

	Sep = D3DXVECTOR3(fabsf(D3DXVec3Dot(&MyAxisX, &Cross)), fabsf(D3DXVec3Dot(&MyAxisY, &Cross)), 0.0f);
	Sep2 = D3DXVECTOR3(fabsf(D3DXVec3Dot(&TargetAxisY, &Cross)), fabsf(D3DXVec3Dot(&TargetAxisZ, &Cross)), 0.0f);

	rA = Sep.x + Sep.y;
	rB = Sep2.x + Sep2.y;
	L = fabsf(D3DXVec3Dot(&Distance, &Cross));
	if (L > rA + rB)
	{
		return false;	// 衝突していない
	}

	// 自分のZ方向と対象のY方向との外積の分離軸
	D3DXVec3Cross(&Cross, &MyAxisZNor, &TargetAxisYNor);

	Sep = D3DXVECTOR3(fabsf(D3DXVec3Dot(&MyAxisX, &Cross)), fabsf(D3DXVec3Dot(&MyAxisY, &Cross)), 0.0f);
	Sep2 = D3DXVECTOR3(fabsf(D3DXVec3Dot(&TargetAxisX, &Cross)), fabsf(D3DXVec3Dot(&TargetAxisZ, &Cross)), 0.0f);

	rA = Sep.x + Sep.y;
	rB = Sep2.x + Sep2.y;
	L = fabsf(D3DXVec3Dot(&Distance, &Cross));
	if (L > rA + rB)
	{
		return false;	// 衝突していない
	}

	// 自分のZ方向と対象のZ方向との外積の分離軸
	D3DXVec3Cross(&Cross, &MyAxisZNor, &TargetAxisZNor);

	Sep = D3DXVECTOR3(fabsf(D3DXVec3Dot(&MyAxisX, &Cross)), fabsf(D3DXVec3Dot(&MyAxisY, &Cross)), 0.0f);
	Sep2 = D3DXVECTOR3(fabsf(D3DXVec3Dot(&TargetAxisX, &Cross)), fabsf(D3DXVec3Dot(&TargetAxisY, &Cross)), 0.0f);

	rA = Sep.x + Sep.y;
	rB = Sep2.x + Sep2.y;
	L = fabsf(D3DXVec3Dot(&Distance, &Cross));
	if (L > rA + rB)
	{
		return false;	// 衝突していない
	}

	PrintDebugProc("MyPos = {%.2f, %.2f, %.2f}\n", MyCollider.pos.x, MyCollider.pos.y, MyCollider.pos.z);;

	// 衝突した
	return true;
}

//=============================================================================
//	矩形と筒との当たり判定処理
//=============================================================================
bool CollisionBoxToCylinder(CollisionInfo& _CollisionInfo, ColliderType MyCollider, ColliderType TargetCollider, bool isTrigger)
{
	// 未実装
	return false;
}

//=============================================================================
//	矩形と球との当たり判定処理
//=============================================================================
bool CollisionBoxToSphere(CollisionInfo& _CollisionInfo, ColliderType MyCollider, ColliderType TargetCollider, bool isTrigger)
{
	// 未実装
	return false;
}

//=============================================================================
//	矩形とカプセルとの当たり判定処理
//=============================================================================
bool CollisionBoxToCapsule(CollisionInfo& _CollisionInfo, ColliderType MyCollider, ColliderType TargetCollider, bool isTrigger)
{
	// 未実装
	return false;
}

//=============================================================================
//	筒と矩形との当たり判定処理
//=============================================================================
bool CollisionCylinderToBox(CollisionInfo& _CollisionInfo, ColliderType MyCollider, ColliderType TargetCollider, bool isTrigger)
{
	// 未実装
	return false;
}

//=============================================================================
//	筒と筒との当たり判定処理
//=============================================================================
bool CollisionCylinderToCylinder(CollisionInfo& _CollisionInfo, ColliderType MyCollider, ColliderType TargetCollider, bool isTrigger)
{
	// 各距離を二乗したものをすべて足す
	float fDiff = powf(MyCollider.cylinder.pos.x - TargetCollider.cylinder.pos.x, 2) + 
		powf(MyCollider.cylinder.pos.z - TargetCollider.cylinder.pos.z, 2);

	// 総合より自分と対象の半径を足して二乗したもののほうが大きければ当たっている
	if (fDiff <= powf(MyCollider.cylinder.fRadius + TargetCollider.cylinder.fRadius, 2))
	{// 当たっていたら
		if (isTrigger)
		{// トリガーだったら
			return true;
		}

		// 距離から弾く角度を出す
		float fAngle = atan2f(MyCollider.cylinder.pos.x - TargetCollider.cylinder.pos.x, 
			MyCollider.cylinder.pos.z - TargetCollider.cylinder.pos.z);

		// 対象の原点から弾く方向に自分と対象の半径の合計分移動
		_CollisionInfo.Intersection.x = TargetCollider.cylinder.pos.x + sinf(fAngle) * (MyCollider.cylinder.fRadius + TargetCollider.cylinder.fRadius);
		_CollisionInfo.Intersection.y = MyCollider.cylinder.pos.y;
		_CollisionInfo.Intersection.z = TargetCollider.cylinder.pos.z + cosf(fAngle) * (MyCollider.cylinder.fRadius + TargetCollider.cylinder.fRadius);

		return true;
	}

	return false;
}

//=============================================================================
//	筒と球との当たり判定処理
//=============================================================================
bool CollisionCylinderToSphere(CollisionInfo& _CollisionInfo, ColliderType MyCollider, ColliderType TargetCollider, bool isTrigger)
{
	// 各距離を二乗したものをすべて足す
	float fDiff = powf(MyCollider.cylinder.pos.x - TargetCollider.cylinder.pos.x, 2) +
		powf(MyCollider.cylinder.pos.z - TargetCollider.cylinder.pos.z, 2);

	// 総合より自分と対象の半径を足して二乗したもののほうが大きければ当たっている
	if (fDiff <= powf(MyCollider.cylinder.fRadius + TargetCollider.cylinder.fRadius, 2))
	{// 当たっていたら
		if (isTrigger)
		{// トリガーだったら
			return true;
		}

		// 距離から弾く角度を出す
		float fAngle = atan2f(MyCollider.cylinder.pos.x - TargetCollider.cylinder.pos.x,
			MyCollider.cylinder.pos.z - TargetCollider.cylinder.pos.z);

		// 対象の原点から弾く方向に自分と対象の半径の合計分移動
		_CollisionInfo.Intersection.x = TargetCollider.cylinder.pos.x + sinf(fAngle) * (MyCollider.cylinder.fRadius + TargetCollider.cylinder.fRadius);
		_CollisionInfo.Intersection.y = MyCollider.cylinder.pos.y;
		_CollisionInfo.Intersection.z = TargetCollider.cylinder.pos.z + cosf(fAngle) * (MyCollider.cylinder.fRadius + TargetCollider.cylinder.fRadius);

		return true;
	}

	return false;
}

//=============================================================================
//	筒とカプセルとの当たり判定処理
//=============================================================================
bool CollisionCylinderToCapsule(CollisionInfo& _CollisionInfo, ColliderType MyCollider, ColliderType TargetCollider, bool isTrigger)
{
	// 未実装
	return false;
}

//=============================================================================
//	球と矩形との当たり判定処理
//=============================================================================
bool CollisionSphereToBox(CollisionInfo& _CollisionInfo, ColliderType MyCollider, ColliderType TargetCollider, bool isTrigger)
{
	// 計算用マトリックス
	D3DXMATRIX TargetmtxRot;

	// マトリックスの初期化
	D3DXMatrixIdentity(&TargetmtxRot);

	// 向きを算出
	D3DXMatrixRotationYawPitchRoll(&TargetmtxRot, TargetCollider.box.rot.y, TargetCollider.box.rot.x, TargetCollider.box.rot.z);

	// 対象の矩形の各軸ベクトル
	D3DXVECTOR3 TargetAxisXNor = { TargetmtxRot._11, TargetmtxRot._12, TargetmtxRot._13 },	// X軸
		TargetAxisX = TargetAxisXNor * TargetCollider.box.fWidth;
	D3DXVECTOR3 TargetAxisYNor = { TargetmtxRot._21, TargetmtxRot._22, TargetmtxRot._23 },	// Y軸
		TargetAxisY = TargetAxisYNor * TargetCollider.box.fHeight;
	D3DXVECTOR3 TargetAxisZNor = { TargetmtxRot._31, TargetmtxRot._32, TargetmtxRot._33 },	// Z軸
		TargetAxisZ = TargetAxisZNor * TargetCollider.box.fDepth;

	// 自分と対象の中心点とのベクトル
	D3DXVECTOR3 Distance = MyCollider.sphere.pos - TargetCollider.box.pos;

	// ローカル座標
	D3DXVECTOR3 MyCenter = D3DXVECTOR3(D3DXVec3Dot(&Distance, &TargetAxisXNor), 
		D3DXVec3Dot(&Distance, &TargetAxisYNor), 
		D3DXVec3Dot(&Distance, &TargetAxisZNor));

	// 交点
	D3DXVECTOR3 Intersection;

	// クランプ処理
	Intersection.x = max(-TargetCollider.box.fWidth, min(TargetCollider.box.fWidth, MyCenter.x));
	Intersection.y = max(-TargetCollider.box.fHeight, min(TargetCollider.box.fHeight, MyCenter.y));
	Intersection.z = max(-TargetCollider.box.fDepth, min(TargetCollider.box.fDepth, MyCenter.z));

	// 距離
	D3DXVECTOR3 diff = MyCenter - Intersection;
	float fDiff = D3DXVec3LengthSq(&diff);

	if (fDiff <= 0.0f)
	{// セーフティ
		fDiff += 0.001f;
	}

	if (fDiff < (MyCollider.sphere.fRadius * MyCollider.sphere.fRadius))
	{
		PrintDebugProc("test\n");
		float fDistance = sqrtf(fDiff);

		// 押し出しベクトル (ローカル)
		D3DXVECTOR3 vecPushNor = diff / fDistance;

		// 押し出し距離
		float fPush = MyCollider.sphere.fRadius - fDistance;

		// 押し出しベクトル (ワールド)
		D3DXVECTOR3 vecPush = (vecPushNor.x * TargetAxisXNor) + (vecPushNor.y * TargetAxisYNor) + (vecPushNor.z * TargetAxisZNor);

		// 距離を含んだ押し出しベクトル
		vecPush = vecPush * fPush;
		
		// 入り込んだ位置に押し出しベクトルを加算
		_CollisionInfo.Intersection = MyCollider.sphere.pos + vecPush;
		_CollisionInfo.isCollision = true;

		if (vecPush.y > 0.0f)
		{// 上方向への押し出しベクトルがあれば
			// 着地判定
			_CollisionInfo.isRand = true;
		}
	}

	// 衝突判定情報を返す
	return _CollisionInfo.isCollision;
}

//=============================================================================
//	球と筒との当たり判定処理
//=============================================================================
bool CollisionSphereToCylinder(CollisionInfo& _CollisionInfo, ColliderType MyCollider, ColliderType TargetCollider, bool isTrigger)
{
	// 各距離を二乗したものをすべて足す
	float fDiff = powf(MyCollider.sphere.pos.x - TargetCollider.cylinder.pos.x, 2) +
		powf(MyCollider.sphere.pos.z - TargetCollider.cylinder.pos.z, 2);

	// 総合より自分と対象の半径を足して二乗したもののほうが大きければ当たっている
	if (fDiff <= powf(MyCollider.sphere.fRadius + TargetCollider.cylinder.fRadius, 2))
	{// 当たっていたら
		if (isTrigger)
		{// トリガーだったら
			return true;
		}

		// 距離から弾く角度を出す
		float fAngle = atan2f(MyCollider.sphere.pos.x - TargetCollider.cylinder.pos.x,
			MyCollider.sphere.pos.z - TargetCollider.cylinder.pos.z);

		// 対象の原点から弾く方向に自分と対象の半径の合計分移動
		_CollisionInfo.Intersection.x = TargetCollider.cylinder.pos.x + sinf(fAngle) * (MyCollider.sphere.fRadius + TargetCollider.cylinder.fRadius);
		_CollisionInfo.Intersection.y = MyCollider.sphere.pos.y;
		_CollisionInfo.Intersection.z = TargetCollider.cylinder.pos.z + cosf(fAngle) * (MyCollider.sphere.fRadius + TargetCollider.cylinder.fRadius);

		return true;
	}

	return false;
}

//=============================================================================
//	球と球との当たり判定処理
//=============================================================================
bool CollisionSphereToSphere(CollisionInfo& _CollisionInfo, ColliderType MyCollider, ColliderType TargetCollider, bool isTrigger)
{
	// 各距離を二乗したものをすべて足す
	float fDiff = powf(MyCollider.sphere.pos.x - TargetCollider.sphere.pos.x, 2) + 
		powf(MyCollider.sphere.pos.y - TargetCollider.sphere.pos.y, 2) + 
		powf(MyCollider.sphere.pos.z - TargetCollider.sphere.pos.z, 2);

	// 総合より自分と対象の半径を足して二乗したもののほうが大きければ当たっている
	if (fDiff <= powf(MyCollider.sphere.fRadius + TargetCollider.sphere.fRadius, 2))
	{// 当たっていたら
		if (isTrigger)
		{// トリガーだったら
			return true;
		}

		// 弾く方向を決める (自分と対象の距離から出た向きを正規化)
		D3DXVECTOR3 VecDir = (MyCollider.sphere.pos - TargetCollider.sphere.pos);
		D3DXVec3Normalize(&VecDir, &VecDir);

		// 交点を算出 (対象の位置から弾く方向に向かって自分と対象の半径を足しただけ進める)
		_CollisionInfo.Intersection = (TargetCollider.sphere.pos +
			(VecDir * (MyCollider.sphere.fRadius + TargetCollider.sphere.fRadius)));

		return true;		// 衝突していたと返す
	}

	return false;			// 衝突していなかったと返す
}

//=============================================================================
//	球とカプセルとの当たり判定処理
//=============================================================================
bool CollisionSphereToCapsule(CollisionInfo& _CollisionInfo, ColliderType MyCollider, ColliderType TargetCollider, bool isTrigger)
{
	// 未実装
	return false;
}

//=============================================================================
//	メッシュコライダーの当たり判定処理
//=============================================================================
bool CollisionMeshCollider(CollisionInfo& _CollisionInfo, D3DXVECTOR3 pos, D3DXVECTOR3 posOld)
{
	MeshCollider* pMeshCollider = &g_aMeshCollider[0];	// メッシュコライダーへのポインタ

	D3DXVECTOR3 vecMove = pos - posOld;					// 移動ベクトル
	D3DXVECTOR3 vecLineA, vecLineB, vecLineC;			// 境界線ベクトル
	D3DXVECTOR3 vecToPos, vecToPosOld;					// 一点から移動前、移動後の点へのベクトル
	float fDist, fDistOld;								// それぞれの点と平面の距離
	bool isRand = false;								// 着地判定

	PrintDebugProc("vecmove = { %f %f %f } \n", vecMove.x, vecMove.y, vecMove.z);

	for (int nCntMeshColldier = 0; nCntMeshColldier < MAX_MESHCOLLIDER; nCntMeshColldier++, pMeshCollider++)
	{
		if (pMeshCollider->bUse == false)
		{// 使っていないメッシュは弾く
			continue;
		}

		// 三角形の数分
		for (int nCntTriangle = 0; nCntTriangle < pMeshCollider->nNumTriangle; nCntTriangle++)
		{
			// 3辺の境界線ベクトルを算出
			vecLineA = pMeshCollider->aTriangle[nCntTriangle].posB - pMeshCollider->aTriangle[nCntTriangle].posA;
			vecLineB = pMeshCollider->aTriangle[nCntTriangle].posC - pMeshCollider->aTriangle[nCntTriangle].posB;
			vecLineC = pMeshCollider->aTriangle[nCntTriangle].posA - pMeshCollider->aTriangle[nCntTriangle].posC;

			// 三角形の一点から移動後、移動前の位置へのベクトル
			vecToPos = pos - pMeshCollider->aTriangle[nCntTriangle].posA;
			vecToPosOld = posOld - pMeshCollider->aTriangle[nCntTriangle].posA;

			// 法線とそれぞれの点とのベクトルで内積を取り距離を算出
			fDist = D3DXVec3Dot(&vecToPos, &pMeshCollider->aTriangle[nCntTriangle].vecNor);
			fDistOld = D3DXVec3Dot(&vecToPosOld, &pMeshCollider->aTriangle[nCntTriangle].vecNor);

			// 
			fDist = fDist * fDistOld;

			if (fDist > 0.00001f && fDist < 1.0f)
			{// 誤差は切り捨て
				fDist = floorf(fDist);
			}

			if (fDist < -0.00001f && fDist > -1.0f)
			{// 誤差は切り捨て
				fDist = ceilf(fDist);		// 負の数は切り上げ
			}

			// それぞれの距離をかけてマイナスが入っていれば交差している (符号が違っているパターン)
			if (fDist > 0.0f)
			{// 符号が同じだったなら
				continue;
			}

			// 交点を算出する
			D3DXVECTOR3 Intersection = pMeshCollider->aTriangle[nCntTriangle].posA + vecToPosOld;

			// 交点と三角形のそれぞれの点とのベクトル
			D3DXVECTOR3 vecToPosA = Intersection - pMeshCollider->aTriangle[nCntTriangle].posA;
			D3DXVECTOR3 vecToPosB = Intersection - pMeshCollider->aTriangle[nCntTriangle].posB;
			D3DXVECTOR3 vecToPosC = Intersection - pMeshCollider->aTriangle[nCntTriangle].posC;

			// 点と移動後、移動前位置とのベクトルと交点と三角形のそれぞれの点とのベクトルで外積
			D3DXVec3Cross(&vecToPosA, &vecLineA, &vecToPosA);
			D3DXVec3Cross(&vecToPosB, &vecLineB, &vecToPosB);
			D3DXVec3Cross(&vecToPosC, &vecLineC, &vecToPosC);

			// 外積の結果と法線ベクトルで内積
			float fDotA = D3DXVec3Dot(&vecToPosA, &pMeshCollider->aTriangle[nCntTriangle].vecNor);
			float fDotB = D3DXVec3Dot(&vecToPosB, &pMeshCollider->aTriangle[nCntTriangle].vecNor);
			float fDotC = D3DXVec3Dot(&vecToPosC, &pMeshCollider->aTriangle[nCntTriangle].vecNor);

			// 誤差は切り捨て
			if (fDotA > 0.00001f && fDotA < 1.0f)
			{
				fDotA = floorf(fDotA);
			}

			if (fDotB > 0.00001f && fDotB < 1.0f)
			{
				fDotB = floorf(fDotB);
			}

			if (fDotC > 0.00001f && fDotC < 1.0f)
			{
				fDotC = floorf(fDotC);
			}

			// 負の数は切り上げ
			if (fDotA < -0.00001f && fDotA > -1.0f)
			{
				fDotA = ceilf(fDotA);
			}

			if (fDotB < -0.00001f && fDotB > -1.0f)
			{
				fDotB = ceilf(fDotB);
			}

			if (fDotC < -0.00001f && fDotC > -1.0f)
			{
				fDotC = ceilf(fDotC);
			}

			if (-fDotA >= 0.0f &&
				-fDotB >= 0.0f &&
				-fDotC >= 0.0f)
			{// 交点が三角形の範囲に含まれていれば
				// 面の角度をチェック
				PrintDebugProc("venNor.Y %f\n", pMeshCollider->aTriangle[nCntTriangle].vecNor.y);

				if (pMeshCollider->aTriangle[nCntTriangle].vecNor.y >= 0.75f)
				{// なだらかだったら
					isRand = true;

					// 移動ベクトルのY軸方向を打ち消す
					vecMove.y = -(vecMove.x * pMeshCollider->aTriangle[nCntTriangle].vecNor.x
						+ vecMove.z * pMeshCollider->aTriangle[nCntTriangle].vecNor.z)
						/ pMeshCollider->aTriangle[nCntTriangle].vecNor.y;
				}

				// 壁刷り距離
				float fDot = D3DXVec3Dot(&vecMove, &pMeshCollider->aTriangle[nCntTriangle].vecNor);

				// 交点に壁刷りベクトルを足して代入
				_CollisionInfo.Intersection = Intersection + (vecMove - (pMeshCollider->aTriangle[nCntTriangle].vecNor * fDot));
				_CollisionInfo.isCollision = true;
				break;
			}
		}
	}

	return isRand;
}

//=============================================================================
//	メッシュコライダーの読み込み処理
//=============================================================================
void LoadMeshColldier(const char* pColliderScript, D3DXVECTOR3 pos, D3DXVECTOR3 rot)
{
	D3DXMATRIX mtx, mtxRot, mtxTrans;			// 計算用マトリックス
	D3DXVECTOR3 vecLineB, vecLineC;				// 境界線ベクトル

	// メッシュコライダーファイルを開く
	FILE* pColliderFile = fopen(pColliderScript, "rb");

	if (pColliderFile == NULL)
	{// ファイルが開けなかったら
		return;
	}

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&mtx);

	// 向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, rot.y, rot.x, rot.z);
	D3DXMatrixMultiply(&mtx, &mtx, &mtxRot);

	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans, pos.x, pos.y, pos.z);
	D3DXMatrixMultiply(&mtx, &mtx, &mtxTrans);

	MeshCollider* pMeshCollider = &g_aMeshCollider[0];	// メッシュコライダーへのポインタ

	for (int nCntMeshCollider = 0; nCntMeshCollider < MAX_MESHCOLLIDER; nCntMeshCollider++, pMeshCollider++)
	{
		if (pMeshCollider->bUse == true)
		{// 使っていたら弾く
			continue;
		}

		// ファイルから情報を取得する
		fread(&pMeshCollider->nNumTriangle, sizeof(int), 1, pColliderFile);
		fread(&pMeshCollider->aTriangle[0], sizeof(Triangle), pMeshCollider->nNumTriangle, pColliderFile);
		pMeshCollider->bUse = true;

		for (int nCntTriangle = 0; nCntTriangle < pMeshCollider->nNumTriangle; nCntTriangle++)
		{
			// 位置と向きを反映した頂点座標を入れる
			D3DXVec3TransformCoord(&pMeshCollider->aTriangle[nCntTriangle].posA, &pMeshCollider->aTriangle[nCntTriangle].posA, &mtx);
			D3DXVec3TransformCoord(&pMeshCollider->aTriangle[nCntTriangle].posB, &pMeshCollider->aTriangle[nCntTriangle].posB, &mtx);
			D3DXVec3TransformCoord(&pMeshCollider->aTriangle[nCntTriangle].posC, &pMeshCollider->aTriangle[nCntTriangle].posC, &mtx);

			// 面のベクトルを算出
			vecLineB = pMeshCollider->aTriangle[nCntTriangle].posB - pMeshCollider->aTriangle[nCntTriangle].posA;
			vecLineC = pMeshCollider->aTriangle[nCntTriangle].posC - pMeshCollider->aTriangle[nCntTriangle].posB;
			pMeshCollider->aTriangle[nCntTriangle].vecNor = { (vecLineB.y * vecLineC.z) - (vecLineB.z * vecLineC.y), (vecLineB.z * vecLineC.x) - (vecLineB.x * vecLineC.z), (vecLineB.x * vecLineC.y) - (vecLineB.y * vecLineC.x) };
			D3DXVec3Normalize(&pMeshCollider->aTriangle[nCntTriangle].vecNor, &pMeshCollider->aTriangle[nCntTriangle].vecNor);

			// 逆ベクトルにする
			pMeshCollider->aTriangle[nCntTriangle].vecNor = -pMeshCollider->aTriangle[nCntTriangle].vecNor;
		}

		break;
	}

	fclose(pColliderFile);		// ファイルを閉じる
}

//=============================================================================
//	現在のコライダー数取得処理
//=============================================================================
int GetNumCollider(void)
{
	return g_nNumCollider;
}