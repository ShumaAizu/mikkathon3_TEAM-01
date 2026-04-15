//=============================================================================
//
//	目標地点処理 [fallpoint.cpp]
//	Author : SHUMA AIZU
// 
//=============================================================================

#include "main.h"
#include "fallpoint.h"
#include "input.h"
#include "delivered.h"
#include "trap.h"
#include "tutorialboard.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define FALLPOINT_SPLIT		(64 + 1)							// 分割数
#define FALLPOINT_ANGLE		(D3DX_PI / (FALLPOINT_SPLIT - 1))
#define FALLPOINTTABLEX		(4)
#define FALLPOINTTABLEZ		(3)

//*****************************************************************************
// グローバル変数
//*****************************************************************************
LPDIRECT3DTEXTURE9 g_pTextureFallPoint = NULL;			// テクスチャへのポインタ
FallPoint g_aFallPoint[MAX_FALLPOINT];					// 目標地点の情報
int g_nFallPaintGrid;									// 目標地点のグリッド

bool g_isFallPointTableX[FALLPOINTTABLEX];				// X軸テーブル
bool g_isFallPointTableZ[FALLPOINTTABLEZ];				// Z軸テーブル

//=============================================================================
//	目標地点の初期化処理
//=============================================================================
void InitFallPoint(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	// テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice,
		"data\\TEXTURE\\ring000.jpg",
		&g_pTextureFallPoint);

	for (int nCntFallPoint = 0; nCntFallPoint < MAX_FALLPOINT; nCntFallPoint++)
	{
		g_aFallPoint[nCntFallPoint].pVtxBuff = NULL;
		g_aFallPoint[nCntFallPoint].pIdxBuff = NULL;
		g_aFallPoint[nCntFallPoint].bUse = false;
	}

	memset(&g_isFallPointTableX[0], false, sizeof(bool) * FALLPOINTTABLEX);
	memset(&g_isFallPointTableZ[0], false, sizeof(bool) * FALLPOINTTABLEZ);

	// 目標地点設定
	SetFallPoint();
}

//=============================================================================
//	目標地点の終了処理
//=============================================================================
void UninitFallPoint(void)
{
	// テクスチャの破棄
	if (g_pTextureFallPoint != NULL)
	{
		g_pTextureFallPoint->Release();
		g_pTextureFallPoint = NULL;
	}
	
	for (int nCntFallPoint = 0; nCntFallPoint < MAX_FALLPOINT; nCntFallPoint++)
	{
		// 頂点バッファの破棄
		if (g_aFallPoint[nCntFallPoint].pVtxBuff != NULL)
		{
			g_aFallPoint[nCntFallPoint].pVtxBuff->Release();
			g_aFallPoint[nCntFallPoint].pVtxBuff = NULL;
		}

		// インデックスバッファの破棄
		if (g_aFallPoint[nCntFallPoint].pIdxBuff != NULL)
		{
			g_aFallPoint[nCntFallPoint].pIdxBuff->Release();
			g_aFallPoint[nCntFallPoint].pIdxBuff = NULL;
		}

		g_aFallPoint[nCntFallPoint].bUse = false;
	}
}

//=============================================================================
//	目標地点の描画処理
//=============================================================================
void DrawFallPoint(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// デバイスの取得
	D3DXMATRIX mtxRot, mtxTrans;				// 計算用マトリックス

	for (int nCntFallPoint = 0; nCntFallPoint < MAX_FALLPOINT; nCntFallPoint++)
	{
		// ワールドマトリックスの初期化
		D3DXMatrixIdentity(&g_aFallPoint[nCntFallPoint].mtxWorld);

		// 向きを反映
		D3DXMatrixRotationYawPitchRoll(&mtxRot, g_aFallPoint[nCntFallPoint].rot.y, g_aFallPoint[nCntFallPoint].rot.x, g_aFallPoint[nCntFallPoint].rot.z);
		D3DXMatrixMultiply(&g_aFallPoint[nCntFallPoint].mtxWorld, &g_aFallPoint[nCntFallPoint].mtxWorld, &mtxRot);

		// 位置を反映
		D3DXMatrixTranslation(&mtxTrans, g_aFallPoint[nCntFallPoint].pos.x, g_aFallPoint[nCntFallPoint].pos.y, g_aFallPoint[nCntFallPoint].pos.z);
		D3DXMatrixMultiply(&g_aFallPoint[nCntFallPoint].mtxWorld, &g_aFallPoint[nCntFallPoint].mtxWorld, &mtxTrans);

		// ワールドマトリックスの設定
		pDevice->SetTransform(D3DTS_WORLD, &g_aFallPoint[nCntFallPoint].mtxWorld);

		// 頂点バッファをデータストリームに設定
		pDevice->SetStreamSource(0, g_aFallPoint[nCntFallPoint].pVtxBuff, 0, sizeof(VERTEX_3D));

		// インデックスバッファをデータストリームに設定
		pDevice->SetIndices(g_aFallPoint[nCntFallPoint].pIdxBuff);

		// 頂点フォーマットの設定
		pDevice->SetFVF(FVF_VERTEX_3D);

		// テクスチャの設定
		pDevice->SetTexture(0, g_pTextureFallPoint);

		if (g_aFallPoint[nCntFallPoint].bUse == true)
		{
			// 目標地点の描画
			pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP,
				0,
				0,
				FALLPOINT_SPLIT * FALLPOINT_SPLIT,
				0,
				(FALLPOINT_SPLIT - 1) + (FALLPOINT_SPLIT - 1));
		}
	}
}

