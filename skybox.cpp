//=============================================================================
//
//	空処理 [skybox.cpp]
//	Author : SHUMA AIZU
// 
//=============================================================================

#include "main.h"
#include "skybox.h"
#include "input.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define SKYBOX_SPLIT_VERTICAL		(64 + 1)											// 縦の分割数
#define SKYBOX_SPLIT_HORIZONTAL		(64 + 1)											// 横の分割数
#define SKYBOX_ANGLE_VERTICAL		(D3DX_PI / (SKYBOX_SPLIT_VERTICAL))					// 縦の分割数に応じた角度
#define SKYBOX_ANGLE_HORIZONTAL		(D3DX_PI / (SKYBOX_SPLIT_HORIZONTAL - 1))			// 横の分割数に応じた角度
#define SKYBOX_MOVESPEED			(0.000025f)											// 空のテクスチャが動く速度

//*****************************************************************************
// グローバル変数
//*****************************************************************************
LPDIRECT3DTEXTURE9 g_pTextureSkyBox = NULL;		// テクスチャへのポインタ
SkyBox g_askybox[MAX_SKYBOX];					// 空の情報

//=============================================================================
//	空の初期化処理
//=============================================================================
void InitSkyBox(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	// テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice,
		"data\\TEXTURE\\sky000.jpg",
		&g_pTextureSkyBox);

	for (int nCntSkyBox = 0; nCntSkyBox < MAX_SKYBOX; nCntSkyBox++)
	{
		g_askybox[nCntSkyBox].pVtxBuff = NULL;
		g_askybox[nCntSkyBox].pIdxBuff = NULL;
		g_askybox[nCntSkyBox].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_askybox[nCntSkyBox].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_askybox[nCntSkyBox].tex = D3DXVECTOR2(0.0f, 0.0f);
		g_askybox[nCntSkyBox].fRadius = 15000.0f;
		g_askybox[nCntSkyBox].bUse = false;
	}

	SetSkyBox(D3DXVECTOR3(0.0f, -50.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), 5000.0f, SKYBOX_SPLIT_VERTICAL - 1, SKYBOX_SPLIT_HORIZONTAL - 1);
}

//=============================================================================
//	空の終了処理
//=============================================================================
void UninitSkyBox(void)
{
	// テクスチャの破棄
	if (g_pTextureSkyBox != NULL)
	{
		g_pTextureSkyBox->Release();
		g_pTextureSkyBox = NULL;
	}
	
	for (int nCntSkyBox = 0; nCntSkyBox < MAX_SKYBOX; nCntSkyBox++)
	{
		// 頂点バッファの破棄
		if (g_askybox[nCntSkyBox].pVtxBuff != NULL)
		{
			g_askybox[nCntSkyBox].pVtxBuff->Release();
			g_askybox[nCntSkyBox].pVtxBuff = NULL;
		}

		// インデックスバッファの破棄
		if (g_askybox[nCntSkyBox].pIdxBuff != NULL)
		{
			g_askybox[nCntSkyBox].pIdxBuff->Release();
			g_askybox[nCntSkyBox].pIdxBuff = NULL;
		}
	}
}

