//=============================================================================
//
//	フェード処理 [fade.h]
//	Author : SHUMA AIZU
// 
//=============================================================================

#ifndef _FADE_H_
#define _FADE_H_

#include "main.h"

// フェードの状態
typedef enum
{
	FADE_NONE = 0,			// 何もしていない状態
	FADE_IN,				// フェードイン状態
	FADE_OUT,				// フェードアウト状態
	FADE_MAX				// 総数
}FADE;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void InitFade(MODE modeNext, D3DXCOLOR col);
void UninitFade(void);
void UpdateFade(void);
void DrawFade(void);

void SetFade(MODE modeNext, D3DXCOLOR col = COLOR_WHITE, float fFadeInSpeed = DEFAULT_FADESPEED, float fFadeOutSpeed = DEFAULT_FADESPEED);
void SetNoneFade(D3DXCOLOR col = COLOR_WHITE, float fFadeInSpeed = DEFAULT_FADESPEED, float fFadeOutSpeed = DEFAULT_FADESPEED);
FADE GetFade(void);

#endif
