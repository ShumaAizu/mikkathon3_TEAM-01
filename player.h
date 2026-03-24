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
#include "main.h"
#include "input.h"
#include "camera.h"

//=========================================================================================
// マクロ定義
//=========================================================================================
//==============================================================
// モデルデータ
#define MAX_PLAYER			(1)				// 最大モデル数
#define MAX_TEX_PLAYER		(8)				// プレイヤーモデルの最大テクスチャ
#define WORLD_END			(3000.0f)		// 世界の端っこ

//==============================================================
// プレイヤーパラメータ
#define PLAYER_PARAMETERS	"data\\INFO\\status\\playerplam.bin"
#define PLAYER_WEIGHT		(10.0f)			// 気球の重さ
#define GRAVITY				(0.008f)		// 重力
#define MOVE_FORCE			(0.5f)			// 移動速度
#define JUMP_FORCE			(0.015f)		// 上昇速度
#define GRAVITY_FOC			(1.0f)			// 重さによる重力への影響係数
#define MOVE_FOC			(1.0f)			// 重さによる左右移動への影響係数
#define SCROLL_FOC			(30)			// スクロール速度の係数
#define MAX_SPEED			(5.0f)			// 最大速度
#define REV_PLAYER			(0.1f)			// 回転速度
#define BULLET_SIZE			(3.0f)			// 弾の大きさ
#define BULLET_SPPED		(3.0f)			// 弾の速度
#define BULLET_LIFE			(50)			// 弾の寿命
#define PLAYER_SHADOWSIZE	(15.0f)			// プレイヤーの影の大きさ
#define POSMOVE_FACTOR		(0.05f)			// 移動慣性の補正係数
#define ROTMOVE_FACTOR		(0.1f)			// 回転慣性の補正係数

//==============================================================
// プレイヤー操作
// キーボード
#define PLAYER_KEY_MOVE_UP		DIK_W		// 上昇
#define PLAYER_KEY_MOVE_R		DIK_D		// 右に動かす
#define PLAYER_KEY_MOVE_L		DIK_A		// 左に動かす
#define PLAYER_KEY_SHOT			DIK_S		// アイテム投下

// コントローラー
#define PLAYER_PAD_MOVE_UP					// 上昇
#define PLAYER_PAD_MOVE_R					// 右に動かす
#define PLAYER_PAD_MOVE_L					// 左に動かす
#define PLAYER_PAD_SHOT			JOYKEY_A	// アイテム投下

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
	float		fWeight;				// 重さ
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
		fJumpforce,
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
void AddPlayerWeight(float fWeight);	// プレイヤーの重さを追加
Player* GetPlayer(void);				// プレイヤー情報取得
PlayerPlam* GetPlyerPlam(void);			// プレイヤーパラメータ情報取得

#endif // !_PLAYER_H_