//=============================================================================
//	空の描画処理
//=============================================================================
void DrawSkyBox(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// デバイスの取得
	D3DXMATRIX mtxRot, mtxTrans;				// 計算用マトリックス

	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);		// ライトを無効にする

	// アルファテストを有効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);		// アルファテストを有効にする
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);	// 比較方法(基準値より大きければ描画)
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);					// アルファテストの参照値を設定(～以上で描画, intで設定)

	for (int nCntSkyBox = 0; nCntSkyBox < MAX_SKYBOX; nCntSkyBox++)
	{
		// ワールドマトリックスの初期化
		D3DXMatrixIdentity(&g_askybox[nCntSkyBox].mtxWorld);

		// 向きを反映
		D3DXMatrixRotationYawPitchRoll(&mtxRot, g_askybox[nCntSkyBox].rot.y, g_askybox[nCntSkyBox].rot.x, g_askybox[nCntSkyBox].rot.z);
		D3DXMatrixMultiply(&g_askybox[nCntSkyBox].mtxWorld, &g_askybox[nCntSkyBox].mtxWorld, &mtxRot);

		// 位置を反映
		D3DXMatrixTranslation(&mtxTrans, g_askybox[nCntSkyBox].pos.x, g_askybox[nCntSkyBox].pos.y, g_askybox[nCntSkyBox].pos.z);
		D3DXMatrixMultiply(&g_askybox[nCntSkyBox].mtxWorld, &g_askybox[nCntSkyBox].mtxWorld, &mtxTrans);

		// ワールドマトリックスの設定
		pDevice->SetTransform(D3DTS_WORLD, &g_askybox[nCntSkyBox].mtxWorld);

		// 頂点バッファをデータストリームに設定
		pDevice->SetStreamSource(0, g_askybox[nCntSkyBox].pVtxBuff, 0, sizeof(VERTEX_3D));

		// インデックスバッファをデータストリームに設定
		pDevice->SetIndices(g_askybox[nCntSkyBox].pIdxBuff);

		// 頂点フォーマットの設定
		pDevice->SetFVF(FVF_VERTEX_3D);

		// テクスチャの設定
		pDevice->SetTexture(0, g_pTextureSkyBox);

		if (g_askybox[nCntSkyBox].bUse == true)
		{
			// 空の描画
			pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP,
				0,
				0,
				(g_askybox[nCntSkyBox].nVertical - 1) * g_askybox[nCntSkyBox].nHorizontal,
				0,
				((g_askybox[nCntSkyBox].nHorizontal - 1) * (g_askybox[nCntSkyBox].nVertical - 2) * 2) + ((g_askybox[nCntSkyBox].nVertical - 3) * 4));

		}
	}

	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);		// ライトを有効にする

	// アルファテストを無効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);		// アルファテストを無効化
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);	// 比較方法(すべて描画)
	pDevice->SetRenderState(D3DRS_ALPHAREF, 255);				// 基準値を設定(すべてを描画している)
}

//=============================================================================
//	空の更新処理
//=============================================================================
void UpdateSkyBox(void)
{
	PSKYBOX pSkyBox = &g_askybox[0];

	for (int nCntSkyBox = 0; nCntSkyBox < MAX_SKYBOX; nCntSkyBox++, pSkyBox++)
	{
		if (pSkyBox->bUse == false)
		{
			continue;
		}

		pSkyBox->tex.x += SKYBOX_MOVESPEED;

		VERTEX_3D* pVtx;			// 頂点情報へのポインタ

		// 頂点バッファをロックし,頂点情報へのポインタを取得
		pSkyBox->pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

		float fAngleVertical = (D3DX_PI * 0.5f) - (D3DX_PI / pSkyBox->nVertical);

		for (int nCntVtxVertical = 0; nCntVtxVertical < pSkyBox->nVertical - 1; nCntVtxVertical++)
		{
			for (int nCntVtxHorizontal = 0; nCntVtxHorizontal < pSkyBox->nHorizontal; nCntVtxHorizontal++)
			{
				// テクスチャ座標の設定
				pVtx[nCntVtxHorizontal].tex = D3DXVECTOR2((1.0f / (pSkyBox->nHorizontal)) * nCntVtxHorizontal + pSkyBox->tex.x, (pSkyBox->tex.y / pSkyBox->nVertical) * nCntVtxVertical);
			}

			// ポインタを進める
			pVtx += pSkyBox->nHorizontal;
		}

		// 頂点バッファをアンロックする
		pSkyBox->pVtxBuff->Unlock();

	}
}

