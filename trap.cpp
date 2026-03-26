//=============================================================================
//
//	トラップ処理 [trap.cpp]
//	Author : SHUMA AIZU
// 
//=============================================================================

#include "main.h"
#include "game.h"
#include "trap.h"
#include "input.h"

#include "debugproc.h"

#include"shadow.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_TRAP			(256)		// トラップの最大数
#define MAX_TRAPPATTERN		(32)		// トラップパターンの最大数

//*****************************************************************************
// グローバル変数
//*****************************************************************************
Trap g_atrap[MAX_TRAP];							// トラップの情報
TrapPattern g_aTrapPattern[MAX_TRAPPATTERN];	// トラップパターンの情報
int g_nNumTrap;									// 現在のトラップ数
int g_nNumTrapPattern;							// 現在のトラップパターン数
int g_nCntGrid;									// 現在のグリッド
bool g_isTrapPattern[MAX_TRAPPATTERN];			// トラップパターンテーブル
int g_nNumUsePattern;							// 使用しているパターン数
float g_fGridPosX;								// 現在のグリッド位置
int g_nGridUsePattern[MAX_GRID];				// そのグリッドで使ったパターン

const int g_nLevelProbability[MAX_GAMELEVEL] =
{
	3, 5, 7, 9, 10
};

// トラップの半径
const float g_aTrapRadius[TRAPTYPE_MAX] =
{
	32.5f,
	20.0f,
};

// トラップの対応モデル
const MODELTYPE g_aTrapModelType[TRAPTYPE_MAX] =
{
	MODELTYPE_TRAP,
	MODELTYPE_TRAP_MINI,
};

//=============================================================================
//	トラップの初期化処理
//=============================================================================
void InitTrap(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	for (int nCntTrap = 0; nCntTrap < MAX_TRAP; nCntTrap++)
	{
		g_atrap[nCntTrap].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_atrap[nCntTrap].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_atrap[nCntTrap].nShadowIdx = -1;
		g_atrap[nCntTrap].bUse = false;
	}

	memset(&g_isTrapPattern[0], false, sizeof(bool) * MAX_TRAPPATTERN);
	memset(&g_nGridUsePattern[0], -1, sizeof(int) * MAX_GRID);

	g_nNumTrap = 0;
	g_nNumTrapPattern = 0;
	g_nCntGrid = 0;
	g_nNumUsePattern = 0;
	g_fGridPosX = INIT_GRIDPOSX;
}

//=============================================================================
//	トラップの終了処理
//=============================================================================
void UninitTrap(void)
{

}

//=============================================================================
//	トラップの描画処理
//=============================================================================
void DrawTrap(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// デバイスの取得
	D3DXMATRIX mtxRot, mtxTrans, mtxScale;		// 計算用マトリックス
	D3DMATERIAL9 matDef;						// 現在のマテリアル保存用
	D3DXMATERIAL* pMat;							// マテリアルデータへのポインタ

	Trap* pTrap = &g_atrap[0];

	for (int nCntTrap = 0; nCntTrap < MAX_TRAP; nCntTrap++, pTrap++)
	{
		if (pTrap->bUse == false)
		{// 使われていなければ戻る
			continue;
		}
		
		// ワールドマトリックスの初期化
		D3DXMatrixIdentity(&pTrap->mtxWorld);

		// 向きを反映
		D3DXMatrixRotationYawPitchRoll(&mtxRot, pTrap->rot.y, pTrap->rot.x, pTrap->rot.z);
		D3DXMatrixMultiply(&pTrap->mtxWorld, &pTrap->mtxWorld, &mtxRot);

		// 位置を反映
		D3DXMatrixTranslation(&mtxTrans, pTrap->pos.x, pTrap->pos.y, pTrap->pos.z);
		D3DXMatrixMultiply(&pTrap->mtxWorld, &pTrap->mtxWorld, &mtxTrans);

		// ワールドマトリックスの設定
		pDevice->SetTransform(D3DTS_WORLD, &pTrap->mtxWorld);

		// 現在のマテリアルを取得
		pDevice->GetMaterial(&matDef);

		// マテリアルデータへのポインタを取得
		pMat = (D3DXMATERIAL*)pTrap->NormalObjectData.pModelData->pBuffMat->GetBufferPointer();

		for (int nCntMat = 0; nCntMat < (int)pTrap->NormalObjectData.pModelData->dwNumMat; nCntMat++)
		{
			// マテリアルの設定
			pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

			// テクスチャの設定
			pDevice->SetTexture(0, pTrap->NormalObjectData.pModelData->apTexture[nCntMat]);

			// トラップ(パーツ)の描画
			pTrap->NormalObjectData.pModelData->pMesh->DrawSubset(nCntMat);
		}

		// 保存していたマテリアルを戻す
		pDevice->SetMaterial(&matDef);
	}
}

