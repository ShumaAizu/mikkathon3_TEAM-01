//=============================================================================
//
//	壁の処理 [wall.cpp]
//	Author : SHUMA AIZU
// 
//=============================================================================

#include "main.h"
#include "wall.h"
#include "input.h"

#include "debugproc.h"
#include "player.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define SPLIT_WIDHT		(5 + 1)									// 横の分割数
#define SPLIT_HEIGTH	(1 + 1)									// 縦の分割数
#define WALL_POS		(D3DXVECTOR3(-5000.0f, 500.0f, 500.0f))	// 壁の位置

//*****************************************************************************
// グローバル変数
//*****************************************************************************
LPDIRECT3DTEXTURE9 g_pTextureWall = NULL;		// テクスチャへのポインタ
Wall g_awall[MAX_WALL];							// 壁の情報

//=============================================================================
//	メッシュウォールの初期化処理
//=============================================================================
void InitWall(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	// テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice,
		"data\\TEXTURE\\buildings000.png",
		&g_pTextureWall);

	for (int nCntWall = 0; nCntWall < MAX_WALL; nCntWall++)
	{
		g_awall[nCntWall].pVtxBuff = NULL;
		g_awall[nCntWall].pIdxBuff = NULL;
		g_awall[nCntWall].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_awall[nCntWall].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_awall[nCntWall].bUse = false;
	}

	SetWall(WALL_POS, INIT_D3DXVEC3, 2500.0f, 500.0f, SPLIT_WIDHT, SPLIT_HEIGTH);

}

//=============================================================================
//	メッシュウォールの終了処理
//=============================================================================
void UninitWall(void)
{
	// テクスチャの破棄
	if (g_pTextureWall != NULL)
	{
		g_pTextureWall->Release();
		g_pTextureWall = NULL;
	}
	
	for (int nCntWall = 0; nCntWall < MAX_WALL; nCntWall++)
	{
		// 頂点バッファの破棄
		if (g_awall[nCntWall].pVtxBuff != NULL)
		{
			g_awall[nCntWall].pVtxBuff->Release();
			g_awall[nCntWall].pVtxBuff = NULL;
		}

		// インデックスバッファの破棄
		if (g_awall[nCntWall].pIdxBuff != NULL)
		{
			g_awall[nCntWall].pIdxBuff->Release();
			g_awall[nCntWall].pIdxBuff = NULL;
		}
	}
}

//=============================================================================
//	メッシュウォールの描画処理
//=============================================================================
void DrawWall(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetDevice();	// デバイスの取得
	D3DXMATRIX mtxRot, mtxTrans;				// 計算用マトリックス

	// アルファテストを有効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);		// アルファテストを有効にする
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);	// 比較方法(基準値より大きければ描画)
	pDevice->SetRenderState(D3DRS_ALPHAREF, 150);				// アルファテストの参照値を設定(～以上で描画, intで設定)

	for (int nCntWall = 0; nCntWall < MAX_WALL; nCntWall++)
	{
		if (g_awall[nCntWall].bUse == false)
		{
			continue;
		}

		// ワールドマトリックスの初期化
		D3DXMatrixIdentity(&g_awall[nCntWall].mtxWorld);

		// 向きを反映
		D3DXMatrixRotationYawPitchRoll(&mtxRot, g_awall[nCntWall].rot.y, g_awall[nCntWall].rot.x, g_awall[nCntWall].rot.z);
		D3DXMatrixMultiply(&g_awall[nCntWall].mtxWorld, &g_awall[nCntWall].mtxWorld, &mtxRot);

		// 位置を反映
		D3DXMatrixTranslation(&mtxTrans, g_awall[nCntWall].pos.x, g_awall[nCntWall].pos.y, g_awall[nCntWall].pos.z);
		D3DXMatrixMultiply(&g_awall[nCntWall].mtxWorld, &g_awall[nCntWall].mtxWorld, &mtxTrans);

		// ワールドマトリックスの設定
		pDevice->SetTransform(D3DTS_WORLD, &g_awall[nCntWall].mtxWorld);

		// 頂点バッファをデータストリームに設定
		pDevice->SetStreamSource(0, g_awall[nCntWall].pVtxBuff, 0, sizeof(VERTEX_3D));

		// インデックスバッファをデータストリームに設定
		pDevice->SetIndices(g_awall[nCntWall].pIdxBuff);

		// 頂点フォーマットの設定
		pDevice->SetFVF(FVF_VERTEX_3D);

		// テクスチャの設定
		pDevice->SetTexture(0, g_pTextureWall);

		// メッシュウォールの描画
		pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP,
			0,
			0,
			g_awall[nCntWall].nSplitWidth * g_awall[nCntWall].nSplitHeight,
			0,
			((g_awall[nCntWall].nSplitWidth - 1) * (g_awall[nCntWall].nSplitHeight - 1) * 2) + ((g_awall[nCntWall].nSplitHeight - 2) * 4));
	}

	// アルファテストを無効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);		// アルファテストを無効化
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);	// 比較方法(すべて描画)
	pDevice->SetRenderState(D3DRS_ALPHAREF, 255);				// 基準値を設定(すべてを描画している)
}

