//==================================================================================================================================
// 
//			プレイヤーの処理 [player.cpp]
//			Author : ENDO HIDETO
// 
//==================================================================================================================================
//**************************************************************
// インクルード
#include "player.h"
#include "debugproc.h"
#include "item.h"
#include "shadow.h"
#include "trap.h"

//**************************************************************
// 構造体の定義

//**************************************************************
// グローバル変数
//LPDIRECT3DTEXTURE9	g_apTexturePlayer[MAX_TEX_PLAYER] = {};	// テクスチャへのポインタ
//LPD3DXMESH			g_pMeshPlayer = NULL;					// メッシュ(頂点情報)へのポインタ
//LPD3DXBUFFER		g_pMatBuffPlayer = NULL;				// マテリアルへのポインタ
//DWORD				g_dwNumMatPlayer = 0;					// マテリアル数
Player				g_player;								// プレイヤーの情報

PlayerPlam			g_playerPlam;

#define GRAVITY_FOC			(0.01f)			// 重さによる重力への影響係数
float g_fFGravity = GRAVITY;
float g_fFocGravity = GRAVITY_FOC;
#define MOVE_FOC			(1.0f)			// 重さによる左右移動への影響係数
float g_fFocMove = MOVE_FOC;
#define SCROLL_FOC			(30)			// スクロール速度の係数
int g_nFocScroll = SCROLL_FOC;

//**************************************************************
// プロトタイプ宣言
void PlayerState(void);			// プレイヤー状態管理
void PlayerContoroll(void);		// プレイヤー操作（共通
void Keyboard(void);			// キーボード
void Joypad(void);				// コントローラー
void PlayerMove(void);			// 移動
void ItemDrop(int nType = -1);	// アイテムを投下
void Collision(void);			// 当たり判定
void PlayerPlamLoad(void);		// プレイヤーパラメータ読み込み
void PlayerPlamSave(void);		// プレイヤーパラメータ保存
void DebugSetValue(float* fVal, float fAdd, int nKey, int nSubKey);

//=========================================================================================
// モデル初期化処理
//=========================================================================================
void InitPlayer(void)
{
	//**************************************************************
	// 変数宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();		// デバイスへのポインタ
	D3DXMATERIAL* pMat;								// マテリアルデータへのポインタ
	int nNumVtx;									// 頂点数
	DWORD dwSizeFVF;								// 頂点データのサイズ
	BYTE* pVtxBuff;									// 頂点情報のバッファ
	D3DXVECTOR3 vtx;								// 一時的に頂点座標を格納する変数

	//**************************************************************
	// 各値の初期化
	// プレイヤー情報
	g_player.pos = D3DXVECTOR3(0.0f, 150.0f, 0.0f);
	g_player.posOld = g_player.pos;
	g_player.rot = vec3_ZORO;
	g_player.move = vec3_ZORO;
	g_player.spin = vec3_ZORO;
	g_player.fWeight = 10.0f;						// 重さ
	g_player.state = PLAYERSTATE_NONE;				// 状態
	g_player.bUse = false;
	g_player.pModel = SetModelData(MODELTYPE_BALLOON);	// モデル呼び出し
	g_player.nShadow = SetShadow(RADIUS_BASKET);

	// 呼び出しに成功したら
	if (g_player.pModel)
	{
		g_player.bUse = true;
	}

	// アイテム欄の初期化
	for (int nCntItem = 0; nCntItem < MAX_GETITEM; nCntItem++)
	{
		g_player.nItem[nCntItem] = -1;
	}

	// プレイヤーパラメータ情報の読み込み
	PlayerPlamLoad();

	// プレイヤーの状態→今後、スタートのカウントダウンで設定
	SetPlayerState(PLAYERSTATE_RUN);
}

//=========================================================================================
// モデル終了処理
//=========================================================================================
void UninitPlayer(void)
{
	//**************************************************************
	// 影インデックスの解放
	if (g_player.nShadow != -1)
	{
		ReleaseShadow(g_player.nShadow);
		g_player.nShadow = -1;
	}
}

