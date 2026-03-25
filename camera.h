//==================================================================================================================================
//
//			プレイヤーカメラ処理 [camera.h]
//			Author : ENDO HIDETO
//
//==================================================================================================================================
#ifndef _CAMERA_H_
#define _CAMERA_H_

//==============================================================
// ヘッダーインクルード
#include "main.h"

//=========================================================================================
// マクロ定義
//=========================================================================================
#define CAMERA_V_DEFPOS			vec3(0.0f, 250.0f, 500.0f)	// 視点のデフォ位置
#define CAMERA_R_DEFPOS			vec3(0.0f, 0.0f, 0.0f)		// 注視点のデフォ位置
#define CAMERA_ROT				vec3(1.3f, 2.73f, 0.0f)	// カメラのデフォ向き
#define CAMERA_DISTANS			(500.0f)					// カメラと注視点の距離
#define VIEW_MINDEPTH			(5.0f)						// 最小描画距離
#define VIEW_MAXDEPTH			(10000.0f)					// 最大描画距離
#define VIEW_RADIAN				(45.0f)						// 視野角

#define CAMERA_MOVE				(10.0f)						// カメラの移動速度
#define CAMERA_REV				(0.1f)						// カメラの回転
#define CAMERA_SPIN_X			(0.05f)						// カメラの水平回転速度
#define CAMERA_SPIN_Y			(0.015f)					// カメラの垂直回転速度
#define CAMERA_FOLLOW_FACTOR	(0.15f)						// カメラが追従移動する時の補正
#define CAMERA_ROTET_FACTOR		(0.1f)						// カメラが追従回転する時の補正

#define CAMERA_UPLIM			(2.8f)						// カメラの上限角度
#define CAMERA_LOWLIM			(2.8f)						// カメラの下限角度

#define UICAMERA_POSV			vec3(0.0f,20.0f,-20.0f)		// インターフェース用カメラの座標
#define UICAMERA_POSR			vec3(0.0f,0.0f,0.0f)		// インターフェース用カメラの注視点座標

#define SetUiCameraCenter(A,size)		SetUICamera(vec3(A.x - (size.x * 0.5f),A.y - (size.y * 0.5f),A.z),size)

//==============================================================
// 列挙型
//==============================================================
typedef enum
{
	CAMERATYPE_PLAYER = 0,				// プレイヤー追従
	CAMERATYPE_MINIMAP,					// ミニマップ
	//CAMERATYPE_3DUI,					// UIに3Dモデルを表示したいときのカメラ
	MAX_CAMERA
}CameraType;

//==============================================================
// カメラの構造体定義
//==============================================================
typedef struct Camera
{
	D3DXVECTOR3 posV;					// 視点	
	D3DXVECTOR3 posR;					// 注視点
	D3DXVECTOR3 posRDest;				// 目的の注視点
	D3DXVECTOR3 rot;					// 向き
	D3DXVECTOR3 rotDest;				// 向けたい方向

	float		fDist;					// 視点と注視点の距離
	float		fViewMin;				// 最小描画距離
	float		fViewMax;				// 最大描画距離
	float		fViewRadian;			// 視野角

	D3DXVECTOR3 vecU;					// 上方向ベクトル
	D3DXMATRIX	mtxProjection;			// プロジェクションマトリックス
	D3DXMATRIX	mtxView;				// ビューマトリックス
	D3DVIEWPORT9 viewport;				// ビューポート

	CameraType	type;					// カメラタイプ
	bool		bUse;					// 配列を使っているか
}Camera;
typedef Camera* P_CAMERA;

//=========================================================================================
//プロトタイプ宣言
//=========================================================================================
void InitCamera(void);
void UninitCamera(void);
void UpdateCamera(MODE mode);
void SetCamera(void);										// カメラを設置（mainのDrawの最初にする）
void SetUICamera(vec3 viewTopLeft, D3DXVECTOR2 size);		// UI用などで画面上にモデルを描画したい場合
P_CAMERA GetCamera(void);									// カメラ除法配列の先頭アドレスを取得
bool IsEnableCameraEdit(void);

#endif// !_CAMERA_H_#endif#endif