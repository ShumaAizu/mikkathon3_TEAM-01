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
#include "sound.h"
#include "modeldata.h"
#include "field.h"
#include "skybox.h"
#include "fog.h"

//**************************************************************
// マクロ定義
#define TITLE_MOVE_COUNT	(180)		// 動いている時間
#define TITLE_WAIT_BLINK	(50)		// 点滅の間隔
#define MAX_TITLEMODEL		(16)		// タイトルに出すモデルの最大数

//**************************************************************
// グローバル変数
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffTitle;			// 頂点バッファへのポインタ
TITLESTATE g_titleState = TITLESTATE_WAIT;			// タイトルの状態
int g_nCounterTitleState = 0;						// 状態管理カウンター
int	g_nSelectTitle = 0;								// 選択中のゲームモード
TitleModel g_titleModel[MAX_TITLEMODEL] = {};		// タイトル表示用モデル

LOADTEXTURE_INFO g_aTitleTex[TITLETEXTURE_MAX] =	// 使うテクスチャの情報
{
	{"data/TEXTURE/titlelogo000.png",false,-1},
	{"data/TEXTURE/PressAButton.png",false,-1},
	{"data/TEXTURE/GameMode3minutes.png",false,-1},
	{"data/TEXTURE/GameModeEndless.png", false,-1},
};

Title g_aTitlePolygon[TITLEPOLYGON_MAX] =				// タイトルの2Dポリゴン
{
	{vec3(SCREEN_WIDTH * 0.35f,SCREEN_HEIGHT * 0.3f,0.0f),vec3(400.0f,200.0f,0.0f), -1,true},		// タイトル
	{vec3(SCREEN_WIDTH * 0.5f,SCREEN_HEIGHT * 0.85f,0.0f),vec3(300.0f,40.0f,0.0f), -1,true},		// Press Any Key
	{vec3(SCREEN_WIDTH * 0.5f,SCREEN_HEIGHT * 0.75f,0.0f),vec3(300.0f,40.0f,0.0f), -1,false},		// GameMode 3Min
	{vec3(SCREEN_WIDTH * 0.5f,SCREEN_HEIGHT * 0.9f,0.0f),vec3(300.0f,30.0f,0.0f), -1,false},		// GameMode Endless
};
//**************************************************************
// プロトタイプ宣言
void TitleWait(void);
void TitleMove(void);
void TitleOp(void);
void TitleMenu(void);
void TitleVtxPos(TITLEPOLYGON type, vec3 pos,vec3 size);
void TitleVtxCol(TITLEPOLYGON type, D3DXCOLOR col);
void SetTitleModel(MODELTYPE type, vec3 pos, vec3 rot, vec3 move, vec3 spin);
void DrawTitle2D(void);
void DrawTitle3D(void);

