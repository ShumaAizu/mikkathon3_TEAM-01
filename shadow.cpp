//==================================================================================================================================
// 
//			影の処理 [shadow.cpp]
//			Author : ENDO HIDETO
// 
//==================================================================================================================================
//**************************************************************
// インクルード
#include "input.h"
#include "shadow.h"

//**************************************************************
//グローバル変数
LPDIRECT3DTEXTURE9 g_pTextureShadow = NULL;				//テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffShadow = NULL;		//頂点バッファへのポインタ
Shadow g_aShadow[MAX_SHADOW];

//**************************************************************
//プロトタイプ宣言

//=========================================================================================
// 影の初期化処理
//=========================================================================================
void InitShadow(void)
{
	//**************************************************************
	//変数宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();		//デバイスへのポインタ
	VERTEX_3D* pVtx;								//頂点情報へのポインタ

	//**************************************************************
	//テクスチャ読み込み
	D3DXCreateTextureFromFile(pDevice,"data\\TEXTURE\\shadow000.jpg",&g_pTextureShadow);

	//頂点バッファの読み込み
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D)* 4 * MAX_SHADOW,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&g_pVtxBuffShadow,
		NULL);

	//**************************************************************
	//位置・サイズの初期化
	for (int nCntShadow = 0; nCntShadow < MAX_SHADOW; nCntShadow++)
	{
		g_aShadow[nCntShadow].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_aShadow[nCntShadow].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_aShadow[nCntShadow].size = D3DXVECTOR3(SHADOW_SIZE, 0.0f, SHADOW_SIZE);
		g_aShadow[nCntShadow].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.8f);
		g_aShadow[nCntShadow].bUse = false;
	}

	//**************************************************************
	//頂点バッファをロックし、頂点情報へのポインタを取得
	g_pVtxBuffShadow->Lock(0, 0,(void**)&pVtx, 0);

	for (int nCntShadow = 0; nCntShadow < MAX_SHADOW; nCntShadow++, pVtx += 4)
	{
		//頂点座標を設定
		pVtx[0].pos = D3DXVECTOR3(-g_aShadow[nCntShadow].size.x, 0.0f, g_aShadow[nCntShadow].size.z);
		pVtx[1].pos = D3DXVECTOR3( g_aShadow[nCntShadow].size.x, 0.0f, g_aShadow[nCntShadow].size.z);
		pVtx[2].pos = D3DXVECTOR3(-g_aShadow[nCntShadow].size.x, 0.0f,-g_aShadow[nCntShadow].size.z);
		pVtx[3].pos = D3DXVECTOR3( g_aShadow[nCntShadow].size.x, 0.0f,-g_aShadow[nCntShadow].size.z);

		//法線ベクトルの設定
		pVtx[0].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		pVtx[1].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		pVtx[2].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		pVtx[3].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

		//頂点カラー設定
		pVtx[0].col = g_aShadow[nCntShadow].col;
		pVtx[1].col = g_aShadow[nCntShadow].col;
		pVtx[2].col = g_aShadow[nCntShadow].col;
		pVtx[3].col = g_aShadow[nCntShadow].col;

		//テクスチャの座標設定
		pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
		pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
		pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);
	}
	//頂点バッファのロック解除
	g_pVtxBuffShadow->Unlock();
}

//=========================================================================================
// 影の終了処理
//=========================================================================================
void UninitShadow(void)
{
	if (g_pTextureShadow != NULL)
	{
		g_pTextureShadow->Release();
		g_pTextureShadow = NULL;
	}

	if (g_pVtxBuffShadow != NULL)
	{
		g_pVtxBuffShadow->Release();
		g_pVtxBuffShadow = NULL;
	}
}

//=========================================================================================
// 影の更新処理
//=========================================================================================
void UpdateShadow(void)
{

}

