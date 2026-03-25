//=============================================================================
//
//	アイテム処理 [item.cpp]
//	Author : SHUMA AIZU
// 
//=============================================================================

#include "main.h"
#include "item.h"
#include "input.h"

#include "debugproc.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_ITEM			(16)								// アイテムの最大数
#define ITEM_COLLOFFSET		(D3DXVECTOR3(0.0f, 9.25f, 0.0f))	// アイテムの当たり判定用オフセット
#define ITEM_RADIUS			(13.5f)								// アイテムの半径

//*****************************************************************************
// グローバル変数
//*****************************************************************************
Item g_aitem[MAX_ITEM];				// アイテムの情報
int g_nNumItemData;					// アイテムデータ数
int g_nNumItem;						// 現在のアイテム数

//=============================================================================
//	アイテムの初期化処理
//=============================================================================
void InitItem(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	for (int nCntItem = 0; nCntItem < MAX_ITEM; nCntItem++)
	{
		g_aitem[nCntItem].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_aitem[nCntItem].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_aitem[nCntItem].bUse = false;
	}

	g_nNumItem = 0;
	g_nNumItemData = 0;

	// test
	//SetItem(D3DXVECTOR3(500.0f, 150.0f, 0.0f), INIT_D3DXVEC3, ITEMTYPE_000, MODELTYPE_PRESENT);
}

//=============================================================================
//	アイテムの終了処理
//=============================================================================
void UninitItem(void)
{

}

//=============================================================================
//	アイテムの描画処理
//=============================================================================
void DrawItem(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// デバイスの取得
	D3DXMATRIX mtxRot, mtxTrans, mtxScale;		// 計算用マトリックス
	D3DMATERIAL9 matDef;						// 現在のマテリアル保存用
	D3DXMATERIAL* pMat;							// マテリアルデータへのポインタ

	Item* pItem = &g_aitem[0];

	for (int nCntItem = 0; nCntItem < MAX_ITEM; nCntItem++, pItem++)
	{
		if (pItem->bUse == false)
		{// 使われていなければ戻る
			continue;
		}
		
		// ワールドマトリックスの初期化
		D3DXMatrixIdentity(&pItem->mtxWorld);

		// 向きを反映
		D3DXMatrixRotationYawPitchRoll(&mtxRot, pItem->rot.y, pItem->rot.x, pItem->rot.z);
		D3DXMatrixMultiply(&pItem->mtxWorld, &pItem->mtxWorld, &mtxRot);

		// 位置を反映
		D3DXMatrixTranslation(&mtxTrans, pItem->pos.x, pItem->pos.y, pItem->pos.z);
		D3DXMatrixMultiply(&pItem->mtxWorld, &pItem->mtxWorld, &mtxTrans);

		// ワールドマトリックスの設定
		pDevice->SetTransform(D3DTS_WORLD, &pItem->mtxWorld);

		// 現在のマテリアルを取得
		pDevice->GetMaterial(&matDef);

		// マテリアルデータへのポインタを取得
		pMat = (D3DXMATERIAL*)pItem->NormalObjectData.pModelData->pBuffMat->GetBufferPointer();

		for (int nCntMat = 0; nCntMat < (int)pItem->NormalObjectData.pModelData->dwNumMat; nCntMat++)
		{
			// マテリアルの設定
			pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

			// テクスチャの設定
			pDevice->SetTexture(0, pItem->NormalObjectData.pModelData->apTexture[nCntMat]);

			// アイテム(パーツ)の描画
			pItem->NormalObjectData.pModelData->pMesh->DrawSubset(nCntMat);
		}

		// 保存していたマテリアルを戻す
		pDevice->SetMaterial(&matDef);
	}
}

//=============================================================================
//	アイテムの更新処理
//=============================================================================
void UpdateItem(void)
{

}

//=============================================================================
//	アイテムの当たり判定処理
//=============================================================================
int CollisionItem(D3DXVECTOR3 pos, float fRadius)
{
	Item* pItem = &g_aitem[0];	// アイテムへのポインタ

	float fDiff = 0.0f;

	for (int nCntItem = 0; nCntItem < MAX_ITEM; nCntItem++, pItem++)
	{
		if (pItem->bUse == false)
		{// 使用していなかったら戻る
			continue;
		}

		D3DXVECTOR3 ItemPos = pItem->pos + pItem->CollOffset;

		// 各値を二乗して距離を算出
		fDiff = powf(ItemPos.x - pos.x, 2) + powf(ItemPos.y - pos.y, 2) + powf(ItemPos.z - pos.z, 2);

		if (fDiff <= powf(fRadius + pItem->fRadius, 2))
		{// 当たっていたら
			pItem->bUse = false;
			return (int)pItem->itemtype;
		}
	}

	return -1;
}

//=============================================================================
//	アイテムの設定処理
//=============================================================================
void SetItem(D3DXVECTOR3 pos, D3DXVECTOR3 rot, ITEMTYPE itemtype, MODELTYPE ModelType)
{
	Item* pItem = &g_aitem[0];

	for (int nCntItem = 0; nCntItem < MAX_ITEM; nCntItem++, pItem++)
	{
		if (pItem->bUse == true)
		{
			continue;
		}

		pItem->pos = pos;
		pItem->rot = rot;
		pItem->itemtype = itemtype;

		switch (itemtype)
		{
		case ITEMTYPE_000:
			pItem->CollOffset = ITEM_COLLOFFSET;
			pItem->fRadius = ITEM_RADIUS;
			break;

		case ITEMTYPE_001:
			pItem->CollOffset = ITEM_COLLOFFSET;
			pItem->fRadius = ITEM_RADIUS;
			break;

		case ITEMTYPE_002:
			pItem->CollOffset = ITEM_COLLOFFSET;
			pItem->fRadius = ITEM_RADIUS;
			break;
		}

		// モデルデータ設定
		pItem->NormalObjectData.pModelData = SetModelData(ModelType);
		pItem->bUse = true;

		g_nNumItem++;
		break;
	}
}