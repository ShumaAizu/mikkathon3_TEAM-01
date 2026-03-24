//=============================================================================
//
//	ゲーム画面処理 [game.cpp]
//	Author : SHUMA AIZU
// 
//=============================================================================

#include "game.h"
#include "debugproc.h"
#include "loadscript.h"
#include "camera.h"
#include "light.h"
#include "input.h"
//#include "sound.h"
#include "fade.h"
//#include "effect.h"
//#include "particle.h"
#include "pause.h"
#include "player.h"

#include "object.h"

//*****************************************************************************
// グローバル変数
//*****************************************************************************
GAMEFLAG g_gameFlag = GAMEFLAG_NORMAL;					// ゲームの状態
GAMEFLAG g_nextgameFlag = GAMEFLAG_NORMAL;				// 次のゲームの状態
int g_nCounterGameFlag = 0;								// 状態管理カウンター
bool g_bPause = false;									// ポーズ中かどうか

//========================================
//	ゲーム画面の初期化処理
//========================================
void InitGame(void)
{
	g_gameFlag = GAMEFLAG_NORMAL;

	g_nextgameFlag = GAMEFLAG_NORMAL;

	g_nCounterGameFlag = 0;

	// ポーズ状態の初期化
	g_bPause = false;

	// ポーズメニューの初期化
	InitPause();

	// プレイヤーの初期化
	InitPlayer();

	// オブジェクトの初期化
	InitObject();

	LoadModelDataScript("data\\SCRIPTS\\modeldata.txt");

	LoadObject("data\\SCRIPTS\\model.txt");
}

//========================================
//	ゲーム画面の終了処理
//========================================
void UninitGame(void)
{
	// ポーズメニューの終了処理
	UninitPause();

	// プレイヤーの終了
	UninitPlayer();

	// オブジェクトの終了
	UninitObject();
}

//========================================
//	ゲーム画面の更新処理
//========================================
void UpdateGame(void)
{
	PrintDebugProc("GAME\n");

	if ((GetKeyboardTrigger(DIK_P) == true || GetJoypadTrigger(JOYKEY_START) == true) && GetFade() != FADE_IN && g_gameFlag == GAMEFLAG_NORMAL)
	{ // ポーズキーが押された
		g_bPause = g_bPause ? false : true;		// ポーズ状態を切り替える
		if (g_bPause == true)
		{

		}
	}

	if (g_bPause == true)
	{ // ポーズ中なら
		// ポーズの更新処理
		UpdatePause();
	}
	else if(GetFade() != FADE_OUT)
	{
		// プレイヤー更新
		UpdatePlayer();

		// オブジェクトの更新
		UpdateObject();
	}

	if (g_nextgameFlag == GAMEFLAG_CLEAR || g_nextgameFlag == GAMEFLAG_GAMEOVER)
	{ // 次の状態がクリアかゲームオーバーになったら
		g_nCounterGameFlag--;			// カウントを減らす
		if (g_nCounterGameFlag <= 0)
		{ // 回り切ったら
			// ゲーム状態を次の状態に移す
			g_gameFlag = g_nextgameFlag;
		}

		switch (g_gameFlag)
		{
			// クリア状態
		case GAMEFLAG_CLEAR: 
			SetFade(MODE_RESULT, COLOR_WHITE, DEFAULT_FADESPEED, DEFAULT_FADESPEED);
			break;
		}
	}
}

//========================================
//	ゲーム画面の描画処理
//========================================
void DrawGame(void)
{
	if (g_bPause == true)
	{ // ポーズ中なら
		// ポーズメニューの描画処理
		DrawPause();
	}

	// プレイヤーの描画
	DrawPlayer();

	DrawPlayerPreview();

	// オブジェクトの描画
	DrawObject();
}

//========================================
//	ゲームの状態設定
//========================================
void SetGameFlag(GAMEFLAG state, int nCounter)
{
	g_gameFlag = GAMEFLAG_NONE;

	g_nextgameFlag = state;

	g_nCounterGameFlag = nCounter;
}

//========================================
//	ゲームの状態の取得
//========================================
GAMEFLAG GetGameFlag(void)
{
	return g_gameFlag;
}

//========================================
//	次のゲームの状態の取得
//========================================
GAMEFLAG GetNextGameFlag(void)
{
	return g_nextgameFlag;
}

//========================================
//	ポーズ状態の取得
//========================================
bool GetPause(void)
{
	return g_bPause;
}

//========================================
//	ポーズの有効無効設定
//========================================
void SetEnablePause(bool bPause)
{
	g_bPause = bPause;
}