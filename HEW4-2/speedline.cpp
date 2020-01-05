//=============================================================================
//
// スピードライン
// Author : 
//
//=============================================================================
#include "main.h"
#include "myDirect3D.h"
#include "debugproc.h"
#include "input.h"
#include "speedline.h"
#include "player.h"
#include "particle.h"
#include "input.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	TEXTURE_LINE		"asset/texture/effect000.jpg"	// 読み込むテクスチャファイル名
#define	LINE_SIZE_X			(50.0f)							// ビルボードの幅
#define	LINE_SIZE_Y			(50.0f)							// ビルボードの高さ
#define	VALUE_MOVE_LINE		(5.0f)							// 移動速度

#define	MAX_LINE			(15)							// ビルボード最大数
#define	MAX_LINE2			(2000)							// ビルボード最大数


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT MakeVertexLine(LPDIRECT3DDEVICE9 pDevice);
void SetVertexLine(int nIdxLine, float fSizeX, float fSizeY);
void SetColorLine(int nIdxLine, D3DXCOLOR col);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static LPDIRECT3DTEXTURE9		g_pD3DTextureLine = NULL;	// テクスチャへのポインタ
static LPDIRECT3DVERTEXBUFFER9 g_pD3DVtxBuffLine = NULL;	// 頂点バッファインターフェースへのポインタ

static D3DXMATRIX				g_mtxWorldLine;				// ワールドマトリックス
static D3DXMATRIX				g_mtxWorldLine2;			// ワールドマトリックス

static PARTICLE					g_aLine[MAX_LINE];			// パーティクルワーク
static PARTICLE					g_aLine2[MAX_LINE2];		// パーティクルワーク

static D3DXVECTOR3				g_posBase;						// ビルボード発生位置
static float					g_fWidthBase = 5.0f;			// 基準の幅
static float					g_fHeightBase = 10.0f;			// 基準の高さ
static float					g_roty = 0.0f;					// 移動方向
static float					g_spd = 0.0f;					// 移動スピード

