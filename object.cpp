//=============================================================================
//
//	オブジェクトの処理 [object.h]
//	Author : SHUMA AIZU
// 
//=============================================================================

#include "object.h"
#include "loadscript.h"
#include "game.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_OBJECT		(256)	// 置けるオブジェクトの最大

//*****************************************************************************
// グローバル変数
//*****************************************************************************
Object g_aObject[MAX_OBJECT] = {};	// オブジェクト情報

//=============================================================================
// オブジェクトの初期化処理
//=============================================================================
void InitObject(void)
{
	// オブジェクトへのポインタ
	Object* pObject = &g_aObject[0];

	// 初期化
	memset(pObject, NULL, sizeof(Object) * MAX_OBJECT);
}

//=============================================================================
// オブジェクトの終了処理
//=============================================================================
void UninitObject(void)
{

}

//=============================================================================
// オブジェクトの更新処理
//=============================================================================
void UpdateObject(void)
{
	// オブジェクトへのポインタ
	Object* pObject = &g_aObject[0];

	for (int nCntObject = 0; nCntObject < MAX_OBJECT; nCntObject++, pObject++)
	{
		if (pObject->bUse == false)
		{// 使っていなければ弾く
			continue;
		}

		if (pObject->ObjectType == OBJECTTYPE_PARENT && pObject->ObjectInfo.ParentObject.motion.pMotionData != NULL)
		{// オブジェクトが階層構造モデルかつモーションがあるならば
			// モーションを更新
			UpdateMotion(&pObject->ObjectInfo.ParentObject.motion,
				pObject->ObjectInfo.ParentObject.pParentModelData,
				&pObject->ObjectInfo.ParentObject.OffSetData);
		}
	}
}

