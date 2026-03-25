//=============================================================================
//
//	背景処理 [cutin.cpp]
//	Author : SHUMA AIZU
// 
//=============================================================================

#include "main.h"
#include "cutin.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_CUTIN		(3)									// 背景の最大数
#define CUTIN_SIZE		(1280.0f)							// カットインの大きさ
#define CUTIN_MOVE		(D3DXVECTOR3(25.6f, 0.0f, 0.0f))		// カットインの移動量

//*****************************************************************************
// グローバル変数
//*****************************************************************************
LPDIRECT3DTEXTURE9 g_pTextureCutIn = NULL;			// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffCutIn = NULL;		// 頂点バッファへのポインタ
CutIn g_cutin;
bool g_isCutIn;										// カットイン状態
bool g_isSetCutIn;									// カットインが設定されたか

//=============================================================================
//	背景の初期化処理
//=============================================================================
void InitCutIn(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	// テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice,
		"data\\TEXTURE\\cloud000.jpg",
		&g_pTextureCutIn);


	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4 * MAX_CUTIN,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&g_pVtxBuffCutIn,
		NULL);

	// 初期化

	VERTEX_2D *pVtx;			// 頂点情報へのポインタ

	// 頂点バッファをロックし,頂点情報へのポインタを取得
	g_pVtxBuffCutIn->Lock(0, 0, (void * *)&pVtx, 0);

	g_cutin.pos = D3DXVECTOR3(-SCREEN_WIDTH, SCREEN_HEIGHT / 2, 0.0f);
	g_cutin.move = CUTIN_MOVE;

	// 頂点座標の設定
	pVtx[0].pos = D3DXVECTOR3(g_cutin.pos.x - CUTIN_SIZE, g_cutin.pos.y - CUTIN_SIZE, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(g_cutin.pos.x + CUTIN_SIZE, g_cutin.pos.y - CUTIN_SIZE, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(g_cutin.pos.x - CUTIN_SIZE, g_cutin.pos.y + CUTIN_SIZE, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(g_cutin.pos.x + CUTIN_SIZE, g_cutin.pos.y + CUTIN_SIZE, 0.0f);

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
	g_pVtxBuffCutIn->Unlock();

	g_isSetCutIn = false;
	g_isCutIn = false;
}

//=============================================================================
//	背景の終了処理
//=============================================================================
void UninitCutIn(void)
{
	// テクスチャの破棄
	if (g_pTextureCutIn != NULL)
	{
		g_pTextureCutIn->Release();
		g_pTextureCutIn = NULL;
	}

	// 頂点バッファの破棄
	if (g_pVtxBuffCutIn != NULL)
	{
		g_pVtxBuffCutIn->Release();
		g_pVtxBuffCutIn = NULL;
	}
}

//=============================================================================
//	背景の描画処理
//=============================================================================
void DrawCutIn(void)
{
	LPDIRECT3DDEVICE9 pDevice;				// デバイスへのポインタ

	// デバイスの取得
	pDevice = GetDevice();

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, g_pVtxBuffCutIn, 0, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	// テクスチャの設定
	pDevice->SetTexture(0, g_pTextureCutIn);

	// ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}

//=============================================================================
//	背景の更新処理
//=============================================================================
void UpdateCutIn(void)
{
	if(g_isCutIn == true)
	{
		g_cutin.pos += g_cutin.move;
	}

	if (g_isSetCutIn == true)
	{
		if (g_cutin.pos.x <= SCREEN_WIDTH * 2)
		{
			g_cutin.pos += g_cutin.move;
		}
		else
		{
			g_cutin.pos = D3DXVECTOR3(-SCREEN_WIDTH, SCREEN_HEIGHT / 2, 0.0f);
			g_isSetCutIn = false;
		}
	}

	// 頂点座標の更新
	VERTEX_2D* pVtx;			// 頂点情報へのポインタ

	// 頂点バッファをロックし,頂点情報へのポインタを取得
	g_pVtxBuffCutIn->Lock(0, 0, (void**)&pVtx, 0);

	// 頂点座標の設定
	pVtx[0].pos = D3DXVECTOR3(g_cutin.pos.x - CUTIN_SIZE, g_cutin.pos.y - CUTIN_SIZE, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(g_cutin.pos.x + CUTIN_SIZE, g_cutin.pos.y - CUTIN_SIZE, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(g_cutin.pos.x - CUTIN_SIZE, g_cutin.pos.y + CUTIN_SIZE, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(g_cutin.pos.x + CUTIN_SIZE, g_cutin.pos.y + CUTIN_SIZE, 0.0f);

	// 頂点バッファをアンロックする
	g_pVtxBuffCutIn->Unlock();

	if (g_cutin.pos.x >= SCREEN_WIDTH - CUTIN_SIZE && g_isSetCutIn == false)
	{
		g_isCutIn = false;
		g_isSetCutIn = true;
	}
}

//=============================================================================
//	カットインの設定処理
//=============================================================================
void SetCutIn(void)
{
	if (g_isCutIn == false)
	{
		g_isCutIn = true;
		g_cutin.pos = D3DXVECTOR3(-SCREEN_WIDTH, SCREEN_HEIGHT / 2, 0.0f);
	}
}

//=============================================================================
//	カットイン状態の取得処理
//=============================================================================
bool GetSetCutIn(void)
{
	return g_isSetCutIn;
}