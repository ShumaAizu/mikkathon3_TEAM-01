//=============================================================================
//
//	スクリプト読み込み処理 [loadsclipt.cpp]
//	Author : SHUMA AIZU
// 
//=============================================================================

#include "loadscript.h"
#include "modeldata.h"
#include "object.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_STRING			(256)						// 読み込める文字列の最大
#define LOAD_START			"SCRIPT"					// 読み込み
#define LOAD_END			"END_SCRIPT"				// 終了
#define LOAD_OBJECT			"OBJECTSCRIPT"				// オブジェクトスクリプト読み込み
#define LOAD_MAGICOBJECT	"MAGICOBJECTSCRIPT"			// 魔法オブジェクトスクリプト読み込み
#define LOAD_PARENTMODEL	"PARENTSCRIPT"				// 階層構造モデルスクリプト読み込み
#define LOAD_PLAYER			"CHARACTERSET"				// プレイヤー情報読み込み
#define LOAD_ENDPLAYER		"END_CHARACTERSET"			// プレイヤー情報読み込み終了
#define LOAD_PARTS			"PARTSSET"					// パーツ情報読み込み
#define LOAD_ENDPARTS		"END_PARTSSET"				// パーツ情報読み込み終了
#define LOAD_MOTION			"MOTIONSCRIPT"				// モーションスクリプト読み込み
#define LOAD_MOTIONINFO		"MOTIONSET"					// モーション情報読み込み
#define LOAD_ENDMOTIONINFO	"END_MOTIONSET"				// モーション情報読み込み終了
#define LOAD_NUMMODEL		"NUM_MODEL"					// モデル数読み込み
#define LOAD_MODEL			"MODEL_FILENAME"			// モデルファイル名読み込み
#define LOAD_ITEM			"ITEM_FILENAME"				// アイテムファイル名読み込み
#define LOAD_MOTIONLOOP		"LOOP"						// ループモーションか読み込み
#define LOAD_NUMKEY			"NUM_KEY"					// キー数読み込み
#define LOAD_KEYSET			"KEYSET"					// キー情報読み込み
#define LOAD_ENDKEYSET		"END_KEYSET"				// キー情報読み込み終了
#define LOAD_FRAME			"FRAME"						// フレーム数読み込み
#define LOAD_KEY			"KEY"						// キー読み込み
#define LOAD_ENDKEY			"END_KEY"					// キー読み込み終了
#define LOAD_MODE			"MODE"						// モード読み込み
#define LOAD_POS			"POS"						// 位置読み込み
#define LOAD_ROT			"ROT"						// 向き読み込み
#define LOAD_WIDTH			"WIDTH"						// 幅読み込み
#define LOAD_HEIGHT			"HEIGHT"					// 高さ読み込み
#define LOAD_DEPTH			"DEPTH"						// 奥行き読み込み
#define LOAD_TYPE			"TYPE"						// 種類読み込み
#define LOAD_PARENTTYPE		"PARENTTYPE"				// 階層構造モデルの種類読み込み
#define LOAD_MOTIONTYPE		"MOTIONTYPE"				// モーションデータの種類読み込み
#define LOAD_MESHTYPE		"MESHTYPE"					// メッシュの種類読み込み
#define LOAD_EVENT			"EVENT"						// イベント読み込み
#define LOAD_COLLISION		"COLLISION"					// 当たり判定有無読み込み
#define LOAD_RADIUS			"RADIUS"					// 半径読み込み
#define LOAD_INDEX			"INDEX"						// インデックス読み込み
#define LOAD_PARENT			"PARENT"					// 親インデックス読み込み
#define LOAD_MODELINFO		"MODELSET"					// モデル情報読み込み
#define LOAD_ENDMODELINFO	"END_MODELSET"				// モデル情報読み込み終了
#define LOAD_COLLIDER		"COLLIDERSET"				// コライダーの情報読み込み
#define LOAD_ENDCOLLIDER	"END_COLLIDERSET"			// コライダーの情報読み込み終了

