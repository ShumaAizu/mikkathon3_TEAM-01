//=============================================================================
// 
// モデルデータ保管 [modeldata.h]
// Author : SHUMA AIZU
// 
//=============================================================================

#ifndef _MODELDATA_H_
#define	_MODELDATA_H_
#include "main.h"

#include "motion.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_MODELMAT		(32)	// マテリアルの最大数
#define MAX_PARTS			(32)	// パーツの最大数

//*****************************************************************************
// モデルの種類
//*****************************************************************************
typedef enum MODELTYPE
{
	MODELTYPE_000 = 0,		// [0]
	MODELTYPE_001 = 0,		// [1]
	MODELTYPE_MAX
}MODELTYPE;

//*****************************************************************************
// 階層構造モデルの種類
//*****************************************************************************
typedef enum PARENTMODELTYPE
{
	PARENTMODELTYPE_000 = 0,	// [0]
	PARENTMODELTYPE_MAX
}PARENTMODELTYPE;

//*****************************************************************************
// オフセットデータの構造体定義
//*****************************************************************************
typedef struct OffSetData
{
	D3DXVECTOR3 pos[MAX_PARTS];						// 位置
	D3DXVECTOR3 rot[MAX_PARTS];						// 向き
}OffSetData;

//*****************************************************************************
// モデルデータの構造体定義
//*****************************************************************************
typedef struct ModelData
{
	LPDIRECT3DTEXTURE9 apTexture[MAX_MODELMAT];	// テクスチャ
	LPD3DXMESH pMesh;							// メッシュへのポインタ
	LPD3DXBUFFER pBuffMat;						// マテリアルへのポインタ
	DWORD dwNumMat;								// マテリアルの数
	int nIdxModel;								// 親モデルのインデックス
	D3DXVECTOR3 pos;							// モデルの位置 (オフセット)
	D3DXVECTOR3 posLocal;						// モデルの位置 (ローカル)
	D3DXVECTOR3 rot;							// モデルのインデックス
	int nIdxModelParent;						// 向き
	D3DXVECTOR3 rotLocal;						// 向き (ローカル)
	D3DXMATRIX mtxWorld;						// ワールドマトリックス
}ModelData;

//*****************************************************************************
// 階層構造モデルデータの構造体定義
//*****************************************************************************
typedef struct ParentModelData
{
	ModelData aModelData[MAX_PARTS];	// モデル (パーツ)
	int nNumModel;						// モデルの総数
	int	nNumParts;						// パーツの総数
}ParentModelData;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void InitModelData(void);
void UninitModelData(void);
void UpdateModelData(void);
void DrawModelData(void);
void LoadModelData(const char* pModelDataPath);
void LoadParentModel(const char* pModelDataPath, int nNumParentModel);
void LoadParentModelOffSet(D3DXVECTOR3 pos, D3DXVECTOR3 rot, int nIdxModel, int nIdxModelParent, int nNumParentModel, int nCntParts);
ModelData* SetModelData(MODELTYPE type);
ParentModelData* SetParentModelData(PARENTMODELTYPE type);

#endif