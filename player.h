//==================================================================================================================================
//
//			プレイヤーの処理 [player.h]
//			Author : ENDO HIDETO
// 
//==================================================================================================================================
#ifndef _PLAYER_H_
#define _PLAYER_H_	// 2重インクルード防止

//==============================================================
// ヘッダーインクルード
#include "input.h"
#include "camera.h"

//=========================================================================================
// マクロ定義
//=========================================================================================
//==============================================================
// モデルデータ
#define MAX_PLAYER			(1)				// 最大モデル数
#define MAX_TEX_PLAYER		(8)				// プレイヤーモデルの最大テクスチャ

//==============================================================
// プレイヤーパラメータ
#define PLAYER_PARAMETERS	"data\\INFO\\status\\playerplam.bin"
#define MOVE_PLAYER			(0.1f)			// 移動速度
#define MAX_SPEED			(5.0f)			// 最大速度
#define PLAYER_JUMPFORCE	(3.5f)			// ジャンプ力
#define REV_PLAYER			(0.1f)			// 回転速度
#define BULLET_SIZE			(3.0f)			// 弾の大きさ
#define BULLET_SPPED		(3.0f)			// 弾の速度
#define BULLET_LIFE			(50)			// 弾の寿命
#define PLAYER_SHADOWSIZE	(15.0f)			// プレイヤーの影の大きさ
#define POSMOVE_FACTOR		(0.1f)			// 移動慣性の補正係数
#define ROTMOVE_FACTOR		(0.1f)			// 回転慣性の補正係数

//==============================================================
// プレイヤー操作
// キーボード
#define PLAYER_MOVE_UP_KEY		DIK_W		// 前方に動かす
#define PLAYER_MOVE_DW_KEY		DIK_S		// 後方に動かす
#define PLAYER_MOVE_R_KEY		DIK_D		// 右に動かす
#define PLAYER_MOVE_L_KEY		DIK_A		// 左に動かす
#define PLAYER_JUMP_KEY			DIK_SPACE	// ジャンプ

// コントローラー
#define PLAYER_MOVE_UP_PAD					// 前方に動かす
#define PLAYER_MOVE_DW_PAD					// 後方に動かす
#define PLAYER_MOVE_R_PAD					// 右に動かす
#define PLAYER_MOVE_L_PAD					// 左に動かす
#define PLAYER_JUMP_PAD						// ジャンプ


//==============================================================
// プレイヤー状態列挙
//==============================================================
typedef enum
{
	PLAYERSTATE_NONE = 0,
	PLAYERSTATE_WAIT,
	PLAYERSTATE_RUN,
	PLAYERSTATE_SPIN,
	PLAYERSTATE_JUMP,
	PLAYERSTATE_DEAD,
	PLAYERSTATE_MAX
}PLAYERSTATE;

//==============================================================
// プレイヤー情報構造体
//==============================================================
typedef struct
{
	D3DXVECTOR3 pos;					// 位置
	D3DXVECTOR3 posOld;					// 過去の位置
	D3DXVECTOR3 rot;					// 向き
	D3DXVECTOR3 move;					// 移動速度
	D3DXVECTOR3 spin;					// 回転速度
	int			nShadow;				// 影番号
	PLAYERSTATE state;					// 状態
	D3DXVECTOR3 vtxMin;					// 頂点の最小座標
	D3DXVECTOR3 vtxMax;					// 頂点の最大座標
	bool		bUse;					// 使っているか
}Player;

//==============================================================
// プレイヤーパラメータ情報構造体
//==============================================================
typedef struct
{
	float fSpeedforce,
	fInertia,
	fMaxSpeed;
}PlayerPlam;

//=========================================================================================
// プロトタイプ宣言
//=========================================================================================
void InitPlayer(void);
void UninitPlayer(void);
void UpdatePlayer(void);
void DrawPlayer(void);
void DrawPlayerPreview(void);
Player* GetPlayer(void);				// プレイヤー情報取得
PlayerPlam* GetPlyerPlam(void);			// プレイヤーパラメータ情報取得

#endif // !_PLAYER_H_