//=============================================================================
//	メッシュウォールの更新処理
//=============================================================================
void UpdateWall(void)
{

}

//=============================================================================
//	メッシュウォールの設定処理
//=============================================================================
void SetWall(D3DXVECTOR3 pos, D3DXVECTOR3 rot, float fWidth, float fHeight, int nSplitWidth, int nSplitHeight)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = GetDevice();

	for (int nCntWall = 0; nCntWall < MAX_WALL; nCntWall++)
	{
		if (g_awall[nCntWall].bUse == true)
		{
			continue;
		}

		// 分割数を頂点数に合わせる
		nSplitWidth++;
		nSplitHeight++;

		// メッシュウォールの設定
		g_awall[nCntWall].pos = pos;
		g_awall[nCntWall].rot = rot;
		g_awall[nCntWall].nSplitWidth = nSplitWidth;
		g_awall[nCntWall].nSplitHeight = nSplitHeight;

		// 頂点バッファの生成
		pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * nSplitWidth * nSplitHeight,
			D3DUSAGE_WRITEONLY,
			FVF_VERTEX_3D,
			D3DPOOL_MANAGED,
			&g_awall[nCntWall].pVtxBuff,
			NULL);

		// 初期化
		VERTEX_3D* pVtx;			// 頂点情報へのポインタ

		// 頂点バッファをロックし,頂点情報へのポインタを取得
		g_awall[nCntWall].pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

		for (int nCntVtxHeight = 0; nCntVtxHeight < nSplitHeight; nCntVtxHeight++)
		{
			for (int nCntVtxWidht = 0; nCntVtxWidht < nSplitWidth; nCntVtxWidht++)
			{
				pVtx[nCntVtxWidht].pos = D3DXVECTOR3(fWidth * nCntVtxWidht, -fHeight * nCntVtxHeight, 0.0f);
				pVtx[nCntVtxWidht].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
				pVtx[nCntVtxWidht].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
				pVtx[nCntVtxWidht].tex = D3DXVECTOR2(1.0f * nCntVtxWidht, 1.0f * nCntVtxHeight);
			}

			pVtx += nSplitWidth;
		}

		// 頂点バッファをアンロックする
		g_awall[nCntWall].pVtxBuff->Unlock();

		// インデックスバッファの設定
		pDevice->CreateIndexBuffer(sizeof(WORD) * (nSplitWidth * 2 * (nSplitHeight - 1) + ((nSplitHeight - 2) * 2)),
			D3DUSAGE_WRITEONLY,
			D3DFMT_INDEX16,
			D3DPOOL_MANAGED,
			&g_awall[nCntWall].pIdxBuff,
			NULL);

		WORD* pIdx;		// インデックス情報へのポインタ

		// インデックスバッファをロックし、頂点情報へのポインタを取得
		g_awall[nCntWall].pIdxBuff->Lock(0, 0, (void**)&pIdx, 0);

		for (int nCntVtxHeight = 0; nCntVtxHeight < nSplitHeight - 1; nCntVtxHeight++)
		{
			for (int nCntVtxWidht = 0; nCntVtxWidht < nSplitWidth; nCntVtxWidht++)
			{
				if (nCntVtxHeight != 0 && nCntVtxWidht == 0)
				{// 奥行きが最初の行以外かつ幅が最初の行なら
					pIdx[0] = (nSplitWidth * (nCntVtxHeight + 1)) + nCntVtxWidht;
					pIdx++;
				}

				pIdx[0] = (nSplitWidth * (nCntVtxHeight + 1)) + nCntVtxWidht;
				pIdx[1] = nSplitWidth * nCntVtxHeight + nCntVtxWidht;

				pIdx += 2;

				if (nCntVtxWidht == nSplitWidth - 1 && nCntVtxHeight != nSplitHeight - 2)
				{// 幅が最後の行だったら
					pIdx[0] = nSplitWidth * nCntVtxHeight + nCntVtxWidht;
					pIdx++;
				}
			}
		}

		// インデックスバッファをアンロックする
		g_awall[nCntWall].pIdxBuff->Unlock();

		g_awall[nCntWall].bUse = true;
		break;
	}
}