//*****************************************************************************
// グローバル変数
//*****************************************************************************
int g_nNumModel;								// 読み込んだモデル数
int g_nNumMotion;								// 読み込んだモーション数
int g_nNumParentModel;							// 読み込んだ階層構造モデル数

//=============================================================================
//	スクリプトの読み込み処理
//=============================================================================
HRESULT LoadScript(const char* pScriptFileName)
{
	// 受け取ったパスからファイルを開く
	FILE* pScriptFile = fopen(pScriptFileName, "r");

	if (pScriptFile == NULL)
	{// スクリプト読み込み失敗
		return E_FAIL;
	}

	char aStr[MAX_STRING] = {};				// 文字列格納
	char aScriptPath[FILENAME_MAX] = {};	// スクリプトファイルパス
	char aTexPath[FILENAME_MAX] = {};		// テクスチャファイルパス
	char* pStart = NULL;					// 文字列の開始位置
	int type = NULL;						// 種類
	g_nNumModel = 0;						// モデル数初期化
	g_nNumMotion = 0;						// モーション数初期化
	g_nNumParentModel = 0;					// 階層構造モデル数初期化

	while (true)
	{
		memset(aStr, NULL, sizeof(aStr));			// 文字列初期化
		fgets(aStr, sizeof(aStr), pScriptFile);		// 一行読み取り

		if (strstr(aStr, LOAD_START) != NULL)
		{// 読み込み開始
			break;
		}

		if (feof(pScriptFile) != NULL)
		{// 読み込み失敗
			return E_FAIL;
		}
	}

	while (true)
	{
		memset(aStr, NULL, sizeof(aStr));					// 文字列初期化
		(void)fgets(aStr, sizeof(aStr), pScriptFile);		// 一行読み取り

		if (strstr(aStr, LOAD_MOTION) != NULL)
		{// MOTIONSCRIPTを読み込んだ
			pStart = strchr(aStr, '=');

			(void)sscanf(pStart + 1, "%s %d", &aScriptPath, &type);

			LoadMotionInfo(aScriptPath);
		}

		if (strstr(aStr, LOAD_PARENTMODEL) != NULL)
		{// PARENTSCRIPTを読み込んだ
			pStart = strchr(aStr, '=');

			(void)sscanf(pStart + 1, "%s %d", &aScriptPath, &type);

			LoadParentModelDataScript(aScriptPath);
		}

		if (strstr(aStr, LOAD_END))
		{// END_SCRIPTを読み込んだ
			fclose(pScriptFile);
			break;
		}
	}

	return S_OK;
}

//=============================================================================
//	モデルデータ読み込み処理
//=============================================================================
HRESULT LoadModelDataScript(const char* pModelDataFileName)
{
	FILE* pModelDataFile = fopen(pModelDataFileName, "r");

	if (pModelDataFile == NULL)
	{// 読み込み失敗
		return E_FAIL;
	}

	char aStr[MAX_STRING] = {};			   // 文字列読み込み
	char aStrCpy[MAX_STRING] = {};		   // 文字列複製(整理)
	char* pStart = NULL;				   // 文字列開始位置
	char aModelPath[FILENAME_MAX] = {};	   // モデルのファイル名読み込み
	char aMeshPath[FILENAME_MAX] = {};	   // メッシュのファイル名読み込み
	bool bSetMesh = false;				   // メッシュを作るかどうか
	int nIdx = 0;						   // モデルのインデックス読み込み
	int nNumModel = 0;					   // モデル数読み込み

	while (true)
	{
		memset(aStr, NULL, sizeof(aStr));					// 文字列クリア
		(void)fgets(aStr, sizeof(aStr), pModelDataFile);	// 一列読み込み

		if (strstr(aStr, LOAD_START) != NULL)
		{// LOAD_STARTを読み込めば読み込み開始
			break;
		}

		if (feof(pModelDataFile) != NULL)
		{// 読み込み失敗
			return E_FAIL;
		}
	}

	while (true)
	{
		memset(aStr, NULL, sizeof(aStr));					// 文字列クリア
		memset(aStrCpy, NULL, sizeof(aStrCpy));				// コピーもクリア
		(void)fgets(aStr, sizeof(aStr), pModelDataFile);	// 一列読み込み
		LoadEnableString(&aStrCpy[0], &aStr[0]);			// 有効文字だけ抜き取って複製

		if (strstr(aStr, LOAD_NUMMODEL))
		{// NUM_MODELを読み込んだ
			if ((pStart = strchr(aStr, '=')) == NULL)
			{
				continue;
			}

			(void)sscanf(pStart + 1, "%d", &nNumModel);
		}

		if (strstr(aStr, LOAD_MODEL))
		{// MODEL_FILENAMEを読み込んだ
			if ((pStart = strchr(aStr, '=')) == NULL)
			{
				continue;
			}

			(void)sscanf(pStart + 1, "%s", &aModelPath);

			LoadModelData(aModelPath);
		}

		if (strcmp(aStrCpy, LOAD_END) == 0)
		{// END_SCRIPTを読み込んだ
			fclose(pModelDataFile);
			break;
		}
	}

	return S_OK;
}