//=============================================================================
//	トラップの更新処理
//=============================================================================
void UpdateTrap(void)
{

}

//=============================================================================
//	トラップの当たり判定処理
//=============================================================================
bool CollisionTrap(D3DXVECTOR3 pos, float fRadius)
{
	Trap* pTrap = &g_atrap[0];	// トラップへのポインタ

	float fDiff = 0.0f;

	for (int nCntItem = 0; nCntItem < MAX_TRAP; nCntItem++, pTrap++)
	{
		if (pTrap->bUse == false)
		{// 使用していなかったら戻る
			continue;
		}

		// 各値を二乗して距離を算出
		fDiff = powf(pTrap->pos.x - pos.x, 2) + powf(pTrap->pos.y - pos.y, 2) + powf(pTrap->pos.z - pos.z, 2);

		if (fDiff <= powf(fRadius + g_aTrapRadius[pTrap->traptype], 2))
		{// 当たっていたら
			pTrap->bUse = false;
			return true;
		}
	}

	return false;
}

//=============================================================================
//	トラップの設定処理
//=============================================================================
void SetTrap(void)
{
	for (int nCntTrapPattern = 0; nCntTrapPattern < MAX_GRID; nCntTrapPattern++)
	{
		int nCurrentPattern = 0;

		// テーブル
		while (true)
		{
			nCurrentPattern = rand() % g_nNumTrapPattern;
			if (g_isTrapPattern[nCurrentPattern] == false)
			{
				g_isTrapPattern[nCurrentPattern] = true;
				g_nGridUsePattern[nCntTrapPattern] = nCurrentPattern;
				g_nNumUsePattern++;
				break;
			}
		}

		if (g_nNumUsePattern >= g_nNumTrapPattern)
		{
			memset(&g_isTrapPattern[0], false, sizeof(bool) * MAX_TRAPPATTERN);
			g_nNumUsePattern = 0;
		}

		TrapPattern* pTrapPattern = &g_aTrapPattern[nCurrentPattern];

		Trap* pTrap = &g_atrap[0];

		for (int nCntTrapInfo = 0; nCntTrapInfo < pTrapPattern->nNumTrap; nCntTrapInfo++)
		{
			if (rand() % 11 > g_nLevelProbability[GetGameLevel()])
			{
				continue;
			}


			for (int nCntTrap = 0; nCntTrap < MAX_TRAP; nCntTrap++, pTrap++)
			{
				if (pTrap->bUse == true)
				{
					continue;
				}

				pTrap->pos = pTrapPattern->aTrapInfo[nCntTrapInfo].pos;
				pTrap->rot = pTrapPattern->aTrapInfo[nCntTrapInfo].rot;
				pTrap->traptype = pTrapPattern->aTrapInfo[nCntTrapInfo].traptype;

				pTrap->pos.x += g_fGridPosX;

				// モデルデータ設定
				pTrap->NormalObjectData.pModelData = SetModelData(g_aTrapModelType[pTrap->traptype]);
				pTrap->nShadowIdx = SetShadow(g_aTrapRadius[pTrap->traptype]);
				SetPotisionShadow(pTrap->nShadowIdx, pTrap->pos, 1.0f);
				pTrap->bUse = true;

				g_nNumTrap++;
				break;
			}
		}

		g_nCntGrid++;
		g_fGridPosX += 250.0f;
	}

	g_fGridPosX = INIT_GRIDPOSX;
}

//=============================================================================
//	トラップパターンの設定処理
//=============================================================================
void SetTrapPattern(TrapPattern TrapPattern)
{
	g_aTrapPattern[g_nNumTrapPattern] = TrapPattern;

	g_nNumTrapPattern++;
}

//=============================================================================
//	トラップのリセット処理
//=============================================================================
void ResetTrap(void)
{
	for (int nCntTrap = 0; nCntTrap < MAX_TRAP; nCntTrap++)
	{
		if (g_atrap[nCntTrap].nShadowIdx != -1)
		{
			ReleaseShadow(g_atrap[nCntTrap].nShadowIdx);
		}

		g_atrap[nCntTrap].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_atrap[nCntTrap].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_atrap[nCntTrap].nShadowIdx = -1;
		g_atrap[nCntTrap].bUse = false;
	}

	g_nNumTrap = 0;
	g_nCntGrid = 0;
	g_nNumUsePattern = 0;
	g_fGridPosX = INIT_GRIDPOSX;
	memset(&g_nGridUsePattern[0], -1, sizeof(int) * MAX_GRID);
	memset(&g_isTrapPattern[0], false, sizeof(bool) * MAX_TRAPPATTERN);

	SetTrap();
}

//=============================================================================
//	グリッドで使っている番号の取得処理
//=============================================================================
int* GetGridUsePattern(void)
{
	return &g_nGridUsePattern[0];
}