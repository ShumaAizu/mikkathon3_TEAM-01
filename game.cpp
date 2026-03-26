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
#include "sound.h"
#include "fade.h"
#include "fog.h"
//#include "effect.h"
//#include "particle.h"
#include "pause.h"
#include "player.h"

#include "object.h"
#include "field.h"
#include "wall.h"
#include "fallpoint.h"
#include "shadow.h"
#include "trap.h"
#include "item.h"
#include "fallitem.h"
#include "skybox.h"

#include "time.h"
#include "delivered.h"
#include "cutin.h"
#include "tutorialboard.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************

//*****************************************************************************
// グローバル変数
//*****************************************************************************
GAMEFLAG g_gameFlag = GAMEFLAG_NORMAL;					// ゲームの状態
GAMEFLAG g_nextgameFlag = GAMEFLAG_NORMAL;				// 次のゲームの状態
int g_nCounterGameFlag = 0;								// 状態管理カウンター
bool g_bPause = false;									// ポーズ中かどうか

int g_nGameLevel;										// ゲームレベル

//=============================================================================
//	ゲーム画面の初期化処理
//=============================================================================
void InitGame(void)
{
	g_gameFlag = GAMEFLAG_NORMAL;

	g_nextgameFlag = GAMEFLAG_NORMAL;

	g_nCounterGameFlag = 0;

	g_nGameLevel = 0;

	// ポーズ状態の初期化
	g_bPause = false;

	// ポーズメニューの初期化
	InitPause();

	// 影の初期化
	InitShadow();

	// プレイヤーの初期化
	InitPlayer();

	// オブジェクトの初期化
	InitObject();

	// トラップの初期化
	InitTrap();

	// アイテムの初期化
	InitItem();

	// 落下アイテムの初期化
	InitFallItem();

	// 落下地点目印の初期化
	InitFallPoint();

	InitTutorialboard();

	// カットインの初期化
	InitCutIn();

	// タイムの初期化
	InitTime();

	// 届けた数の初期化
	InitDelivered();

	// ステージ情報の読み込み
	LoadObject("data\\SCRIPTS\\model.txt");

	// トラップパターンの読み込み
	LoadTrapPattern("data\\SCRIPTS\\TrapPatternInfo.txt");

	// アイテムパターンの読み込み
	LoadItemPattern("data\\SCRIPTS\\ItemPatternInfo.txt");

	// 最初のトラップ設置
	SetTrap();

	// 最初のアイテムを設置
	SetItem();

	PlaySound(SOUND_LABEL_001);
}

//=============================================================================
//	ゲーム画面の終了処理
//=============================================================================
void UninitGame(void)
{
	// ポーズメニューの終了処理
	UninitPause();

	// プレイヤーの終了
	UninitPlayer();

	// オブジェクトの終了
	UninitObject();

	// トラップの終了
	UninitTrap();

	// アイテムの終了
	UninitItem();

	// 落下アイテムの終了
	UninitFallItem();

	// 落下地点目印の終了
	UninitFallPoint();

	// 
	UninitTutorialboard();

	// 影の終了
	UninitShadow();

	// カットインの終了
	UninitCutIn();

	// タイムの終了
	UninitTime();

	// 届けた数の終了
	UninitDelivered();

	// 再生終了
	StopSound();
}

//=============================================================================
//	ゲーム画面の更新処理
//=============================================================================
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

		// フィールドの更新
		UpdateField();

		// 壁の更新
		UpdateWall();

		// トラップの更新
		UpdateTrap();

		// アイテムの更新
		UpdateItem();

		// 落下アイテムの更新
		UpdateFallItem();

		// 落下地点目印の更新
		UpdateFallPoint();

		UpdateTutorialboard();

		// 影の更新
		UpdateShadow();

		// 空の更新
		UpdateSkyBox();

		// カットインの更新
		UpdateCutIn();

		// タイムの更新
		UpdateTime();

		// 届けた数の更新
		UpdateDelivered();
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
			// ゲームオーバー
		case GAMEFLAG_GAMEOVER:
			SetFade(MODE_RESULT,COLOR_BLACK);
			break;
			// クリア状態
		case GAMEFLAG_CLEAR: 
			SetFade(MODE_RESULT, COLOR_WHITE, DEFAULT_FADESPEED, DEFAULT_FADESPEED);
			break;
		}
	}
}

//=============================================================================
//	ゲーム画面の描画処理
//=============================================================================
void DrawGame(void)
{

	// プレイヤーの描画
	DrawPlayer();

	// オブジェクトの描画
	DrawObject();

	// フィールドの描画
	DrawField();

	// 壁の描画
	DrawWall();

	// トラップの描画
	DrawTrap();

	// アイテムの描画
	DrawItem();

	// 落下アイテムの描画
	DrawFallItem();

	// 落下地点目印の描画
	DrawFallPoint();

	DrawTutorialboard();

	// 空の描画
	DrawSkyBox();

	// 霧の切り替え
	bool isFog = SetFogEnable(false);

	// 影の描画
	DrawShadow();

	// カットインの描画
	DrawCutIn();

	// タイムの描画
	DrawTime();

	// 届けた数の描画
	DrawDelivered();

	if (g_bPause == true)
	{ // ポーズ中なら
		// ポーズメニューの描画処理
		DrawPause();
	}

	// 再設定
	SetFogEnable(isFog);
}

//=============================================================================
//	ゲームの状態設定
//=============================================================================
void SetGameFlag(GAMEFLAG state, int nCounter)
{
	g_gameFlag = GAMEFLAG_NONE;

	g_nextgameFlag = state;

	g_nCounterGameFlag = nCounter;
}

//=============================================================================
//	ゲームの状態の取得
//=============================================================================
GAMEFLAG GetGameFlag(void)
{
	return g_gameFlag;
}

//=============================================================================
//	次のゲームの状態の取得
//=============================================================================
GAMEFLAG GetNextGameFlag(void)
{
	return g_nextgameFlag;
}

//=============================================================================
//	ポーズ状態の取得
//=============================================================================
bool GetPause(void)
{
	return g_bPause;
}

//=============================================================================
//	ポーズの有効無効設定
//=============================================================================
void SetEnablePause(bool bPause)
{
	g_bPause = bPause;
}

//=============================================================================
//	ゲームレベルの取得
//=============================================================================
int GetGameLevel(void)
{
	return g_nGameLevel;
}

//=============================================================================
//	ゲームレベルの増加
//=============================================================================
void AddGameLevel(void)
{
	if (g_nGameLevel >= MAX_GAMELEVEL - 1)
	{
		return;
	}

	g_nGameLevel++;
}