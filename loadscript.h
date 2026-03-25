//=============================================================================
//
//	スクリプト読み込み処理 [loadsclipt.h]
//	Author : SHUMA AIZU
// 
//=============================================================================

#ifndef _LOADSCRIPT_H_
#define _LOADSCRIPT_H_

#include "main.h"
#include "collision.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT LoadScript(const char* pScriptFileName);
HRESULT LoadModelDataScript(const char* pModelDataFileName);
HRESULT LoadParentModelDataScript(const char* pParentModelDataFileName);
HRESULT LoadMotionInfo(const char* pMotionFileName);
HRESULT LoadObject(const char* pObjectFileName);
ColliderInfo LoadCollider(FILE* pFile);
HRESULT LoadTrapPattern(const char* pTrapPatternScript);
void LoadEnableString(char* aStrCpy, char* pStart);

#endif	_LOADSCRIPT_H_