//=============================================================================
//	階層構造モデルデータ読み込み処理
//=============================================================================
HRESULT LoadParentModelDataScript(const char* pParentModelDataFileName)
{
	FILE* pParentModelDataFile = fopen(pParentModelDataFileName, "r");

	if (pParentModelDataFile == NULL)
	{// 読み込み失敗
		return E_FAIL;
	}

	char aStr[MAX_STRING] = {};			   // 文字列読み込み
	char aStrCpy[MAX_STRING] = {};		   // 文字列複製(整理)
	char* pStart = NULL;				   // 文字列開始位置
	char aModelPath[FILENAME_MAX] = {};	   // モデルのファイル名読み込み
	int nIdx = 0;						   // モデルのインデックス読み込み
	int nParent = 0;					   // モデルの親インデックス読み込み
	D3DXVECTOR3 pos = {};				   // 位置読み込み
	D3DXVECTOR3 rot = {};				   // 向き読み込み
	int nNumModel = 0;					   // モデル数読み込み
	int nCntParts = 0;					   // パーツ数カウント

	while (true)
	{
		memset(aStr, NULL, sizeof(aStr));						// 文字列クリア
		(void)fgets(aStr, sizeof(aStr), pParentModelDataFile);	// 一列読み込み

		if (strstr(aStr, LOAD_START) != NULL)
		{// LOAD_STARTを読み込めば読み込み開始
			break;
		}

		if (feof(pParentModelDataFile) != NULL)
		{// 読み込み失敗
			return E_FAIL;
		}
	}

	while (true)
	{
		memset(aStr, NULL, sizeof(aStr));				// 文字列クリア
		memset(aStrCpy, NULL, sizeof(aStrCpy));			// コピーもクリア
		(void)fgets(aStr, sizeof(aStr), pParentModelDataFile);	// 一列読み込み
		LoadEnableString(&aStrCpy[0], &aStr[0]);		// 有効文字だけ抜き取って複製

		if (strstr(aStr, LOAD_NUMMODEL))
		{// NUM_MODELを読み込んだ
			if ((pStart = strchr(aStr, '=')) == NULL)
			{
				continue;
			}

			(void)sscanf(pStart + 1, "%d", &nNumModel);
		}

		if (strstr(aStr, LOAD_MODEL))
		{// MODEL_FILENAMEを読み込んだ
			if ((pStart = strchr(aStr, '=')) == NULL)
			{
				continue;
			}

			(void)sscanf(pStart + 1, "%s", &aModelPath);

			LoadParentModel(aModelPath, g_nNumParentModel);
		}

		if (strstr(aStr, LOAD_PLAYER))
		{// CHARACTERSETを読み込んだ
			while (true)
			{
				memset(aStr, NULL, sizeof(aStr));
				memset(aStrCpy, NULL, sizeof(aStrCpy));
				(void)fgets(aStr, sizeof(aStr), pParentModelDataFile);
				LoadEnableString(&aStrCpy[0], &aStr[0]);

				if (strcmp(aStrCpy, LOAD_PARTS) == 0)
				{// PARTSSETを読み込んだ
					while (true)
					{
						memset(aStr, NULL, sizeof(aStr));
						memset(aStrCpy, NULL, sizeof(aStrCpy));
						(void)fgets(aStr, sizeof(aStr), pParentModelDataFile);
						LoadEnableString(&aStrCpy[0], &aStr[0]);

						if (strstr(aStr, LOAD_INDEX))
						{// INDEXを読み込んだ
							if ((pStart = strchr(aStr, '=')) == NULL)
							{
								continue;
							}

							(void)sscanf(pStart + 1, "%d", &nIdx);

							continue;
						}

						if (strstr(aStr, LOAD_PARENT))
						{// PARENTを読み込んだ
							if ((pStart = strchr(aStr, '=')) == NULL)
							{
								continue;
							}

							(void)sscanf(pStart + 1, "%d", &nParent);

							continue;
						}

						if (strstr(aStr, LOAD_POS))
						{// POSを読み込んだ
							if ((pStart = strchr(aStr, '=')) == NULL)
							{
								continue;
							}

							(void)sscanf(pStart + 1, "%f %f %f", &pos.x, &pos.y, &pos.z);

							continue;
						}

						if (strstr(aStr, LOAD_ROT))
						{// ROTを読み込んだ
							if ((pStart = strchr(aStr, '=')) == NULL)
							{
								continue;
							}

							(void)sscanf(pStart + 1, "%f %f %f", &rot.x, &rot.y, &rot.z);

							continue;
						}

						if (strcmp(aStrCpy, LOAD_ENDPARTS) == 0)
						{// END_PARTSSETを読み込んだ
							LoadParentModelOffSet(pos, rot, nIdx, nParent, g_nNumParentModel, nCntParts);

							nCntParts++;

							memset(&pos, NULL, sizeof(D3DXVECTOR3));
							memset(&rot, NULL, sizeof(D3DXVECTOR3));
							int nIdx = 0;
							int nParet = 0;
							break;
						}
					}
				}

				if (strcmp(aStrCpy, LOAD_ENDPLAYER) == 0)
				{// END_CHARACTERSETを読み込んだ
					nCntParts = 0;
					break;
				}
			}
		}

		if (strcmp(aStrCpy, LOAD_END) == 0)
		{// END_SCRIPTを読み込んだ
			// 読み込み終了
			fclose(pParentModelDataFile);
			g_nNumParentModel++;
			break;
		}
	}

	return S_OK;
}

