//=============================================================================
//
//	モーションの管理 [motion.cpp]
//	Author : SHUMA AIZU
// 
//=============================================================================

#include "motion.h"
#include "loadscript.h"
#include "object.h"

//*****************************************************************************
// グローバル変数
//*****************************************************************************
MotionData g_aMotionData[MAX_MOTIONDATA];		// モーションデータ格納

//=============================================================================
//	モーションの初期化処理
//=============================================================================
void InitMotion(void)
{
	MotionData* pMotionData = &g_aMotionData[0];

	memset(pMotionData, NULL, sizeof(MotionData) * MAX_MOTIONDATA);
}

//=============================================================================
//	モーションの設定処理
//=============================================================================
void SetMotion(Motion* pMotion, ParentModelData* pParentModelData, OffSetData* pOffSetData, MOTIONTYPE motiontype, bool bLoopMotion, bool bBlendMotion, int nFrameBlend)
{
	if (pMotion->pMotionData == NULL)
	{
		return;
	}
	if (bBlendMotion == true)
	{
		pMotion->bBlendMotion = true;
		pMotion->nFrameBlend = nFrameBlend;
		pMotion->nCounterBlend = 0;

		pMotion->motionTypeBlend = motiontype;
		pMotion->bLoopMotionBlend = bLoopMotion;
		pMotion->nNumKeyBlend = pMotion->pMotionData->aMotionInfo[motiontype].nNumKey;
		pMotion->nKeyBlend = 0;
		pMotion->nCounterMotionBlend = 0;
		pMotion->bFinishMotion = false;
	}
	else
	{
		pMotion->nNumKey = pMotion->pMotionData->aMotionInfo[motiontype].nNumKey;
		pMotion->motionType = motiontype;
		pMotion->bLoopMotion = bLoopMotion;
		pMotion->nKey = 0;
		pMotion->nCounterMotion = 0;
		pMotion->bFinishMotion = false;

		ModelData* pModelData = &pParentModelData->aModelData[0];
		KEY_INFO* pKeyInfo = &pMotion->pMotionData->aMotionInfo[pMotion->motionType].aKeyInfo[0];
		D3DXVECTOR3* pOffSetPos = &pOffSetData->pos[0];
		D3DXVECTOR3* pOffSetRot = &pOffSetData->rot[0];

		for (int nCntModel = 0; nCntModel < pParentModelData->nNumModel; nCntModel++, pModelData++, pKeyInfo++, pOffSetPos++, pOffSetRot++)
		{// 全パーツの初期化
			pOffSetPos->x = pKeyInfo->aKey[0].fPosX;
			pOffSetPos->y = pKeyInfo->aKey[0].fPosY;
			pOffSetPos->z = pKeyInfo->aKey[0].fPosZ;

			pOffSetRot->x = pKeyInfo->aKey[0].fRotX;
			pOffSetRot->y = pKeyInfo->aKey[0].fRotY;
			pOffSetRot->z = pKeyInfo->aKey[0].fRotZ;

			*pOffSetPos += pModelData->posLocal;
			*pOffSetRot += pModelData->rotLocal;
		}
	}
}

