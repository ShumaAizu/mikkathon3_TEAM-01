//=============================================================================
//
//	リザルト画面処理 [result.cpp]
//	Author : SHUMA AIZU
// 
//=============================================================================

#include "main.h"
#include "debugproc.h"
#include "result.h"
#include "loadscript.h"
#include "input.h"
#include "fade.h"
#include "game.h"
#include "camera.h"
#include "fog.h"
#include "sound.h"

#include "field.h"
#include "wall.h"
#include "skybox.h"
#include "resultdelivered.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_RESULT			(2)			// リザルトテクスチャの枚数
#define RESULTFADE_TIMER	(300)		// タイマーの秒数
#define RESULTFOG_START		(0.0f)		// リザルトでの霧の開始位置
#define RESULTFOG_END		(10000.0f)	// リザルトでの霧の終了位置

//*****************************************************************************
// グローバル変数
//*****************************************************************************
LPDIRECT3DTEXTURE9 g_pTextureResult[MAX_RESULT] = {};	// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffResult = NULL;		// 頂点バッファへのポインタ
int g_nResultFadeCounter = NULL;						// 画面遷移までのカウント
int g_bResultEvent = true;								// リザルトイベントを行っているか
RESULTSTATE g_ResultState;								// リザルトの状態

//====================================
//	リザルトの初期化処理
//====================================
void InitResult(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	g_nResultFadeCounter = 0;

	g_bResultEvent = true;

	// リザルトでの届けた数の初期化
	InitResultDelivered();

	// 霧の設定
	SetFog(D3DXCOLOR(1.0f, 0.87f, 0.56f, 1.0f), RESULTFOG_START, RESULTFOG_END);
}

//====================================
//	リザルトの終了処理
//====================================
void UninitResult(void)
{
	// リザルトでの届けた数の終了
	UninitResultDelivered();
}

//====================================
//	リザルトの更新処理
//====================================
void UpdateResult(void)
{
	PrintDebugProc("RESULT\n");

	// フィールドの更新
	UpdateField();

	// 壁の更新
	UpdateWall();

	// 空の更新
	UpdateSkyBox();

	// リザルトでの届けた数の更新
	UpdateResultDelivered();

	if (GetKeyboardTrigger(DIK_RETURN))
	{
		SetFade(MODE_TITLE);
	}

	// 再生終了
	StopSound();
}

//====================================
//	リザルトの描画処理
//====================================
void DrawResult(void)
{
	// フィールドの描画
	DrawField();

	// 壁の描画
	DrawWall();

	// 空の描画処理
	DrawSkyBox();

	bool isFog = SetFogEnable(false);

	// リザルトでの届けた数の描画
	DrawResultDelivered();

	SetFogEnable(isFog);
}

//=============================================================================
//	リザルトイベント取得処理
//=============================================================================
RESULTSTATE GetResultState(void)
{
	return g_ResultState;
}

//=============================================================================
//	リザルトイベント設定処理
//=============================================================================
void SetResultState(RESULTSTATE state)
{
	g_ResultState = state;
}