//=========================================================================================
// プレイヤー更新処理
//=========================================================================================
void UpdatePlayer(void)
{
	if (g_player.bUse)
	{
		//**************************************************************
		// プレイヤーの過去の位置を保存
		g_player.posOld = g_player.pos;

		//**************************************************************
		// 状態管理
		PlayerState();

		//**************************************************************
		// 操作
		PlayerContoroll();

		//**************************************************************
		// 移動
		PlayerMove();

		//**************************************************************
		// 移動
		ItemDrop();

		//**************************************************************
		// 判定
		Collision();

#if _DEBUG
		// 重さ変更
		DebugSetValue(&g_player.fWeight, 0.5f, DIK_UP, DIK_DOWN);
		PrintDebugProc("\nプレイヤーの重さ [↑/↓]: %f", g_player.fWeight);

		// 重力
		DebugSetValue(&g_fFGravity, 0.001f, DIK_O, DIK_L);
		PrintDebugProc("\n重力             [ O/ L]: %f", g_fFGravity);

		DebugSetValue(&g_fFocGravity, 0.01f, DIK_I, DIK_K);
		PrintDebugProc("\n重力係数         [ I/ K]: %f", g_fFocGravity);

		DebugSetValue(&g_playerPlam.fSpeedforce, 0.01f, DIK_U, DIK_J);
		PrintDebugProc("\n加速力           [ U/ J]: %f", g_playerPlam.fSpeedforce);

		DebugSetValue(&g_fFocMove, 0.01f, DIK_Y, DIK_H);
		PrintDebugProc("\n左右速度係数     [ Y/ H]: %f", g_fFocMove);

		if (GetKeyboardRepeat(DIK_T))
		{
			g_nFocScroll++;
		}
		if (GetKeyboardRepeat(DIK_G))
		{
			g_nFocScroll--;
			if (g_nFocScroll <= 0)
				g_nFocScroll = 1;
		}
		PrintDebugProc("\nスクロール係数   [ T/ G]: %d", g_nFocScroll);

		DebugSetValue(&g_playerPlam.fJumpforce, 0.001f, DIK_R, DIK_F);
		PrintDebugProc("\n上昇力           [ R/ F]: %f", g_playerPlam.fJumpforce);

		PrintDebugProc("\nPlayerpos :[%f, %f, %f]\n", g_player.pos.x, g_player.pos.y, g_player.pos.z);

#endif
	}
}

//==============================================================
// プレイヤーの状態管理
void PlayerState(void)
{
	//**************************************************************
	// 死んでいたら
	if (g_player.state == PLAYERSTATE_DEAD)
	{
		g_player.pos = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		g_player.posOld = g_player.pos;
		g_player.rot = vec3_ZORO;
		g_player.move = vec3_ZORO;
		g_player.spin = vec3_ZORO;
	}
}

//==============================================================
// プレイヤーの操作
void PlayerContoroll(void)
{
	// キーボード操作
	Keyboard();

	// ジョイスティック操作
	Joypad();
}

//==============================================================
// キーボード操作
void Keyboard(void)
{
	//**************************************************************
	// 変数宣言

	//**************************************************************
	// 移動
	do
	{
		if (GetKeyboardPress(PLAYER_KEY_MOVE_UP))
		{// 左の入力のみ
			g_player.move.y += g_playerPlam.fJumpforce / g_player.fWeight;
		}
		if (GetKeyboardPress(PLAYER_KEY_MOVE_L))
		{// 左の入力のみ
			g_player.move.z += g_playerPlam.fSpeedforce;
		}
		if (GetKeyboardPress(PLAYER_KEY_MOVE_R))
		{// 右の入力のみ
			g_player.move.z -= g_playerPlam.fSpeedforce;
		}
	} while (0);
}

//==============================================================
// パッド操作
void Joypad(void)
{
	//**************************************************************
	// 変数宣言
	D3DXVECTOR3 ref = GetCamera()->rot;
	D3DXVECTOR3 leftStick = vec3_ZORO;
	GetJoypadStickLeft(&leftStick.x, &leftStick.y);

	//**************************************************************
	// 移動
	if (leftStick.x != 0)
		g_player.move.z -= leftStick.x * g_playerPlam.fSpeedforce;

	if(0 < leftStick.y)
		g_player.move.y += leftStick.y * g_playerPlam.fJumpforce / g_player.fWeight;

}

