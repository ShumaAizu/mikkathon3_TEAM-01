//=============================================================================
//
//	Locus of Memory [main.h]
//	Author : SHUMA AIZU
// 
//=============================================================================

#ifndef _MAIN_H_			// このマクロ定義がされていなかったら
#define _MAIN_H_			// 2重インクルード防止のマクロを定義する

struct ModelData;
struct OffSetData;

//*****************************************************************************
// モードの種類
//*****************************************************************************
typedef enum
{
	MODE_NONE = -1,			// なにもない
	MODE_TITLE,				// タイトル画面
	MODE_GAME,				// ゲーム画面
	MODE_RESULT,			// リザルト画面
	MODE_MAX				// 総数
}MODE;

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define NORMAL_PLANE	(D3DXVECTOR3(0.0f, 1.0f, 0.0f))	// 平面の頂点情報

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
LPDIRECT3DDEVICE9 GetDevice(void);
void SetMode(MODE mode);
MODE GetMode(void);
float AngleNormalize(float fAngle);
float DegreeToRadian(float fDegree);
int RadianToDegree(float fRadian);
bool CrossCollision(D3DXVECTOR3* pPos, D3DXVECTOR3* pPosOld, D3DXVECTOR3 posStart, D3DXVECTOR3 posEnd, bool bRetrun, bool bIn);
void DrawParentModel(D3DXVECTOR3* pPos, D3DXVECTOR3* pRot, D3DXMATRIX* pMtxWorld, ModelData* pModelData, OffSetData *pOffSetData);
void ToggleFullscreen(HWND hWnd);
void SetTextureStageStateAlpha(int nIdx, DWORD Stage, D3DTEXTUREOP Type, DWORD Value);
void ResetTextureStageStateAlpha(int nNumIdx);
void SetTextureStageStateColor(int nIdx, DWORD Stage, D3DTEXTUREOP Type, DWORD Value);
void ResetTextureStageStateColor(int nNumIdx);
void ProcessingSpeed(bool isStart, const char* ptext = "");

#endif