//==============================================================================
// オブジェクトの描画処理
//==============================================================================
void DrawObject(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// デバイスの取得
	D3DXMATRIX mtxRot, mtxTrans;				// 計算用マトリックス
	D3DMATERIAL9 matDef;						// 現在のマテリアルを保存
	D3DXMATERIAL* pMat;							// マテリアルデータへのポインタ

	// オブジェクトへのポインタ
	Object* pObject = &g_aObject[0];

	for (int nCntObject = 0; nCntObject < MAX_OBJECT; nCntObject++, pObject++)
	{
		if (pObject->bUse == false)
		{// 使っていなければ弾く
			continue;
		}

		if (pObject->ObjectType == OBJECTTYPE_NORMAL)
		{// 通常オブジェクト
			// ワールドマトリックスの初期化(デフォルトの値にする)
			D3DXMatrixIdentity(&pObject->mtxWorld);

			// 向きを反映
			D3DXMatrixRotationYawPitchRoll(&mtxRot, pObject->rot.y, pObject->rot.x, pObject->rot.z);
			D3DXMatrixMultiply(&pObject->mtxWorld, &pObject->mtxWorld, &mtxRot);

			// 位置を反映
			D3DXMatrixTranslation(&mtxTrans, pObject->pos.x, pObject->pos.y, pObject->pos.z);
			D3DXMatrixMultiply(&pObject->mtxWorld, &pObject->mtxWorld, &mtxTrans);

			// ワールドマトリックスの設定
			pDevice->SetTransform(D3DTS_WORLD, &pObject->mtxWorld);

			// 現在のマテリアルを取得
			pDevice->GetMaterial(&matDef);

			// マテリアルデータへのポインタを取得
			pMat = (D3DXMATERIAL*)pObject->ObjectInfo.NormalObject.pModelData->pBuffMat->GetBufferPointer();

			for (int nCntMat = 0; nCntMat < (int)pObject->ObjectInfo.NormalObject.pModelData->dwNumMat; nCntMat++)
			{
				// マテリアルの設定
				pDevice->SetMaterial(&pMat[nCntMat].MatD3D);
				
				pDevice->SetTexture(0, pObject->ObjectInfo.NormalObject.pModelData->apTexture[nCntMat]);
				
				// オブジェクトパーツの描画
				pObject->ObjectInfo.NormalObject.pModelData->pMesh->DrawSubset(nCntMat);	// ここでモデルの形を指定しているため、g_aObjectModelの中身を設定する必要がある
			}

			// 保存していたマテリアルに戻す
			pDevice->SetMaterial(&matDef);

		}
		else if (pObject->ObjectType == OBJECTTYPE_PARENT)
		{// 階層構造オブジェクト
			// ワールドマトリックスの初期化
			D3DXMatrixIdentity(&pObject->mtxWorld);

			// 向きを反映
			D3DXMatrixRotationYawPitchRoll(&mtxRot, pObject->rot.y, pObject->rot.x, pObject->rot.z);
			D3DXMatrixMultiply(&pObject->mtxWorld, &pObject->mtxWorld, &mtxRot);

			// 位置を反映
			D3DXMatrixTranslation(&mtxTrans, pObject->pos.x, pObject->pos.y, pObject->pos.z);
			D3DXMatrixMultiply(&pObject->mtxWorld, &pObject->mtxWorld, &mtxTrans);

			// ワールドマトリックスの設定
			pDevice->SetTransform(D3DTS_WORLD, &pObject->mtxWorld);

			// 現在のマテリアルを取得
			pDevice->GetMaterial(&matDef);

			// 全モデル(パーツ)の描画
			for (int nCntParentModel = 0; nCntParentModel < pObject->ObjectInfo.ParentObject.pParentModelData->nNumParts; nCntParentModel++)
			{
				D3DXMATRIX mtxRotOffSetModel, mtxTransOffSetModel;	// 計算用マトリックス
				D3DXMATRIX mtxParent;								// 親のマトリックス

				// パーツのワールドマトリックスを初期化
				D3DXMatrixIdentity(&pObject->ObjectInfo.ParentObject.pParentModelData->aModelData[nCntParentModel].mtxWorld);

				// パーツの向きを反映
				D3DXMatrixRotationYawPitchRoll(&mtxRotOffSetModel,
					pObject->ObjectInfo.ParentObject.OffSetData.rot[nCntParentModel].y, 
					pObject->ObjectInfo.ParentObject.OffSetData.rot[nCntParentModel].x, 
					pObject->ObjectInfo.ParentObject.OffSetData.rot[nCntParentModel].z);

				D3DXMatrixMultiply(&pObject->ObjectInfo.ParentObject.pParentModelData->aModelData[nCntParentModel].mtxWorld,
					&pObject->ObjectInfo.ParentObject.pParentModelData->aModelData[nCntParentModel].mtxWorld,
					&mtxRotOffSetModel);

				// パーツの位置を反映(オフセット)
				D3DXMatrixTranslation(&mtxTransOffSetModel, 
					pObject->ObjectInfo.ParentObject.OffSetData.pos[nCntParentModel].x,
					pObject->ObjectInfo.ParentObject.OffSetData.pos[nCntParentModel].y, 
					pObject->ObjectInfo.ParentObject.OffSetData.pos[nCntParentModel].z);

				D3DXMatrixMultiply(&pObject->ObjectInfo.ParentObject.pParentModelData->aModelData[nCntParentModel].mtxWorld,
					&pObject->ObjectInfo.ParentObject.pParentModelData->aModelData[nCntParentModel].mtxWorld,
					&mtxTransOffSetModel);

				// パーツの「親のマトリックス」を設定
				if (pObject->ObjectInfo.ParentObject.pParentModelData->aModelData[nCntParentModel].nIdxModelParent != -1)
				{// 親モデルがある場合
					mtxParent = pObject->ObjectInfo.ParentObject.pParentModelData->aModelData[pObject->ObjectInfo.ParentObject.pParentModelData->aModelData[nCntParentModel].nIdxModelParent].mtxWorld;
				}
				else
				{// 親モデルがない場合
					mtxParent = pObject->mtxWorld;
				}

				// 算出した「パーツのワールドマトリックス」と「親のマトリックス」を掛け合わせる
				D3DXMatrixMultiply(&pObject->ObjectInfo.ParentObject.pParentModelData->aModelData[nCntParentModel].mtxWorld,
					&pObject->ObjectInfo.ParentObject.pParentModelData->aModelData[nCntParentModel].mtxWorld,
					&mtxParent);

				// パーツのワールドマトリックスを設定
				pDevice->SetTransform(D3DTS_WORLD, &pObject->ObjectInfo.ParentObject.pParentModelData->aModelData[nCntParentModel].mtxWorld);

				// マテリアルデータへのポインタを取得
				pMat = (D3DXMATERIAL*)pObject->ObjectInfo.ParentObject.pParentModelData->aModelData[pObject->ObjectInfo.ParentObject.pParentModelData->aModelData[nCntParentModel].nIdxModel].pBuffMat->GetBufferPointer();


				for (int nCntMat = 0; nCntMat < (int)pObject->ObjectInfo.ParentObject.pParentModelData->aModelData[pObject->ObjectInfo.ParentObject.pParentModelData->aModelData[nCntParentModel].nIdxModel].dwNumMat; nCntMat++)
				{
					// マテリアルの設定
					pDevice->SetMaterial(&pMat->MatD3D);

					// テクスチャの設定
					pDevice->SetTexture(0, pObject->ObjectInfo.ParentObject.pParentModelData->aModelData[pObject->ObjectInfo.ParentObject.pParentModelData->aModelData[nCntParentModel].nIdxModel].apTexture[nCntMat]);

					// パーツの描画
					pObject->ObjectInfo.ParentObject.pParentModelData->aModelData[pObject->ObjectInfo.ParentObject.pParentModelData->aModelData[nCntParentModel].nIdxModel].pMesh->DrawSubset(nCntMat);
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

	for (int nCntModel = 0; nCntModel < MAX_EVENTOBJECT; nCntModel++, pObject++)
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
void SetObjectNormal(D3DXVECTOR3 pos, D3DXVECTOR3 rot, MODELTYPE type, ColliderInfo* pColliderInfo, int nNumCollider, bool isCollision, bool isCollider)
{
	// オブジェクトへのポインタ
	Object* pObject = &g_aObject[0];

	for (int nCntObject = 0; nCntObject < MAX_OBJECT; nCntObject++, pObject++)
	{
		if (pObject->bUse == true)
		{// 使っていたら弾く
			continue;
		}

		// 弧度法への変換
		rot.x = DegreeToRadian(rot.x);
		rot.y = DegreeToRadian(rot.y);
		rot.z = DegreeToRadian(rot.z);

		// 各種変数設定
		pObject->ObjectInfo.NormalObject.type = type;
		pObject->ObjectInfo.NormalObject.pModelData = SetModelData(type);
		pObject->pos = pos;
		pObject->rot = rot;
		pObject->fAlpha = 1.0f;
		pObject->bUse = true;

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
void SetObjectParent(D3DXVECTOR3 pos, D3DXVECTOR3 rot, PARENTMODELTYPE parentmodeltype, MOTIONDATATYPE motiondatatype, ColliderInfo* pColliderInfo, int nNumCollider, bool isCollision, bool isCollider)
{
	// オブジェクトへのポインタ
	Object* pObject = &g_aObject[0];

	for (int nCntParentObject = 0; nCntParentObject < MAX_OBJECT; nCntParentObject++, pObject++)
	{
		if (pObject->bUse == true)
		{// 使っていたら弾く
			continue;
		}

		// 弧度法への変換
		rot.x = DegreeToRadian(rot.x);
		rot.y = DegreeToRadian(rot.y);
		rot.z = DegreeToRadian(rot.z);

		// 階層構造モデル情報を設定
		pObject->ObjectInfo.ParentObject.pParentModelData = SetParentModelData(parentmodeltype);		// モデルデータ設定
		pObject->ObjectInfo.ParentObject.type = parentmodeltype;

		if (motiondatatype == MOTIONDATATYPE_MAX)
		{
			
		}
		else
		{
			// モーションデータのポインタを取得、保持
			pObject->ObjectInfo.ParentObject.motion.pMotionData = SetMotionData(motiondatatype);	// モーションデータ設定

			// ニュートラルモーションで開始
			SetMotion(&pObject->ObjectInfo.ParentObject.motion,
				pObject->ObjectInfo.ParentObject.pParentModelData,
				&pObject->ObjectInfo.ParentObject.OffSetData,
				MOTIONTYPE_NEUTRAL, true, false, 10);
		}

		// 各種設定
		pObject->pos = pos;							// 位置
		pObject->rot = rot;							// 向き
		pObject->fAlpha = 1.0f;						// アルファ値
		pObject->ObjectType = OBJECTTYPE_PARENT;	// 通常か階層構造か
		pObject->bUse = true;							// 使用状態に

		// オフセット情報を格納するメンバ変数へのポインタを取得
		ModelData* pModelData = &pObject->ObjectInfo.ParentObject.pParentModelData->aModelData[0];		// モデル情報
		D3DXVECTOR3* pOffSetPos = &pObject->ObjectInfo.ParentObject.OffSetData.pos[0];					// オブジェクトのオフセット座標
		D3DXVECTOR3* pOffSetRot = &pObject->ObjectInfo.ParentObject.OffSetData.rot[0];					// オブジェクトのオフセット向き

		for (int nCntModel = 0; nCntModel < pObject->ObjectInfo.ParentObject.pParentModelData->nNumParts; nCntModel++, pOffSetPos++, pOffSetRot++, pModelData++)
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