//=========================================================================================
// 影の描画処理
//=========================================================================================
void DrawShadow(void)
{
	//**************************************************************
	//変数宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();		//デバイスへのポインタ
	D3DXMATRIX mtxRot, mtxTrans;					//マトリックス計算用

	for (int nCntShadow = 0; nCntShadow < MAX_SHADOW; nCntShadow++)
	{
		if (g_aShadow[nCntShadow].bUse)
		{//使用されている影なら

			//**************************************************************
			//ワールドマトリックスの初期化
			D3DXMatrixIdentity(&g_aShadow[nCntShadow].mtxWorld);

			//向きを反映
			D3DXMatrixRotationYawPitchRoll(&mtxRot, g_aShadow[nCntShadow].rot.y, g_aShadow[nCntShadow].rot.x, g_aShadow[nCntShadow].rot.z);
			D3DXMatrixMultiply(&g_aShadow[nCntShadow].mtxWorld, &g_aShadow[nCntShadow].mtxWorld, &mtxRot);

			//位置を反映
			D3DXMatrixTranslation(&mtxTrans, g_aShadow[nCntShadow].pos.x, g_aShadow[nCntShadow].pos.y, g_aShadow[nCntShadow].pos.z);
			D3DXMatrixMultiply(&g_aShadow[nCntShadow].mtxWorld, &g_aShadow[nCntShadow].mtxWorld, &mtxTrans);

			//ワールドマトリックスの設定
			pDevice->SetTransform(D3DTS_WORLD, &g_aShadow[nCntShadow].mtxWorld);


			//**************************************************************
			//頂点バッファをデータストリームに設定
			pDevice->SetStreamSource(0,
				g_pVtxBuffShadow,
				0,
				sizeof(VERTEX_3D));

			//頂点フォーマットの設定
			pDevice->SetFVF(FVF_VERTEX_3D);

			//テクスチャの設定
			pDevice->SetTexture(0, g_pTextureShadow);

			//**************************************************************
			//アルファブレンドを減算合成に設定
			pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_REVSUBTRACT);
			pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

			//影の描画
			pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 4 * nCntShadow, 2);

			//アルファブレンドを元に戻す
			pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
			pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);


		}
	}
}

//=========================================================================================
// 影の番号を取得
//=========================================================================================
int SetShadow(float fSize)
{
	//**************************************************************
	//変数宣言
	VERTEX_3D* pVtx;								//頂点情報へのポインタ

	for (int nCntShadow = 0; nCntShadow < MAX_SHADOW; nCntShadow++)
	{
		if (g_aShadow[nCntShadow].bUse == false)
		{
			g_aShadow[nCntShadow].size = D3DXVECTOR3(fSize,0.0f,fSize);
			g_aShadow[nCntShadow].bUse = true;

			//**************************************************************
			//頂点バッファをロックし、頂点情報へのポインタを取得
			g_pVtxBuffShadow->Lock(0, 0, (void**)&pVtx, 0);
			pVtx += 4 * nCntShadow;

			//頂点座標を設定
			pVtx[0].pos = D3DXVECTOR3(-g_aShadow[nCntShadow].size.x, 0.0f,  g_aShadow[nCntShadow].size.z);
			pVtx[1].pos = D3DXVECTOR3( g_aShadow[nCntShadow].size.x, 0.0f,  g_aShadow[nCntShadow].size.z);
			pVtx[2].pos = D3DXVECTOR3(-g_aShadow[nCntShadow].size.x, 0.0f, -g_aShadow[nCntShadow].size.z);
			pVtx[3].pos = D3DXVECTOR3( g_aShadow[nCntShadow].size.x, 0.0f, -g_aShadow[nCntShadow].size.z);

			//頂点バッファのロック解除
			g_pVtxBuffShadow->Unlock();

			return nCntShadow;
		}
	}

	return -1;
}

//=========================================================================================
// 影の位置をセット
//=========================================================================================
void SetPotisionShadow(int nIdxShadow, D3DXVECTOR3 pos, float fShadowHeight)
{
	//**************************************************************
	//変数宣言
	VERTEX_3D* pVtx;				//頂点情報へのポインタ
	float fSpread;					//ぼやかす倍率

	//位置を保存
	g_aShadow[nIdxShadow].pos = D3DXVECTOR3(pos.x, fShadowHeight, pos.z);

	if (fShadowHeight < pos.y && -1 < nIdxShadow)
	{//影より実体が高いところにあったら
		fSpread = (pos.y - fShadowHeight) * 0.1f;

		//**************************************************************
		//頂点バッファをロックし、頂点情報へのポインタを取得
		g_pVtxBuffShadow->Lock(0, 0, (void**)&pVtx, 0);
		pVtx += nIdxShadow * 4;

		//頂点座標を設定
		pVtx[0].pos = D3DXVECTOR3(-g_aShadow[nIdxShadow].size.x - fSpread, 0.0f,  g_aShadow[nIdxShadow].size.z + fSpread);
		pVtx[1].pos = D3DXVECTOR3( g_aShadow[nIdxShadow].size.x + fSpread, 0.0f,  g_aShadow[nIdxShadow].size.z + fSpread);
		pVtx[2].pos = D3DXVECTOR3(-g_aShadow[nIdxShadow].size.x - fSpread, 0.0f, -g_aShadow[nIdxShadow].size.z - fSpread);
		pVtx[3].pos = D3DXVECTOR3( g_aShadow[nIdxShadow].size.x + fSpread, 0.0f, -g_aShadow[nIdxShadow].size.z - fSpread);

		//頂点バッファのロック解除
		g_pVtxBuffShadow->Unlock();
	}
}

//=========================================================================================
// 影を開放
//=========================================================================================
void ReleaseShadow(int nIdxShadow)
{
	if (0 <= nIdxShadow)
		if (g_aShadow[nIdxShadow].bUse)
			g_aShadow[nIdxShadow].bUse = false;
}