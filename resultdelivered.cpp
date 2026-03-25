//=============================================================================
//
//	届けた数処理 [resultdelivered.cpp]
//	Author : SHUMA AIZU
// 
//=============================================================================

#include "main.h"
#include "resultdelivered.h"
#include "delivered.h"
#include "game.h"
#include "fog.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define NUM_PLACE							(2)			// 届けた数の桁数
#define RESULTDELIVERED_POSX				(700.0f)	// 届けた数の座標X
#define RESULTDELIVERED_POSY				(247.25f)	// 届けた数の座標Y
#define RESULTDELIVEREDSTRING_POSX			(380.0f)	// 届けた数の文字の座標X
#define RESULTDELIVEREDSTRING_POSY			(250.0f)	// 届けた数の文字の座標Y
#define RESULTDELIVERED_SIZEX				(75.0f)		// 届けた数のサイズX
#define RESULTDELIVERED_SIZEY				(150.0f)	// 届けた数のサイズY
#define RESULTDELIVEREDSTRING_WIDTH			(600.0f)	// 届けた数の文字の幅
#define RESULTDELIVEREDSTRING_HEIGHT		(150.0f)	// 届けた数の文字の高さ
#define RESULTDELIVEREDSTRING_MAXTEX		(2)			// テクスチャ数

//*****************************************************************************
// グローバル変数
//*****************************************************************************
LPDIRECT3DTEXTURE9 g_apTextureResultDelivered[RESULTDELIVEREDSTRING_MAXTEX] = {};	// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffResultDelivered = NULL;						// 頂点バッファへのポインタ
D3DXVECTOR3 g_posResultDelivered;												// 届けた数の位置
int g_nResultDelivered;														// 届けた数の値

//=============================================================================
//	届けた数の初期化処理
//=============================================================================
void InitResultDelivered(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	int nCntResultDelivered = 0;

	// 初期化
	g_posResultDelivered = D3DXVECTOR3(RESULTDELIVERED_POSX, RESULTDELIVERED_POSY, 0.0f);
	g_nResultDelivered = 0;

	// テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice,"data\\TEXTURE\\number000.png", &g_apTextureResultDelivered[0]);

	D3DXCreateTextureFromFile(pDevice, "data\\TEXTURE\\delivered000.png", &g_apTextureResultDelivered[1]);

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4 * (NUM_PLACE + 1),		// 桁の数だけ
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&g_pVtxBuffResultDelivered,
		NULL);

	// 初期化
	g_nResultDelivered = GetDelivered();

	VERTEX_2D *pVtx;			// 頂点情報へのポインタ

	// 頂点バッファをロックし,頂点情報へのポインタを取得
	g_pVtxBuffResultDelivered->Lock(0, 0, (void * *)&pVtx, 0);

	for (nCntResultDelivered = 0; nCntResultDelivered < NUM_PLACE; nCntResultDelivered++)
	{
		// 頂点座標の設定
		pVtx[0].pos = D3DXVECTOR3(RESULTDELIVERED_POSX + (nCntResultDelivered * RESULTDELIVERED_SIZEX), RESULTDELIVERED_POSY, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(RESULTDELIVERED_POSX + (nCntResultDelivered * RESULTDELIVERED_SIZEX) + RESULTDELIVERED_SIZEX, RESULTDELIVERED_POSY, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(RESULTDELIVERED_POSX + (nCntResultDelivered * RESULTDELIVERED_SIZEX), RESULTDELIVERED_POSY + RESULTDELIVERED_SIZEY, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(RESULTDELIVERED_POSX + (nCntResultDelivered * RESULTDELIVERED_SIZEX) + RESULTDELIVERED_SIZEX, RESULTDELIVERED_POSY + RESULTDELIVERED_SIZEY, 0.0f);

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
	pVtx[0].pos = D3DXVECTOR3(RESULTDELIVEREDSTRING_POSX, RESULTDELIVEREDSTRING_POSY, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(RESULTDELIVEREDSTRING_POSX + RESULTDELIVEREDSTRING_WIDTH, RESULTDELIVEREDSTRING_POSY, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(RESULTDELIVEREDSTRING_POSX, RESULTDELIVEREDSTRING_POSY + RESULTDELIVEREDSTRING_HEIGHT, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(RESULTDELIVEREDSTRING_POSX + RESULTDELIVEREDSTRING_WIDTH, RESULTDELIVEREDSTRING_POSY + RESULTDELIVEREDSTRING_HEIGHT, 0.0f);

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
	g_pVtxBuffResultDelivered->Unlock();
}

//=============================================================================
//	届けた数の終了処理
//=============================================================================
void UninitResultDelivered(void)
{
	for (int nCntTex = 0; nCntTex < RESULTDELIVEREDSTRING_MAXTEX; nCntTex++)
	{
		// テクスチャの破棄
		if (g_apTextureResultDelivered[nCntTex] != NULL)
		{
			g_apTextureResultDelivered[nCntTex]->Release();
			g_apTextureResultDelivered[nCntTex] = NULL;
		}
	}

	// 頂点バッファの破棄
	if (g_pVtxBuffResultDelivered != NULL)
	{
		g_pVtxBuffResultDelivered->Release();
		g_pVtxBuffResultDelivered = NULL;
	}
}

//=============================================================================
//	届けた数の更新処理
//=============================================================================
void UpdateResultDelivered(void)
{
	int aTexU[NUM_PLACE];		// 各桁の数字を格納
	int nCntNumber = 0;

	if (g_nResultDelivered <= 0)
	{// 0以下を0に
		g_nResultDelivered = 0;
	}

	aTexU[0] = g_nResultDelivered % 100 / 10;
	aTexU[1] = g_nResultDelivered % 10 / 1;

	// テクスチャ座標の設定
	VERTEX_2D* pVtx;			// 頂点情報へのポインタ

	// 頂点バッファをロックし,頂点情報へのポインタを取得
	g_pVtxBuffResultDelivered->Lock(0, 0, (void**)&pVtx, 0);

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
	g_pVtxBuffResultDelivered->Unlock();
}

//=============================================================================
//	届けた数の描画処理
//=============================================================================
void DrawResultDelivered(void)
{
	LPDIRECT3DDEVICE9 pDevice;				// デバイスへのポインタ
	int nCntNumber;

	// デバイスの取得
	pDevice = GetDevice();

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, g_pVtxBuffResultDelivered, 0, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	// テクスチャの設定
	pDevice->SetTexture(0, g_apTextureResultDelivered[0]);

	for (nCntNumber = 0; nCntNumber < NUM_PLACE + 1; nCntNumber++)
	{
		if (nCntNumber == NUM_PLACE)
		{
			// テクスチャの設定
			pDevice->SetTexture(0, g_apTextureResultDelivered[1]);
		}

		// ポリゴンの描画
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, nCntNumber * 4, 2);
	}
}

//====================================
//	届けた数の加算処理
//====================================
void AddResultDelivered(int nValue)
{
	g_nResultDelivered += nValue;
}

//====================================
//	届けた数の取得
//====================================
int GetResultDelivered(void)
{
	return g_nResultDelivered;
}