//=============================================================================
//	モーション情報読み込み処理
//=============================================================================
HRESULT LoadMotionInfo(const char* pMotionFileName)
{
	FILE* pMotionFile = fopen(pMotionFileName, "r");

	if (pMotionFile == NULL)
	{// 読み込み失敗
		return E_FAIL;
	}

	char aStr[MAX_STRING] = {};			   // 文字列読み込み
	char aStrCpy[MAX_STRING] = {};		   // 文字列複製(整理)
	char* pStart = NULL;				   // 文字列開始位置
	char aModelPath[FILENAME_MAX] = {};	   // モデルのファイル名読み込み
	int nIdx = 0;						   // モデルのインデックス読み込み
	int nParent = 0;					   // モデルの親インデックス読み込み
	D3DXVECTOR3 pos = {};				   // 位置読み込み
	D3DXVECTOR3 rot = {};				   // 向き読み込み
	int nNumModel = 0;					   // モデル数読み込み
	int nCntMotion = 0;					   // モーション数カウント
	int bLoop = false;					   // ループするかどうか読み込み
	int nNumKey = 0;					   // キー数読み込み
	KEY_INFO KeyInfo[MAX_KEY] = {};		   // キー情報読み込み
	int nCntKey = 0;					   // キー数カウント
	int nCntParts = 0;					   // パーツ数カウント

	while (true)
	{
		memset(aStr, NULL, sizeof(aStr));				// 文字列クリア
		(void)fgets(aStr, sizeof(aStr), pMotionFile);	// 一列読み込み

		if (strstr(aStr, LOAD_START) != NULL)
		{// LOAD_STARTを読み込めば読み込み開始
			break;
		}

		if (feof(pMotionFile) != NULL)
		{// 読み込み失敗
			return E_FAIL;
		}
	}

	while (true)
	{
		memset(aStr, NULL, sizeof(aStr));
		memset(aStrCpy, NULL, sizeof(aStrCpy));
		(void)fgets(aStr, sizeof(aStr), pMotionFile);
		LoadEnableString(&aStrCpy[0], &aStr[0]);

		if (strcmp(aStrCpy, LOAD_MOTIONINFO) == 0)
		{// MOTIONSETを読み込んだ
			while (true)
			{
				memset(aStr, NULL, sizeof(aStr));
				memset(aStrCpy, NULL, sizeof(aStrCpy));
				(void)fgets(aStr, sizeof(aStr), pMotionFile);
				LoadEnableString(&aStrCpy[0], &aStr[0]);

				if (strstr(aStr, LOAD_MOTIONLOOP))
				{// LOOPを読み込んだ
					if ((pStart = strchr(aStr, '=')) == NULL)
					{
						continue;
					}

					(void)sscanf(pStart + 1, "%d", &bLoop);

					continue;
				}

				if (strstr(aStr, LOAD_NUMKEY))
				{// NUM_KEYを読み込んだ
					if ((pStart = strchr(aStr, '=')) == NULL)
					{
						continue;
					}

					(void)sscanf(pStart + 1, "%d", &nNumKey);

					continue;
				}

				if (strcmp(aStrCpy, LOAD_KEYSET) == 0)
				{// KEYSETを読み込んだ
					while (true)
					{
						memset(aStr, NULL, sizeof(aStr));
						memset(aStrCpy, NULL, sizeof(aStrCpy));
						(void)fgets(aStr, sizeof(aStr), pMotionFile);
						LoadEnableString(&aStrCpy[0], &aStr[0]);

						if (strstr(aStr, LOAD_FRAME))
						{// FRAMEを読み込んだ
							if ((pStart = strchr(aStr, '=')) == NULL)
							{
								continue;
							}

							(void)sscanf(pStart + 1, "%d", &KeyInfo[nCntKey].nFrame);

							continue;
						}

						if (strcmp(aStrCpy, LOAD_KEY) == 0)
						{// KEYを読み込んだ
							while (true)
							{
								memset(aStr, NULL, sizeof(aStr));
								memset(aStrCpy, NULL, sizeof(aStrCpy));
								(void)fgets(aStr, sizeof(aStr), pMotionFile);
								LoadEnableString(&aStrCpy[0], &aStr[0]);

								if (strstr(aStr, LOAD_POS))
								{// POSを読み込んだ
									if ((pStart = strchr(aStr, '=')) == NULL)
									{
										continue;
									}

									(void)sscanf(pStart + 1, "%f %f %f", &KeyInfo[nCntKey].aKey[nCntParts].fPosX, &KeyInfo[nCntKey].aKey[nCntParts].fPosY, &KeyInfo[nCntKey].aKey[nCntParts].fPosZ);

									continue;
								}

								if (strstr(aStr, LOAD_ROT))
								{// ROTを読み込んだ
									if ((pStart = strchr(aStr, '=')) == NULL)
									{
										continue;
									}

									(void)sscanf(pStart + 1, "%f %f %f", &KeyInfo[nCntKey].aKey[nCntParts].fRotX, &KeyInfo[nCntKey].aKey[nCntParts].fRotY, &KeyInfo[nCntKey].aKey[nCntParts].fRotZ);

									continue;
								}

								if (strcmp(aStrCpy, LOAD_ENDKEY) == 0)
								{// END_KEYを読み込んだ
									nCntParts++;
									break;
								}
							}
						}

						if (strcmp(aStrCpy, LOAD_ENDKEYSET) == 0)
						{// END_KEYSET
							nCntKey++;
							nCntParts = 0;
							break;
						}
					}
				}

				if (strcmp(aStrCpy, LOAD_ENDMOTIONINFO) == 0)
				{// END_MOTIONSETを読み込んだ

					LoadMotion(bLoop, nNumKey, &KeyInfo[0], nCntMotion, g_nNumMotion);

					nCntMotion++;

					// 各種変数初期化
					memset(&KeyInfo, NULL, sizeof(KEY_INFO));
					bLoop = false;
					nNumKey = 0;
					nCntKey = 0;
					break;
				}

			}
		}

		if (strcmp(aStrCpy, LOAD_END) == 0)
		{// END_SCRIPTを読み込んだ
			// 読み込み終了
			fclose(pMotionFile);
			g_nNumMotion++;
			break;
		}
	}

	return S_OK;
}

