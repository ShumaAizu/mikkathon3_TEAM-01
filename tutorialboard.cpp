//=============================================================================
//
//	オブジェクトの処理 [tutorialboard.h]
//	Author : SHUMA AIZU
// 
//=============================================================================

#include "tutorialboard.h"
#include "loadscript.h"
#include "game.h"
#include "player.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_TUTORIALBOARD		(256)	// 置けるオブジェクトの最大
#define TUTORIALBOARDOFFSET		(D3DXVECTOR3(125.0f, 25.0f, 300.0f))
#define TUTORIALBOARDROT		(D3DXVECTOR3(0.0f, D3DX_PI * 0.125f, 0.0f))

//*****************************************************************************
// グローバル変数
//*****************************************************************************
Tutorialboard g_aTutorialboard[MAX_TUTORIALBOARD] = {};	// オブジェクト情報
TUTORIALLEVEL g_TutorialLevel;							// チュートリアルレベル
TUTORIALLEVEL g_NextTutorialLevel;						// 新しいチュートリアルレベル
int g_nCounterNextLevel;								// 
bool g_isNextTutorialLevel;								

//=============================================================================
// オブジェクトの初期化処理
//=============================================================================
void InitTutorialboard(void)
{
	// オブジェクトへのポインタ
	Tutorialboard* pTutorialboard = &g_aTutorialboard[0];

	// 初期化
	memset(pTutorialboard, NULL, sizeof(Tutorialboard) * MAX_TUTORIALBOARD);

	g_TutorialLevel = TUTORIALLEVEL_001;
	g_NextTutorialLevel = TUTORIALLEVEL_002;

	g_nCounterNextLevel = 0;
	g_isNextTutorialLevel = false;

	SetTutorialboardNormal(TUTORIALBOARDOFFSET, TUTORIALBOARDROT, MODELTYPE_TUTORIALBOARD);
}

//=============================================================================
// オブジェクトの終了処理
//=============================================================================
void UninitTutorialboard(void)
{

}

//=============================================================================
// オブジェクトの更新処理
//=============================================================================
void UpdateTutorialboard(void)
{
	// オブジェクトへのポインタ
	Tutorialboard* pTutorialboard = &g_aTutorialboard[0];

	static bool isIn = false, isOut = false;

	if (g_isNextTutorialLevel == true)
	{
		g_nCounterNextLevel--;
		if (g_nCounterNextLevel < 0)
		{
			isOut = true;
		}
	}

	for (int nCntTutorialboard = 0; nCntTutorialboard < MAX_TUTORIALBOARD; nCntTutorialboard++, pTutorialboard++)
	{
		if (pTutorialboard->bUse == false)
		{// 使っていなければ弾く
			continue;
		}

		if (isOut == true && g_isNextTutorialLevel == true)
		{
			pTutorialboard->pos.y += 5.0f;
			if (pTutorialboard->pos.y >= 500.0f)
			{
				isOut = false;
				g_isNextTutorialLevel = false;
				switch (g_TutorialLevel)
				{
				case TUTORIALLEVEL_001:
					g_TutorialLevel = TUTORIALLEVEL_002;
					isIn = true;
					break;

				case TUTORIALLEVEL_002:
					g_TutorialLevel = TUTORIALLEVEL_MAX;
					break;
				}
			}
		}

		if (isIn == true)
		{
			if (g_TutorialLevel == TUTORIALLEVEL_MAX)
			{
				pTutorialboard->bUse = false;
			}

			pTutorialboard->pos.y += -5.0f;
			if (pTutorialboard->pos.y <= 25.0f)
			{
				isIn = false;
			}
		}
	}
}

