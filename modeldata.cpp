//=============================================================================
// 
// モデルデータ保管 [modeldata.cpp]
// Author : SHUMA AIZU
//
//=============================================================================

#include "main.h"
#include "modeldata.h"
#include "input.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define VTX_MIN			(D3DXVECTOR3(10000.0f, 10000.0f, 10000.0f))		// オブジェクトの大きさの初期化値(最小)
#define VTX_MAX			(D3DXVECTOR3(-10000.0f, -10000.0f, -10000.0f))	// オブジェクトの大きさの初期化値(最大)

//*****************************************************************************
// グローバル変数
//*****************************************************************************
ModelData g_aModelData[MODELTYPE_MAX];							// オブジェクトの情報を格納
ParentModelData g_aParentModelData[PARENTMODELTYPE_MAX];		// 階層構造オブジェクトの情報を格納
int g_nNumModelDataModel;										// モデル数を管理

//======================================================================================
// オブジェクトの初期化処理
//======================================================================================
void InitModelData(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// デバイスの取得
	g_nNumModelDataModel = 0;

	ModelData* pModelData = &g_aModelData[0];										// モデルデータへのアドレス
	ParentModelData* pParentModelData = &g_aParentModelData[0];						// 階層構造モデルデータへのアドレス

	memset(pModelData, NULL, sizeof(ModelData) * MODELTYPE_MAX);					// モデルデータの初期化
	memset(pParentModelData, NULL, sizeof(ParentModelData) * PARENTMODELTYPE_MAX);	// 階層構造オブジェクト情報の初期化
}

//======================================================================================
// オブジェクトの終了処理
//======================================================================================
void UninitModelData(void)
{
	// メッシュの破棄
	for (int nCntModel = 0; nCntModel < MODELTYPE_MAX; nCntModel++)
	{
		if (g_aModelData[nCntModel].pMesh != NULL)
		{
			g_aModelData[nCntModel].pMesh->Release();
			g_aModelData[nCntModel].pMesh = NULL;
		}
	}

	// 階層構造モデルのメッシュ破棄
	for (int nCntModelData = 0; nCntModelData < PARENTMODELTYPE_MAX; nCntModelData++)
	{
		for (int nCntModel = 0; nCntModel < MAX_PARTS; nCntModel++)
		{
			if (g_aParentModelData[nCntModelData].aModelData[nCntModel].pMesh != NULL)
			{
				g_aParentModelData[nCntModelData].aModelData[nCntModel].pMesh->Release();
				g_aParentModelData[nCntModelData].aModelData[nCntModel].pMesh = NULL;
			}
		}
	}

	// テクスチャの破棄
	for (int nCntModel = 0; nCntModel < MODELTYPE_MAX; nCntModel++)
	{
		for (int nCntMat = 0; nCntMat < (int)g_aModelData[nCntModel].dwNumMat; nCntMat++)
		{
			if (g_aModelData[nCntModel].apTexture[nCntMat] != NULL)
			{
				g_aModelData[nCntModel].apTexture[nCntMat]->Release();
				g_aModelData[nCntModel].apTexture[nCntMat] = NULL;
			}
		}
	}

	// 階層構造モデルのテクスチャ破棄
	for (int nCntModelData = 0; nCntModelData < PARENTMODELTYPE_MAX; nCntModelData++)
	{
		for (int nCntModel = 0; nCntModel < MAX_PARTS; nCntModel++)
		{
			for (int nCntMat = 0; nCntMat < (int)g_aParentModelData[nCntModelData].aModelData[nCntModel].dwNumMat; nCntMat++)
			{
				if (g_aParentModelData[nCntModelData].aModelData[nCntModel].apTexture[nCntMat] != NULL)
				{
					g_aParentModelData[nCntModelData].aModelData[nCntModel].apTexture[nCntMat]->Release();
					g_aParentModelData[nCntModelData].aModelData[nCntModel].apTexture[nCntMat] = NULL;
				}
			}
		}
	}

	// マテリアルの破棄
	for (int nCntModel = 0; nCntModel < MODELTYPE_MAX; nCntModel++)
	{
		if (g_aModelData[nCntModel].pBuffMat != NULL)
		{
			g_aModelData[nCntModel].pBuffMat->Release();
			g_aModelData[nCntModel].pBuffMat = NULL;
		}
	}

	// 階層構造モデルのマテリアル破棄
	for (int nCntModelData = 0; nCntModelData < PARENTMODELTYPE_MAX; nCntModelData++)
	{
		for (int nCntModel = 0; nCntModel < MAX_PARTS; nCntModel++)
		{
			if (g_aParentModelData[nCntModelData].aModelData[nCntModel].pBuffMat != NULL)
			{
				g_aParentModelData[nCntModelData].aModelData[nCntModel].pBuffMat->Release();
				g_aParentModelData[nCntModelData].aModelData[nCntModel].pBuffMat = NULL;
			}
		}
	}
}

//======================================================================================
// オブジェクトの更新処理
//======================================================================================
void UpdateModelData(void)
{

}

//======================================================================================
// オブジェクトの描画処理
//======================================================================================
void DrawModelData(void)
{

}

