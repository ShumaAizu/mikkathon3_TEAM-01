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

#include "trap.h"
#include "shadow.h"
#include "particle.h"
#include "player.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_ITEM			(128)								// アイテムの最大数
#define ITEM_COLLOFFSET		(D3DXVECTOR3(0.0f, 9.25f, 0.0f))	// アイテムの当たり判定用オフセット
#define ITEM_RADIUS			(13.5f)								// アイテムの半径
#define MAX_ITEMPATTERN		(16)								// アイテムのパターン数
#define MAGNET_AREA			(3)									// 引き寄せる範囲の倍率

#define ITEM_SPIN			(0.01f)								// アイテムの回転速度
#define ITEM_FLOATING		(0.01f)								// アイテム上下にふわふわ

//*****************************************************************************
// グローバル変数
//*****************************************************************************
Item g_aitem[MAX_ITEM];							// アイテムの情報
ItemPattern g_aItemPattern[MAX_ITEMPATTERN];	// トラップパターンの情報
int g_nNumItem;									// 現在のアイテム数
int g_nNumItemPattern;							// アイテムパターン数

// アイテムの半径
const float g_aItemRadius[ITEMTYPE_MAX] =
{
	ITEM_RADIUS,
};

MODELTYPE g_aItemModelType[ITEMTYPE_MAX] =
{
	MODELTYPE_BALLOONPRESENT,
};

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
		g_aitem[nCntItem].curve = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_aitem[nCntItem].nShadowIdx = -1;
		g_aitem[nCntItem].bMg = false;
		g_aitem[nCntItem].bUse = false;
	}

	g_nNumItem = 0;
	g_nNumItemPattern = 0;
}

//=============================================================================
//	アイテムの終了処理
//=============================================================================
void UninitItem(void)
{

}

//=============================================================================
//	アイテムの更新処理
//=============================================================================
void UpdateItem(void)
{
	Item* pItem = &g_aitem[0];	// アイテムへのポインタ

	for (int nCntItem = 0; nCntItem < MAX_ITEM; nCntItem++,pItem++)
	{
		if (pItem->bUse)
		{
			pItem->rot.y += ITEM_SPIN;
			pItem->curve.x += ITEM_FLOATING;

			if (D3DX_PI < pItem->rot.y)
				pItem->rot.y += -D3DX_PI * 2;

			if (D3DX_PI < pItem->curve.x)
				pItem->curve.x += -D3DX_PI * 2;

			pItem->pos.y += sinf(pItem->curve.x) * 0.1f;

		}
	}
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

//=========================================================================================
// UI表示用 描画の簡略化のための関数わけ
//=========================================================================================
void OnUIitemEnable(void)
{
	D3DXMATRIX			mtxView, mtxRot, mtxTrans, mtxWorld;	// マトリックス計算用
	ModelData*			pModel;						// モデルデータへのポインタ
	LPDIRECT3DDEVICE9	pDevice = GetDevice();		// デバイスへのポインタ
	D3DMATERIAL9		matDef;						// 現在のマテリアル保存用
	D3DXMATERIAL*		pMat;						// マテリアルデータへのポインタ
	vec3				pos = vec3_ZORO, rot = vec3_ZORO, posWin = vec3(90.0f, 50.0f, 0.0f);

	int* pItem = GetPlayerItem();

	for (int nCntItem = 0; nCntItem < MAX_GETITEM; nCntItem++, pItem++)
	{
		if (*pItem == -1)
		{// 使われていなければ戻る
			continue;
		}


		// カメラ設置
		SetUiCameraCenter(posWin, D3DXVECTOR2(80.0f, 80.0f));
		posWin.x += 80.0f;

		//------------------------------
		// モデル設置	
		// インデックスからモデルデータを取得
		pModel = SetModelData(MODELTYPE_PRESENT);

		//**************************************************************
		// ワールドマトリックスの初期化
		D3DXMatrixIdentity(&mtxWorld);

		// 向きを反映
		D3DXMatrixRotationYawPitchRoll(&mtxRot, rot.y, rot.x, rot.z);
		D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxRot);

		// 位置を反映
		D3DXMatrixTranslation(&mtxTrans, pos.x, pos.y, pos.z);
		D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxTrans);

		// ワールドマトリックスの設定
		pDevice->SetTransform(D3DTS_WORLD, &mtxWorld);

		//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
		// 現在のマテリアルを取得
		pDevice->GetMaterial(&matDef);

		// マテリアルデータへのポインタを取得
		pMat = (D3DXMATERIAL*)pModel->pBuffMat->GetBufferPointer();

		for (int nCntMat = 0; nCntMat < (int)pModel->dwNumMat; nCntMat++)
		{
			// マテリアルの設定
			pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

			// テクスチャの設定
			pDevice->SetTexture(0, pModel->apTexture[nCntMat]);

			// モデル(パーツ)の描画
			pModel->pMesh->DrawSubset(nCntMat);
		}

		// 保存していたマテリアルに戻す
		pDevice->SetMaterial(&matDef);
		//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
	}
}

