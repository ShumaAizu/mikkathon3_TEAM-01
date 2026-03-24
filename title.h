//=============================================================================
//
//	タイトル画面処理 [title.h]
//	Author : SHUMA AIZU
// 
//=============================================================================

#ifndef _TITLE_H_
#define _TITLE_H_

#include "main.h"

//*****************************************************************************
// タイトルの状態
//*****************************************************************************
typedef enum
{
	TITLESTATE_WAIT = 0,		// 待機
	TITLESTATE_MOVE,			// 移動
	TITLESTATE_OP,				// オープニング
	TITLESTATE_MENU,			// メニュー
	TITLESTATE_MAX
}TITLESTATE;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void InitTitle(void);
void UninitTitle(void);
void UpdateTitle(void);
void DrawTitle(void);
void SetTitleState(TITLESTATE state);
TITLESTATE GetTitleState(void);

#endif