//==============================================================================
// オブジェクトの描画処理
//==============================================================================
void DrawTutorialboard(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// デバイスの取得
	D3DXMATRIX mtxRot, mtxTrans;				// 計算用マトリックス
	D3DMATERIAL9 matDef;						// 現在のマテリアルを保存
	D3DXMATERIAL* pMat;							// マテリアルデータへのポインタ
	D3DXVECTOR3 pos;

	// オブジェクトへのポインタ
	Tutorialboard* pTutorialboard = &g_aTutorialboard[0];

	for (int nCntTutorialboard = 0; nCntTutorialboard < MAX_TUTORIALBOARD; nCntTutorialboard++, pTutorialboard++)
	{
		if (pTutorialboard->bUse == false)
		{// 使っていなければ弾く
			continue;
		}

		if (pTutorialboard->ObjectType == OBJECTTYPE_NORMAL)
		{// 通常オブジェクト
			pos = pTutorialboard->pos + *pTutorialboard->OffPos;
			pos.z = 300.0f;

			// ワールドマトリックスの初期化(デフォルトの値にする)
			D3DXMatrixIdentity(&pTutorialboard->mtxWorld);

			// 向きを反映
			D3DXMatrixRotationYawPitchRoll(&mtxRot, pTutorialboard->rot.y, pTutorialboard->rot.x, pTutorialboard->rot.z);
			D3DXMatrixMultiply(&pTutorialboard->mtxWorld, &pTutorialboard->mtxWorld, &mtxRot);

			// 位置を反映
			D3DXMatrixTranslation(&mtxTrans, pos.x, pos.y, pos.z);
			D3DXMatrixMultiply(&pTutorialboard->mtxWorld, &pTutorialboard->mtxWorld, &mtxTrans);

			// ワールドマトリックスの設定
			pDevice->SetTransform(D3DTS_WORLD, &pTutorialboard->mtxWorld);

			// 現在のマテリアルを取得
			pDevice->GetMaterial(&matDef);

			// マテリアルデータへのポインタを取得
			pMat = (D3DXMATERIAL*)pTutorialboard->ObjectInfo.NormalObject.pModelData->pBuffMat->GetBufferPointer();

			for (int nCntMat = 0; nCntMat < (int)pTutorialboard->ObjectInfo.NormalObject.pModelData->dwNumMat; nCntMat++)
			{
				if (g_TutorialLevel != TUTORIALLEVEL_001 && nCntMat == 4)
				{
					continue;
				}

				if (g_TutorialLevel != TUTORIALLEVEL_002 && nCntMat == 5)
				{
					continue;
				}

				// マテリアルの設定
				pDevice->SetMaterial(&pMat[nCntMat].MatD3D);
				
				pDevice->SetTexture(0, pTutorialboard->ObjectInfo.NormalObject.pModelData->apTexture[nCntMat]);
				
				// オブジェクトパーツの描画
				pTutorialboard->ObjectInfo.NormalObject.pModelData->pMesh->DrawSubset(nCntMat);	// ここでモデルの形を指定しているため、g_aObjectModelの中身を設定する必要がある
			}

			// 保存していたマテリアルに戻す
			pDevice->SetMaterial(&matDef);

		}
		else if (pTutorialboard->ObjectType == OBJECTTYPE_PARENT)
		{// 階層構造オブジェクト
			// ワールドマトリックスの初期化
			D3DXMatrixIdentity(&pTutorialboard->mtxWorld);

			// 向きを反映
			D3DXMatrixRotationYawPitchRoll(&mtxRot, pTutorialboard->rot.y, pTutorialboard->rot.x, pTutorialboard->rot.z);
			D3DXMatrixMultiply(&pTutorialboard->mtxWorld, &pTutorialboard->mtxWorld, &mtxRot);

			// 位置を反映
			D3DXMatrixTranslation(&mtxTrans, pTutorialboard->pos.x, pTutorialboard->pos.y, pTutorialboard->pos.z);
			D3DXMatrixMultiply(&pTutorialboard->mtxWorld, &pTutorialboard->mtxWorld, &mtxTrans);

			// ワールドマトリックスの設定
			pDevice->SetTransform(D3DTS_WORLD, &pTutorialboard->mtxWorld);

			// 現在のマテリアルを取得
			pDevice->GetMaterial(&matDef);

			// 全モデル(パーツ)の描画
			for (int nCntParentModel = 0; nCntParentModel < pTutorialboard->ObjectInfo.ParentObject.pParentModelData->nNumParts; nCntParentModel++)
			{
				D3DXMATRIX mtxRotOffSetModel, mtxTransOffSetModel;	// 計算用マトリックス
				D3DXMATRIX mtxParent;								// 親のマトリックス

				// パーツのワールドマトリックスを初期化
				D3DXMatrixIdentity(&pTutorialboard->ObjectInfo.ParentObject.pParentModelData->aModelData[nCntParentModel].mtxWorld);

				// パーツの向きを反映
				D3DXMatrixRotationYawPitchRoll(&mtxRotOffSetModel,
					pTutorialboard->ObjectInfo.ParentObject.OffSetData.rot[nCntParentModel].y, 
					pTutorialboard->ObjectInfo.ParentObject.OffSetData.rot[nCntParentModel].x, 
					pTutorialboard->ObjectInfo.ParentObject.OffSetData.rot[nCntParentModel].z);

				D3DXMatrixMultiply(&pTutorialboard->ObjectInfo.ParentObject.pParentModelData->aModelData[nCntParentModel].mtxWorld,
					&pTutorialboard->ObjectInfo.ParentObject.pParentModelData->aModelData[nCntParentModel].mtxWorld,
					&mtxRotOffSetModel);

				// パーツの位置を反映(オフセット)
				D3DXMatrixTranslation(&mtxTransOffSetModel, 
					pTutorialboard->ObjectInfo.ParentObject.OffSetData.pos[nCntParentModel].x,
					pTutorialboard->ObjectInfo.ParentObject.OffSetData.pos[nCntParentModel].y, 
					pTutorialboard->ObjectInfo.ParentObject.OffSetData.pos[nCntParentModel].z);

				D3DXMatrixMultiply(&pTutorialboard->ObjectInfo.ParentObject.pParentModelData->aModelData[nCntParentModel].mtxWorld,
					&pTutorialboard->ObjectInfo.ParentObject.pParentModelData->aModelData[nCntParentModel].mtxWorld,
					&mtxTransOffSetModel);

				// パーツの「親のマトリックス」を設定
				if (pTutorialboard->ObjectInfo.ParentObject.pParentModelData->aModelData[nCntParentModel].nIdxModelParent != -1)
				{// 親モデルがある場合
					mtxParent = pTutorialboard->ObjectInfo.ParentObject.pParentModelData->aModelData[pTutorialboard->ObjectInfo.ParentObject.pParentModelData->aModelData[nCntParentModel].nIdxModelParent].mtxWorld;
				}
				else
				{// 親モデルがない場合
					mtxParent = pTutorialboard->mtxWorld;
				}

				// 算出した「パーツのワールドマトリックス」と「親のマトリックス」を掛け合わせる
				D3DXMatrixMultiply(&pTutorialboard->ObjectInfo.ParentObject.pParentModelData->aModelData[nCntParentModel].mtxWorld,
					&pTutorialboard->ObjectInfo.ParentObject.pParentModelData->aModelData[nCntParentModel].mtxWorld,
					&mtxParent);

				// パーツのワールドマトリックスを設定
				pDevice->SetTransform(D3DTS_WORLD, &pTutorialboard->ObjectInfo.ParentObject.pParentModelData->aModelData[nCntParentModel].mtxWorld);

				// マテリアルデータへのポインタを取得
				pMat = (D3DXMATERIAL*)pTutorialboard->ObjectInfo.ParentObject.pParentModelData->aModelData[pTutorialboard->ObjectInfo.ParentObject.pParentModelData->aModelData[nCntParentModel].nIdxModel].pBuffMat->GetBufferPointer();

				for (int nCntMat = 0; nCntMat < (int)pTutorialboard->ObjectInfo.ParentObject.pParentModelData->aModelData[pTutorialboard->ObjectInfo.ParentObject.pParentModelData->aModelData[nCntParentModel].nIdxModel].dwNumMat; nCntMat++)
				{

					// マテリアルの設定
					pDevice->SetMaterial(&pMat->MatD3D);

					// テクスチャの設定
					pDevice->SetTexture(0, pTutorialboard->ObjectInfo.ParentObject.pParentModelData->aModelData[pTutorialboard->ObjectInfo.ParentObject.pParentModelData->aModelData[nCntParentModel].nIdxModel].apTexture[nCntMat]);

					// パーツの描画
					pTutorialboard->ObjectInfo.ParentObject.pParentModelData->aModelData[pTutorialboard->ObjectInfo.ParentObject.pParentModelData->aModelData[nCntParentModel].nIdxModel].pMesh->DrawSubset(nCntMat);
				}
			}

			// 保存していたマテリアルを戻す
			pDevice->SetMaterial(&matDef);
		}
	}
}

