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
#include "modeldata.h"

//=========================================================================================
// マクロ定義
//=========================================================================================
#define WORLD_END			(3000.0f)		// 世界の端っこ

//==============================================================
// モデルデータ
#define MAX_PLAYER			(1)				// 最大モデル数
#define MAX_TEX_PLAYER		(8)				// プレイヤーモデルの最大テクスチャ

//==============================================================
// プレイヤーパラメータ
#define PLAYER_PARAMETERS	"data\\INFO\\status\\playerplam.bin"
#define MAX_GETITEM			(12)			// 持てるアイテムの最大数
#define ITEM_WEIGHT			(0.2f)			// アイテム一つの重さ
#define PLAYER_FUEL			(4000)			// 初期燃料
#define PLAYER_LIFE			(1)				// プレイヤーの初期体力
#define OFFSET_BALLOON		(52.25f)		// 気球の風船の当たり判定のオフセット
#define RADIUS_BALLOON		(27.5f)			// 気球の風船の当たり判定の半径
#define OFFSET_BASKET		(7.5f)			// 気球のかごの当たり判定のオフセット
#define RADIUS_BASKET		(15.0f)			// 気球のかごの当たり判定の半径
#define PLAYER_WEIGHT		(10.0f)			// 気球の重さ
#define GRAVITY				(0.0006f)		// 重力
#define MOVE_FORCE			(0.8f)			// 加速力
#define JUMP_FORCE			(0.1f)			// 上昇速度
#define MOVE_FOC			(1.0f)			// 重さによる左右移動への影響係数
#define SCROLL_FOC			(210)			// スクロール速度の係数
#define MAX_MOVE_SPEED		(1.0f)			// 最大移動速度
#define MAX_RISE_SPEED		(0.4f)			// 最大上昇速度
#define MAX_DROP_SPEED		(-0.3f)			// 最大下降速度
#define PLAYER_SHADOWSIZE	(15.0f)			// プレイヤーの影の大きさ
#define POSMOVE_FACTOR		(0.05f)			// 移動慣性の補正係数
#define ROTMOVE_FACTOR		(0.1f)			// 回転慣性の補正係数
#define GRAVITY_FAC			(0.002f)		// 重さによる重力への影響係数
#define MOVE_FAC			(1.0f)			// 重さによる左右移動への影響係数
#define SCROLL_FAC			(30)			// スクロール速度の係数

//==============================================================
// 制限区間
#define HEIGHT_LIMIT		(400.0f)		// 限界高度
#define HEIGHT_RES			(330.0f)		// 制限高度
#define HEIGHT_EASE			(50.0f)			// 上昇力ブーストエリア
#define PENALTY_WEIGHT		(8.0f)			// 制限高度による負荷
#define EASE_WEIGHT			(5.0f)			// 制限高度による緩和
#define HEIGHT_GROUND		(0.0f)			// 地上
#define DEPTH_FRONT			(-100.0f)		// 手前
#define DEPTH_END			(150.0f)		// 奥

//==============================================================
// プレイヤー操作
// キーボード
#define PLAYER_KEY_MOVE_UP		DIK_W		// 上昇
#define PLAYER_KEY_MOVE_R		DIK_D		// 右に動かす
#define PLAYER_KEY_MOVE_L		DIK_A		// 左に動かす
#define PLAYER_KEY_SHOT			DIK_S		// アイテム投下

// コントローラー
#define PLAYER_PAD_MOVE_UP		JOYKEY_UP		// 上昇
#define PLAYER_PAD_MOVE_R		JOYKEY_RIGHT	// 右に動かす
#define PLAYER_PAD_MOVE_L		JOYKEY_LEFT		// 左に動かす
#define PLAYER_PAD_SHOT_SUB		JOYKEY_A		// アイテム投下
#define PLAYER_PAD_SHOT			JOYKEY_DOWN		// アイテム投下

//==============================================================
// プレイヤー状態列挙
//==============================================================
typedef enum
{
	PLAYERSTATE_NONE = 0,	// 動きなし
	PLAYERSTATE_RUN,		// 生きてる
	PLAYERSTATE_DEAD,		// 死亡
	PLAYERSTATE_MAX
}PLAYERSTATE;

//==============================================================
// プレイヤー情報構造体
//==============================================================
typedef struct Player
{
	D3DXVECTOR3 pos;				// 位置
	D3DXVECTOR3 posOld;				// 過去の位置
	D3DXVECTOR3 rot;				// 向き
	D3DXVECTOR3 move;				// 移動速度
	D3DXVECTOR3 spin;				// 回転速度
	int			nLife;				// 体力
	float		fFuel;				// 燃料
	int			nShadow;			// 影番号
	float		fWeight;			// 重さ
	PLAYERSTATE state;				// 状態
	ModelData*	pModel;				// モデルデータ
	D3DXMATRIX	mtxWorldr;			// ワールドマトリックス
	int			nItem[MAX_GETITEM];	// 獲得したアイテム
	bool		bUse;				// 使っているか
}Player;
typedef Player* P_PLAYER;

//=========================================================================================
// プロトタイプ宣言
//=========================================================================================
void InitPlayer(void);
void UninitPlayer(void);
void UpdatePlayer(void);
void DrawPlayer(void);
void AddPlayerWeight(float fWeight);	// プレイヤーの重さを追加
void SetPlayerState(PLAYERSTATE state);	// プレイヤーの状態を設定
Player* GetPlayer(void);				// プレイヤー情報取得
int* GetPlayerItem(void);				// プレイヤーの所持アイテム欄を取得

#endif // !_PLAYER_H_
