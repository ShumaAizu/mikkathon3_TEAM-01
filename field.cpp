//=============================================================================
//
//	メッシュフィールド処理 [meshfield.cpp]
//	Author : SHUMA AIZU
// 
//=============================================================================

#include "main.h"
#include "field.h"
#include "input.h"

#include "debugproc.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define FIELD_SPLIT_WIDHT		(25 + 1)		// 横の分割数
#define FIELD_SPLIT_DEPTH		(25 + 1)		// 縦の分割数
#define MAX_FIELD				(32)			// メッシュフィールドの最大数

//*****************************************************************************
// グローバル変数
//*****************************************************************************
LPDIRECT3DTEXTURE9 g_apTextureField[FIELDTYPE_MAX] = {};		// テクスチャへのポインタ
Field g_aField[MAX_FIELD];										// メッシュフィールドの情報

const char* c_pFieldTextureName[FIELDTYPE_MAX] =
{
	"data\\TEXTURE\\stone001.jpg",
};

//=============================================================================
//	メッシュフィールドの初期化処理
//=============================================================================
void InitField(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	PFIELD pField = &g_aField[0];	// 先頭アドレス

	// テクスチャの読み込み
	for (int nCntTex = 0; nCntTex < FIELDTYPE_MAX; nCntTex++)
	{
		D3DXCreateTextureFromFile(pDevice,
			c_pFieldTextureName[nCntTex],
			&g_apTextureField[nCntTex]);
	}

	// 初期化
	memset(pField, NULL, sizeof(Field) * MAX_FIELD);

	// 設置
	SetField(D3DXVECTOR3(-5000.0f, 0.0f, 5000.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), 500.0f, 500.0f, FIELD_SPLIT_WIDHT, FIELD_SPLIT_DEPTH, FIELDTYPE_000);
}

//=============================================================================
//	メッシュフィールドの終了処理
//=============================================================================
void UninitField(void)
{
	PFIELD pField = &g_aField[0];		// 先頭アドレス

	// テクスチャの破棄
	for (int nCntTex = 0; nCntTex < FIELDTYPE_MAX; nCntTex++)
	{
		if (g_apTextureField[nCntTex] != NULL)
		{
			g_apTextureField[nCntTex]->Release();
			g_apTextureField[nCntTex] = NULL;
		}
	}

	for (int nCntField = 0; nCntField < MAX_FIELD; nCntField++, pField++)
	{
		// 頂点バッファの破棄
		if (pField->pVtxBuff != NULL)
		{
			pField->pVtxBuff->Release();
			pField->pVtxBuff = NULL;
		}

		// インデックスバッファの破棄
		if (pField->pIdxBuff != NULL)
		{
			pField->pIdxBuff->Release();
			pField->pIdxBuff = NULL;
		}
	}
}

//=============================================================================
//	メッシュフィールドの描画処理
//=============================================================================
void DrawField(void)
{
	PFIELD pField = &g_aField[0];	// 先頭アドレス

	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// デバイスの取得
	D3DXMATRIX mtxRot, mtxTrans;				// 計算用マトリックス

	for (int nCntField = 0; nCntField < MAX_FIELD; nCntField++, pField++)
	{
		if (pField->bUse == false/* || pField->bMesh == true*/)
		{// 使っていないもしくは当たり判定用メッシュなら弾く
			continue;
		}

		// ワールドマトリックスの初期化
		D3DXMatrixIdentity(&pField->mtxWorld);

		// 向きを反映
		D3DXMatrixRotationYawPitchRoll(&mtxRot, pField->rot.y, pField->rot.x, pField->rot.z);
		D3DXMatrixMultiply(&pField->mtxWorld, &pField->mtxWorld, &mtxRot);

		// 位置を反映
		D3DXMatrixTranslation(&mtxTrans, pField->pos.x, pField->pos.y, pField->pos.z);
		D3DXMatrixMultiply(&pField->mtxWorld, &pField->mtxWorld, &mtxTrans);

		// ワールドマトリックスの設定
		pDevice->SetTransform(D3DTS_WORLD, &pField->mtxWorld);

		// 頂点バッファをデータストリームに設定
		pDevice->SetStreamSource(0, pField->pVtxBuff, 0, sizeof(VERTEX_3D));

		// インデックスバッファをデータストリームに設定
		pDevice->SetIndices(pField->pIdxBuff);

		// 頂点フォーマットの設定
		pDevice->SetFVF(FVF_VERTEX_3D);

		// テクスチャの設定
		pDevice->SetTexture(0, g_apTextureField[0]);

		// メッシュフィールドの描画
		pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP,
			0,
			0,
			pField->nSplitWidth * pField->nSplitDepth,
			0,
			((pField->nSplitWidth - 1) * (pField->nSplitDepth - 1) * 2) + ((pField->nSplitDepth - 2) * 4));
	}
}

