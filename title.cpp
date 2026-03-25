//=============================================================================
//
//	タイトル画面処理 [title.cpp]
//	Author : ENDO HIDETO
// 
//=============================================================================

#include "main.h"
#include "title.h"
#include "debugproc.h"
#include "input.h"
#include "fade.h"
#include "loadscript.h"
#include "texture.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_TITLE		(4)		// タイトルの2Dポリゴン
#define TITLE_TEX		(5)		// タイトルで読み込むテクスチャ数

//*****************************************************************************
// グローバル変数
//*****************************************************************************
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffTitle;		// 頂点バッファへのポインタ
TITLESTATE g_titleState = TITLESTATE_OP;		// タイトルの状態
int g_nCounterTitleState = 0;					// 状態管理カウンター
LOADTEXTURE_INFO g_aTitleTex[TITLE_TEX] =		// 使うテクスチャの情報
{
	{"data/TEXTURE/titlelogo000.png",false,-1},
	{"data/TEXTURE/titlelogo000.png",false,-1},
	{"data/TEXTURE/titlelogo000.png",false,-1},
	{"data/TEXTURE/GameMode3minutes.png",false,-1},
	{"data/TEXTURE/GameModeEndless.png", false,-1},
};

Title g_aTitlePolygon[MAX_TITLE] =				// タイトルの2Dポリゴン
{
	{vec3(SCREEN_WIDTH * 0.5f,SCREEN_WIDTH * 0.5f,0.0f),vec3(512.0f,256.0f,0.0f), -1},		// タイトル
	{vec3(SCREEN_WIDTH * 0.5f,SCREEN_WIDTH * 0.7f,0.0f),vec3(512.0f,64.0f,0.0f), -1},		// Press Any Key
	{vec3(SCREEN_WIDTH * 0.5f,SCREEN_WIDTH * 0.5f,0.0f),vec3(512.0f,64.0f,0.0f), -1},		// GameMode 3Min
	{vec3(SCREEN_WIDTH * 0.5f,SCREEN_WIDTH * 0.7f,0.0f),vec3(512.0f,64.0f,0.0f), -1},		// GameMode Endless
};

//====================================
//	タイトルの初期化処理
//====================================
void InitTitle(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// デバイスへのポインタ
	VERTEX_2D* pVtx;							// 頂点情報へのポインタ

	// テクスチャの読み込み
	P_LOADTEXTURE_INFO pTexInfo = &g_aTitleTex[0];
	for (int nCntTex = 0; nCntTex < TITLE_TEX; nCntTex++, pTexInfo++)
	{
		if (SUCCEEDED(LoadTexture(pTexInfo->pTexFileName, &pTexInfo->nTex)))
		{
			pTexInfo->bSafe = true;
		}
	}

	//頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4 * MAX_TITLE,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&g_pVtxBuffTitle,
		NULL);

	// ポリゴンの設置
	P_TITLE pTitle = &g_aTitlePolygon[0];
	for (int nCnt = 0; nCnt < MAX_TITLE; nCnt++, pTitle++)
	{
		// 頂点バッファをロックし,頂点情報へのポインタを取得
		g_pVtxBuffTitle->Lock(0, 0, (void**)&pVtx, 0);

		// 頂点座標の設定
		pVtx[0].pos = D3DXVECTOR3(pTitle->pos.x - (pTitle->size.x * 0.5f), pTitle->pos.y - (pTitle->size.y * 0.5f), 0.0f);
		pVtx[1].pos = D3DXVECTOR3(pTitle->pos.x + (pTitle->size.x * 0.5f), pTitle->pos.y - (pTitle->size.y * 0.5f), 0.0f);
		pVtx[2].pos = D3DXVECTOR3(pTitle->pos.x - (pTitle->size.x * 0.5f), pTitle->pos.y + (pTitle->size.y * 0.5f), 0.0f);
		pVtx[3].pos = D3DXVECTOR3(pTitle->pos.x + (pTitle->size.x * 0.5f), pTitle->pos.y + (pTitle->size.y * 0.5f), 0.0f);

		// rhwの設定
		pVtx[0].rhw = 1.0f;
		pVtx[1].rhw = 1.0f;
		pVtx[2].rhw = 1.0f;
		pVtx[3].rhw = 1.0f;

		// 頂点カラーの設定
		pVtx[0].col = COLOR_WHITE;
		pVtx[1].col = COLOR_WHITE;
		pVtx[2].col = COLOR_WHITE;
		pVtx[3].col = COLOR_WHITE;

		// テクスチャ座標の設定
		pVtx[0].tex = D3DXVECTOR2(0.0f,0.0f);
		pVtx[1].tex = D3DXVECTOR2(1.0f,0.0f);
		pVtx[2].tex = D3DXVECTOR2(0.0f,1.0f);
		pVtx[3].tex = D3DXVECTOR2(1.0f,1.0f);

		pVtx += 4;
		// 頂点バッファをアンロックする
		g_pVtxBuffTitle->Unlock();
	}
}

//====================================
//	タイトルの終了処理
//====================================
void UninitTitle(void)
{
	if (g_pVtxBuffTitle != NULL)
	{
		g_pVtxBuffTitle->Release();
		g_pVtxBuffTitle = NULL;
	}
}

//====================================
//	タイトルの描画処理
//====================================
void DrawTitle(void)
{
	LPDIRECT3DDEVICE9 pDevice;				// デバイスへのポインタ

	// デバイスの取得
	pDevice = GetDevice();

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, g_pVtxBuffTitle, 0, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	P_TITLE pTitle = &g_aTitlePolygon[0];
	for (int nCntTex = 0; nCntTex < MAX_TITLE; nCntTex++, pTitle++)
	{
		if (pTitle->nTex != -1)
		{
			// テクスチャの設定
			pDevice->SetTexture(0, GetTexture(pTitle->nTex));
		}
		else
		{
			// テクスチャを使用できない場合はNULL
			pDevice->SetTexture(0, NULL);
		}
		// ポリゴンの描画
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, nCntTex * 4, 2);
	}
}

//====================================
//	タイトルの更新処理
//====================================
void UpdateTitle(void)
{
	PrintDebugProc("TITLE\n");

	if (GetKeyboardTrigger(DIK_RETURN))
	{
		SetFade(MODE_GAME);
	}
}

//========================================
//	タイトルの状態設定
//========================================
void SetTitleState(TITLESTATE titlestate)
{
	g_titleState = titlestate;
}

//========================================
//	タイトルの状態取得
//========================================
TITLESTATE GetTitleState(void)
{
	return g_titleState;
}