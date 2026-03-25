//==================================================================================================================================
//
//			タイトル画面処理 [title.cpp]
//			Author : ENDO HIDETO
// 
//==================================================================================================================================
//**************************************************************
// インクルード
#include "main.h"
#include "title.h"
#include "debugproc.h"
#include "input.h"
#include "fade.h"
#include "loadscript.h"
#include "texture.h"

//**************************************************************
// マクロ定義

//**************************************************************
// グローバル変数
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffTitle;			// 頂点バッファへのポインタ
TITLESTATE g_titleState = TITLESTATE_WAIT;			// タイトルの状態
int g_nCounterTitleState = 0;						// 状態管理カウンター
LOADTEXTURE_INFO g_aTitleTex[TITLETEXTURE_MAX] =	// 使うテクスチャの情報
{
	{"data/TEXTURE/titlelogo000.png",false,-1},
	{"data/TEXTURE/titlelogo000.png",false,-1},
	{"data/TEXTURE/titlelogo000.png",false,-1},
	{"data/TEXTURE/GameMode3minutes.png",false,-1},
	{"data/TEXTURE/GameModeEndless.png", false,-1},
};

Title g_aTitlePolygon[TITLEPOLYGON_MAX] =				// タイトルの2Dポリゴン
{
	{vec3(SCREEN_WIDTH * 0.5f,SCREEN_HEIGHT * 0.4f,0.0f),vec3(600.0f,300.0f,0.0f), -1,true},		// タイトル
	{vec3(SCREEN_WIDTH * 0.5f,SCREEN_HEIGHT * 0.7f,0.0f),vec3(600.0f,150.0f,0.0f), -1,true},		// Press Any Key
	{vec3(SCREEN_WIDTH * 0.5f,SCREEN_HEIGHT * 0.5f,0.0f),vec3(600.0f,150.0f,0.0f), -1,false},		// GameMode 3Min
	{vec3(SCREEN_WIDTH * 0.5f,SCREEN_HEIGHT * 0.7f,0.0f),vec3(600.0f,150.0f,0.0f), -1,false},		// GameMode Endless
};
//**************************************************************
// プロトタイプ宣言
void TitleWait(void);
void TitleMove(void);
void TitleOp(void);
void TitleMenu(void);

//=========================================================================================
//	タイトルの初期化処理
//=========================================================================================
void InitTitle(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// デバイスへのポインタ
	VERTEX_2D* pVtx;							// 頂点情報へのポインタ

	// テクスチャの読み込み
	P_LOADTEXTURE_INFO pTexInfo = &g_aTitleTex[0];
	for (int nCntTex = 0; nCntTex < TITLETEXTURE_MAX; nCntTex++, pTexInfo++)
	{
		if (SUCCEEDED(LoadTexture(pTexInfo->pTexFileName, &pTexInfo->nTex)))
		{
			pTexInfo->bSafe = true;
		}
	}

	//頂点バッファの生成
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4 * TITLEPOLYGON_MAX,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&g_pVtxBuffTitle,
		NULL);

	// ポリゴンの設置
	P_TITLE pTitle = &g_aTitlePolygon[0];

	// 頂点バッファをロックし,頂点情報へのポインタを取得
	g_pVtxBuffTitle->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCnt = 0; nCnt < TITLEPOLYGON_MAX; nCnt++, pTitle++)
	{
		switch (nCnt)
		{
		case TITLEPOLYGON_TITLE:
			pTitle->nTex = g_aTitleTex[TITLETEXTURE_TITLE].nTex;
			break;
		case TITLEPOLYGON_START:
			pTitle->nTex = g_aTitleTex[0].nTex;
			break;
		case TITLEPOLYGON_3MIN:
			pTitle->nTex = g_aTitleTex[TITLETEXTURE_3MIN].nTex;
			break;
		case TITLEPOLYGON_ENDLESS:
			pTitle->nTex = g_aTitleTex[TITLETEXTURE_ENDLESS].nTex;
			break;
		default:
			break;
		}

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
	}
	// 頂点バッファをアンロックする
	g_pVtxBuffTitle->Unlock();
}

//=========================================================================================
//	タイトルの終了処理
//=========================================================================================
void UninitTitle(void)
{
	if (g_pVtxBuffTitle != NULL)
	{
		g_pVtxBuffTitle->Release();
		g_pVtxBuffTitle = NULL;
	}
}

//=========================================================================================
//	タイトルの更新処理
//=========================================================================================
void UpdateTitle(void)
{
	PrintDebugProc("TITLE\n");
	switch (g_titleState)
	{
	case TITLESTATE_WAIT:	// 待機
		TitleWait();
		PrintDebugProc("TITLESTATE_WAIT\n");
		break;

	case TITLESTATE_MOVE:	// 移動
		TitleMove();
		PrintDebugProc("TITLESTATE_MOVE\n");
		break;

	case TITLESTATE_OP:		// オープニング
		TitleOp();
		PrintDebugProc("TITLESTATE_OP\n");
		break;

	case TITLESTATE_MENU:	// メニュー
		TitleMenu();
		PrintDebugProc("TITLESTATE_MENU\n");
		break;

	default:				// 例外処理
		g_titleState = TITLESTATE_WAIT;
		break;
	}
}

//==============================================================
// 待機状態
void TitleWait(void)
{
	if (GetKeyboardTrigger(DIK_RETURN))
	{
		SetTitleState(TITLESTATE_MOVE);
	}
}

//==============================================================
// ぐいーん
void TitleMove(void)
{
	if (GetKeyboardTrigger(DIK_RETURN))
	{
		SetTitleState(TITLESTATE_OP);
	}
}

//==============================================================
// ちかちかきらーん
void TitleOp(void)
{
	if (GetKeyboardTrigger(DIK_RETURN))
	{
		g_aTitlePolygon[TITLEPOLYGON_TITLE].bDraw = false;
		g_aTitlePolygon[TITLEPOLYGON_START].bDraw = false;
		g_aTitlePolygon[TITLEPOLYGON_3MIN].bDraw = true;
		g_aTitlePolygon[TITLEPOLYGON_ENDLESS].bDraw = true;
		SetTitleState(TITLESTATE_MENU);
	}
}

//==============================================================
// モードセレクト
void TitleMenu(void)
{
	if (GetKeyboardTrigger(DIK_RETURN))
	{
		SetFade(MODE_GAME);
	}
}

//=========================================================================================
//	タイトルの描画処理
//=========================================================================================
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
	for (int nCntTex = 0; nCntTex < TITLEPOLYGON_MAX; nCntTex++, pTitle++)
	{
		if (pTitle->bDraw)
		{
			// テクスチャの設定
			pDevice->SetTexture(0, GetTexture(pTitle->nTex));

			// ポリゴンの描画
			pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, nCntTex * 4, 2);
		}
	}
}

//=========================================================================================
//	タイトルの状態設定
//=========================================================================================
void SetTitleState(TITLESTATE titlestate)
{
	g_titleState = titlestate;
}

//=========================================================================================
//	タイトルの状態取得
//=========================================================================================
TITLESTATE GetTitleState(void)
{
	return g_titleState;
}