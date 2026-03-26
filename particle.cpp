//==================================================================================================================================
//
//			パーティクルの処理 [particle.h]
//			Author : ENDO HIDETO
// 
//==================================================================================================================================
//**************************************************************
// インクルード
#include "particle.h"
#include "player.h"

//**************************************************************
//グローバル変数
const PARTICLE g_particleInfo[PARTICLE_MAX] =
{//発生位置					、パーティクル寿命、角度							、角度幅	、発生数	、色							、速さ、大きさ、粒子寿命、種類			、設定したよ
	{D3DXVECTOR3(0.0f,0.0f,0.0f)	,0	,D3DXVECTOR3(0.0f,0.0f,0.0f)			,0			,0		,D3DXCOLOR(0,0,0,0)					,0		,0		,0		,PARTICLE_NONE		,false},	// 未設定（NONE
	{D3DXVECTOR3(0.0f,0.0f,0.0f)	,20	,D3DXVECTOR3(0.0f,0.0f,0.0f)			,314		,10		,COLX_GREEN							,1.0f	,5.0f	,150	,PARTICLE_ITEMGET	,true},		// アイテムゲット
	{D3DXVECTOR3(0.0f,0.0f,0.0f)	,20	,D3DXVECTOR3(0.0f,0.0f,0.0f)			,314		,10		,COLX_RED							,1.0f	,5.0f	,150	,PARTICLE_ITEMGET	,true},		// アイテム使用
	{D3DXVECTOR3(0.0f,0.0f,0.0f)	,10	,D3DXVECTOR3(0.0f,0.0f,0.0f)			,314		,10		,D3DXCOLOR(1.0f,0.8f,0.8f,0.5f)		,1.0f	,1.0f	,150	,PARTICLE_EXPLOSION	,true},		// 弾爆発
	{D3DXVECTOR3(0.0f,0.0f,0.0f)	,0	,D3DXVECTOR3(0.0f,3.0f,0.0f)			,10			,1		,D3DXCOLOR(1.0f,0.8f,0.8f,0.5f)		,80.0f	,15.0f	,100	,PARTICLE_CLEAR		,true},		// クリア
};//セット済みパーティクル情報
PARTICLE g_particle[MAX_PARTICLE];//設置したパーティクル

//=========================================================================================
// パーティクルの初期化
//=========================================================================================
void InitParticle(void)
{
	//**************************************************************
	// 構造体未使用
	for (int nCntParticle = 0; nCntParticle < MAX_PARTICLE; nCntParticle++)
	{
		g_particle[nCntParticle].bUse = false;
	}
}

//=========================================================================================
// パーティクルの終了
//=========================================================================================
void UninitParticle(void)
{

}

//=========================================================================================
// パーティクルの更新
//=========================================================================================
void UpdateParticle(void)
{	
	//**************************************************************
	// 変数宣言
	D3DXVECTOR3 fRandRot;
	D3DXVECTOR3 randMove;

	//**************************************************************
	// パーティクル更新
	for (int nCntParticle = 0; nCntParticle < MAX_PARTICLE; nCntParticle++)
	{
		if (g_particle[nCntParticle].bUse == true)
		{
			// 寿命減
			g_particle[nCntParticle].nPartLife--;

			// お前を殺す
			if (g_particle[nCntParticle].nPartLife < 0)
			{
				g_particle[nCntParticle].bUse = false;
			}

			//**************************************************************
			// セットエフェクト
			for (int nCntEffect = 0; nCntEffect < g_particle[nCntParticle].nGener; nCntEffect++)
			{
				// ランダムに拡散
				fRandRot.y = (float)((rand() % 628 - 314) * 0.01f);
				fRandRot.z = (float)((rand() % g_particle[nCntParticle].nAngleWidth) * 0.01f);

				// 粒子速度設定
				randMove.x = sinf(g_particle[nCntParticle].rot.z + fRandRot.y) * sinf(fRandRot.z) * g_particle[nCntParticle].fSpeed;
				randMove.y = cosf(g_particle[nCntParticle].rot.y + fRandRot.z) * g_particle[nCntParticle].fSpeed;
				randMove.z = cosf(g_particle[nCntParticle].rot.x + fRandRot.y) * sinf(fRandRot.z) * g_particle[nCntParticle].fSpeed;

				SetEffect(g_particle[nCntParticle].pos, randMove,g_particle[nCntParticle].col,g_particle[nCntParticle].nLife,g_particle[nCntParticle].fRadius);				
			}
		}
	}
}

