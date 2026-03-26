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
#include "fade.h"
#include "fallitem.h"
#include "item.h"
#include "shadow.h"
#include "sound.h"
#include "trap.h"

#include "cutin.h"
#include "fallpoint.h"
#include "game.h"
#include "tutorialboard.h"

//**************************************************************
// 構造体の定義

//**************************************************************
// パラメータ情報構造体
typedef struct
{
	float fSpeedforce,	// 加速力
		fJumpforce,		// 上昇力
		fMaxMoveSpeed,	// 最大水平移動速度
		fMaxRiseSpeed,	// 最大上昇速度
		fInertia,		// 慣性割合
		fGravity,		// 重力
		fFacMove,		// 重さによる左右移動への影響
		fFacGravity;	// 重さによる重力への影響係数

	int nFocScroll;		// スクロールの係数
}Parameter;


//**************************************************************
// グローバル変数
Player		g_player;						// プレイヤーの情報
Parameter	g_parameter;					// 各種値

bool g_bInvincible = false;					// 無敵モード
int	 g_nDamageCounter = 0;
int  g_nFireCounter = 0;
static bool g_isMoveUP = false, g_isMoveLeft = false, g_isMoveRight = false;

//**************************************************************
// プロトタイプ宣言
void PlayerState(void);			// プレイヤー状態管理
void PlayerContoroll(void);		// プレイヤー操作（共通
void Keyboard(void);			// キーボード
void Joypad(void);				// コントローラー
void PlayerMove(void);			// 移動
void ItemDrop(int nType = -1);	// アイテムを投下
void Collision(void);			// 当たり判定
void ParameterLoad(void);		// プレイヤーパラメータ読み込み
void ParameterSave(void);		// プレイヤーパラメータ保存
void DebugSetValue(float* fVal, float fAdd, int nKey, int nSubKey);
Parameter* GetParameter(void);	// パラメータ情報取得

//=========================================================================================
// モデル初期化処理
//=========================================================================================
void InitPlayer(void)
{
	g_nDamageCounter = 0;
	g_nFireCounter = 0;

	//**************************************************************
	// 変数宣言
	LPDIRECT3DDEVICE9 pDevice = GetDevice();		// デバイスへのポインタ

	//**************************************************************
	// 各値の初期化
	// プレイヤー情報
	g_player.pos = D3DXVECTOR3(-WORLD_END, 150.0f, 0.0f);
	g_player.posOld = g_player.pos;
	g_player.rot = vec3_ZORO;
	g_player.move = vec3_ZORO;
	g_player.spin = vec3_ZORO;
	g_player.fWeight = 10.0f;						// 重さ
	g_player.state = PLAYERSTATE_NONE;				// 状態
	g_player.bUse = false;
	g_player.pModel = SetModelData(MODELTYPE_BALLOON);	// モデル呼び出し
	g_player.nLife = PLAYER_LIFE;
	g_player.fFuel = PLAYER_FUEL;
	g_player.nShadow = SetShadow(RADIUS_BASKET);

	g_isMoveUP = false;
	g_isMoveLeft = false;
	g_isMoveRight = false;

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
	ParameterLoad();

	// プレイヤーの状態→今後、スタートのカウントダウンで設定
	SetPlayerState(PLAYERSTATE_RUN);

	g_bInvincible = true;
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
	if (GetTutorialLevel() == TUTORIALLEVEL_MAX && g_bInvincible == true)
	{
		g_bInvincible = false;
	}

	if (g_isMoveUP == true && g_isMoveLeft == true && g_isMoveRight == true && 
		GetTutorialLevel() == TUTORIALLEVEL_001 && IsNextTutorialLevel() == false)
	{
		NextTutorialLevel(300);
	}

	if (g_player.bUse)
	{
		//**************************************************************
		// プレイヤーの過去の位置を保存
		g_player.posOld = g_player.pos;

		if (IsEnableCameraEdit() == false)
		{
			//**************************************************************
			// 状態管理
			PlayerState();

			if (g_player.state != PLAYERSTATE_DEAD && g_player.state != PLAYERSTATE_NONE)
			{
				//**************************************************************
				// 操作
				PlayerContoroll();

				//**************************************************************
				// 移動
				PlayerMove();

				//**************************************************************
				// アイテム投下
				ItemDrop();
			}
		}

		//**************************************************************
		// 判定
		Collision();

		if (0 < g_nFireCounter)
			g_nFireCounter--;
		else
			g_nFireCounter = 0;

		if (0 < g_nDamageCounter)
			g_nDamageCounter--;
		else
			g_nDamageCounter = 0;
		PrintDebugProc("DMG CNT %d", g_nDamageCounter);

#if _DEBUG
		DebugSetValue(&g_player.fWeight, 0.1f, DIK_UP, DIK_DOWN);
		PrintDebugProc("\nプレイヤーの重さ [↑/↓]: %f", g_player.fWeight);

		//DebugSetValue(&g_parameter.fGravity, 0.0001f, DIK_O, DIK_L);
		//PrintDebugProc("\n重力             [ O/ L]: %f", g_parameter.fGravity);
		DebugSetValue(&g_parameter.fMaxRiseSpeed, 0.0001f, DIK_O, DIK_L);
		PrintDebugProc("\n最大上昇速度     [ O/ L]: %f", g_parameter.fMaxRiseSpeed);

		DebugSetValue(&g_parameter.fMaxMoveSpeed, 0.0001f, DIK_I, DIK_K);
		PrintDebugProc("\n最大移動速度     [ I/ K]: %f", g_parameter.fMaxMoveSpeed);

		DebugSetValue(&g_parameter.fSpeedforce, 0.01f, DIK_U, DIK_J);
		PrintDebugProc("\n加速力           [ U/ J]: %f", g_parameter.fSpeedforce);

		DebugSetValue(&g_parameter.fFacMove, 0.01f, DIK_Y, DIK_H);
		PrintDebugProc("\n左右速度係数     [ Y/ H]: %f", g_parameter.fFacMove);

		if (GetKeyboardRepeat(DIK_T))
		{
			g_parameter.nFocScroll++;
		}
		if (GetKeyboardRepeat(DIK_G))
		{
			g_parameter.nFocScroll--;
			if (g_parameter.nFocScroll <= 0)
				g_parameter.nFocScroll = 1;
		}
		PrintDebugProc("\nスクロール係数   [ T/ G]: %d", g_parameter.nFocScroll);

		DebugSetValue(&g_parameter.fJumpforce, 0.001f, DIK_R, DIK_F);
		PrintDebugProc("\n上昇力           [ R/ F]: %f", g_parameter.fJumpforce);

		PrintDebugProc("\nPlayerpos :[%f, %f, %f]\n", g_player.pos.x, g_player.pos.y, g_player.pos.z);

		if (GetKeyboardPress(DIK_LCONTROL) && GetKeyboardTrigger(DIK_S))
			ParameterSave();
#endif
	}
}