//=============================================================================
//	メッシュフィールドの更新処理
//=============================================================================
void UpdateField(void)
{

}

//=============================================================================
//	メッシュフィールドの設定処理
//=============================================================================
void SetField(D3DXVECTOR3 pos, D3DXVECTOR3 rot, float fWidth, float fDepth, int nSplitWidth, int nSplitDepth, FIELDTYPE type)
{
	PFIELD pField = &g_aField[0];	// 先頭アドレスポインタ

	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	for (int nCntField = 0; nCntField < MAX_FIELD; nCntField++, pField++)
	{
		if (pField->bUse == true)
		{// 使ってたら弾く
			continue;
		}

		// 分割数を頂点数に合わせる
		nSplitWidth++;
		nSplitDepth++;

		// メッシュフィールドの設定
		pField->pos = pos;
		pField->rot = rot;
		pField->tex = D3DXVECTOR2(1.0f, 1.0f);
		pField->type = type;
		pField->nSplitWidth = nSplitWidth;
		pField->nSplitDepth = nSplitDepth;

		// 頂点バッファの生成
		pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * nSplitWidth * nSplitDepth,
			D3DUSAGE_WRITEONLY,
			FVF_VERTEX_3D,
			D3DPOOL_MANAGED,
			&pField->pVtxBuff,
			NULL);

		// 初期化
		VERTEX_3D* pVtx;			// 頂点情報へのポインタ

		// 頂点バッファをロックし,頂点情報へのポインタを取得
		pField->pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

		for (int nCntVtxDepth = 0; nCntVtxDepth < nSplitDepth; nCntVtxDepth++)
		{
			for (int nCntVtxWidht = 0; nCntVtxWidht < nSplitWidth; nCntVtxWidht++)
			{
				pVtx[nCntVtxWidht].pos = D3DXVECTOR3(fWidth * nCntVtxWidht, 0.0f, -fDepth * nCntVtxDepth);
				pVtx[nCntVtxWidht].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
				pVtx[nCntVtxWidht].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
				pVtx[nCntVtxWidht].tex = D3DXVECTOR2(pField->tex.x * nCntVtxWidht, pField->tex.y * nCntVtxDepth);
			}

			pVtx += nSplitWidth;
		}

		// 頂点バッファをアンロックする
		pField->pVtxBuff->Unlock();

		// インデックスバッファの設定
		pDevice->CreateIndexBuffer(sizeof(WORD) * (nSplitWidth * 2 * (nSplitDepth - 1) + ((nSplitDepth - 2) * 2)),
			D3DUSAGE_WRITEONLY,
			D3DFMT_INDEX16,
			D3DPOOL_MANAGED,
			&pField->pIdxBuff,
			NULL);

		WORD* pIdx;		// インデックス情報へのポインタ

		// インデックスバッファをロックし、頂点情報へのポインタを取得
		pField->pIdxBuff->Lock(0, 0, (void**)&pIdx, 0);

		for (int nCntVtxDepth = 0; nCntVtxDepth < nSplitDepth - 1; nCntVtxDepth++)
		{
			for (int nCntVtxWidht = 0; nCntVtxWidht < nSplitWidth; nCntVtxWidht++)
			{
				if (nCntVtxDepth != 0 && nCntVtxWidht == 0)
				{// 奥行きが最初の行以外かつ幅が最初の行なら
					pIdx[0] = (nSplitWidth * (nCntVtxDepth + 1)) + nCntVtxWidht;
					pIdx++;
				}

				pIdx[0] = (nSplitWidth * (nCntVtxDepth + 1)) + nCntVtxWidht;
				pIdx[1] = nSplitWidth * nCntVtxDepth + nCntVtxWidht;

				pIdx += 2;

				if (nCntVtxWidht == nSplitWidth - 1 && nCntVtxDepth != nSplitDepth - 2)
				{// 幅が最後の行だったら
					pIdx[0] = nSplitWidth * nCntVtxDepth + nCntVtxWidht;
					pIdx++;
				}
			}
		}

		// インデックスバッファをアンロックする
		pField->pIdxBuff->Unlock();

		pField->bUse = true;
		break;
	}
}