//=============================================================================
//	オブジェクトの当たり判定処理
//=============================================================================
#if 0
bool CollisionObject(D3DXVECTOR3* pPos, D3DXVECTOR3* pPosOld, D3DXVECTOR3* pMove, int nCollisionIdx)
{
	Object* pObject = &g_aObject[0];	// 先頭アドレス
	bool isRand = false;							// 着地判定

	for (int nCntModel = 0; nCntModel < MAX_EVENTTUTORIALBOARD; nCntModel++, pObject++)
	{
		if (pObject->bUse == false)
		{// 使用していなかったら戻る
			continue;
		}

		if (pObject->nCollisionIdx != -1)
		{// 当たり判定が設定されていれば
			CollisionInfo CollisionInfo = UpdateCollision(nCollisionIdx, pObject->nCollisionIdx);

			if (CollisionInfo.isCollision)
			{// 当たっていれば
				*pPos = CollisionInfo.Intersection;
				isRand = CollisionInfo.isRand;
				break;
			}
		}
	}

	return isRand;	// 着地したかどうかを返す
}
#endif

//==============================================================================
// オブジェクトの設定処理
//==============================================================================
void SetTutorialboardNormal(D3DXVECTOR3 pos, D3DXVECTOR3 rot, MODELTYPE type)
{
	// オブジェクトへのポインタ
	Tutorialboard* pTutorialboard = &g_aTutorialboard[0];

	for (int nCntTutorialboard = 0; nCntTutorialboard < MAX_TUTORIALBOARD; nCntTutorialboard++, pTutorialboard++)
	{
		if (pTutorialboard->bUse == true)
		{// 使っていたら弾く
			continue;
		}

		//// 弧度法への変換
		//rot.x = DegreeToRadian(rot.x);
		//rot.y = DegreeToRadian(rot.y);
		//rot.z = DegreeToRadian(rot.z);

		// 各種変数設定
		pTutorialboard->ObjectInfo.NormalObject.type = type;
		pTutorialboard->ObjectInfo.NormalObject.pModelData = SetModelData(type);
		pTutorialboard->pos = pos;
		pTutorialboard->rot = rot;
		pTutorialboard->fAlpha = 1.0f;
		pTutorialboard->bUse = true;

		pTutorialboard->OffPos = &GetPlayer()->pos;

#if 0
		// 当たり判定
		if (isCollider == true)
		{// コライダーを使っているなら
			// 当たり判定を設定
			pObject->nCollisionIdx = SetCollision(pos, rot);

			// 使うコライダーの分だけ
			for (int nCntCollider = 0; nCntCollider < nNumCollider; nCntCollider++, pColliderInfo++)
			{
				// コライダー情報をポインタから設定する
				SetCollider(pObject->nCollisionIdx, *pColliderInfo);
			}
		}
		else
		{
			pObject->nCollisionIdx = -1;
		}
#endif

		break;
	}
}