//=============================================================================
//	目標地点の更新処理
//=============================================================================
void UpdateFallPoint(void)
{
	// 初期化
	VERTEX_3D* pVtx;			// 頂点情報へのポインタ

	for (int nCntFallPoint = 0; nCntFallPoint < MAX_FALLPOINT; nCntFallPoint++)
	{
		if (g_aFallPoint[nCntFallPoint].bUse == false)
		{
			continue;
		}

		// 頂点バッファをロックし,頂点情報へのポインタを取得
		g_aFallPoint[nCntFallPoint].pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

		float fAngle = D3DX_PI;

		for (int nCntVtx = 0; nCntVtx < FALLPOINT_SPLIT; nCntVtx++)
		{
			pVtx[0].pos = D3DXVECTOR3(sinf(fAngle) * g_aFallPoint[nCntFallPoint].fOutRadius, 0.0f, cosf(fAngle) * g_aFallPoint[nCntFallPoint].fOutRadius);
			pVtx[1].pos = D3DXVECTOR3(sinf(fAngle) * g_aFallPoint[nCntFallPoint].fInRadius, 0.0f, cosf(fAngle) * g_aFallPoint[nCntFallPoint].fInRadius);

			fAngle += -FALLPOINT_ANGLE * 2;
			pVtx += 2;
		}

		// 頂点バッファをアンロックする
		g_aFallPoint[nCntFallPoint].pVtxBuff->Unlock();
	}
}

