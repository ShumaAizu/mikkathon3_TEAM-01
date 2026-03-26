//=============================================================================
//
// サウンド処理 [sound.h]
// Author : SHUMA AIZU
//          AKIRA TANAKA
//
//=============================================================================
#ifndef _SOUND_H_
#define _SOUND_H_

#include "main.h"

//*****************************************************************************
// サウンド一覧
//*****************************************************************************
typedef enum
{
	SOUND_LABEL_000 = 0,// [0] BGM タイトル
	SOUND_LABEL_001,	// [1] BGM ゲーム
	SOUND_LABEL_002,	// [2] BGM リザルト
	SOUND_LABEL_003,	// [3] SE 爆発
	SOUND_LABEL_004,	// [4] SE 落下
	SOUND_LABEL_005,	// [5] SE 上昇
	SOUND_LABEL_006,	// [6] SE 笛
	SOUND_LABEL_MAX,
} SOUND_LABEL;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitSound(HWND hWnd);
void UninitSound(void);
HRESULT PlaySound(SOUND_LABEL label);
void StopSound(SOUND_LABEL label);
void StopSound(void);
bool CheckSound(SOUND_LABEL label);

#endif