//==============================================================================
// オブジェクトの設定処理
//==============================================================================
void SetTutorialboardParent(D3DXVECTOR3 pos, D3DXVECTOR3 rot, PARENTMODELTYPE parentmodeltype, MOTIONDATATYPE motiondatatype, ColliderInfo* pColliderInfo, int nNumCollider, bool isCollision, bool isCollider)
{
	// オブジェクトへのポインタ
	Tutorialboard* pTutorialboard = &g_aTutorialboard[0];

	for (int nCntParentObject = 0; nCntParentObject < MAX_TUTORIALBOARD; nCntParentObject++, pTutorialboard++)
	{
		if (pTutorialboard->bUse == true)
		{// 使っていたら弾く
			continue;
		}

		// 弧度法への変換
		rot.x = DegreeToRadian(rot.x);
		rot.y = DegreeToRadian(rot.y);
		rot.z = DegreeToRadian(rot.z);

		// 階層構造モデル情報を設定
		pTutorialboard->ObjectInfo.ParentObject.pParentModelData = SetParentModelData(parentmodeltype);		// モデルデータ設定
		pTutorialboard->ObjectInfo.ParentObject.type = parentmodeltype;

		if (motiondatatype == MOTIONDATATYPE_MAX)
		{
			
		}
		else
		{
			// モーションデータのポインタを取得、保持
			pTutorialboard->ObjectInfo.ParentObject.motion.pMotionData = SetMotionData(motiondatatype);	// モーションデータ設定

			// ニュートラルモーションで開始
			SetMotion(&pTutorialboard->ObjectInfo.ParentObject.motion,
				pTutorialboard->ObjectInfo.ParentObject.pParentModelData,
				&pTutorialboard->ObjectInfo.ParentObject.OffSetData,
				MOTIONTYPE_NEUTRAL, true, false, 10);
		}

		// 各種設定
		pTutorialboard->pos = pos;							// 位置
		pTutorialboard->rot = rot;							// 向き
		pTutorialboard->fAlpha = 1.0f;						// アルファ値
		pTutorialboard->ObjectType = OBJECTTYPE_PARENT;		// 通常か階層構造か
		pTutorialboard->bUse = true;							// 使用状態に

		// オフセット情報を格納するメンバ変数へのポインタを取得
		ModelData* pModelData = &pTutorialboard->ObjectInfo.ParentObject.pParentModelData->aModelData[0];		// モデル情報
		D3DXVECTOR3* pOffSetPos = &pTutorialboard->ObjectInfo.ParentObject.OffSetData.pos[0];					// オブジェクトのオフセット座標
		D3DXVECTOR3* pOffSetRot = &pTutorialboard->ObjectInfo.ParentObject.OffSetData.rot[0];					// オブジェクトのオフセット向き

		for (int nCntModel = 0; nCntModel < pTutorialboard->ObjectInfo.ParentObject.pParentModelData->nNumParts; nCntModel++, pOffSetPos++, pOffSetRot++, pModelData++)
		{// ローカル座標を設定
			*pOffSetPos += pModelData->posLocal;
			*pOffSetRot += pModelData->rotLocal;
		}

#if 0
		// 当たり判定
		if (isCollider == true)
		{// コライダーを使っているなら
			// 当たり判定を設定
			pObject->nCollisionIdx = SetCollision(pos, rot);

			// 使っているコライダーの分だけ
			for (int nCntCollider = 0; nCntCollider < nNumCollider; nCntCollider++, pColliderInfo++)
			{
				// コライダー情報をポインタから設定する
				SetCollider(pObject->nCollisionIdx, *pColliderInfo);
			}
		}
		else
		{// 使っていなければインデックスを無効に
			pObject->nCollisionIdx = -1;
		}

		break;
#endif
	}
}

//=============================================================================
//	チュートリアルレベル取得処理
//=============================================================================
TUTORIALLEVEL GetTutorialLevel(void)
{
	return g_TutorialLevel;
}

//=============================================================================
//	チュートリアルレベル進行処理
//=============================================================================
void NextTutorialLevel(int nCounter)
{
	g_isNextTutorialLevel = true;
	g_nCounterNextLevel = nCounter;
}

//=============================================================================
//	チュートリアルレベル取得処理
//=============================================================================
bool IsNextTutorialLevel(void)
{
	return g_isNextTutorialLevel;
}