//==============================================================
// プレイヤーを移動
void PlayerMove(void)
{
	//**************************************************************
	// 変数宣言	
	float fRotMove = atan2f(-g_player.move.x, -g_player.move.z) - g_player.rot.y;	// 変化させる角度(目的値と現在値の差)
	D3DXVECTOR3 ref = GetCamera()->rot;			// カメラの向き

	//**************************************************************
	// 移動に合わせて向きを変える
	if ((g_player.move.x != 0 || g_player.move.z != 0) && 0)
	{// 動いていれば
		if (D3DX_PI < fRotMove)
		{//	逆回転補正
			fRotMove -= D3DX_PI * 2;
		}
		if (fRotMove < -D3DX_PI)
		{//	逆回転補正
			fRotMove += D3DX_PI * 2;
		}

		// 回転を保存
		fRotMove *= ROTMOVE_FACTOR;
		g_player.spin.y = fRotMove;
	}

	//**************************************************************
	// 異常な角度(X_PIを超える)値を修正
	if (g_player.rot.y < -D3DX_PI)
		g_player.rot.y = D3DX_PI;
	else if (g_player.rot.y > D3DX_PI)
		g_player.rot.y = -D3DX_PI;

	//**************************************************************
	// 移動・回転
	if (g_player.move.x != 0 || g_player.move.z != 0)
	{// 水平移動
		g_player.pos.x += g_player.move.x / g_player.fWeight * g_fFocMove;
		g_player.pos.z += g_player.move.z / g_player.fWeight * g_fFocMove;
	}
	if (g_player.move.y != 0)
	{// 垂直移動
		g_player.pos.y += g_player.move.y;
	}

	// 回転
	if (g_player.spin.y != 0)
	{
		g_player.rot += g_player.spin;
	}

	//**************************************************************
	// 影の追従
	SetPotisionShadow(g_player.nShadow, g_player.pos, 1.0f);
	
	//**************************************************************
	// 慣性
	g_player.move += D3DXVECTOR3(-g_player.move.x * g_playerPlam.fInertia, 0.0f, -g_player.move.z * g_playerPlam.fInertia);
	g_player.spin += D3DXVECTOR3(0.0f, -g_player.spin.y * ROTMOVE_FACTOR, 0.0f);

	//**************************************************************
	// 重力
	g_player.move.y -= g_fFGravity * g_player.fWeight * 0.01f;

	//**************************************************************
	// 強制スクロール
	if (g_player.move.x <= g_nFocScroll / g_player.fWeight)
		g_player.move.x += MOVE_FORCE;
}

//==============================================================
// アイテムを投下
void ItemDrop(int nItem)
{
	//**************************************************************
	// 変数宣言
	int* pItem = &g_player.nItem[0];
	int* pItemNext = &g_player.nItem[1];

	// 書っとボタンが押されたら
	if (GetKeyboardTrigger(PLAYER_KEY_SHOT) || GetJoypadTrigger(PLAYER_PAD_SHOT))
	{
		// 前に詰める
		for (int nCntItem = 1; nCntItem < MAX_GETITEM; nCntItem++,pItem++,pItemNext++)
		{
			*pItem = *pItemNext;
		}

		// 最後を開ける
		*pItemNext = nItem;
	}
}

//==============================================================
// 当たり判定
void Collision(void)
{
	//**************************************************************
	// 変数宣言
	vec3	Pos[2] = { vec3(g_player.pos.x, g_player.pos.y + OFFSET_BALLOON, g_player.pos.z), vec3(g_player.pos.x, g_player.pos.y + OFFSET_BASKET, g_player.pos.z) };
	float	fRasius[2] = { RADIUS_BALLOON ,RADIUS_BASKET };
	int		nItem = -1;

	//**************************************************************
	// アイテム
	for (int nCnt = 0; nCnt < 2; nCnt++,nItem = -1)
	{
		nItem = CollisionItem(Pos[nCnt], fRasius[nCnt]);	// 風船
		// あたったアイテムを保存
		if (-1 < nItem && nItem < ITEMTYPE_MAX)
		{
			for (int nCntItem = 0; nCntItem <= MAX_GETITEM; nCntItem++)
			{
				if (g_player.nItem[nCntItem] == -1)
				{// アイテム欄に空きがあれば取得
					g_player.nItem[nCntItem] = nItem;
					break;
				}
				if (nCntItem == MAX_GETITEM)
				{// 最後まで空きがなければ、その場で一つ落としてから取得
					ItemDrop(nItem);
				}
			}
		}
	}

	// トラップ
	if (CollisionTrap(Pos[0], RADIUS_BALLOON) || CollisionTrap(Pos[1], RADIUS_BASKET))
	{

	}
	
	// 地面
	if (g_player.pos.y <= 0.0f)
	{
		g_player.pos.y = 0.0f;

		g_player.move.x = 0.0f;
		g_player.move.z = 0.0f;
		if (g_player.move.y < 0.0f)
			g_player.move.y = 0.0f;
	}

	if (WORLD_END < g_player.pos.x)
	{
		g_player.pos.x = -WORLD_END;
	}
}