int cnt=0;
int cnt2 = 1;
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT SpeedlineManager::InitSpeedline(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetD3DDevice();

	// 頂点情報の作成
	MakeVertexLine(pDevice);

	// テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice,						// デバイスへのポインタ
		TEXTURE_LINE,			// ファイルの名前
		&g_pD3DTextureLine);	// 読み込むメモリー

	for (int nCntLine = 0; nCntLine < MAX_LINE; nCntLine++)
	{
		g_aLine[nCntLine].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_aLine[nCntLine].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_aLine[nCntLine].scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
		g_aLine[nCntLine].move = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
		g_aLine[nCntLine].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		g_aLine[nCntLine].fSizeX = LINE_SIZE_X;
		g_aLine[nCntLine].fSizeY = LINE_SIZE_Y;
		g_aLine[nCntLine].nIdxShadow = -1;
		g_aLine[nCntLine].nLife = 0;
		g_aLine[nCntLine].bUse = false;
	}

	g_posBase = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void SpeedlineManager::UninitSpeedline(void)
{
	if (g_pD3DTextureLine != NULL)
	{// テクスチャの開放
		g_pD3DTextureLine->Release();
		g_pD3DTextureLine = NULL;
	}

	if (g_pD3DVtxBuffLine != NULL)
	{// 頂点バッファの開放
		g_pD3DVtxBuffLine->Release();
		g_pD3DVtxBuffLine = NULL;
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void SpeedlineManager::UpdateSpeedline(void)
{
	if (cnt2 < 60)
	{
		if (Keyboard_IsPress(DIK_A))cnt2++;
	}
	if (cnt2 > 1)
	{
		if (Keyboard_IsPress(DIK_D))cnt2--;
	}

	DebugProc_Print((char *)"カウント : %d\n",cnt2);


	DebugProc_Print((char *)"パーティクルx : %f\n", g_aLine[0].pos.x);
	DebugProc_Print((char *)"パーティクルy : %f\n", g_aLine[0].pos.y);
	DebugProc_Print((char *)"パーティクルz : %f\n", g_aLine[0].pos.z);
	DebugProc_Print((char *)"パーティクル2x : %f\n", g_aLine2[0].pos.x);
	DebugProc_Print((char *)"パーティクル2y : %f\n", g_aLine2[0].pos.y);
	DebugProc_Print((char *)"パーティクル2z : %f\n", g_aLine2[0].pos.z);


	if (cnt++ % cnt2 == 0)
	{
		// 更新処理
		for (int nCntLine = 0; nCntLine < MAX_LINE; nCntLine++)
		{
			// 使用中
			if (g_aLine[nCntLine].bUse)
			{
				// 移動
				g_aLine[nCntLine].pos.x += g_aLine[nCntLine].move.x;
				g_aLine[nCntLine].pos.z += g_aLine[nCntLine].move.z;
				// 重力などもできる
				g_aLine[nCntLine].pos.y += g_aLine[nCntLine].move.y;

				// 寿命が減る
				g_aLine[nCntLine].nLife--;

				// 寿命がなくなった時
				if (g_aLine[nCntLine].nLife <= 0)
				{
					// 表示されなくなる
					g_aLine[nCntLine].bUse = false;
				}
				else
				{
					// 色の設定
					SetColorLine(nCntLine, g_aLine[nCntLine].col);

					// 残像を生成
					SpeedlineManager::SetSpeedline(g_aLine[nCntLine].pos, D3DXVECTOR3(0.0f, 0.0f, 0.0f), g_aLine[nCntLine].col,
						g_aLine[nCntLine].fSizeX, g_aLine[nCntLine].fSizeY, g_aLine[nCntLine].nLife, 2);
				}
			}
		}
		for (int nCntLine2 = 0; nCntLine2 < MAX_LINE2; nCntLine2++)
		{
			// 使用中
			if (g_aLine2[nCntLine2].bUse)
			{
				// 寿命が減る
				g_aLine2[nCntLine2].nLife -= 4;

				// 寿命がなくなった時
				if (g_aLine2[nCntLine2].nLife <= 0)
				{
					// 表示されなくなる
					g_aLine2[nCntLine2].bUse = false;
				}
				else
				{
					// α値設定
					//g_aLine2[nCntLine2].col.a -= 0.01f;
					if (g_aLine2[nCntLine2].col.a < 0.0f)
					{
						g_aLine2[nCntLine2].col.a = 0.0f;
					}

					// 色の設定
					SetColorLine(MAX_LINE + nCntLine2, g_aLine2[nCntLine2].col);
				}
			}
		}

		// パーティクル発生
		if (rand() % 10 == 0)
		{
			D3DXVECTOR3 pos;
			D3DXVECTOR3 move;
			float fAngle;
			int nLife;
			float fSize;

			// ポジション
			pos.x = GetPlayer_Pos().x + 5.0f * cosf(GetPlayer_Rot().y)*(float)(rand() % 10 - 5.0f);
			pos.y = GetPlayer_Pos().y + 40.0f *(float)(rand() % 10) / 10;
			pos.z = GetPlayer_Pos().z + 20.0f * sinf(GetPlayer_Rot().y)*(float)(rand() % 10 - 5.0f);

			// 生成されてからどこに飛ぶか？
			move.x = -sinf(GetPlayer_Rot().y);
			move.y = 0.0f;
			move.z = -cosf(GetPlayer_Rot().y);

			// ランダムで命の設定
			nLife = 200;

			// パーティクルの大きさ
			fSize = (float)(rand() % 10 + 5.0f);

			// ビルボードの設定								　　 赤    緑    青
			SpeedlineManager::SetSpeedline(pos, move, D3DXCOLOR(1.0f, 0.0f, 0.0f, 0.5f), fSize, fSize, nLife, 1);
		}
	}
}


//=============================================================================
// 描画処理
//=============================================================================
void SpeedlineManager::DrawSpeedline(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetD3DDevice();
	D3DXMATRIX mtxView, mtxScale, mtxTranslate;

	// ライティングを無効に
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	// 加算合成に設定
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);			// 結果 = 転送元(SRC) + 転送先(DEST)
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);		// αソースカラーの指定
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);			// αデスティネーションカラーの指定
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);				// Zバッファーの書き込みをしない

	// ビューマトリックスを取得
	pDevice->GetTransform(D3DTS_VIEW, &mtxView);

	for (int nCntLine = 0; nCntLine < MAX_LINE; nCntLine++)
	{
		if (g_aLine[nCntLine].bUse)
		{
			// ワールドマトリックスの初期化
			D3DXMatrixIdentity(&g_mtxWorldLine);

			// 逆行列を求める
			D3DXMatrixInverse(&g_mtxWorldLine, NULL, &mtxView);

			// 平行移動の部分を求める
			g_mtxWorldLine._41 = 0.0f;
			g_mtxWorldLine._42 = 0.0f;
			g_mtxWorldLine._43 = 0.0f;

			// スケールを反映
			D3DXMatrixScaling(&mtxScale, g_aLine[nCntLine].scale.x, g_aLine[nCntLine].scale.y, g_aLine[nCntLine].scale.z);
			D3DXMatrixMultiply(&g_mtxWorldLine, &g_mtxWorldLine, &mtxScale);

			// 移動を反映
			D3DXMatrixTranslation(&mtxTranslate, g_aLine[nCntLine].pos.x, g_aLine[nCntLine].pos.y, g_aLine[nCntLine].pos.z);
			D3DXMatrixMultiply(&g_mtxWorldLine, &g_mtxWorldLine, &mtxTranslate);

			// ワールドマトリックスの設定
			pDevice->SetTransform(D3DTS_WORLD, &g_mtxWorldLine);

			// 頂点バッファをデバイスのデータストリームにバインド
			pDevice->SetStreamSource(0, g_pD3DVtxBuffLine, 0, sizeof(VERTEX_3D));

			// 頂点フォーマットの設定
			pDevice->SetFVF(FVF_VERTEX_3D);

			// テクスチャの設定
			pDevice->SetTexture(0, g_pD3DTextureLine);

			// ポリゴンの描画
			pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, (nCntLine * 4), NUM_POLYGON);
		}
	}
	for (int nCntLine2 = 0; nCntLine2 < MAX_LINE2; nCntLine2++)
	{
		if (g_aLine2[nCntLine2].bUse)
		{
			// ワールドマトリックスの初期化
			D3DXMatrixIdentity(&g_mtxWorldLine2);

			// 逆行列を求める
			D3DXMatrixInverse(&g_mtxWorldLine2, NULL, &mtxView);

			// 平行移動の部分を求める
			g_mtxWorldLine2._41 = 0.0f;
			g_mtxWorldLine2._42 = 0.0f;
			g_mtxWorldLine2._43 = 0.0f;

			// スケールを反映
			D3DXMatrixScaling(&mtxScale, g_aLine2[nCntLine2].scale.x, g_aLine2[nCntLine2].scale.y, g_aLine2[nCntLine2].scale.z);
			D3DXMatrixMultiply(&g_mtxWorldLine2, &g_mtxWorldLine2, &mtxScale);

			// 移動を反映
			D3DXMatrixTranslation(&mtxTranslate, g_aLine2[nCntLine2].pos.x, g_aLine2[nCntLine2].pos.y, g_aLine2[nCntLine2].pos.z);
			D3DXMatrixMultiply(&g_mtxWorldLine2, &g_mtxWorldLine2, &mtxTranslate);

			// ワールドマトリックスの設定
			pDevice->SetTransform(D3DTS_WORLD, &g_mtxWorldLine2);

			// 頂点バッファをデバイスのデータストリームにバインド
			pDevice->SetStreamSource(0, g_pD3DVtxBuffLine, 0, sizeof(VERTEX_3D));

			// 頂点フォーマットの設定
			pDevice->SetFVF(FVF_VERTEX_3D);

			// テクスチャの設定
			pDevice->SetTexture(0, g_pD3DTextureLine);

			// ポリゴンの描画
			pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, (nCntLine2 * 4), NUM_POLYGON);
		}
	}

	// ライティングを有効に
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

	// 通常ブレンドに戻す
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);			// 結果 = 転送元(SRC) + 転送先(DEST)
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);		// αソースカラーの指定
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);	// αデスティネーションカラーの指定
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);				// Zバッファーの書き込みをする
}