//=============================================================================
//	目標地点の設定処理
//=============================================================================
void SetFallPoint(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	D3DXVECTOR3 pos = D3DXVECTOR3(-2500.0f, 1.0f, 0.0f);
	D3DXVECTOR3 rot = INIT_D3DXVEC3;
	int nTable = 0;

	static int nCntTableX = 0;
	static int nCntTableZ = 0;

	for (int nCntGrid = 0; nCntGrid < MAX_GRID; nCntGrid++)
	{
		int nNumFallPoint = rand() % 3 + 1;

		for (int nCntNumFallPoint = 0; nCntNumFallPoint < nNumFallPoint; nCntNumFallPoint++)
		{
			if (rand() % 11 / 6 == 0)
			{// 6割で設置スキップ
				continue;
			}

			FallPoint* pFallPoint = &g_aFallPoint[0];	// 目標地点へのポインタ

			for (int nCntFallPoint = 0; nCntFallPoint < MAX_FALLPOINT; nCntFallPoint++, pFallPoint++)
			{
				if (g_aFallPoint[nCntFallPoint].bUse == true)
				{
					continue;
				}

				// 目標地点の設定
				pFallPoint->pos = pos;
				pFallPoint->rot = rot;
				pFallPoint->fInRadius = FALLPOINT_INRADIUS;
				pFallPoint->fOutRadius = FALLPOINT_OUTRADIUS;
				pFallPoint->bUse = true;

				while (true)
				{
					nTable = rand() % FALLPOINTTABLEX;
					if (g_isFallPointTableX[nTable] == false)
					{
						g_isFallPointTableX[nTable] = true;
						nCntTableX++;
						if (nCntTableX >= FALLPOINTTABLEX)
						{
							memset(&g_isFallPointTableX[0], false, sizeof(bool) * FALLPOINTTABLEX);
							nCntTableX = 0;
						}
						break;
					}
				}

				pFallPoint->pos.x += (nCntGrid * 250.0f + 50.0f) + (nTable * 50.0f);

				while (true)
				{
					nTable = rand() % FALLPOINTTABLEZ;
					if (g_isFallPointTableZ[nTable] == false)
					{
						g_isFallPointTableZ[nTable] = true;
						nCntTableZ++;
						if (nCntTableZ >= FALLPOINTTABLEZ)
						{
							memset(&g_isFallPointTableZ[0], false, sizeof(bool) * FALLPOINTTABLEZ);
							nCntTableZ = 0;
						}
						break;
					}
				}
				pFallPoint->pos.z += -100.0f + (nTable * 125.0f);

				// 頂点バッファの生成
				pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * FALLPOINT_SPLIT * FALLPOINT_SPLIT,
					D3DUSAGE_WRITEONLY,
					FVF_VERTEX_3D,
					D3DPOOL_MANAGED,
					&g_aFallPoint[nCntFallPoint].pVtxBuff,
					NULL);

				// 初期化
				VERTEX_3D* pVtx;			// 頂点情報へのポインタ

				// 頂点バッファをロックし,頂点情報へのポインタを取得
				pFallPoint->pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

				float fAngle = D3DX_PI;

				for (int nCntVtx = 0; nCntVtx < FALLPOINT_SPLIT; nCntVtx++)
				{
					pVtx[0].pos = D3DXVECTOR3(sinf(fAngle) * pFallPoint->fOutRadius, 0.0f, cosf(fAngle) * pFallPoint->fOutRadius);
					pVtx[1].pos = D3DXVECTOR3(sinf(fAngle) * pFallPoint->fInRadius, 0.0f, cosf(fAngle) * pFallPoint->fInRadius);

					pVtx[0].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
					pVtx[1].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

					pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f);
					pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f);

					pVtx[0].tex = D3DXVECTOR2(1.0f * nCntVtx, 1.0f);
					pVtx[1].tex = D3DXVECTOR2(1.0f * nCntVtx, 0.0f);

					fAngle += -FALLPOINT_ANGLE * 2;
					pVtx += 2;
				}

				// 頂点バッファをアンロックする
				pFallPoint->pVtxBuff->Unlock();

				// インデックスバッファの設定
				pDevice->CreateIndexBuffer(sizeof(WORD) * FALLPOINT_SPLIT * FALLPOINT_SPLIT,
					D3DUSAGE_WRITEONLY,
					D3DFMT_INDEX16,
					D3DPOOL_MANAGED,
					&g_aFallPoint[nCntFallPoint].pIdxBuff,
					NULL);

				WORD* pIdx;		// インデックス情報へのポインタ

				// インデックスバッファをロックし、頂点情報へのポインタを取得
				g_aFallPoint[nCntFallPoint].pIdxBuff->Lock(0, 0, (void**)&pIdx, 0);

				for (int nCntVtx = 0; nCntVtx < FALLPOINT_SPLIT * FALLPOINT_SPLIT; nCntVtx++)
				{
					pIdx[nCntVtx] = nCntVtx;
				}

				// インデックスバッファをアンロックする
				g_aFallPoint[nCntFallPoint].pIdxBuff->Unlock();

				break;
			}
		}
	}
}

//=============================================================================
//	目標地点との当たり判定処理
//=============================================================================
void CollisionFallPoint(D3DXVECTOR3 pos)
{
	FallPoint* pFallPoint = &g_aFallPoint[0];	// 目標地点へのポインタ

	for (int nCntFallPoint = 0; nCntFallPoint < MAX_FALLPOINT; nCntFallPoint++, pFallPoint++)
	{
		if (pFallPoint->bUse == false)
		{// 使用していなかったら戻る
			continue;
		}

		float fDiff = SQRTF((pFallPoint->pos.x - pos.x), (pFallPoint->pos.z - pos.z));

		if (fDiff <= pFallPoint->fInRadius * 1.5f)
		{// 当たっていたら
			pFallPoint->bUse = false;
			if (GetTutorialLevel() == TUTORIALLEVEL_002 && IsNextTutorialLevel() == false)
			{
				NextTutorialLevel(5);
			}
			else if(GetTutorialLevel() == TUTORIALLEVEL_MAX)
			{
				AddDelivered(1);
			}
		}
	}
}