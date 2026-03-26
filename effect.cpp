//==================================================================================================================================
// 
//			エフェクトの処理 [Effect.cpp]
//			Author : ENDO HIDETO
// 
//==================================================================================================================================
//**************************************************************
// インクルード
#include "effect.h"
#include "shadow.h"

//**************************************************************
//グローバル変数
LPDIRECT3DTEXTURE9 g_pTextureEffect = NULL;				//テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffEffect = NULL;		//頂点バッファへのポインタ
Effect g_aEffect[MAX_EFFECT];							//エフェクト情報
int g_nCntEffect = 0;									//発射済みのエフェクト数

//**************************************************************
//プロトタイプ宣言

//=========================================================================================
// エフェクト初期化処理
//=========================================================================================
void InitEffect(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();		//デバイスへのポインタ
	VERTEX_3D* pVtx;								//頂点情報へのポインタ

	//**************************************************************
	//テクスチャ読み込み
	D3DXCreateTextureFromFile(pDevice,"data\\TEXTURE\\shadow000.jpg",&g_pTextureEffect);

	//頂点バッファの読み込み
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D)* 4 * MAX_EFFECT,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&g_pVtxBuffEffect,
		NULL);

	//頂点バッファをロックし、頂点情報へのポインタを取得
	g_pVtxBuffEffect->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntEffect = 0; nCntEffect < MAX_EFFECT; nCntEffect++,pVtx += 4)
	{
		//**************************************************************
		//構造体の初期化
		g_aEffect[nCntEffect].fSize = 0.0f;
		g_aEffect[nCntEffect].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_aEffect[nCntEffect].nLife = 0;
		g_aEffect[nCntEffect].bUse = false;

		//頂点座標を設定
		pVtx[0].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

		//法線ベクトルの設定
		pVtx[0].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		pVtx[1].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		pVtx[2].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		pVtx[3].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

		//頂点カラー設定
		pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

		//テクスチャの座標設定
		pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
		pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
		pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);
	}

	//頂点バッファのロック解除
	g_pVtxBuffEffect->Unlock();
}

//=========================================================================================
// エフェクト終了処理
//=========================================================================================
void UninitEffect(void)
{
	//**************************************************************
	//テクスチャの破棄
	if (g_pTextureEffect != NULL)
	{
		g_pTextureEffect->Release();
		g_pTextureEffect = NULL;
	}

	//**************************************************************
	//頂点バッファの破棄
	if (g_pVtxBuffEffect != NULL)
	{
		g_pVtxBuffEffect->Release();
		g_pVtxBuffEffect = NULL;
	}
}

//=========================================================================================
// エフェクト更新処理
//=========================================================================================
void UpdateEffect(void)
{
	VERTEX_3D* pVtx;			// 頂点情報へのポインタ
	Effect* pEffect;			// 弾の情報ポインタ（簡略化用

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	g_pVtxBuffEffect->Lock(0, 0, (void**)&pVtx, 0);
	
	for (int nCntEffect = 0; nCntEffect < MAX_EFFECT; nCntEffect++,pVtx +=4)
	{
		if (g_aEffect[nCntEffect].bUse)
		{			
			pEffect = &g_aEffect[nCntEffect];// ポインタを保存

			//**************************************************************
			// 寿命を減らす
			g_aEffect[nCntEffect].nLife--;
			if (g_aEffect[nCntEffect].nLife < 0)
			{// 寿命が尽きたらエフェクトを消す
				g_aEffect[nCntEffect].bUse = false;
				continue;
			}
			else
			{// 寿命があれば
				g_aEffect[nCntEffect].nLife--;			// 寿命を減らす
				g_aEffect[nCntEffect].col.a -= g_aEffect[nCntEffect].fSubAlpha;			// 薄く
				g_aEffect[nCntEffect].fSize -= g_aEffect[nCntEffect].fSubLength;		// 小さく
				g_aEffect[nCntEffect].pos += g_aEffect[nCntEffect].move;				// 動かす
			}
				
			// 頂点座標を設定
			pVtx[0].pos = D3DXVECTOR3(-g_aEffect[nCntEffect].fSize,  g_aEffect[nCntEffect].fSize, 0.0f);
			pVtx[1].pos = D3DXVECTOR3( g_aEffect[nCntEffect].fSize,  g_aEffect[nCntEffect].fSize, 0.0f);
			pVtx[2].pos = D3DXVECTOR3(-g_aEffect[nCntEffect].fSize, -g_aEffect[nCntEffect].fSize, 0.0f);
			pVtx[3].pos = D3DXVECTOR3( g_aEffect[nCntEffect].fSize, -g_aEffect[nCntEffect].fSize, 0.0f);

			// 頂点カラー設定
			pVtx[0].col = g_aEffect[nCntEffect].col;
			pVtx[1].col = g_aEffect[nCntEffect].col;
			pVtx[2].col = g_aEffect[nCntEffect].col;
			pVtx[3].col = g_aEffect[nCntEffect].col;
		}
	}
	//頂点バッファのロック解除
	g_pVtxBuffEffect->Unlock();
}