//======================================================================================
// オブジェクトファイルの読み込み処理
//======================================================================================
void LoadModelData(const char* pModelDataPath)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// デバイスの取得
	D3DXMATERIAL* pMat;							// マテリアルへのポインタ
	int nNumVtx = 0;							// 頂点数
	DWORD dwSizeFVF;							// 頂点フォーマットのサイズ
	BYTE* pVtxBuff;								// 頂点バッファへのポインタ

	// Xファイルの読み込み
	D3DXVECTOR3 vtxMin = VTX_MIN;	// オブジェクトの大きさをローカルで保存
	D3DXVECTOR3 vtxMax = VTX_MAX;	// オブジェクトの大きさをローカルで保存

	D3DXLoadMeshFromX(pModelDataPath,
		D3DXMESH_SYSTEMMEM, 
		pDevice, 
		NULL, 
		&g_aModelData[g_nNumModelDataModel].pBuffMat, 
		NULL, &g_aModelData[g_nNumModelDataModel].dwNumMat,
		&g_aModelData[g_nNumModelDataModel].pMesh);

	if (g_aModelData[g_nNumModelDataModel].pMesh == NULL)
	{ // ファイルが読み込めなかった場合、処理を抜ける
		g_nNumModelDataModel++;	// 読み込んだモデル数加算
		return;
	}

	// 頂点数を取得
	nNumVtx = g_aModelData[g_nNumModelDataModel].pMesh->GetNumVertices();

	// 頂点フォーマットのサイズを取得
	dwSizeFVF = D3DXGetFVFVertexSize(g_aModelData[g_nNumModelDataModel].pMesh->GetFVF());

	// 頂点バッファをロック
	g_aModelData[g_nNumModelDataModel].pMesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&pVtxBuff);

	// オブジェクトの大きさを取得
	for (int nCntVtx = 0; nCntVtx < nNumVtx; nCntVtx++, pVtxBuff += dwSizeFVF)
	{
		D3DXVECTOR3 vtx = *(D3DXVECTOR3*)pVtxBuff;	// 頂点座標の代入
		// X座標の比較
		if (vtxMin.x >= vtx.x)		// 保存されている最小のX座標より小さい場合
		{
			vtxMin.x = vtx.x;
		}
		else if (vtxMax.x <= vtx.x)	// 保存されている最大のX座標より大きい場合
		{
			vtxMax.x = vtx.x;
		}

		// Y座標の比較
		if (vtxMin.y >= vtx.y)		// 保存されている最小のY座標より小さい場合
		{
			vtxMin.y = vtx.y;
		}
		else if (vtxMax.y <= vtx.y)	// 保存されている最大のY座標より大きい場合
		{
			vtxMax.y = vtx.y;
		}

		// Z座標の比較
		if (vtxMin.z >= vtx.z)		// 保存されている最小のZ座標より小さい場合
		{
			vtxMin.z = vtx.z;
		}
		else if (vtxMax.z <= vtx.z)	// 保存されている最大のZ座標より大きい場合
		{
			vtxMax.z = vtx.z;
		}
	}

	// 頂点バッファをアンロック
	g_aModelData[g_nNumModelDataModel].pMesh->UnlockVertexBuffer();

	// マテリアルデータへのポインタを取得
	pMat = (D3DXMATERIAL*)g_aModelData[g_nNumModelDataModel].pBuffMat->GetBufferPointer();

	// テクスチャの読み込み
	for (int nCntMat = 0; nCntMat < (int)g_aModelData[g_nNumModelDataModel].dwNumMat; nCntMat++)
	{
		if (pMat[nCntMat].pTextureFilename != NULL)	// テクスチャファイルが存在する場合
		{
			D3DXCreateTextureFromFile(pDevice, 
				pMat[nCntMat].pTextureFilename, 
				&g_aModelData[g_nNumModelDataModel].apTexture[nCntMat]);
		}
	}

	g_nNumModelDataModel++;	// 読み込んだモデル数加算

}

//======================================================================================
// 階層構造モデルファイルの読み込み処理
//======================================================================================
void LoadParentModel(const char* pModelDataPath, int nNumParentModel)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// デバイスの取得

	D3DXMATERIAL* pMat;

	ModelData* pModelData = &g_aParentModelData[nNumParentModel].aModelData[g_aParentModelData[nNumParentModel].nNumModel];

	// Xファイルの読み込み
	D3DXLoadMeshFromX(pModelDataPath,
		D3DXMESH_SYSTEMMEM,
		pDevice,
		NULL,
		&pModelData->pBuffMat,
		NULL,
		&pModelData->dwNumMat,
		&pModelData->pMesh);

	// マテリアルデータへのポインタを取得
	pMat = (D3DXMATERIAL*)pModelData->pBuffMat->GetBufferPointer();

	for (int nCntMat = 0; nCntMat < (int)pModelData->dwNumMat; nCntMat++)
	{
		if (pMat[nCntMat].pTextureFilename != NULL)
		{// テクスチャファイルが存在する
			// テクスチャの読み込み
			D3DXCreateTextureFromFile(pDevice,
				pMat[nCntMat].pTextureFilename,
				&pModelData->apTexture[nCntMat]);
		}
	}

	g_aParentModelData[nNumParentModel].nNumModel++;
}

//=============================================================================
//	階層構造モデルのオフセット読み込み処理
//=============================================================================
void LoadParentModelOffSet(D3DXVECTOR3 pos, D3DXVECTOR3 rot, int nIdxModel, int nIdxModelParent, int nNumParentModel, int nCntParts)
{
	g_aParentModelData[nNumParentModel].nNumParts++;
	ModelData* pModelData = &g_aParentModelData[nNumParentModel].aModelData[nCntParts];

	pModelData->nIdxModel = nIdxModel;
	pModelData->nIdxModelParent = nIdxModelParent;
	pModelData->pos = pModelData->posLocal = pos;
	pModelData->rot = pModelData->rotLocal = rot;
}

//======================================================================================
// モデルデータ紐づけ処理
//======================================================================================
ModelData* SetModelData(MODELTYPE type)
{
	return &g_aModelData[type];
}

//======================================================================================
// 階層構造モデルデータ紐づけ処理
//======================================================================================
ParentModelData* SetParentModelData(PARENTMODELTYPE type)
{
	return &g_aParentModelData[type];
}