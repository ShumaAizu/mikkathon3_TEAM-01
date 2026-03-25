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
#include "shadow.h"

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

#define GRAVITY_FOC			(1.0f)			// 重さによる重力への影響係数
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
void ItemDrop(void);			// アイテムを投下
void Collision(void);			// 当たり判定
void PlayerPlamLoad(void);		// プレイヤーパラメータ読み込み
void PlayerPlamSave(void);		// プレイヤーパラメータ保存

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
	g_player.pModel = SetModelData(MODELTYPE_002);	// モデル呼び出し

	// 呼び出しに成功したら
	if (g_player.pModel)
	{
		g_player.bUse = true;
	}

	// プレイヤーパラメータ情報の読み込み
	PlayerPlamLoad();

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
		if (GetKeyboardRepeat(DIK_UP))
		{
			g_player.fWeight += 1.0f;
		}
		if (GetKeyboardRepeat(DIK_DOWN))
		{
			g_player.fWeight -= 1.0f;
		}
		PrintDebugProc("\nplayer weight : %f", g_player.fWeight);

		//**************************************************************
		// プレイヤーパラメータ情報
		if (GetKeyboardTrigger(DIK_F9) && GetKeyboardPress(DIK_LCONTROL) && 0)
		{// 保存
			PlayerPlamSave();
		}

		//// 慣性力
		//if (GetKeyboardRepeat(DIK_9))
		//	g_playerPlam.fInertia += 0.01f;
		//if (GetKeyboardRepeat(DIK_8))
		//	g_playerPlam.fInertia -= 0.01f;

		//// 加速力
		//if (GetKeyboardRepeat(DIK_7))
		//	g_playerPlam.fSpeedforce += 0.01f;
		//if (GetKeyboardRepeat(DIK_6))
		//	g_playerPlam.fSpeedforce -= 0.01f;

		//// 最高速
		//if (GetKeyboardRepeat(DIK_5))
		//	g_playerPlam.fMaxSpeed += 0.01f;
		//if (GetKeyboardRepeat(DIK_4))
		//	g_playerPlam.fMaxSpeed -= 0.01f;;

		PrintDebugProc("\nPlayerpos :[%f, %f, %f]\n", g_player.pos.x, g_player.pos.y, g_player.pos.z);
		// PrintDebugProc(DEBUG_LEFT, "move:[%f, %f, %f]\n", g_player.move.x, g_player.move.y, g_player.move.z);

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

	//**************************************************************
	// プレイヤー状態を初期化
	g_player.state = PLAYERSTATE_WAIT;
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
		g_player.state = PLAYERSTATE_RUN;
	}
	if (g_player.move.y != 0)
	{// 垂直移動
		g_player.pos.y += g_player.move.y;
		if (0 < g_player.move.y)
		{
			g_player.state = PLAYERSTATE_JUMP;
		}
		else
		{
			g_player.state = PLAYERSTATE_JUMP;
		}
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
	g_player.move.y -= GRAVITY * g_player.fWeight * 0.01f;

	//**************************************************************
	// 強制スクロール
	if (g_player.move.x <= g_nFocScroll / g_player.fWeight)
		g_player.move.x += MOVE_FORCE;
}

//==============================================================
// アイテムを投下
void ItemDrop(void)
{
	if (GetKeyboardTrigger(PLAYER_KEY_SHOT) || GetJoypadTrigger(PLAYER_PAD_SHOT))
	{

	}
}

//==============================================================
// 当たり判定
void Collision(void)
{
	//**************************************************************
	// 変数宣言

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