//==============================================================
// プレイヤーの状態管理
void PlayerState(void)
{
	//**************************************************************
	// 無敵モード中
	if (g_bInvincible)
	{
		g_player.nLife = PLAYER_LIFE;
		g_player.fFuel = PLAYER_FUEL;
	}

	//**************************************************************
	// 死んでいたら
	if (g_player.nLife <= 0)
	{
		g_player.state = PLAYERSTATE_DEAD;
	}

	//**************************************************************
	// 死んでいたら
	if (g_player.state == PLAYERSTATE_DEAD)
	{
		// フラグを設定
		if (GetNextGameFlag() != GAMEFLAG_GAMEOVER)
			SetGameFlag(GAMEFLAG_GAMEOVER, 60);

		g_player.pos.y -= (g_player.pos.y - 10.0f) * 0.1f;
		g_player.rot.z += ((-D3DX_PI * 0.5f) - g_player.rot.z) * 0.1f;
	}

	//**************************************************************
	// 無敵モード
	if (g_bInvincible)
		PrintDebugProc("\n無敵中！");
	else
		PrintDebugProc("\n");

	if (GetKeyboardTrigger(DIK_F2))
		g_bInvincible = g_bInvincible ^ 1;

	PrintDebugProc("  F2で切り替え]\n");
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
		{// 上昇
			if (g_player.pos.y <= HEIGHT_EASE)
				// 地上付近では上昇力をブースト
				g_player.move.y += g_parameter.fJumpforce / (g_player.fWeight - EASE_WEIGHT);
			else if (g_player.pos.y <= HEIGHT_RES)
				g_player.move.y += g_parameter.fJumpforce / g_player.fWeight;
			else
				// 上空では上昇力を減衰
				g_player.move.y += g_parameter.fJumpforce / (g_player.fWeight + PENALTY_WEIGHT);

			if (g_nFireCounter <= 0)
			{
				g_nFireCounter = 60;
				PlaySound(SOUND_LABEL_005);
			}
			g_isMoveUP = true;
		}

		if (GetKeyboardPress(PLAYER_KEY_MOVE_L))
		{// 奥
			g_player.move.z += g_parameter.fSpeedforce;
			g_isMoveLeft = true;
		}

		if (GetKeyboardPress(PLAYER_KEY_MOVE_R))
		{// 手前
			g_player.move.z -= g_parameter.fSpeedforce;
			g_isMoveRight = true;
		}
	} while (0);
}