//=============================================================================
// 頂点情報の作成
//=============================================================================
HRESULT MakeVertexLine(LPDIRECT3DDEVICE9 pDevice)
{
	// オブジェクトの頂点バッファを生成
	if (FAILED(pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * NUM_VERTEX * (MAX_LINE + MAX_LINE2),	// 頂点データ用に確保するバッファサイズ(バイト単位)
		D3DUSAGE_WRITEONLY,							// 頂点バッファの使用法　
		FVF_VERTEX_3D,								// 使用する頂点フォーマット
		D3DPOOL_MANAGED,							// リソースのバッファを保持するメモリクラスを指定
		&g_pD3DVtxBuffLine,						// 頂点バッファインターフェースへのポインタ
		NULL)))										// NULLに設定
	{
		return E_FAIL;
	}

	{//頂点バッファの中身を埋める
		VERTEX_3D *pVtx;

		// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
		g_pD3DVtxBuffLine->Lock(0, 0, (void**)&pVtx, 0);

		for (int nCntLine = 0; nCntLine < MAX_LINE + MAX_LINE2; nCntLine++, pVtx += 4)
		{
			// 頂点座標の設定
			pVtx[0].pos = D3DXVECTOR3(-LINE_SIZE_X / 2, -LINE_SIZE_Y / 2, 0.0f);
			pVtx[1].pos = D3DXVECTOR3(LINE_SIZE_X / 2, -LINE_SIZE_Y / 2, 0.0f);
			pVtx[2].pos = D3DXVECTOR3(-LINE_SIZE_X / 2, LINE_SIZE_Y / 2, 0.0f);
			pVtx[3].pos = D3DXVECTOR3(LINE_SIZE_X / 2, LINE_SIZE_Y / 2, 0.0f);

			// 法線の設定
			pVtx[0].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
			pVtx[1].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
			pVtx[2].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
			pVtx[3].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

			// 反射光の設定
			pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

			// テクスチャ座標の設定
			pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
			pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
			pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
			pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);
		}

		// 頂点データをアンロックする
		g_pD3DVtxBuffLine->Unlock();
	}

	return S_OK;
}