//=========================================================================================
// パーティクルの描画
//=========================================================================================
void DrawParticle(void)
{

}

//=========================================================================================
// パーティクル設置(設定済み)
//=========================================================================================
void SetParticle(D3DXVECTOR3 pos, PARTICLETYPE type)
{
	for (int nCntParticle = 0; nCntParticle < MAX_PARTICLE; nCntParticle++)
	{
		if (g_particle[nCntParticle].bUse == false)
		{//発生位置、パーティクル寿命、角度、角度幅、発生数、色、速さ、大きさ、粒子寿命、種類、設定したよ
			g_particle[nCntParticle].pos = pos;
			g_particle[nCntParticle].nPartLife = g_particleInfo[type].nPartLife;
			g_particle[nCntParticle].rot = g_particleInfo[type].rot;
			g_particle[nCntParticle].nAngleWidth = g_particleInfo[type].nAngleWidth;
			g_particle[nCntParticle].nGener = g_particleInfo[type].nGener;
			g_particle[nCntParticle].col = g_particleInfo[type].col;
			g_particle[nCntParticle].fSpeed = g_particleInfo[type].fSpeed;
			g_particle[nCntParticle].fRadius = g_particleInfo[type].fRadius;
			g_particle[nCntParticle].nLife= g_particleInfo[type].nLife;
			g_particle[nCntParticle].type = type;
			g_particle[nCntParticle].bUse = true;
			break;
		}
	}
}

//=========================================================================================
// パーティクル設置(新設定)
//=======================================
void SetParticle(D3DXVECTOR3 pos, int nPartLife, D3DXVECTOR3 rot, int nAngle, int nGener, D3DXCOLOR col, float fSpeed, float fRadius, int nLife)
{
	for (int nCntParticle = 0; nCntParticle < MAX_PARTICLE; nCntParticle++)
	{
		if (g_particle[nCntParticle].bUse == false)
		{//発生位置、パーティクル寿命、角度、角度幅、発生数、色、速さ、大きさ、粒子寿命、種類、設定したよ
			g_particle[nCntParticle].pos = pos;
			g_particle[nCntParticle].nPartLife = nPartLife;
			g_particle[nCntParticle].rot = rot;
			g_particle[nCntParticle].nAngleWidth = nAngle;
			g_particle[nCntParticle].nGener = nGener;
			g_particle[nCntParticle].col = col;
			g_particle[nCntParticle].fSpeed = fSpeed;
			g_particle[nCntParticle].fRadius = fRadius;
			g_particle[nCntParticle].nLife= nLife;
			g_particle[nCntParticle].type = PARTICLE_NONE;
			g_particle[nCntParticle].bUse = true;
			break;
		}
	}
}

//=========================================================================================
// 螺旋状にエフェクトを設置する できたけど...びみょ
//=========================================================================================
void Spiral(vec3 pos,float fTall, float fRadius, float fRoll, int nTotal)
{
	//**************************************************************
	// 変数宣言
	vec3	set;
	float	fRot = 0.0f;						// 今の角度
	float	fHeight = 0.0f;
	float	fCircle = fRoll * 2 * D3DX_PI;		// 全体で巻く角度
	bool	b = false;

	for (int nCntEffect = 0; nCntEffect < nTotal; nCntEffect++)
	{
		set = vec3(sinf(fRot) * fRadius, fHeight, cosf(fRot) * fRadius);
		fRot += fCircle / nTotal;
		
		fHeight += fTall / nTotal;
		fRadius -= fRadius / (nTotal - nCntEffect);


		SetEffect(set + pos,vec3_ZORO,colX(0.0f,0.0f,1.0f,1.0f),100,10);
	}
}
