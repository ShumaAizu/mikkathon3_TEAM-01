//=============================================================================
//
//	リザルト画面処理 [result.cpp]
//	Author : SHUMA AIZU
// 
//=============================================================================

#include "main.h"
#include "result.h"
#include "loadscript.h"
#include "input.h"
#include "fade.h"
#include "game.h"
#include "camera.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_RESULT			(2)			// リザルトテクスチャの枚数
#define RESULTFADE_TIMER	(300)		// タイマーの秒数

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
}

//====================================
//	リザルトの終了処理
//====================================
void UninitResult(void)
{

}

//====================================
//	リザルトの描画処理
//====================================
void DrawResult(void)
{

}

//====================================
//	リザルトの更新処理
//====================================
void UpdateResult(void)
{
	if ((GetKeyboardTrigger(DIK_RETURN) == true || GetJoypadTrigger(JOYKEY_A) == true) && g_ResultState == RESULTSTATE_RESULT)
	{
		SetFade(MODE_TITLE, COLOR_WHITE, DEFAULT_FADESPEED, DEFAULT_FADESPEED);
	}
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
