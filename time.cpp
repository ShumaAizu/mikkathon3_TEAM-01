//=============================================================================
//
//	制限時間処理 [time.cpp]
//	Author : SHUMA AIZU
// 
//=============================================================================

#include "main.h"
#include "time.h"
#include "game.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define NUM_PLACE		(3)			// 制限時間の桁数
#define TIME_POSX		(615.0f)	// 制限時間の座標X
#define TIME_POSY		(0.0f)		// 制限時間の座標Y
#define LIMIT_POSX		(470.0f)	// 残り時間の座標X
#define LIMIT_POSY		(0.0f)		// 残り時間の座標Y
#define TIME_SIZEX		(50.0f)		// 制限時間のサイズX
#define TIME_SIZEY		(100.0f)	// 制限時間のサイズY
#define LIMIT_WIDTH		(300.0f)	// 残り時間の幅
#define LIMIT_HEIGHT	(100.0f)	// 残り時間の高さ
#define LIMIT_TIME		(60)		// 制限時間
#define LIMIT_MAXTEX	(2)			// テクスチャ数

//*****************************************************************************
// グローバル変数
//*****************************************************************************
LPDIRECT3DTEXTURE9 g_apTextureTime[LIMIT_MAXTEX] = {};	// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffTime = NULL;			// 頂点バッファへのポインタ
D3DXVECTOR3 g_posTime;									// 制限時間の位置
int g_nTime;											// 制限時間の値

//=============================================================================
//	制限時間の初期化処理
//=============================================================================
void InitTime(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();
	int nCntTime = 0;

	// 初期化
	g_posTime = D3DXVECTOR3(TIME_POSX, TIME_POSY, 0.0f);
	g_nTime = 0;

	// テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice,"data\\TEXTURE\\number000.png", &g_apTextureTime[0]);

	D3DXCreateTextureFromFile(pDevice, "data\\TEXTURE\\limit001.png", &g_apTextureTime[1]);

	// 頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4 * (NUM_PLACE + 1),		// 桁の数だけ
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&g_pVtxBuffTime,
		NULL);

	// 初期化
	g_nTime = LIMIT_TIME;

#ifdef _DEBUG
	g_nTime = 999;
#endif

	VERTEX_2D *pVtx;			// 頂点情報へのポインタ

	// 頂点バッファをロックし,頂点情報へのポインタを取得
	g_pVtxBuffTime->Lock(0, 0, (void * *)&pVtx, 0);

	for (nCntTime = 0; nCntTime < NUM_PLACE; nCntTime++)
	{
		// 頂点座標の設定
		pVtx[0].pos = D3DXVECTOR3(TIME_POSX + (nCntTime * TIME_SIZEX), TIME_POSY, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(TIME_POSX + (nCntTime * TIME_SIZEX) + TIME_SIZEX, TIME_POSY, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(TIME_POSX + (nCntTime * TIME_SIZEX), TIME_POSY + TIME_SIZEY, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(TIME_POSX + (nCntTime * TIME_SIZEX) + TIME_SIZEX, TIME_POSY + TIME_SIZEY, 0.0f);

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

	// 残り時間設定

	// 頂点座標の設定
	pVtx[0].pos = D3DXVECTOR3(LIMIT_POSX, LIMIT_POSY, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(LIMIT_POSX + LIMIT_WIDTH, LIMIT_POSY, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(LIMIT_POSX, LIMIT_POSY + LIMIT_HEIGHT, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(LIMIT_POSX + LIMIT_WIDTH, LIMIT_POSY + LIMIT_HEIGHT, 0.0f);

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
	g_pVtxBuffTime->Unlock();
}

//====================================
//	制限時間の終了処理
//====================================
void UninitTime(void)
{
	for (int nCntTex = 0; nCntTex < LIMIT_MAXTEX; nCntTex++)
	{
		// テクスチャの破棄
		if (g_apTextureTime[nCntTex] != NULL)
		{
			g_apTextureTime[nCntTex]->Release();
			g_apTextureTime[nCntTex] = NULL;
		}
	}

	// 頂点バッファの破棄
	if (g_pVtxBuffTime != NULL)
	{
		g_pVtxBuffTime->Release();
		g_pVtxBuffTime = NULL;
	}
}

//====================================
//	制限時間の描画処理
//====================================
void DrawTime(void)
{
	LPDIRECT3DDEVICE9 pDevice;				// デバイスへのポインタ
	int nCntNumber;

	// デバイスの取得
	pDevice = GetDevice();

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, g_pVtxBuffTime, 0, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	// テクスチャの設定
	pDevice->SetTexture(0, g_apTextureTime[0]);

	for (nCntNumber = 0; nCntNumber < NUM_PLACE + 1; nCntNumber++)
	{
		if (nCntNumber == NUM_PLACE)
		{
			// テクスチャの設定
			pDevice->SetTexture(0, g_apTextureTime[1]);
		}

		// ポリゴンの描画
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, nCntNumber * 4, 2);
	}
}

//====================================
//	制限時間の更新処理
//====================================
void UpdateTime(void)
{

	static int nTimeCounter = 0;
	int aTexU[NUM_PLACE];		// 各桁の数字を格納
	int nCntNumber = 0;

	nTimeCounter++;
	if (nTimeCounter % 60 == 0)
	{// 60フレームごとに1秒
		g_nTime--;
		nTimeCounter = 0;
	}

	if (g_nTime < 0 && GetNextGameFlag() != GAMEFLAG_CLEAR)
	{// 残り時間がなくなったらゲームの状態を設定
		SetGameFlag(GAMEFLAG_CLEAR, 0);
	}

	if (g_nTime <= 0)
	{// 0以下を0に
		g_nTime = 0;
	}

	aTexU[0] = g_nTime % 100 / 10;
	aTexU[1] = g_nTime % 10 / 1;

	// テクスチャ座標の設定
	VERTEX_2D* pVtx;			// 頂点情報へのポインタ

	// 頂点バッファをロックし,頂点情報へのポインタを取得
	g_pVtxBuffTime->Lock(0, 0, (void**)&pVtx, 0);

	for (nCntNumber = 0; nCntNumber < NUM_PLACE; nCntNumber++)
	{
		// テクスチャ座標の設定
		pVtx[0].tex = D3DXVECTOR2(0.1f * aTexU[nCntNumber], 0.0f);
		pVtx[1].tex = D3DXVECTOR2(0.1f * (aTexU[nCntNumber] + 1), 0.0f);
		pVtx[2].tex = D3DXVECTOR2(0.1f * aTexU[nCntNumber], 1.0f);
		pVtx[3].tex = D3DXVECTOR2(0.1f * (aTexU[nCntNumber] + 1), 1.0f);

		if (g_nTime <= 10)
		{
			// 頂点カラーの設定
			pVtx[0].col = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
			pVtx[1].col = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
			pVtx[2].col = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
			pVtx[3].col = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
		}

		pVtx += 4;
	}

	// 頂点バッファをアンロックする
	g_pVtxBuffTime->Unlock();
}

//====================================
//	制限時間の確認処理
//====================================
int GetTime(void)
{
	return g_nTime;
}