//=========================================================================================
//	タイトルの初期化処理
//=========================================================================================
void InitTitle(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// デバイスへのポインタ
	VERTEX_2D* pVtx;							// 頂点情報へのポインタ

	// グローバル変数初期化
	g_titleState = TITLESTATE_WAIT;
	g_nCounterTitleState = 0;

	g_titleModel[0].pModel = SetModelData(MODELTYPE_BALLOON);
	g_titleModel[0].pos = vec3(-80.0f,70.0f,-165.0f);
	g_titleModel[0].rot = vec3(0.0f, D3DX_PI * 0.125f, 0.0f);
	for (int nCnt = 1; nCnt < 6; nCnt++)
	{
		g_titleModel[nCnt].pModel = SetModelData(MODELTYPE_PRESENT);
	}
	g_titleModel[1].pos = vec3(0.0f, 0.0f, 0.0f);
	g_titleModel[2].pos = vec3(-20.0f, 0.0f, 10.0f);
	g_titleModel[2].rot.y = 1;
	g_titleModel[3].pos = vec3(10.0f, 0.0f, 25.0f);
	g_titleModel[3].rot.y = -1;
	g_titleModel[4].pos = vec3(20.0f, 0.0f, -40.0f);
	g_titleModel[5].pos = vec3(45.0f, 0.0f, -45.0f);
	g_titleModel[5].rot.y = 2;

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
		// 頂点座標の設定
		pVtx[0].pos = D3DXVECTOR3(pTitle->pos.x - pTitle->size.x, pTitle->pos.y - pTitle->size.y, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(pTitle->pos.x + pTitle->size.x, pTitle->pos.y - pTitle->size.y, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(pTitle->pos.x - pTitle->size.x, pTitle->pos.y + pTitle->size.y, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(pTitle->pos.x + pTitle->size.x, pTitle->pos.y + pTitle->size.y, 0.0f);

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
		pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
		pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
		pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

		pVtx += 4;
	}
	// 頂点バッファをアンロックする
	g_pVtxBuffTitle->Unlock();

	pTitle = &g_aTitlePolygon[0];
	for (int nCnt = 0; nCnt < TITLEPOLYGON_MAX; nCnt++, pTitle++)
	{
		switch (nCnt)
		{
		case TITLEPOLYGON_TITLE:
			pTitle->bDraw = true;
			pTitle->nTex = g_aTitleTex[TITLETEXTURE_TITLE].nTex;
			TitleVtxPos(TITLEPOLYGON_TITLE, vec3(pTitle->pos.x, -400.0f, 0.0f), pTitle->size);
			break;
		case TITLEPOLYGON_START:
			pTitle->bDraw = true;
			pTitle->nTex = g_aTitleTex[TITLETEXTURE_START_JOY].nTex;
			TitleVtxCol(TITLEPOLYGON_START, D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));
			break;
		case TITLEPOLYGON_3MIN:
			pTitle->bDraw = false;
			pTitle->nTex = g_aTitleTex[TITLETEXTURE_3MIN].nTex;
			break;
		case TITLEPOLYGON_ENDLESS:
			pTitle->bDraw = false;
			pTitle->nTex = g_aTitleTex[TITLETEXTURE_ENDLESS].nTex;
			TitleVtxCol(TITLEPOLYGON_ENDLESS, D3DXCOLOR(0.7f, 0.7f, 0.7f, 1.0f));
			break;
		default:
			break;
		}
	}

	// 霧の設定
	SetFogEnable(true);
	SetFog(COLOR_WHITE, GAMEFOG_START, GAMEFOG_END);

	// 音楽再生
	PlaySound(SOUND_LABEL_000);
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

	// 再生終了
	StopSound();
}

//=========================================================================================
//	タイトルの更新処理
//=========================================================================================
void UpdateTitle(void)
{
	PrintDebugProc("TITLE\n");
	g_nCounterTitleState++;
	PrintDebugProc("\nCounterTitle: %d\n", g_nCounterTitleState);

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
	g_titleModel[0].spin.x += 0.01f;
	g_titleModel[0].spin.y += 0.008f;
	g_titleModel[0].rot.y += cosf(g_titleModel[0].spin.y) * 0.005f;
	g_titleModel[0].pos.y += cosf(g_titleModel[0].spin.x) * 0.1f;

	UpdateField();
	UpdateSkyBox();
}

//==============================================================
// 待機状態
void TitleWait(void)
{
	// フェードが終わったら次へ
	if(GetFade() == FADE_NONE)
		SetTitleState(TITLESTATE_MOVE);
}

//==============================================================
// ぐいーん
void TitleMove(void)
{
	P_TITLE pTitle = &g_aTitlePolygon[TITLEPOLYGON_TITLE];
	float F = (float)g_nCounterTitleState / TITLE_MOVE_COUNT;
	PrintDebugProc("F : %f\n", F);

	TitleVtxPos(TITLEPOLYGON_TITLE, vec3(pTitle->pos.x, -pTitle->pos.y + (220.0f + pTitle->pos.y) * F, 0.0f), pTitle->size);
	TitleVtxCol(TITLEPOLYGON_START, D3DXCOLOR(1.0f, 1.0f, 1.0f, F));

	// 次へ
	if (GetKeyboardTrigger(DIK_RETURN) || GetKeyboardTrigger(DIK_A) || GetJoypadTrigger(JOYKEY_A) || TITLE_MOVE_COUNT <= g_nCounterTitleState)
	{
		pTitle = &g_aTitlePolygon[TITLEPOLYGON_TITLE];
		TitleVtxPos(TITLEPOLYGON_TITLE, pTitle->pos, pTitle->size);
		TitleVtxCol(TITLEPOLYGON_START, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
		SetTitleState(TITLESTATE_OP);
	}
}

//==============================================================
// ちかちかきらーん
void TitleOp(void)
{
	// 次へ
	if (GetKeyboardTrigger(DIK_RETURN) || GetKeyboardTrigger(DIK_A) || GetJoypadTrigger(JOYKEY_A))
	{
		g_aTitlePolygon[TITLEPOLYGON_TITLE].bDraw = false;
		g_aTitlePolygon[TITLEPOLYGON_START].bDraw = false;
		g_aTitlePolygon[TITLEPOLYGON_3MIN].bDraw = true;
		g_aTitlePolygon[TITLEPOLYGON_ENDLESS].bDraw = true;
		SetTitleState(TITLESTATE_MENU);
		return;
	}

	if (g_nCounterTitleState % (TITLE_WAIT_BLINK * 2) == 0)
	{
		TitleVtxCol(TITLEPOLYGON_START, COLOR_WHITE);

		g_nCounterTitleState = 0;
	}
	else if (g_nCounterTitleState % TITLE_WAIT_BLINK == 0)
	{
		TitleVtxCol(TITLEPOLYGON_START, D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f));
	}
}

//==============================================================
// モードセレクト
void TitleMenu(void)
{
	if (GetKeyboardTrigger(DIK_RETURN) || GetKeyboardTrigger(DIK_A) || GetJoypadTrigger(JOYKEY_A))
	{
		PlaySound(SOUND_LABEL_007);
		SetFade(MODE_GAME);
	}
}

//==============================================================
// 頂点座標設定
void TitleVtxPos(TITLEPOLYGON type, vec3 pos, vec3 size)
{
	VERTEX_2D* pVtx;							// 頂点情報へのポインタ

	// 頂点バッファをロックし,頂点情報へのポインタを取得
	g_pVtxBuffTitle->Lock(0, 0, (void**)&pVtx, 0);
	pVtx += (int)type * 4;

	// 頂点座標の設定
	pVtx[0].pos = D3DXVECTOR3(pos.x - size.x, pos.y - size.y, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(pos.x + size.x, pos.y - size.y, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(pos.x - size.x, pos.y + size.y, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(pos.x + size.x, pos.y + size.y, 0.0f);

	// 頂点バッファをアンロックする
	g_pVtxBuffTitle->Unlock();
}

//==============================================================
// 頂点カラー設定
void TitleVtxCol(TITLEPOLYGON type, D3DXCOLOR col)
{
	VERTEX_2D* pVtx;							// 頂点情報へのポインタ
	P_TITLE pTitle = &g_aTitlePolygon[type];	// タイトル用ポリゴン情報

	// 頂点バッファをロックし,頂点情報へのポインタを取得
	g_pVtxBuffTitle->Lock(0, 0, (void**)&pVtx, 0);
	pVtx += (int)type * 4;

	// 頂点カラーの設定
	pVtx[0].col = col;
	pVtx[1].col = col;
	pVtx[2].col = col;
	pVtx[3].col = col;

	// 頂点バッファをアンロックする
	g_pVtxBuffTitle->Unlock();
}

//=========================================================================================
//	タイトルの描画処理
//=========================================================================================
void DrawTitle(void)
{
	DrawField();
	DrawSkyBox();

	DrawTitle2D();

	DrawTitle3D();
}

//==============================================================
// タイトル2D描画
void DrawTitle2D(void)
{
	bool bFog = SetFogEnable(false);

	LPDIRECT3DDEVICE9 pDevice = GetDevice();			// デバイスへのポインタを取得

	// 頂点バッファをデータストリームに設定
	pDevice->SetStreamSource(0, g_pVtxBuffTitle, 0, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	P_TITLE pTitle = &g_aTitlePolygon[0];

	for (int nCntTex = 0; nCntTex < TITLEPOLYGON_MAX; nCntTex++, pTitle++)
	{
		if (g_titleState == TITLESTATE_MOVE && nCntTex == TITLEPOLYGON_START)
		{
			// アルファテストを無効にする
			pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);		// アルファテストを無効化
			pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);	// 比較方法(すべて描画)
			pDevice->SetRenderState(D3DRS_ALPHAREF, 255);				// 基準値を設定(すべてを描画している)
		}
		else
		{
			// アルファテストを有効にする
			pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);		// アルファテストを有効にする
			pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);	// 比較方法(基準値より大きければ描画)
			pDevice->SetRenderState(D3DRS_ALPHAREF, 150);				// アルファテストの参照値を設定(～以上で描画, intで設定)
		}

		if (pTitle->bDraw)
		{
			// テクスチャの設定
			pDevice->SetTexture(0, GetTexture(pTitle->nTex));

			// ポリゴンの描画
			pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, nCntTex * 4, 2);
		}
	}

	// アルファテストを無効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);		// アルファテストを無効化
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);	// 比較方法(すべて描画)
	pDevice->SetRenderState(D3DRS_ALPHAREF, 255);				// 基準値を設定(すべてを描画している)

	SetFogEnable(bFog);
}

