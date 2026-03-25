//=============================================================================
//
//	タイトル画面処理 [title.h]
//	Author : ENDO HIDETO
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
// タイトルの表示物
//*****************************************************************************
typedef enum
{
	TITLEPOLYGON_TITLE = 0,		// タイトル
	TITLEPOLYGON_START,			// スタート
	TITLEPOLYGON_3MIN,			// ３分モード
	TITLEPOLYGON_ENDLESS,		// エンドレスモード
	TITLEPOLYGON_MAX
}TITLEPOLYGON;
typedef enum
{
	TITLEMODEL_BALLOON = 0,		// 気球
	TITLEMODEL_PRESENT,			// プレゼントボックス
	TITLEMODEL_MAX
}TITLEMODEL;

//*****************************************************************************
// タイトルのテクスチャ
//*****************************************************************************
typedef enum
{
	TITLETEXTURE_TITLE = 0,		// タイトル
	//TITLETEXTURE_START_KEY,		// キーボードのスタート
	TITLETEXTURE_START_JOY,		// コントローラーのスタート
	TITLETEXTURE_3MIN,			// ３分モード
	TITLETEXTURE_ENDLESS,		// エンドレスモード
	TITLETEXTURE_MAX
}TITLETEXTURE;

//==============================================================
// 2Dポリ情報構造体
//==============================================================
typedef struct Title
{
	vec3	pos;		// 位置
	vec3	size;		// 大きさ
	int		nTex;		// テクスチャ
	bool	bDraw;		// 描画しているか
}Title;
typedef Title* P_TITLE;

//==============================================================
// モデル情報構造体
//==============================================================
typedef struct TitleModel
{
	vec3		pos;		// 位置
	vec3		move;		// 速度
	vec3		rot;		// 向き
	vec3		spin;		// 回転
	D3DXMATRIX	mtxWorldr;	// ワールドマトリックス
	ModelData*	pModel;		// モデル情報
	bool		bDraw;		// 描画しているか
}TitleModel;
typedef TitleModel* P_TITLEMODLE;

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