//=============================================================================
//	メッシュウォールの当たり判定処理
//=============================================================================
void CollisionWall(D3DXVECTOR3* pPos, D3DXVECTOR3* pPosOld, D3DXVECTOR3* pMove)
{
	Wall* pWall = &g_awall[0];							// メッシュウォールの先頭アドレス

	D3DXMATRIX mtxRot, mtxTrans;									// 計算用マトリックス
	D3DXVECTOR3 pos0, pos1;											// 始点と終点
	D3DXVECTOR3 vecLine, vecToPos, vecToPosOld, vecMove, vecNor;	// 各種ベクトル
	D3DXVECTOR3 Intersection = {};									// 交点
	float fRate;													// 比率
	float fAngleVecLine;											// 境界線ベクトルの角度
	float fLenght = 0.0f;											// 壁ずりの長さ
	float fPosOldCross = 0.0f;

	for (int nCntWall = 0; nCntWall < MAX_WALL; nCntWall++, pWall++)
	{
		if (pWall->bUse == false)
		{// 使用していなければ戻る
			continue;
		}

		// ワールドマトリックスの初期化
		D3DXMatrixIdentity(&pWall->mtxWorld);

		// 向きを反映
		D3DXMatrixRotationYawPitchRoll(&mtxRot, pWall->rot.y, pWall->rot.x, pWall->rot.z);
		D3DXMatrixMultiply(&pWall->mtxWorld, &pWall->mtxWorld, &mtxRot);

		// 位置を反映
		D3DXMatrixTranslation(&mtxTrans, pWall->pos.x, pWall->pos.y, pWall->pos.z);
		D3DXMatrixMultiply(&pWall->mtxWorld, &pWall->mtxWorld, &mtxTrans);

		// 初期化
		VERTEX_3D* pVtx;			// 頂点情報へのポインタ

		// 頂点バッファをロックし,頂点情報へのポインタを取得
		pWall->pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

		// 位置と向きを反映した頂点座標を入れる
		D3DXVec3TransformCoord(&pos0, &pVtx[0].pos, &pWall->mtxWorld);
		D3DXVec3TransformCoord(&pos1, &pVtx[pWall->nSplitWidth - 1].pos, &pWall->mtxWorld);

		vecLine = pos1 - pos0;			// 境界線ベクトル

		vecToPos = *pPos - pos0;		// 

		vecToPosOld = *pPosOld - pos0;	// 

		vecMove = *pPos - *pPosOld;		// 移動ベクトル

		fAngleVecLine = atan2f(vecLine.x, vecLine.z);	// 境界線ベクトルの角度

		// 法線ベクトルを算出
		vecNor = D3DXVECTOR3(sinf((0.5f * D3DX_PI) + fAngleVecLine), 0.0f, cosf((0.5f * D3DX_PI) + fAngleVecLine));

		// 比率を算出
		fRate = ((vecToPos.z * vecMove.x) - (vecToPos.x * vecMove.z)) / ((vecLine.z * vecMove.x) - (vecLine.x * vecMove.z));

		// 壁から過去の位置へのベクトル
		fPosOldCross = (vecLine.z * vecToPosOld.x) - (vecLine.x * vecToPosOld.z);

		if (fPosOldCross < -0.0000001f && fPosOldCross > -0.1f)
		{// 小数点以下の誤差は切り捨て
			fPosOldCross = ceilf(fPosOldCross);		// 負の数は切り上げ？
		}

		if ((vecLine.z * vecToPos.x) - (vecLine.x * vecToPos.z) < 0.0f && 
			fPosOldCross >= 0.0f &&
			fRate <= 1.0f && fRate >= 0.0f)
		{
			Intersection = D3DXVECTOR3(vecLine.x * fRate, vecLine.y * fRate, vecLine.z * fRate);		// 交点

			D3DXVec3Normalize(&vecLine, &vecLine);		// 正規化

			// 交点を代入
			pPos->x = pos0.x + Intersection.x;
			pPos->z = pos0.z + Intersection.z;

			fLenght = (vecNor.z * vecMove.x) - (vecNor.x * vecMove.z);		// 壁ずりの長さ算出

			fLenght = fabsf(fLenght);		// 絶対値化
			
			if ((vecNor.z * vecMove.x) - (vecNor.x * vecMove.z) < 0)
			{// 外側なら加算
				pPos->x += vecLine.x * fLenght;
				pPos->z += vecLine.z * fLenght;
			}
			else if ((vecNor.z * vecMove.x) - (vecNor.x * vecMove.z) > 0)
			{// 内側なら減算
				pPos->x += -vecLine.x * fLenght;
				pPos->z += -vecLine.z * fLenght;
			}
		}

		// 頂点バッファをアンロックする
		g_awall[nCntWall].pVtxBuff->Unlock();
	}
}