//==============================================================
// タイトル3D描画
void DrawTitle3D(void)
{
	//**************************************************************
	// 変数宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();		// デバイスへのポインタ
	D3DXMATRIX		mtxRot, mtxTrans;				// マトリックス計算用
	D3DMATERIAL9	matDef;							// 現在のマテリアル保存用
	D3DXMATERIAL*	pMat;							// マテリアルデータへのポインタ

	for (int nCnt = 0; nCnt < MAX_TITLEMODEL; nCnt++)
	{
		if (g_titleModel[nCnt].pModel)
		{
			//**************************************************************
			// ワールドマトリックスの初期化
			D3DXMatrixIdentity(&g_titleModel[nCnt].mtxWorldr);

			// 向きを反映
			D3DXMatrixRotationYawPitchRoll(&mtxRot, g_titleModel[nCnt].rot.y, g_titleModel[nCnt].rot.x, g_titleModel[nCnt].rot.z);
			D3DXMatrixMultiply(&g_titleModel[nCnt].mtxWorldr, &g_titleModel[nCnt].mtxWorldr, &mtxRot);

			// 位置を反映
			D3DXMatrixTranslation(&mtxTrans, g_titleModel[nCnt].pos.x, g_titleModel[nCnt].pos.y, g_titleModel[nCnt].pos.z);
			D3DXMatrixMultiply(&g_titleModel[nCnt].mtxWorldr, &g_titleModel[nCnt].mtxWorldr, &mtxTrans);

			// ワールドマトリックスの設定
			pDevice->SetTransform(D3DTS_WORLD, &g_titleModel[nCnt].mtxWorldr);

			//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
			// 現在のマテリアルを取得
			pDevice->GetMaterial(&matDef);

			// マテリアルデータへのポインタを取得
			pMat = (D3DXMATERIAL*)g_titleModel[nCnt].pModel->pBuffMat->GetBufferPointer();

			for (int nCntMat = 0; nCntMat < (int)g_titleModel[nCnt].pModel->dwNumMat; nCntMat++)
			{
				// マテリアルの設定
				pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

				// テクスチャの設定
				pDevice->SetTexture(0, g_titleModel[nCnt].pModel->apTexture[nCntMat]);

				// モデル(パーツ)の描画
				g_titleModel[nCnt].pModel->pMesh->DrawSubset(nCntMat);
			}

			// 保存していたマテリアルに戻す
			pDevice->SetMaterial(&matDef);
			//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
		}
	}
}

//=========================================================================================
//	タイトルの状態設定
//=========================================================================================
void SetTitleState(TITLESTATE titlestate)
{
	g_titleState = titlestate;
	g_nCounterTitleState = 0;
}

//=========================================================================================
//	タイトルの状態取得
//=========================================================================================
TITLESTATE GetTitleState(void)
{
	return g_titleState;
}