//==============================================================
// パッド操作
void Joypad(void)
{
	//**************************************************************
	// 変数宣言
	D3DXVECTOR3 leftStick = vec3_ZORO;
	GetJoypadStickLeft(&leftStick.x, &leftStick.y);
	// PrintDebugProc("\nLeftStick\nx: %f\ny: %f", leftStick.x, leftStick.y);

	//**************************************************************
	// 移動
	if (leftStick.x != 0)
	{
		g_player.move.z -= leftStick.x * g_parameter.fSpeedforce;
		g_isMoveLeft = true;
		g_isMoveRight = true;
	}

	if (0 < leftStick.y)
	{
		if (g_player.pos.y <= HEIGHT_EASE)
			// 地上付近では上昇力をブースト
			g_player.move.y += leftStick.y * g_parameter.fJumpforce / (g_player.fWeight - EASE_WEIGHT);
		else if (g_player.pos.y <= HEIGHT_RES)
			g_player.move.y += leftStick.y * g_parameter.fJumpforce / g_player.fWeight;
		else
			g_player.move.y += leftStick.y * g_parameter.fJumpforce / (g_player.fWeight + PENALTY_WEIGHT);

		if (g_nFireCounter <= 0)
		{
			g_nFireCounter = 60;
			PlaySound(SOUND_LABEL_005);
		}
		g_isMoveUP = true;
	}

	do
	{
		if (GetJoypadPress(PLAYER_PAD_MOVE_UP))
		{// 上昇
			if (g_player.pos.y <= HEIGHT_EASE)
				// 地上付近では上昇力をブースト
				g_player.move.y += g_parameter.fJumpforce / (g_player.fWeight - EASE_WEIGHT);
			else if (g_player.pos.y <= HEIGHT_RES)
				g_player.move.y += g_parameter.fJumpforce / g_player.fWeight;
			else
				// 上空では上昇力を減衰
				g_player.move.y += g_parameter.fJumpforce / (g_player.fWeight + PENALTY_WEIGHT);
			
			if (g_nFireCounter <= 0)
			{
				g_nFireCounter = 60;
				PlaySound(SOUND_LABEL_005);
			}
			g_isMoveUP = true;
		}

		if (GetJoypadPress(PLAYER_PAD_MOVE_L))
		{// 奥
			g_player.move.z += g_parameter.fSpeedforce;
			g_isMoveLeft = true;
		}

		if (GetJoypadPress(PLAYER_PAD_MOVE_R))
		{// 手前
			g_player.move.z -= g_parameter.fSpeedforce;
			g_isMoveRight = true;
		}
	} while (0);

}