//=========================================================================================
// エフェクト描画処理
//=========================================================================================
void DrawEffect(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();		//デバイスへのポインタ
	D3DXMATRIX mtxView, mtxTrans;					//マトリックス計算用

	for (int nCntEffect = 0; nCntEffect < MAX_EFFECT; nCntEffect++)
	{
		if (g_aEffect[nCntEffect].bUse)
		{
			//ワールドマトリックスの初期化
			D3DXMatrixIdentity(&g_aEffect[nCntEffect].mtxWorld);

			//ビューマトリックスを取得
			pDevice->GetTransform(D3DTS_VIEW, &mtxView);

			//ポリゴンをカメラに対して正面に向ける
			D3DXMatrixInverse(&g_aEffect[nCntEffect].mtxWorld, NULL, &mtxView);
			g_aEffect[nCntEffect].mtxWorld._41 = 0.0f;
			g_aEffect[nCntEffect].mtxWorld._42 = 0.0f;
			g_aEffect[nCntEffect].mtxWorld._43 = 0.0f;

			//位置を反映
			D3DXMatrixTranslation(&mtxTrans, g_aEffect[nCntEffect].pos.x, g_aEffect[nCntEffect].pos.y, g_aEffect[nCntEffect].pos.z);
			D3DXMatrixMultiply(&g_aEffect[nCntEffect].mtxWorld, &g_aEffect[nCntEffect].mtxWorld, &mtxTrans);

			//ワールドマトリックスの設定
			pDevice->SetTransform(D3DTS_WORLD, &g_aEffect[nCntEffect].mtxWorld);

			//頂点バッファをデータストリームに設定
			pDevice->SetStreamSource(0,
				g_pVtxBuffEffect,
				0,
				sizeof(VERTEX_3D));

			//頂点フォーマットの設定
			pDevice->SetFVF(FVF_VERTEX_3D);

			//テクスチャの設定
			pDevice->SetTexture(0, g_pTextureEffect);

			//**************************************************************
			//アルファブレンドを加算合成に設定
			pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
			pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

			//// アルファテストを有効にする
			//pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);		// アルファテストを有効にする
			//pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);	// 比較方法(基準値より大きければ描画)
			//pDevice->SetRenderState(D3DRS_ALPHAREF, 150);				// アルファテストの参照値を設定(～以上で描画, intで設定)

			// Zテストを無効にする
			pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);
			pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

			//エフェクトの描画
			pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 4 * nCntEffect, 2);

			//アルファブレンドを元に戻す
			pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
			pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

			//// アルファテストを無効にする
			//pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);		// アルファテストを無効化
			//pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);	// 比較方法(すべて描画)
			//pDevice->SetRenderState(D3DRS_ALPHAREF, 255);				// 基準値を設定(すべてを描画している)

			//Zテストを有効にする
			pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
			pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
		}
	}
}

//=========================================================================================
// エフェクトを設置
//=========================================================================================
void SetEffect(D3DXVECTOR3 pos, D3DXVECTOR3 move, D3DXCOLOR col, int nLife, float fRadius) 
{
	//**************************************************************
	// 変数宣言s
	VERTEX_3D* pVtx;			//頂点情報へのポインタ

	for (int nCntEffect = 0; nCntEffect < MAX_EFFECT; nCntEffect++)
	{
		if (g_aEffect[nCntEffect].bUse == false)
		{
			g_aEffect[nCntEffect].pos = pos;
			g_aEffect[nCntEffect].fSize = fRadius;
			g_aEffect[nCntEffect].move = move;
			g_aEffect[nCntEffect].col = col;
			g_aEffect[nCntEffect].fSubLength = (float)fRadius / nLife;
			g_aEffect[nCntEffect].fSubAlpha = (float)1 / nLife;
			g_aEffect[nCntEffect].nLife = nLife;
			g_aEffect[nCntEffect].bUse = true;

			//頂点バッファをロックし、頂点情報へのポインタを取得
			g_pVtxBuffEffect->Lock(0, 0, (void**)&pVtx, 0);
			pVtx += 4 * nCntEffect;

			//頂点座標を設定
			pVtx[0].pos = D3DXVECTOR3(-g_aEffect[nCntEffect].fSize,  g_aEffect[nCntEffect].fSize, 0.0f);
			pVtx[1].pos = D3DXVECTOR3( g_aEffect[nCntEffect].fSize,  g_aEffect[nCntEffect].fSize, 0.0f);
			pVtx[2].pos = D3DXVECTOR3(-g_aEffect[nCntEffect].fSize, -g_aEffect[nCntEffect].fSize, 0.0f);
			pVtx[3].pos = D3DXVECTOR3( g_aEffect[nCntEffect].fSize, -g_aEffect[nCntEffect].fSize, 0.0f);
		
			//頂点バッファのロック解除
			g_pVtxBuffEffect->Unlock();

			g_nCntEffect++;
			break;
		}
	}
}

//=========================================================================================
// エフェクト情報を取得
//=========================================================================================
int GetEffect(void)
{
	return g_nCntEffect;
}