//=========================================================================================
// モデル描画処理
//=========================================================================================
void DrawPlayer(void)
{
	//**************************************************************
	// 変数宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();		// デバイスへのポインタ
	D3DXMATRIX		mtxRot, mtxTrans;				// マトリックス計算用
	D3DMATERIAL9	matDef;							// 現在のマテリアル保存用
	D3DXMATERIAL*	pMat;							// マテリアルデータへのポインタ
	P_PLAYER		pPlayer = &g_player;
	if (g_player.bUse)
	{
		//**************************************************************
		// ワールドマトリックスの初期化
		D3DXMatrixIdentity(&pPlayer->mtxWorldr);

		// 向きを反映
		D3DXMatrixRotationYawPitchRoll(&mtxRot, g_player.rot.y, g_player.rot.x, g_player.rot.z);
		D3DXMatrixMultiply(&pPlayer->mtxWorldr, &pPlayer->mtxWorldr, &mtxRot);

		// 位置を反映
		D3DXMatrixTranslation(&mtxTrans, g_player.pos.x, g_player.pos.y, g_player.pos.z);
		D3DXMatrixMultiply(&pPlayer->mtxWorldr, &pPlayer->mtxWorldr, &mtxTrans);

		// ワールドマトリックスの設定
		pDevice->SetTransform(D3DTS_WORLD, &pPlayer->mtxWorldr);

		//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
		// 現在のマテリアルを取得
		pDevice->GetMaterial(&matDef);

		// マテリアルデータへのポインタを取得
		pMat = (D3DXMATERIAL*)pPlayer->pModel->pBuffMat->GetBufferPointer();

		for (int nCntMat = 0; nCntMat < (int)pPlayer->pModel->dwNumMat; nCntMat++)
		{
			// マテリアルの設定
			pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

			// テクスチャの設定
			pDevice->SetTexture(0, pPlayer->pModel->apTexture[nCntMat]);

			// モデル(パーツ)の描画
			pPlayer->pModel->pMesh->DrawSubset(nCntMat);
		}

		// 保存していたマテリアルに戻す
		pDevice->SetMaterial(&matDef);
		//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
	}
}

//=========================================================================================
// プレイヤーの重さを追加
//=========================================================================================
void AddPlayerWeight(float fWeight)
{
	g_player.fWeight += fWeight;
}

//=========================================================================================
// プレイヤーの状態を設定
//=========================================================================================
void SetPlayerState(PLAYERSTATE state)
{
	g_player.state = state;
}

//=========================================================================================
// プレイヤー情報取得
//=========================================================================================
Player* GetPlayer(void)
{
	return &g_player;
}

//=========================================================================================
// プレイヤーパラメータ情報取得
//=========================================================================================
PlayerPlam* GetPlyerPlam(void)
{
	return &g_playerPlam;
}


//=========================================================================================
// パラメータ調整	
void DebugSetValue(float* fVal, float fAdd, int nAddKey,int nSubKey)
{
	if (GetKeyboardRepeat(nAddKey))
	{
		(*fVal) += fAdd;
	}
	if (GetKeyboardRepeat(nSubKey))
	{
		(*fVal) -= fAdd;
		if ((*fVal) <= 0)
			(*fVal) = fAdd;
	}

}

//=========================================================================================
// プレイヤーパラメータ読み込み		
void PlayerPlamLoad(void)
{
	FILE* pFile;

	pFile = fopen(PLAYER_PARAMETERS, "rb");
	if (pFile != NULL)
	{// ファイルが開けたら
		fread(&g_playerPlam, sizeof(PlayerPlam), 1, pFile);
		fclose(pFile);
	}
	else
	{
		g_playerPlam.fSpeedforce = MOVE_FORCE;
		g_playerPlam.fJumpforce = JUMP_FORCE;
		g_playerPlam.fInertia = POSMOVE_FACTOR;
		g_playerPlam.fMaxSpeed = MAX_SPEED;
	}
}

//=========================================================================================
// プレイヤーパラメータ保存
void PlayerPlamSave(void)
{
	FILE* pFile;

	pFile = fopen(PLAYER_PARAMETERS, "wb");
	if (pFile != NULL)
	{// ファイルが開けたら
		fwrite(&g_playerPlam, sizeof(PlayerPlam), 1, pFile);
		fclose(pFile);
	}
}