//==============================================================
// プレイヤーを移動
void PlayerMove(void)
{
	//**************************************************************
	// 変数宣言	
	float fRotMove = atan2f(-g_player.move.x, -g_player.move.z) - g_player.rot.y;	// 変化させる角度(目的値と現在値の差)

	//**************************************************************
	// 速度の制限
	if (g_parameter.fMaxRiseSpeed <= g_player.move.y)
		g_player.move.y = g_parameter.fMaxRiseSpeed;

	//**************************************************************
	// 移動・回転
	if (g_player.move.x != 0 || g_player.move.z != 0)
	{// 水平移動
		g_player.pos.x += g_player.move.x / g_player.fWeight * g_parameter.fFacMove;
		g_player.pos.z += g_player.move.z / g_player.fWeight * g_parameter.fFacMove;
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
	// 異常な角度(X_PIを超える)値を修正
	if (g_player.rot.y < -D3DX_PI)
		g_player.rot.y = D3DX_PI;
	else if (g_player.rot.y > D3DX_PI)
		g_player.rot.y = -D3DX_PI;

	//**************************************************************
	// 影の追従
	SetPotisionShadow(g_player.nShadow, g_player.pos, 1.0f);
	
	//**************************************************************
	// 慣性
	g_player.move += D3DXVECTOR3(-g_player.move.x * g_parameter.fInertia, 0.0f, -g_player.move.z * g_parameter.fInertia);
	g_player.spin += D3DXVECTOR3(0.0f, -g_player.spin.y * ROTMOVE_FACTOR, 0.0f);

	//**************************************************************
	// 重力
	if (g_player.pos.y <= HEIGHT_RES)
		g_player.move.y -= g_parameter.fGravity * g_player.fWeight;
	else
		g_player.move.y -= g_parameter.fGravity * (g_player.fWeight + PENALTY_WEIGHT);

	//**************************************************************
	// 強制スクロール
	if (g_player.move.x <= g_parameter.nFocScroll / g_player.fWeight)
		g_player.move.x += g_parameter.fFacMove;
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
	if (GetKeyboardTrigger(PLAYER_KEY_SHOT)
		|| GetKeyboardTrigger(DIK_SPACE)
		|| GetKeyboardTrigger(DIK_RETURN)
		|| GetJoypadTrigger(PLAYER_PAD_SHOT)
		|| GetJoypadTrigger(PLAYER_PAD_SHOT_SUB)
		|| GetJoypadStickPressL(JOYSTICK_DOWN))
	{
		if (pItem && 0 <= *pItem)
			// アイテム投下
			SetFallItem(g_player.pos, g_player.rot, (ITEMTYPE)*pItem);

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
					g_player.fWeight += 0.1f;
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
		g_player.nLife--;
		g_nDamageCounter = 120;
	}
	
	// 地面
	if (g_player.pos.y <= HEIGHT_GROUND)
	{
		g_player.pos.y = HEIGHT_GROUND;		// 接地
		g_player.move.x = 0.0f;				// 移動不可
		g_player.move.z = 0.0f;

		if (g_player.move.y < 0.0f)			// それ以下には移動しないように
			g_player.move.y = 0.0f;
	}

	// 大人の壁
	if (g_player.pos.z <= DEPTH_FRONT)
	{
		g_player.pos.z = DEPTH_FRONT;
		if (g_player.move.z <= 0.0f)
			g_player.move.z = 0.0f;
	}
	if (DEPTH_END <= g_player.pos.z)
	{
		g_player.pos.z = DEPTH_END;
		if (0.0f <= g_player.move.z)
			g_player.move.z = 0.0f;
	}

	// 天井
	if (HEIGHT_LIMIT <= g_player.pos.y)
	{
		g_player.pos.y = HEIGHT_LIMIT;
		if (0.0f <= g_player.move.y)
			g_player.move.y = 0.0f;
	}

	// ステージの端
	if (WORLD_END < g_player.pos.x)
	{
		if (GetSetCutIn())
		{
			g_player.pos.x = -WORLD_END;
			SetPosCamera(g_player.pos,CAMERATYPE_PLAYER);
			ResetTrap();
			ResetItem();
			UninitFallPoint();
			InitFallPoint();
			AddGameLevel();
		}
		else
		{
			SetCutIn();
		}
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
	D3DXMATERIAL*	pMatBlight;						// 点滅用マテリアルデータへのポインタ
	P_PLAYER		pPlayer = &g_player;

	if (g_player.bUse)
	{
		if (((0 <= g_nDamageCounter && (g_nDamageCounter / 10) % 2 == 0) && g_bInvincible)|| g_bInvincible == false)
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
			pMatBlight = pMat = (D3DXMATERIAL*)pPlayer->pModel->pBuffMat->GetBufferPointer();

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

//==============================================================
// パラメータ情報取得
Parameter* GetParameter(void)
{
	return &g_parameter;
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
void ParameterLoad(void)
{
	FILE* pFile;

	pFile = fopen(PLAYER_PARAMETERS, "rb");
	if (pFile != NULL)
	{// ファイルが開けたら
		fread(&g_parameter, sizeof(Parameter), 1, pFile);
		fclose(pFile);
	}
	else
	{
		g_parameter.fSpeedforce = MOVE_FORCE;
		g_parameter.fJumpforce = JUMP_FORCE;
		g_parameter.fMaxMoveSpeed = MAX_MOVE_SPEED;
		g_parameter.fMaxRiseSpeed = MAX_RISE_SPEED;
		g_parameter.fInertia = POSMOVE_FACTOR;
		g_parameter.fGravity = GRAVITY;
		g_parameter.fFacMove = MOVE_FAC;
		g_parameter.fFacGravity = GRAVITY_FAC;
		g_parameter.nFocScroll = SCROLL_FOC;
	}
}

//=========================================================================================
// プレイヤーパラメータ保存
void ParameterSave(void)
{
	FILE* pFile;

	pFile = fopen(PLAYER_PARAMETERS, "wb");
	if (pFile != NULL)
	{// ファイルが開けたら
		fwrite(&g_parameter, sizeof(Parameter), 1, pFile);
		fclose(pFile);
	}
}