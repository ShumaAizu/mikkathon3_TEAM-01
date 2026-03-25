//=============================================================================
//
//	届けた数処理 [delivered.cpp]
//	Author : SHUMA AIZU
// 
//=============================================================================

#include "main.h"
#include "delivered.h"
#include "game.h"
#include "fog.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define NUM_PLACE						(2)			// 届けた数の桁数
#define DELIVERED_POSX					(1185.0f)	// 届けた数の座標X
#define DELIVERED_POSY					(57.25f)	// 届けた数の座標Y
#define DELIVEREDSTRING_POSX			(1080.0f)	// 届けた数の文字の座標X
#define DELIVEREDSTRING_POSY			(58.25f)	// 届けた数の文字の座標Y
#define DELIVERED_SIZEX					(25.0f)		// 届けた数のサイズX
#define DELIVERED_SIZEY					(50.0f)		// 届けた数のサイズY
#define DELIVEREDSTRING_WIDTH			(200.0f)	// 届けた数の文字の幅
#define DELIVEREDSTRING_HEIGHT			(50.0f)		// 届けた数の文字の高さ
#define DELIVEREDSTRING_MAXTEX			(2)			// テクスチャ数

//*****************************************************************************
// グローバル変数
//*****************************************************************************
LPDIRECT3DTEXTURE9 g_apTextureDelivered[DELIVEREDSTRING_MAXTEX] = {};	// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffDelivered = NULL;						// 頂点バッファへのポインタ
D3DXVECTOR3 g_posDelivered;												// 届けた数の位置
int g_nDelivered;														// 届けた数の値