//=============================================================================
//	空の設定処理
//=============================================================================
void SetSkyBox(D3DXVECTOR3 pos, D3DXVECTOR3 rot, float fRadius, int nVertical, int nHorizontal)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	PSKYBOX pSkyBox = &g_askybox[0];

	for (int nCntSkyBox = 0; nCntSkyBox < MAX_SKYBOX; nCntSkyBox++, pSkyBox++)
	{
		if (pSkyBox->bUse == true)
		{
			continue;
		}

		// 分割数を頂点数に合わせる
		nVertical++;
		nHorizontal++;

		// 空の設定
		pSkyBox->pos = pos;
		pSkyBox->rot = rot;
		pSkyBox->tex = D3DXVECTOR2(1.0f, 1.0f);
		pSkyBox->fRadius = fRadius;
		pSkyBox->nVertical = nVertical;
		pSkyBox->nHorizontal = nHorizontal;

		// 頂点バッファの生成
		pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * (nVertical - 1) * nHorizontal,
			D3DUSAGE_WRITEONLY,
			FVF_VERTEX_3D,
			D3DPOOL_MANAGED,
			&pSkyBox->pVtxBuff,
			NULL);

		// 初期化
		VERTEX_3D* pVtx;			// 頂点情報へのポインタ

		// 頂点バッファをロックし,頂点情報へのポインタを取得
		pSkyBox->pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

		float fAngleVertical = (D3DX_PI * 0.5f) - (D3DX_PI / nVertical);

		for (int nCntVtxVertical = 0; nCntVtxVertical < nVertical - 1; nCntVtxVertical++)
		{
			D3DXVECTOR3 diff;
			D3DXVECTOR3 vecDir;
			float fRadius;
			float fAngleHorizontal = -D3DX_PI;

			diff = D3DXVECTOR3(sinf(0.0f) * pSkyBox->fRadius, cosf(fAngleVertical) * pSkyBox->fRadius, cosf(0.0f) * pSkyBox->fRadius);

			fRadius = SQRTF(diff.x, diff.y);

			for (int nCntVtxHorizontal = 0; nCntVtxHorizontal < nHorizontal; nCntVtxHorizontal++)
			{
				// 頂点座標の設定
				pVtx[nCntVtxHorizontal].pos = D3DXVECTOR3(sinf(fAngleHorizontal) * fRadius, sinf(fAngleVertical) * pSkyBox->fRadius, cosf(fAngleHorizontal) * fRadius);

				// 法線ベクトルの設定
				vecDir = D3DXVECTOR3(pVtx[nCntVtxHorizontal].pos.x, 0.0f, pVtx[nCntVtxHorizontal].pos.z);
				D3DXVec3Normalize(&vecDir, &vecDir);
				pVtx[nCntVtxHorizontal].nor = vecDir;

				// 頂点カラーの設定
				pVtx[nCntVtxHorizontal].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

				// テクスチャ座標の設定
				pVtx[nCntVtxHorizontal].tex = D3DXVECTOR2((1.0f / (nHorizontal / 4)) * nCntVtxHorizontal, (1.0f / nVertical) * nCntVtxVertical);

				// 角度を加算
				fAngleHorizontal += (D3DX_PI / (nHorizontal - 1)) * 2.0f;	// 合計6.28加算したいので倍にする
			}

			// 角度を減算
			fAngleVertical += -(D3DX_PI / nVertical) * 0.5f;			// 半円で留めたいので半分にして減算

			// ポインタを進める
			pVtx += nHorizontal;
		}

		// 頂点バッファをアンロックする
		pSkyBox->pVtxBuff->Unlock();

		// インデックスバッファの設定
		pDevice->CreateIndexBuffer(sizeof(WORD) * ((nHorizontal) * 2 * (nVertical - 2) + ((nVertical - 3) * 2)),
			D3DUSAGE_WRITEONLY,
			D3DFMT_INDEX16,
			D3DPOOL_MANAGED,
			&pSkyBox->pIdxBuff,
			NULL);

		WORD* pIdx;		// インデックス情報へのポインタ

		// インデックスバッファをロックし、頂点情報へのポインタを取得
		pSkyBox->pIdxBuff->Lock(0, 0, (void**)&pIdx, 0);

		for (int nCntIdxVertical = 0; nCntIdxVertical < nVertical - 2; nCntIdxVertical++)
		{
			for (int nCntIdxHorizontal = 0; nCntIdxHorizontal < nHorizontal; nCntIdxHorizontal++)
			{
				if (nCntIdxVertical != 0 && nCntIdxHorizontal == 0)
				{// 高さが最初の行以外かつ幅が最初の行
					pIdx[0] = (nHorizontal * (nCntIdxVertical + 1)) + nCntIdxHorizontal;

					pIdx++;
				}

				pIdx[0] = (nHorizontal * (nCntIdxVertical + 1)) + nCntIdxHorizontal;
				pIdx[1] = nHorizontal * nCntIdxVertical + nCntIdxHorizontal;

				pIdx += 2;

				if (nCntIdxVertical != nHorizontal - 2 && nCntIdxHorizontal == nHorizontal - 1)
				{// 高さが最後の行以外かつ幅が最後の行
					pIdx[0] = nHorizontal * nCntIdxVertical + nCntIdxHorizontal;

					pIdx++;
				}
			}
		}

		// インデックスバッファをアンロックする
		pSkyBox->pIdxBuff->Unlock();

		pSkyBox->bUse = true;
		break;
	}
}