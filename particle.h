//==================================================================================================================================
//
//			パーティクルの処理 [particle.h]
//			Author : ENDO HIDETO
// 
//==================================================================================================================================
#ifndef _PARTICLE_H_
#define _PARTICLE_H_

//==============================================================
// ヘッダーインクルード
#include "main.h"
#include "effect.h"

//=========================================================================================
// マクロ定義
//=========================================================================================
#define MAX_PARTICLE	(64)		// パーティクルの最大数

//=========================================================================================
// パーティクルの種類列挙
//=========================================================================================
typedef enum
{
	PARTICLE_NONE = 0,
	PARTICLE_ITEMGET,
	PARTICLE_ITEMUSE,
	PARTICLE_EXPLOSION,
	PARTICLE_CLEAR,
	PARTICLE_MAX
}PARTICLETYPE;

//=========================================================================================
// パーティクル情報構造体定義
//=========================================================================================
typedef struct
{
	D3DXVECTOR3 pos;				// 発生位置
	int nPartLife;					// パーティクル寿命
	D3DXVECTOR3 rot;				// 飛散角度
	int nAngleWidth;				// 飛散角度幅
	int nGener;						// 発生数
	D3DXCOLOR col;					// 色
	float fSpeed;					// 移動速度
	float fRadius;					// 大きさ
	int nLife;						// 粒子寿命
	PARTICLETYPE type;				// 種類
	bool bUse;						// 構造体を使用しているか
}PARTICLE;

//=========================================================================================
// プロトタイプ宣言
//=========================================================================================
void InitParticle(void);
void UninitParticle(void);
void UpdateParticle(void);
void DrawParticle(void);

void SetParticle(D3DXVECTOR3 pos,PARTICLETYPE type);
void SetParticle(D3DXVECTOR3 pos,int nPartLife, D3DXVECTOR3 rot,int nAngle,int nGener, D3DXCOLOR col,float fSpeed,float fRadius,int nLife);
void Spiral(vec3 pos, float fHeight, float fRadius, float fRoll, int nTotal);

#endif // !_PARTICLE_H_