//=============================================================================
//	アイテムの当たり判定処理
//=============================================================================
int CollisionItem(D3DXVECTOR3 pos, float fRadius)
{
	Item* pItem = &g_aitem[0];	// アイテムへのポインタ

	float fDiff = 0.0f;			// 実際の距離
	float fRange = 0.0f;		// あたる距離

	for (int nCntItem = 0; nCntItem < MAX_ITEM; nCntItem++, pItem++)
	{
		if (pItem->bUse == false)
		{// 使用していなかったら戻る
			continue;
		}

		D3DXVECTOR3 ItemPos = pItem->pos + pItem->CollOffset;

		// 各値を二乗して距離を算出
		fDiff = powf(ItemPos.x - pos.x, 2) + powf(ItemPos.y - pos.y, 2) + powf(ItemPos.z - pos.z, 2);
		fRange = powf(fRadius + pItem->fRadius, 2);

		if (fDiff <= fRange * MAGNET_AREA)
		{// アイテムの当たり判定の距離２倍の範囲内なら
			// 引き寄せON
			pItem->bMg = true;
			float fRatio = 1 - (fDiff / (fRange * MAGNET_AREA));
			pItem->pos += (pos - pItem->pos) * 0.08f * fRatio;

			// 当たり判定
			if (fDiff <= fRange)
			{// 当たっていたら
				pItem->bUse = false;
				ReleaseShadow(pItem->nShadowIdx);
				pItem->nShadowIdx = -1;
				SetParticle(pItem->pos, PARTICLE_ITEMGET);
				return (int)pItem->itemtype;
			}
		}

		// 引き寄せ
		if (pItem->bMg && pItem->bUse == true)
		{
		}
	}

	return -1;
}

//=============================================================================
//	アイテムの設定処理
//=============================================================================
void SetItem(void)
{
	int* pGridUsePattern = GetGridUsePattern();
	static float fGridPosX = INIT_GRIDPOSX;

	for (int nCntGrid = 0; nCntGrid < MAX_GRID; nCntGrid++, pGridUsePattern++)
	{
		ItemPattern* pItemPattern = &g_aItemPattern[*pGridUsePattern];

		Item* pItem = &g_aitem[0];

		for (int nCntItemInfo = 0; nCntItemInfo < pItemPattern->nNumItem; nCntItemInfo++)
		{
			if (rand() % 11 > 7)
			{
				continue;
			}


			for (int nCntItem = 0; nCntItem < MAX_ITEM; nCntItem++, pItem++)
			{
				if (pItem->bUse == true)
				{
					continue;
				}

				pItem->pos = pItemPattern->aItemInfo[nCntItemInfo].pos;
				pItem->rot = pItemPattern->aItemInfo[nCntItemInfo].rot;
				pItem->itemtype = pItemPattern->aItemInfo[nCntItemInfo].itemtype;
				pItem->fRadius = g_aItemRadius[pItem->itemtype];

				pItem->pos.x += fGridPosX;

				// モデルデータ設定
				pItem->NormalObjectData.pModelData = SetModelData(g_aItemModelType[pItem->itemtype]);
				pItem->nShadowIdx = SetShadow(g_aItemRadius[pItem->itemtype]);
				SetPotisionShadow(pItem->nShadowIdx, pItem->pos, 1.0f);
				pItem->bUse = true;
				pItem->bMg = false;

				g_nNumItem++;
				break;
			}
		}

		fGridPosX += 250.0f;
	}

	fGridPosX = INIT_GRIDPOSX;
}

//=============================================================================
//	アイテムパターンの設定処理
//=============================================================================
void SetItemPattern(ItemPattern ItemPattern)
{
	g_aItemPattern[g_nNumItemPattern] = ItemPattern;

	g_nNumItemPattern++;
}

//=============================================================================
//	アイテムのリセット処理
//=============================================================================
void ResetItem(void)
{
	for (int nCntItem = 0; nCntItem < MAX_ITEM; nCntItem++)
	{
		if (g_aitem[nCntItem].nShadowIdx != -1)
		{
			ReleaseShadow(g_aitem[nCntItem].nShadowIdx);
		}

		g_aitem[nCntItem].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_aitem[nCntItem].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_aitem[nCntItem].nShadowIdx = -1;
		g_aitem[nCntItem].bUse = false;
	}

	g_nNumItem = 0;
	SetItem();
}