//=============================================================================
//	イベント用オブジェクト情報読み込み処理
//=============================================================================
HRESULT LoadObject(const char* pObjectFileName)
{
	FILE* pStageFile = fopen(pObjectFileName, "r");

	if (pStageFile == NULL)
	{// 読み込み失敗
		return E_FAIL;
	}

	char aStr[MAX_STRING] = {};			   // 文字列読み込み
	char aStrCpy[MAX_STRING] = {};		   // 文字列複製(整理)
	char* pStart = NULL;				   // 文字列開始位置
	int nIdx = 0;						   // モデルのインデックス読み込み
	int nParent = 0;					   // モデルの親インデックス読み込み
	D3DXVECTOR3 pos = {};				   // 位置読み込み
	D3DXVECTOR3 rot = {};				   // 向き読み込み
	float fRadius = 0.0f;				   // 半径読み込み
	int type = -1;						   // 種類読み込み
	int motiontype = MOTIONDATATYPE_MAX;   // モーションデータの種類
	int nEvent = -1;					   // イベント読み込み
	int Parenttype = -1;				   // 階層構造モデルの種類読み込み
	int nCollision = true;				   // 当たり判定するか
	int nCollider = false;				   // コライダーを使うか
	int nNumModel = 0;					   // モデル数読み込み
	int nNumCollider = 0;				   // コライダー数
	int nCntModel = -1;					   // 直前に読み込んだモデル
	int nCntParentModel = -1;			   // 直前に読み込んだ階層構造モデル

	ColliderInfo aColliderInfo[10] = {};   // コライダー情報読み込み

	while (true)
	{
		memset(aStr, NULL, sizeof(aStr));				// 文字列クリア
		(void)fgets(aStr, sizeof(aStr), pStageFile);	// 一列読み込み

		if (strstr(aStr, LOAD_START) != NULL)
		{// LOAD_STARTを読み込めば読み込み開始
			break;
		}

		if (feof(pStageFile) != NULL)
		{// 読み込み失敗
			return E_FAIL;
		}
	}

	while (true)
	{
		memset(aStr, NULL, sizeof(aStr));				// 文字列クリア
		memset(aStrCpy, NULL, sizeof(aStrCpy));			// コピーもクリア
		(void)fgets(aStr, sizeof(aStr), pStageFile);	// 一列読み込み
		LoadEnableString(&aStrCpy[0], &aStr[0]);		// 有効文字だけ抜き取って複製

		if (strcmp(aStrCpy, LOAD_MODELINFO) == 0)
		{// MODELSETを読み込んだ
			while (true)
			{
				memset(aStr, NULL, sizeof(aStr));				// 文字列クリア
				memset(aStrCpy, NULL, sizeof(aStrCpy));			// コピーもクリア
				(void)fgets(aStr, sizeof(aStr), pStageFile);	// 一列読み込み
				LoadEnableString(&aStrCpy[0], &aStr[0]);		// 有効文字だけ抜き取って複製

				if (strstr(aStr, LOAD_POS))
				{// POSを読み込んだ
					if ((pStart = strchr(aStr, '=')) == NULL)
					{
						continue;
					}

					(void)sscanf(pStart + 1, "%f %f %f", &pos.x, &pos.y, &pos.z);

					continue;
				}

				if (strstr(aStr, LOAD_ROT))
				{// ROTを読み込んだ
					if ((pStart = strchr(aStr, '=')) == NULL)
					{
						continue;
					}

					(void)sscanf(pStart + 1, "%f %f %f", &rot.x, &rot.y, &rot.z);

					continue;
				}

				if (strncmp(aStrCpy, LOAD_TYPE, sizeof(LOAD_TYPE + 1)) == 0)
				{// TYPEを読み込んだ
					if ((pStart = strchr(aStr, '=')) == NULL)
					{
						continue;
					}

					(void)sscanf(pStart + 1, "%d", &type);

					continue;
				}

				if (strncmp(aStrCpy, LOAD_MOTIONTYPE, sizeof(LOAD_MOTIONTYPE + 1)) == 0)
				{// MOTIONTYPEを読み込んだ
					if ((pStart = strchr(aStr, '=')) == NULL)
					{
						continue;
					}

					(void)sscanf(pStart + 1, "%d", &motiontype);

					continue;
				}

				if (strstr(aStr, LOAD_COLLISION))
				{// COLLISIONを読み込んだ
					if ((pStart = strchr(aStr, '=')) == NULL)
					{
						continue;
					}

					(void)sscanf(pStart + 1, "%d", &nCollision);

					continue;
				}

				if (strncmp(aStrCpy, LOAD_PARENTTYPE, sizeof(LOAD_PARENTTYPE + 1)) == 0)
				{// PARENTTYPEを読み込んだ
					if ((pStart = strchr(aStr, '=')) == NULL)
					{
						continue;
					}

					(void)sscanf(pStart + 1, "%d", &Parenttype);

					continue;
				}

				if (strcmp(aStrCpy, LOAD_COLLIDER) == 0)
				{// COLLIDERSETを読み込んだ
					aColliderInfo[nNumCollider] = LoadCollider(pStageFile);
					nNumCollider++;
					nCollider = true;
				}

				if (strcmp(aStrCpy, LOAD_ENDMODELINFO) == 0)
				{// END_MODELSETを読み込んだ
					if (Parenttype != -1)
					{
						SetObjectParent(pos, rot, (PARENTMODELTYPE)Parenttype, (MOTIONDATATYPE)motiontype,
							&aColliderInfo[0], nNumCollider, (bool)nCollision, (bool)nCollider);
					}
					else
					{
						SetObjectNormal(pos, rot, (MODELTYPE)type, &aColliderInfo[0], nNumCollider, (bool)nCollision, (bool)nCollider);
					}

					nCntModel++;

					nNumCollider = 0;
					nCollider = false;
					Parenttype = -1;
					break;
				}
			}
		}

		if (strcmp(aStrCpy, LOAD_END) == 0)
		{// END_SCRIPTを読み込んだ
			fclose(pStageFile);
			break;
		}
	}

	return S_OK;
}