//=============================================================================
//	届けた数の初期化処理
//=============================================================================
void InitDelivered(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	int nCntDelivered = 0;

	// 初期化
	g_posDelivered = D3DXVECTOR3(DELIVERED_POSX, DELIVERED_POSY, 0.0f);
	g_nDelivered = 0;

	// テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice,"data\\TEXTURE\\number000.png", &g_apTextureDelivered[0]);

	D3DXCreateTextureFromFile(pDevice, "data\\TEXTURE\\delivered000.png", &g_apTextureDelivered[1]);

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4 * (NUM_PLACE + 1),		// 桁の数だけ
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&g_pVtxBuffDelivered,
		NULL);

	// 初期化
	g_nDelivered = 1;

	VERTEX_2D *pVtx;			// 頂点情報へのポインタ

	// 頂点バッファをロックし,頂点情報へのポインタを取得
	g_pVtxBuffDelivered->Lock(0, 0, (void * *)&pVtx, 0);

	for (nCntDelivered = 0; nCntDelivered < NUM_PLACE; nCntDelivered++)
	{
		// 頂点座標の設定
		pVtx[0].pos = D3DXVECTOR3(DELIVERED_POSX + (nCntDelivered * DELIVERED_SIZEX), DELIVERED_POSY, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(DELIVERED_POSX + (nCntDelivered * DELIVERED_SIZEX) + DELIVERED_SIZEX, DELIVERED_POSY, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(DELIVERED_POSX + (nCntDelivered * DELIVERED_SIZEX), DELIVERED_POSY + DELIVERED_SIZEY, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(DELIVERED_POSX + (nCntDelivered * DELIVERED_SIZEX) + DELIVERED_SIZEX, DELIVERED_POSY + DELIVERED_SIZEY, 0.0f);

		// rhwの設定
		pVtx[0].rhw = 1.0f;
		pVtx[1].rhw = 1.0f;
		pVtx[2].rhw = 1.0f;
		pVtx[3].rhw = 1.0f;

		// 頂点カラーの設定
		pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

		// テクスチャ座標の設定
		pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[1].tex = D3DXVECTOR2(0.1f, 0.0f);
		pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
		pVtx[3].tex = D3DXVECTOR2(0.1f, 1.0f);

		pVtx += 4;			// 頂点データのポインタを4つ分進める
	}

	// 文字設定

	// 頂点座標の設定
	pVtx[0].pos = D3DXVECTOR3(DELIVEREDSTRING_POSX, DELIVEREDSTRING_POSY, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(DELIVEREDSTRING_POSX + DELIVEREDSTRING_WIDTH, DELIVEREDSTRING_POSY, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(DELIVEREDSTRING_POSX, DELIVEREDSTRING_POSY + DELIVEREDSTRING_HEIGHT, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(DELIVEREDSTRING_POSX + DELIVEREDSTRING_WIDTH, DELIVEREDSTRING_POSY + DELIVEREDSTRING_HEIGHT, 0.0f);

	// rhwの設定
	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;

	// 頂点カラーの設定
	pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	// テクスチャ座標の設定
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);


	// 頂点バッファをアンロックする
	g_pVtxBuffDelivered->Unlock();
}

//=============================================================================
//	届けた数の終了処理
//=============================================================================
void UninitDelivered(void)
{
	for (int nCntTex = 0; nCntTex < DELIVEREDSTRING_MAXTEX; nCntTex++)
	{
		// テクスチャの破棄
		if (g_apTextureDelivered[nCntTex] != NULL)
		{
			g_apTextureDelivered[nCntTex]->Release();
			g_apTextureDelivered[nCntTex] = NULL;
		}
	}

	// 頂点バッファの破棄
	if (g_pVtxBuffDelivered != NULL)
	{
		g_pVtxBuffDelivered->Release();
		g_pVtxBuffDelivered = NULL;
	}
}

//=============================================================================
//	届けた数の更新処理
//=============================================================================
void UpdateDelivered(void)
{
	int aTexU[NUM_PLACE];		// 各桁の数字を格納
	int nCntNumber = 0;

	if (g_nDelivered <= 0)
	{// 0以下を0に
		g_nDelivered = 0;
	}

	aTexU[0] = g_nDelivered % 100 / 10;
	aTexU[1] = g_nDelivered % 10 / 1;

	// テクスチャ座標の設定
	VERTEX_2D* pVtx;			// 頂点情報へのポインタ

	// 頂点バッファをロックし,頂点情報へのポインタを取得
	g_pVtxBuffDelivered->Lock(0, 0, (void**)&pVtx, 0);

	for (nCntNumber = 0; nCntNumber < NUM_PLACE; nCntNumber++)
	{
		// テクスチャ座標の設定
		pVtx[0].tex = D3DXVECTOR2(0.1f * aTexU[nCntNumber], 0.0f);
		pVtx[1].tex = D3DXVECTOR2(0.1f * (aTexU[nCntNumber] + 1), 0.0f);
		pVtx[2].tex = D3DXVECTOR2(0.1f * aTexU[nCntNumber], 1.0f);
		pVtx[3].tex = D3DXVECTOR2(0.1f * (aTexU[nCntNumber] + 1), 1.0f);

		pVtx += 4;
	}

	// 頂点バッファをアンロックする
	g_pVtxBuffDelivered->Unlock();
}

//=============================================================================
//	届けた数の描画処理
//=============================================================================
void DrawDelivered(void)
{
	LPDIRECT3DDEVICE9 pDevice;				// デバイスへのポインタ
	int nCntNumber;

	// デバイスの取得
	pDevice = GetDevice();

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, g_pVtxBuffDelivered, 0, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	// テクスチャの設定
	pDevice->SetTexture(0, g_apTextureDelivered[0]);

	for (nCntNumber = 0; nCntNumber < NUM_PLACE + 1; nCntNumber++)
	{
		if (nCntNumber == NUM_PLACE)
		{
			// テクスチャの設定
			pDevice->SetTexture(0, g_apTextureDelivered[1]);
		}

		// ポリゴンの描画
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, nCntNumber * 4, 2);
	}
}

//====================================
//	届けた数の加算処理
//====================================
void AddDelivered(int nValue)
{
	g_nDelivered += nValue;
}

//====================================
//	届けた数の取得
//====================================
int GetDelivered(void)
{
	return g_nDelivered;
}