//=============================================================================
// 頂点座標の設定
//=============================================================================
void SetVertexLine(int nIdxLine, float fSizeX, float fSizeY)
{
	{//頂点バッファの中身を埋める
		VERTEX_3D *pVtx;

		// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
		g_pD3DVtxBuffLine->Lock(0, 0, (void**)&pVtx, 0);

		pVtx += (nIdxLine * 4);

		// 頂点座標の設定
		pVtx[0].pos = D3DXVECTOR3(-fSizeX / 2, -fSizeY / 2, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(-fSizeX / 2, fSizeY / 2, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(fSizeX / 2, -fSizeY / 2, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(fSizeX / 2, fSizeY / 2, 0.0f);

		// 頂点データをアンロックする
		g_pD3DVtxBuffLine->Unlock();
	}
}

//=============================================================================
// 頂点カラーの設定
//=============================================================================
void SetColorLine(int nIdxLine, D3DXCOLOR col)
{
	{//頂点バッファの中身を埋める
		VERTEX_3D *pVtx;

		// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
		g_pD3DVtxBuffLine->Lock(0, 0, (void**)&pVtx, 0);

		pVtx += (nIdxLine * 4);

		// 頂点座標の設定
		pVtx[0].col =
			pVtx[1].col =
			pVtx[2].col =
			pVtx[3].col = col;

		// 頂点データをアンロックする
		g_pD3DVtxBuffLine->Unlock();
	}
}

//=============================================================================
// 頂点情報の作成
//=============================================================================
int SpeedlineManager::SetSpeedline(D3DXVECTOR3 pos, D3DXVECTOR3 move, D3DXCOLOR col, float fSizeX, float fSizeY, int nLife, int num)
{
	int nIdxLine = -1;

	if (num == 1)
	{
		for (int nCntLine = 0; nCntLine < MAX_LINE; nCntLine++)
		{
			if (!g_aLine[nCntLine].bUse)
			{
				g_aLine[nCntLine].pos = pos;
				g_aLine[nCntLine].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
				g_aLine[nCntLine].scale = D3DXVECTOR3(0.1f, 0.1f, 0.1f);
				g_aLine[nCntLine].move = move;
				g_aLine[nCntLine].col = col;
				g_aLine[nCntLine].fSizeX = fSizeX;
				g_aLine[nCntLine].fSizeY = fSizeY;
				g_aLine[nCntLine].nLife = nLife;
				g_aLine[nCntLine].bUse = true;

				// 頂点座標の設定
				SetVertexLine(nCntLine, fSizeX, fSizeY);

				nIdxLine = nCntLine;

				break;
			}	
		}
	}
	else
	{
		for (int nCntLine2 = 0; nCntLine2 < MAX_LINE2; nCntLine2++)
		{
			if (!g_aLine2[nCntLine2].bUse)
			{
				g_aLine2[nCntLine2].pos = pos;
				g_aLine2[nCntLine2].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
				g_aLine2[nCntLine2].scale = D3DXVECTOR3(0.1f, 0.1f, 0.1f);
				g_aLine2[nCntLine2].move = move;
				g_aLine2[nCntLine2].col = col;
				g_aLine2[nCntLine2].fSizeX = fSizeX;
				g_aLine2[nCntLine2].fSizeY = fSizeY;
				g_aLine2[nCntLine2].nLife = nLife;
				g_aLine2[nCntLine2].bUse = true;

				// 頂点座標の設定
				SetVertexLine(MAX_LINE + nCntLine2, fSizeX, fSizeY);

				nIdxLine = nCntLine2;

				break;
			}
		}
	}

	return nIdxLine;
}