//=============================================================================
//	当たり判定情報読み込み処理
//=============================================================================
ColliderInfo LoadCollider(FILE *pFile)
{
	ColliderInfo ColliderInfo = {};

	char aStr[MAX_STRING] = {};								// 文字列読み込み
	char aStrCpy[MAX_STRING] = {};							// 文字列複製(整理)
	char* pStart = NULL;									// 文字列開始位置
	D3DXVECTOR3 pos = {};									// 位置読み込み
	D3DXVECTOR3 rot = {};									// 向き読み込み
	float fRadius = 0.0f;									// 半径読み込み
	int type = -1;											// 種類読み込み
	float fWidth, fHeight, fDepth;							// 大きさ読み込み

	while (true)
	{
		memset(aStr, NULL, sizeof(aStr));				// 文字列クリア
		memset(aStrCpy, NULL, sizeof(aStrCpy));			// コピーもクリア
		(void)fgets(aStr, sizeof(aStr), pFile);			// 一列読み込み
		LoadEnableString(&aStrCpy[0], &aStr[0]);		// 有効文字だけ抜き取って複製

		if (strncmp(aStrCpy, LOAD_TYPE, sizeof(LOAD_TYPE + 1)) == 0)
		{// TYPEを読み込んだ
			if ((pStart = strchr(aStr, '=')) == NULL)
			{
				continue;
			}

			(void)sscanf(pStart + 1, "%d", &type);

			continue;
		}

		if (strstr(aStr, LOAD_POS))
		{// POSを読み込んだ
			if ((pStart = strchr(aStr, '=')) == NULL)
			{
				continue;
			}

			(void)sscanf(pStart + 1, "%f %f %f", &pos.x, &pos.y, &pos.z);

			continue;
		}

		if (strstr(aStr, LOAD_ROT))
		{// ROTを読み込んだ
			if ((pStart = strchr(aStr, '=')) == NULL)
			{
				continue;
			}

			(void)sscanf(pStart + 1, "%f %f %f", &rot.x, &rot.y, &rot.z);

			continue;
		}

		if (strstr(aStr, LOAD_RADIUS))
		{// RADIUSを読み込んだ
			if ((pStart = strchr(aStr, '=')) == NULL)
			{
				continue;
			}

			(void)sscanf(pStart + 1, "%f", &fRadius);

			continue;
		}

		if (strstr(aStr, LOAD_WIDTH))
		{// WIDTHを読み込んだ
			if ((pStart = strchr(aStr, '=')) == NULL)
			{
				continue;
			}

			(void)sscanf(pStart + 1, "%f", &fWidth);

			continue;
		}

		if (strstr(aStr, LOAD_HEIGHT))
		{// HEIGHTを読み込んだ
			if ((pStart = strchr(aStr, '=')) == NULL)
			{
				continue;
			}

			(void)sscanf(pStart + 1, "%f", &fHeight);

			continue;
		}

		if (strstr(aStr, LOAD_DEPTH))
		{// DEPTHを読み込んだ
			if ((pStart = strchr(aStr, '=')) == NULL)
			{
				continue;
			}

			(void)sscanf(pStart + 1, "%f", &fDepth);

			continue;
		}

		if (strcmp(aStrCpy, LOAD_ENDCOLLIDER) == 0)
		{// END_COLLIDERSETを読み込んだ
			ColliderInfo.type = (COLLIDERTYPE)type;

			switch (ColliderInfo.type)
			{
				// 矩形
			case COLLIDERTYPE_BOX:
				ColliderInfo.Collidertype.box.pos = pos;
				ColliderInfo.Collidertype.box.rot = rot;
				ColliderInfo.Collidertype.box.fWidth = fWidth;
				ColliderInfo.Collidertype.box.fHeight = fHeight;
				ColliderInfo.Collidertype.box.fDepth = fDepth;
				break;

				// 筒
			case COLLIDERTYPE_CYLINDER:
				ColliderInfo.Collidertype.cylinder.pos = pos;
				ColliderInfo.Collidertype.cylinder.rot = rot;
				ColliderInfo.Collidertype.cylinder.fRadius = fRadius;
				break;

				// 球
			case COLLIDERTYPE_SPHERE:
				ColliderInfo.Collidertype.sphere.pos = pos;
				ColliderInfo.Collidertype.sphere.fRadius = fRadius;
				break;

				// カプセル
			case COLLIDERTYPE_CAPSULE:

				break;
			}

			return ColliderInfo;
		}
	}
}

//=============================================================================
//	有効文字抜き出し処理
//=============================================================================
void LoadEnableString(char* aStrCpy, char* pStart)
{
	while (true)
	{
		if (*pStart == '\n')
		{
			pStart++;
			continue;
		}
		else if (*pStart == '\t')
		{
			pStart++;
			continue;
		}
		else if (*pStart == 0x32)
		{
			pStart++;
			continue;
		}
		else if (*pStart == '#' || *pStart == '\0')
		{
			break;
		}
		else
		{
			strncat(aStrCpy, pStart, 1);
			pStart++;
		}
	}
}