//=============================================================================
//	モーションの更新処理
//=============================================================================
void UpdateMotion(Motion* pMotion, ParentModelData* pParentModelData, OffSetData* pOffSetData)
{
	if (pMotion == NULL)
	{// モーションが存在しなければ返す
		return;
	}
	if (pMotion->pMotionData == NULL)
	{
		return;
	}

	if (pMotion->bFinishMotion == true)
	{// モーションが終わっていれば返す
		return;
	}

	float fPosX, fPosY, fPosZ, fRotX, fRotY, fRotZ;
	float fDiffKey;

	// 全モデル(パーツ)の更新
	for (int nCntModel = 0; nCntModel < pParentModelData->nNumParts; nCntModel++)
	{
		KEY DiffKey = {};
		KEY RateKey = {};

		// 現在のキー
		KEY* pKey = &pMotion->pMotionData->aMotionInfo[pMotion->motionType].aKeyInfo[pMotion->nKey].aKey[nCntModel];

		// 次のキー
		KEY* pKeyNext = &pMotion->pMotionData->aMotionInfo[pMotion->motionType].aKeyInfo[(pMotion->nKey + 1) % pMotion->nNumKey].aKey[nCntModel];

		// 対象モデル
		ModelData* pModelData = &pParentModelData->aModelData[nCntModel];
		D3DXVECTOR3* pOffSetPos = &pOffSetData->pos[nCntModel];
		D3DXVECTOR3* pOffSetRot = &pOffSetData->rot[nCntModel];

		// 最大フレームと現在のフレームの比率
		float fRateKey = (float)pMotion->nCounterMotion / (float)pMotion->pMotionData->aMotionInfo[pMotion->motionType].aKeyInfo[pMotion->nKey].nFrame;

		if (pMotion->bBlendMotion == true)
		{// ブレンドあり

			// 現在の値格納用
			float fPosXCurrent, fRotXCurrent,	// 位置・向きX
				fPosYCurrent, fRotYCurrent, 	// 位置・向きY
				fPosZCurrent, fRotZCurrent;		// 位置・向きZ

			// ブレンド用
			float fPosXBlend, fRotXBlend,
				fPosYBlend, fRotYBlend,
				fPosZBlend, fRotZBlend;

			float fDiffKeyBlend, fDiffBlend;	// 差分

			// ブレンドモーションの現在のキー
			KEY* pKeyBlend = &pMotion->pMotionData->aMotionInfo[pMotion->motionTypeBlend].aKeyInfo[pMotion->nKeyBlend].aKey[nCntModel];

			// ブレンドモーションの次のキー
			KEY* pKeyNextBlend = &pMotion->pMotionData->aMotionInfo[pMotion->motionTypeBlend].aKeyInfo[(pMotion->nKeyBlend + 1) % pMotion->nNumKeyBlend].aKey[nCntModel];

			// ブレンドモーションの最大フレームと現在のフレームの比率
			float fRateKeyBlend = (float)pMotion->nCounterMotionBlend / (float)pMotion->pMotionData->aMotionInfo[pMotion->motionTypeBlend].aKeyInfo[pMotion->nKeyBlend].nFrame;

			// モーションブレンドの最大フレームと現在のフレームの比率
			float fRateBlend = (float)pMotion->nCounterBlend / (float)pMotion->nFrameBlend;

			// 座標差分
			// X座標
			fDiffKey = pKeyNext->fPosX - pKey->fPosX;
			fPosXCurrent = pKey->fPosX + (fDiffKey * fRateKey);

			fDiffKeyBlend = pKeyNextBlend->fPosX - pKeyBlend->fPosX;
			fPosXBlend = pKeyBlend->fPosX + (fDiffKeyBlend * fRateKeyBlend);

			fDiffBlend = fPosXBlend - fPosXCurrent;

			fPosX = fPosXCurrent + (fDiffBlend * fRateBlend);

			// Y座標
			fDiffKey = pKeyNext->fPosY - pKey->fPosY;
			fPosYCurrent = pKey->fPosY + (fDiffKey * fRateKey);

			fDiffKeyBlend = pKeyNextBlend->fPosY - pKeyBlend->fPosY;
			fPosYBlend = pKeyBlend->fPosY + (fDiffKeyBlend * fRateKeyBlend);

			fDiffBlend = fPosYBlend - fPosYCurrent;

			fPosY = fPosYCurrent + (fDiffBlend * fRateBlend);

			// Z座標
			fDiffKey = pKeyNext->fPosZ - pKey->fPosZ;
			fPosZCurrent = pKey->fPosZ + (fDiffKey * fRateKey);

			fDiffKeyBlend = pKeyNextBlend->fPosZ - pKeyBlend->fPosZ;
			fPosZBlend = pKeyBlend->fPosZ + (fDiffKeyBlend * fRateKeyBlend);

			fDiffBlend = fPosZBlend - fPosZCurrent;

			fPosZ = fPosZCurrent + (fDiffBlend * fRateBlend);

			// 向き差分
			// X軸
			fDiffKey = pKeyNext->fRotX - pKey->fRotX;
			fRotXCurrent = pKey->fRotX + (fDiffKey * fRateKey);
			fRotXCurrent = AngleNormalize(fRotXCurrent);

			fDiffKeyBlend = pKeyNextBlend->fRotX - pKeyBlend->fRotX;
			fRotXBlend = pKeyBlend->fRotX + (fDiffKeyBlend * fRateKeyBlend);
			fRotXBlend = AngleNormalize(fRotXBlend);

			fDiffBlend = fRotXBlend - fRotXCurrent;
			fDiffBlend = AngleNormalize(fDiffBlend);

			fRotX = fRotXCurrent + (fDiffBlend * fRateBlend);

			fRotX = AngleNormalize(fRotX);

			// Y軸
			fDiffKey = pKeyNext->fRotY - pKey->fRotY;
			fRotYCurrent = pKey->fRotY + (fDiffKey * fRateKey);
			fRotYCurrent = AngleNormalize(fRotYCurrent);

			fDiffKeyBlend = pKeyNextBlend->fRotY - pKeyBlend->fRotY;
			fRotYBlend = pKeyBlend->fRotY + (fDiffKeyBlend * fRateKeyBlend);
			fRotYBlend = AngleNormalize(fRotYBlend);

			fDiffBlend = fRotYBlend - fRotYCurrent;
			fDiffBlend = AngleNormalize(fDiffBlend);

			fRotY = fRotYCurrent + (fDiffBlend * fRateBlend);

			fRotY = AngleNormalize(fRotY);

			// Z軸
			fDiffKey = pKeyNext->fRotZ - pKey->fRotZ;
			fRotZCurrent = pKey->fRotZ + (fDiffKey * fRateKey);
			fRotZCurrent = AngleNormalize(fRotZCurrent);

			fDiffKeyBlend = pKeyNextBlend->fRotZ - pKeyBlend->fRotZ;
			fRotZBlend = pKeyBlend->fRotZ + (fDiffKeyBlend * fRateKeyBlend);
			fRotZBlend = AngleNormalize(fRotZBlend);

			fDiffBlend = fRotZBlend - fRotZCurrent;
			fDiffBlend = AngleNormalize(fDiffBlend);

			fRotZ = fRotZCurrent + (fDiffBlend * fRateBlend);

			fRotY = AngleNormalize(fRotY);
		}
		else
		{
			// 座標差分
			fDiffKey = pKeyNext->fPosX - pKey->fPosX;
			fPosX = pKey->fPosX + (fDiffKey * fRateKey);

			fDiffKey = pKeyNext->fPosY - pKey->fPosY;
			fPosY = pKey->fPosY + (fDiffKey * fRateKey);

			fDiffKey = pKeyNext->fPosZ - pKey->fPosZ;
			fPosZ = pKey->fPosZ + (fDiffKey * fRateKey);

			// 向き差分
			fDiffKey = pKeyNext->fRotX - pKey->fRotX;
			fDiffKey = AngleNormalize(fDiffKey);

			fRotX = AngleNormalize(pKey->fRotX + (fDiffKey * fRateKey));

			fDiffKey = pKeyNext->fRotY - pKey->fRotY;
			fDiffKey = AngleNormalize(fDiffKey);
			fRotY = AngleNormalize(pKey->fRotY + (fDiffKey * fRateKey));

			fDiffKey = pKeyNext->fRotZ - pKey->fRotZ;
			fDiffKey = AngleNormalize(fDiffKey);
			fRotZ = AngleNormalize(pKey->fRotZ + (fDiffKey * fRateKey));
		}

		// パーツの向きと位置を設定
		*pOffSetPos = D3DXVECTOR3(fPosX, fPosY, fPosZ);
		*pOffSetRot = D3DXVECTOR3(fRotX, fRotY, fRotZ);

		// オフセット位置加算
		*pOffSetPos += pModelData->posLocal;
		*pOffSetRot += pModelData->rotLocal;

		// 補正
		pOffSetRot->x = AngleNormalize(pOffSetRot->x);
		pOffSetRot->y = AngleNormalize(pOffSetRot->y);
		pOffSetRot->z = AngleNormalize(pOffSetRot->z);
	}

	if (pMotion->bBlendMotion == true)
	{// ブレンドあり
		// ブレンドモーションのカウンターを更新
		pMotion->nCounterMotionBlend++;

		if (pMotion->nCounterMotionBlend >= pMotion->pMotionData->aMotionInfo[pMotion->motionTypeBlend].aKeyInfo[pMotion->nKeyBlend].nFrame)
		{
			// キーを進める
			pMotion->nCounterMotionBlend = 0;	// カウンターリセット
			if ((pMotion->nKeyBlend = (pMotion->nKeyBlend + 1) % pMotion->nNumKeyBlend) == 0 && pMotion->bLoopMotionBlend == false)
			{// キーが終着点まで来たかつループ状態じゃなければ
				pMotion->bFinishMotion = true;					// 終了フラグ
			}
		}

		// ブレンドカウンターを更新
		pMotion->nCounterBlend++;
		if (pMotion->nCounterBlend >= pMotion->nFrameBlend)
		{// カウンターが回り切ったら
			pMotion->nCounterBlend = 0;									// カウンターリセット
			pMotion->motionType = pMotion->motionTypeBlend;				// 次のモーションに切り替える
			pMotion->nCounterMotion = pMotion->nCounterMotionBlend;		// モーションカウンターをブレンドと同期
			pMotion->nNumKey = pMotion->nNumKeyBlend;					// キーの総数をブレンドと同期
			pMotion->nKey = pMotion->nKeyBlend;							// キー番号をブレンドと同期
			pMotion->bLoopMotion = pMotion->bLoopMotionBlend;			// ループ状態をブレンドと同期
			pMotion->bBlendMotion = false;								// ブレンド状態を終了
		}
	}
	else
	{// ブレンドなし
		pMotion->nCounterMotion++;

		if (pMotion->nCounterMotion >= pMotion->pMotionData->aMotionInfo[pMotion->motionType].aKeyInfo[pMotion->nKey].nFrame)
		{
			// キーを進める
			pMotion->nCounterMotion = 0;	// カウンターリセット
			pMotion->nKey = (pMotion->nKey + 1) % pMotion->nNumKey;

			if (pMotion->nKey == pMotion->nNumKey - 1 && pMotion->bLoopMotion == false)
			{// 最後のキーに到達していたら
				pMotion->bFinishMotion = true;					// 終了フラグ
			}
		}
	}
}

//=============================================================================
//	モーション読み込み処理
//=============================================================================
void LoadMotion(bool bLoop, int nNumKey, KEY_INFO* pKeyInfo, int nMotion, int nNumMotion)
{
	MOTION_INFO* pMotionInfo = &g_aMotionData[nNumMotion].aMotionInfo[nMotion];

	pMotionInfo->bLoop = bLoop;
	pMotionInfo->nNumKey = nNumKey;

	for (int nCntKeyInfo = 0; nCntKeyInfo < nNumKey; nCntKeyInfo++, pKeyInfo++)
	{
		pMotionInfo->aKeyInfo[nCntKeyInfo] = *pKeyInfo;
	}

	g_aMotionData[nNumMotion].nNumMotion++;
}

//=============================================================================
//	モーションデータの設定処理
//=============================================================================
MotionData* SetMotionData(MOTIONDATATYPE type)
{
	return &g_aMotionData[type];
}