//=============================================================================
//	メッシュウォールの内積テスト処理
//=============================================================================
bool CollisionDot(D3DXVECTOR3* pPos, D3DXVECTOR3* pPosOld, D3DXVECTOR3* pMove, D3DXVECTOR3* pRot)
{
	D3DXMATRIX mtxRot, mtxTrans;				// 計算用マトリックス
	D3DXVECTOR3 pos0, pos1;
	D3DXVECTOR3 vecLine, vecToPos, vecToPosOld, vecMove;	// 各種ベクトル
	D3DXVECTOR3 Intersection = {};							// 交点
	float fRate;											// 比率

	Player* pPlayer = GetPlayer();

	for (int nCntWall = 0; nCntWall < MAX_WALL; nCntWall++)
	{
		if (g_awall[nCntWall].bUse == false)
		{
			continue;
		}

		// ワールドマトリックスの初期化
		D3DXMatrixIdentity(&g_awall[nCntWall].mtxWorld);

		// 向きを反映
		D3DXMatrixRotationYawPitchRoll(&mtxRot, g_awall[nCntWall].rot.y, g_awall[nCntWall].rot.x, g_awall[nCntWall].rot.z);
		D3DXMatrixMultiply(&g_awall[nCntWall].mtxWorld, &g_awall[nCntWall].mtxWorld, &mtxRot);

		// 位置を反映
		D3DXMatrixTranslation(&mtxTrans, g_awall[nCntWall].pos.x, g_awall[nCntWall].pos.y, g_awall[nCntWall].pos.z);
		D3DXMatrixMultiply(&g_awall[nCntWall].mtxWorld, &g_awall[nCntWall].mtxWorld, &mtxTrans);

		// 初期化
		VERTEX_3D* pVtx;			// 頂点情報へのポインタ

		// 頂点バッファをロックし,頂点情報へのポインタを取得
		g_awall[nCntWall].pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

		// 位置と向きを反映した頂点座標を入れる
		D3DXVec3TransformCoord(&pos0, &pVtx[0].pos, &g_awall[nCntWall].mtxWorld);
		D3DXVec3TransformCoord(&pos1, &pVtx[g_awall[nCntWall].nSplitWidth - 1].pos, &g_awall[nCntWall].mtxWorld);

		vecLine = pos1 - pos0;		// 境界線ベクトル

		D3DXVECTOR3 vecNor = { 0.0f, 0.0f, 0.0f };

		vecNor = vecLine;

		float fAngle = atan2f(vecNor.x, vecNor.z);		// 境界線ベクトルの角度

		// 法線ベクトル
		vecNor.x = sinf((0.5f * D3DX_PI) + fAngle);
		vecNor.z = cosf((0.5f * D3DX_PI) + fAngle);

		D3DXVec3Normalize(&vecNor, &vecNor);			// 正規化

		vecToPos = *pPos - pos0;	// 

		vecMove = *pPos - *pPosOld;	// 移動ベクトル

		D3DXVec3Normalize(&vecMove, &vecMove);

		// 比率を算出
		fRate = ((vecToPos.z * vecMove.x) - (vecToPos.x * vecMove.z)) / ((vecLine.z * vecMove.x) - (vecLine.x * vecMove.z));

		if ((vecLine.z * vecToPos.x) - (vecLine.x * vecToPos.z) < 0 &&
			//(vecLine.z * vecToPosOld.x) - (vecLine.x * vecToPosOld.z) >= 0 &&
			fRate <= 1.0f && fRate >= 0.0f)
		{
			D3DXVECTOR3 VecMoveRef = vecMove + vecNor * 2;		// 移動ベクトルと法線ベクトルから反射ベクトルを求める (B = A - C)

			Intersection = D3DXVECTOR3(vecLine.x * fRate, vecLine.y * fRate, vecLine.z * fRate);	// 交点

			pRot->y = atan2f(VecMoveRef.x, VecMoveRef.z);		// 弾の角度を修正

			// 反射ベクトルの方向に余ったエネルギーをかける
			pPos->x += (pPos->x - (g_awall[nCntWall].pos.x + Intersection.x)) * VecMoveRef.x;
			pPos->z += (pPos->z - (g_awall[nCntWall].pos.z + Intersection.z)) * VecMoveRef.z;

			float fDot = (-vecMove.x * vecNor.x) + (-vecMove.z * vecNor.z);		// 内積を求める

			// 位置を反映する
			pPos->x = pPos->x + ((vecNor.x * fDot) * 2);
			pPos->z = pPos->z + ((vecNor.z * fDot) * 2);

			return true;
		}

		// 頂点バッファをアンロックする
		g_awall[nCntWall].pVtxBuff->Unlock();
	}

	return false;
}