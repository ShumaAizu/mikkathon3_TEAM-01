//=============================================================================
//
//	アイテム処理 [fallitem.cpp]
//	Author : SHUMA AIZU
// 
//=============================================================================

#include "main.h"
#include "fallitem.h"
#include "input.h"

#include "fallpoint.h"

#include "debugproc.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_FALLITEM			(16)								// アイテムの最大数
#define FALLITEM_COLLOFFSET		(D3DXVECTOR3(0.0f, 9.25f, 0.0f))	// アイテムの当たり判定用オフセット
#define FALLITEM_RADIUS			(13.5f)								// アイテムの半径
#define FALLITEM_MOVE			(D3DXVECTOR3(0.0f, -2.5f, 0.0f))	// 落下アイテムの移動速度

//*****************************************************************************
// グローバル変数
//*****************************************************************************
FallItem g_afallitem[MAX_FALLITEM];		// アイテムの情報

MODELTYPE g_ItemModelType[ITEMTYPE_MAX] =
{
	MODELTYPE_PRESENT,
};

//=============================================================================
//	アイテムの初期化処理
//=============================================================================
void InitFallItem(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	FallItem* pFallItem = &g_afallitem[0];	// アイテムへのポインタ

	for (int nCntFallItem = 0; nCntFallItem < MAX_FALLITEM; nCntFallItem++, pFallItem++)
	{
		pFallItem->pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		pFallItem->rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		pFallItem->move = FALLITEM_MOVE;
		pFallItem->bUse = false;
	}
}

//=============================================================================
//	アイテムの終了処理
//=============================================================================
void UninitFallItem(void)
{

}

//=============================================================================
//	アイテムの描画処理
//=============================================================================
void DrawFallItem(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// デバイスの取得
	D3DXMATRIX mtxRot, mtxTrans, mtxScale;		// 計算用マトリックス
	D3DMATERIAL9 matDef;						// 現在のマテリアル保存用
	D3DXMATERIAL* pMat;							// マテリアルデータへのポインタ

	FallItem* pFallItem = &g_afallitem[0];

	for (int nCntFallItem = 0; nCntFallItem < MAX_FALLITEM; nCntFallItem++, pFallItem++)
	{
		if (pFallItem->bUse == false)
		{// 使われていなければ戻る
			continue;
		}
		
		// ワールドマトリックスの初期化
		D3DXMatrixIdentity(&pFallItem->mtxWorld);

		// 向きを反映
		D3DXMatrixRotationYawPitchRoll(&mtxRot, pFallItem->rot.y, pFallItem->rot.x, pFallItem->rot.z);
		D3DXMatrixMultiply(&pFallItem->mtxWorld, &pFallItem->mtxWorld, &mtxRot);

		// 位置を反映
		D3DXMatrixTranslation(&mtxTrans, pFallItem->pos.x, pFallItem->pos.y, pFallItem->pos.z);
		D3DXMatrixMultiply(&pFallItem->mtxWorld, &pFallItem->mtxWorld, &mtxTrans);

		// ワールドマトリックスの設定
		pDevice->SetTransform(D3DTS_WORLD, &pFallItem->mtxWorld);

		// 現在のマテリアルを取得
		pDevice->GetMaterial(&matDef);

		// マテリアルデータへのポインタを取得
		pMat = (D3DXMATERIAL*)pFallItem->NormalObjectData.pModelData->pBuffMat->GetBufferPointer();

		for (int nCntMat = 0; nCntMat < (int)pFallItem->NormalObjectData.pModelData->dwNumMat; nCntMat++)
		{
			// マテリアルの設定
			pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

			// テクスチャの設定
			pDevice->SetTexture(0, pFallItem->NormalObjectData.pModelData->apTexture[nCntMat]);

			// アイテム(パーツ)の描画
			pFallItem->NormalObjectData.pModelData->pMesh->DrawSubset(nCntMat);
		}

		// 保存していたマテリアルを戻す
		pDevice->SetMaterial(&matDef);
	}
}

//=============================================================================
//	アイテムの更新処理
//=============================================================================
void UpdateFallItem(void)
{
	FallItem* pFallItem = &g_afallitem[0];	// アイテムへのポインタ

	for (int nCntFallItem = 0; nCntFallItem < MAX_FALLITEM; nCntFallItem++, pFallItem++)
	{
		if (pFallItem->bUse == false)
		{
			continue;
		}

		pFallItem->pos += pFallItem->move;

		if (pFallItem->pos.y < 0.0f)
		{
			pFallItem->bUse = false;
			CollisionFallPoint(pFallItem->pos);
		}
	}
}

//=============================================================================
//	アイテムの設定処理
//=============================================================================
void SetFallItem(D3DXVECTOR3 pos, D3DXVECTOR3 rot, ITEMTYPE itemtype)
{
	FallItem* pFallItem = &g_afallitem[0];

	for (int nCntItem = 0; nCntItem < MAX_FALLITEM; nCntItem++, pFallItem++)
	{
		if (pFallItem->bUse == true)
		{
			continue;
		}

		pFallItem->pos = pos;
		pFallItem->rot = rot;
		pFallItem->itemtype = itemtype;

		// モデルデータ設定
		pFallItem->NormalObjectData.pModelData = SetModelData(g_ItemModelType[itemtype]);
		pFallItem->bUse = true;
		break;
	}
}