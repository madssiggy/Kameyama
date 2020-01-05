//=============================================================================
//
// パーティクル処理 [particle.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "myDirect3D.h"
#include "debugproc.h"
#include "input.h"
#include "particle.h"
#include "player.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	TEXTURE_PARTICLE	"asset/texture/effect000.jpg"	// 読み込むテクスチャファイル名
#define	PARTICLE_SIZE_X		(50.0f)							// ビルボードの幅
#define	PARTICLE_SIZE_Y		(50.0f)							// ビルボードの高さ
#define	VALUE_MOVE_PARTICLE	(5.0f)							// 移動速度

#define	MAX_PARTICLE		(40)							// ビルボード最大数
#define	MAX_PARTICLE2		(720)							// ビルボード最大数

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT MakeVertexParticle(LPDIRECT3DDEVICE9 pDevice);
void SetVertexParticle(int nIdxParticle, float fSizeX, float fSizeY);
void SetColorParticle(int nIdxParticle, D3DXCOLOR col);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static LPDIRECT3DTEXTURE9		g_pD3DTextureParticle = NULL;	// テクスチャへのポインタ
static LPDIRECT3DVERTEXBUFFER9 g_pD3DVtxBuffParticle = NULL;	// 頂点バッファインターフェースへのポインタ

static D3DXMATRIX				g_mtxWorldParticle;				// ワールドマトリックス
static D3DXMATRIX				g_mtxWorldParticle2;			// ワールドマトリックス

static PARTICLE					g_aParticle[MAX_PARTICLE];		// パーティクルワーク
static PARTICLE					g_aParticle2[MAX_PARTICLE2];	// パーティクルワーク

static D3DXVECTOR3				g_posBase;						// ビルボード発生位置
static float					g_fWidthBase = 5.0f;			// 基準の幅
static float					g_fHeightBase = 10.0f;			// 基準の高さ
static bool						g_bPause = false;				// ポーズON/OFF
static float					g_roty = 0.0f;					// 移動方向
static float					g_spd = 0.0f;					// 移動スピード

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT ParticleManager::InitParticle(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetD3DDevice();

	// 頂点情報の作成
	MakeVertexParticle(pDevice);

	// テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice,						// デバイスへのポインタ
								TEXTURE_PARTICLE,			// ファイルの名前
								&g_pD3DTextureParticle);	// 読み込むメモリー

	for(int nCntParticle = 0; nCntParticle < MAX_PARTICLE+MAX_PARTICLE2; nCntParticle++)
	{
		g_aParticle[nCntParticle].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_aParticle[nCntParticle].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_aParticle[nCntParticle].scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
		g_aParticle[nCntParticle].move = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
		g_aParticle[nCntParticle].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		g_aParticle[nCntParticle].fSizeX = PARTICLE_SIZE_X;
		g_aParticle[nCntParticle].fSizeY = PARTICLE_SIZE_Y;
		g_aParticle[nCntParticle].nIdxShadow = -1;
		g_aParticle[nCntParticle].nLife = 0;
		g_aParticle[nCntParticle].bUse = false;
	}

	g_posBase = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_roty = 0.0f;
	g_spd = 0.0f;

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void ParticleManager::UninitParticle(void)
{
	if(g_pD3DTextureParticle != NULL)
	{// テクスチャの開放
		g_pD3DTextureParticle->Release();
		g_pD3DTextureParticle = NULL;
	}

	if(g_pD3DVtxBuffParticle != NULL)
	{// 頂点バッファの開放
		g_pD3DVtxBuffParticle->Release();
		g_pD3DVtxBuffParticle = NULL;
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void ParticleManager::UpdateParticle(void)
{
	// 更新処理
	if (Get_driftcnt() >= 20)
	{
		for (int nCntParticle = 0; nCntParticle < MAX_PARTICLE; nCntParticle++)
		{
			// 使用中
			if (g_aParticle[nCntParticle].bUse)
			{
				// 移動
				g_aParticle[nCntParticle].pos.x += g_aParticle[nCntParticle].move.x+ Get_driftmove().x;
				g_aParticle[nCntParticle].pos.z += g_aParticle[nCntParticle].move.z+ Get_driftmove().z;
				// 重力などもできる
				g_aParticle[nCntParticle].pos.y += g_aParticle[nCntParticle].move.y + Get_driftmove().y;

				g_aParticle[nCntParticle].move.y -= 0.05f*(float)((rand() % 10+5) / 10);

				// 寿命が減る
				g_aParticle[nCntParticle].nLife--;

				// 寿命がなくなった時
				if (g_aParticle[nCntParticle].nLife <= 0)
				{
					// 表示されなくなる
					g_aParticle[nCntParticle].bUse = false;
					// ReleaseShadow(g_aParticle[nCntParticle].nIdxShadow);
					// 影の数を減らす
					g_aParticle[nCntParticle].nIdxShadow = -1;
				}
				else
				{
					// 寿命が減った時
					if (g_aParticle[nCntParticle].nLife <= 8)
					{
						g_aParticle[nCntParticle].col = D3DXCOLOR(0.0f, 0.7f, 0.7f, g_aParticle[nCntParticle].col.a);
					}

					// 色の設定
					SetColorParticle(nCntParticle, g_aParticle[nCntParticle].col);
				}
				// 残像を生成
				ParticleManager::SetParticle(g_aParticle[nCntParticle].pos, D3DXVECTOR3(0.0f, 0.0f, 0.0f), g_aParticle[nCntParticle].col,
					g_aParticle[nCntParticle].fSizeX*1.4f, g_aParticle[nCntParticle].fSizeY*1.4f, g_aParticle[nCntParticle].nLife, 2);
			}
		}
		for (int nCntParticle2 = 0; nCntParticle2 < MAX_PARTICLE2; nCntParticle2++)
		{
			// 使用中
			if (g_aParticle2[nCntParticle2].bUse)
			{
				// 移動
				g_aParticle2[nCntParticle2].pos.x += Get_driftmove().x;
				g_aParticle2[nCntParticle2].pos.z += Get_driftmove().z;
				g_aParticle2[nCntParticle2].pos.y += Get_driftmove().y;

				// 寿命が減る
				g_aParticle2[nCntParticle2].nLife -= 2;

				// 寿命がなくなった時
				if (g_aParticle2[nCntParticle2].nLife <= 0)
				{
					// 表示されなくなる
					g_aParticle2[nCntParticle2].bUse = false;
					// ReleaseShadow(g_aParticle2[nCntParticle2].nIdxShadow);
					// 影の数を減らす
					g_aParticle2[nCntParticle2].nIdxShadow = -1;
				}
				else
				{
					// α値設定
					g_aParticle2[nCntParticle2].col.a -= 0.05f;
					// サイズ設定
					g_aParticle2[nCntParticle2].fSizeX -= 2.0f;
					g_aParticle2[nCntParticle2].fSizeY -= 2.0f;
					if (g_aParticle2[nCntParticle2].col.a < 0.0f)
					{
						g_aParticle2[nCntParticle2].col.a = 0.0f;
					}

					// 色の設定
					SetColorParticle(nCntParticle2, g_aParticle2[nCntParticle2].col);
				}
			}
		}

		// パーティクル発生
		{
			D3DXVECTOR3 pos;
			D3DXVECTOR3 move;
			float fAngle;
			int nLife;
			float fSize;

			// ポジション
			pos.x = GetPlayer_Pos().x - 25.0f * sinf(GetPlayer_Rot().y);
			pos.y = GetPlayer_Pos().y-10.0f;
			pos.z = GetPlayer_Pos().z - 25.0f * cosf(GetPlayer_Rot().y);

			// 生成されてからどこに飛ぶか
			move.x = (1.0f - sinf(GetPlayer_Rot().y))* (float)((rand() % 10 - 5) / 10.0f);
			move.y = (rand() % 30 + 25) / 75.0f;
			move.z = (1.0f - cosf(GetPlayer_Rot().y))* (float)((rand() % 10 - 5) / 10.0f);

			// ランダムで命の設定
			nLife = rand() % 20 + 25;

			// パーティクルの大きさ
			fSize = (float)(rand() % 8 + 1.0f);

			// ビルボードの設定									    				　　　赤	緑	　青	
			if (Get_driftcnt() >= 80) ParticleManager::SetParticle(pos, move, D3DXCOLOR(1.0f, 0.8f, 0.0f, 1.0f), fSize, fSize, nLife,1);
			else ParticleManager::SetParticle(pos, move, D3DXCOLOR(0.0f, 0.0f, 1.0f, 0.5f), fSize, fSize, nLife,1);
		}
	}
	else
	{
		for (int nCntParticle = 0; nCntParticle < MAX_PARTICLE + MAX_PARTICLE2; nCntParticle++)
		{
			g_aParticle[nCntParticle].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			g_aParticle[nCntParticle].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			g_aParticle[nCntParticle].scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
			g_aParticle[nCntParticle].move = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
			g_aParticle[nCntParticle].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			g_aParticle[nCntParticle].fSizeX = PARTICLE_SIZE_X;
			g_aParticle[nCntParticle].fSizeY = PARTICLE_SIZE_Y;
			g_aParticle[nCntParticle].nIdxShadow = -1;
			g_aParticle[nCntParticle].nLife = 0;
			g_aParticle[nCntParticle].bUse = false;
		}
	}
}


//=============================================================================
// 描画処理
//=============================================================================
void ParticleManager::DrawParticle(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetD3DDevice();
	D3DXMATRIX mtxView,mtxScale,mtxTranslate;

	// ライティングを無効に
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	// 加算合成に設定
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);			// 結果 = 転送元(SRC) + 転送先(DEST)
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);		// αソースカラーの指定
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);			// αデスティネーションカラーの指定
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);				// Zバッファーの書き込みをしない

	// ビューマトリックスを取得
	pDevice->GetTransform(D3DTS_VIEW, &mtxView);

	for(int nCntParticle = 0; nCntParticle < MAX_PARTICLE; nCntParticle++)
	{
		if(g_aParticle[nCntParticle].bUse)
		{
			// ワールドマトリックスの初期化
			D3DXMatrixIdentity(&g_mtxWorldParticle);

			// 逆行列を求める
			D3DXMatrixInverse(&g_mtxWorldParticle, NULL, &mtxView);

			// 平行移動の部分を求める
			g_mtxWorldParticle._41 = 0.0f;
			g_mtxWorldParticle._42 = 0.0f;
			g_mtxWorldParticle._43 = 0.0f;

			// スケールを反映
			D3DXMatrixScaling(&mtxScale, g_aParticle[nCntParticle].scale.x, g_aParticle[nCntParticle].scale.y, g_aParticle[nCntParticle].scale.z);
			D3DXMatrixMultiply(&g_mtxWorldParticle, &g_mtxWorldParticle, &mtxScale);

			// 移動を反映
			D3DXMatrixTranslation(&mtxTranslate, g_aParticle[nCntParticle].pos.x, g_aParticle[nCntParticle].pos.y, g_aParticle[nCntParticle].pos.z);
			D3DXMatrixMultiply(&g_mtxWorldParticle, &g_mtxWorldParticle, &mtxTranslate);

			// ワールドマトリックスの設定
			pDevice->SetTransform(D3DTS_WORLD, &g_mtxWorldParticle);

			// 頂点バッファをデバイスのデータストリームにバインド
			pDevice->SetStreamSource(0, g_pD3DVtxBuffParticle, 0, sizeof(VERTEX_3D));

			// 頂点フォーマットの設定
			pDevice->SetFVF(FVF_VERTEX_3D);

			// テクスチャの設定
			pDevice->SetTexture(0, g_pD3DTextureParticle);

			// ポリゴンの描画
			pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, (nCntParticle * 4), NUM_POLYGON);
		}
	}
	for (int nCntParticle2 = 0; nCntParticle2 < MAX_PARTICLE2; nCntParticle2++)
	{
		if (g_aParticle2[nCntParticle2].bUse)
		{
			// ワールドマトリックスの初期化
			D3DXMatrixIdentity(&g_mtxWorldParticle2);

			// 逆行列を求める
			D3DXMatrixInverse(&g_mtxWorldParticle2, NULL, &mtxView);

			// 平行移動の部分を求める
			g_mtxWorldParticle2._41 = 0.0f;
			g_mtxWorldParticle2._42 = 0.0f;
			g_mtxWorldParticle2._43 = 0.0f;

			// スケールを反映
			D3DXMatrixScaling(&mtxScale, g_aParticle2[nCntParticle2].scale.x, g_aParticle2[nCntParticle2].scale.y, g_aParticle2[nCntParticle2].scale.z);
			D3DXMatrixMultiply(&g_mtxWorldParticle2, &g_mtxWorldParticle2, &mtxScale);

			// 移動を反映
			D3DXMatrixTranslation(&mtxTranslate, g_aParticle2[nCntParticle2].pos.x, g_aParticle2[nCntParticle2].pos.y, g_aParticle2[nCntParticle2].pos.z);
			D3DXMatrixMultiply(&g_mtxWorldParticle2, &g_mtxWorldParticle2, &mtxTranslate);

			// ワールドマトリックスの設定
			pDevice->SetTransform(D3DTS_WORLD, &g_mtxWorldParticle2);

			// 頂点バッファをデバイスのデータストリームにバインド
			pDevice->SetStreamSource(0, g_pD3DVtxBuffParticle, 0, sizeof(VERTEX_3D));

			// 頂点フォーマットの設定
			pDevice->SetFVF(FVF_VERTEX_3D);

			// テクスチャの設定
			pDevice->SetTexture(0, g_pD3DTextureParticle);

			// ポリゴンの描画
			pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, (nCntParticle2 * 4), NUM_POLYGON);
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
HRESULT MakeVertexParticle(LPDIRECT3DDEVICE9 pDevice)
{
	// オブジェクトの頂点バッファを生成
    if( FAILED( pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * NUM_VERTEX * (MAX_PARTICLE+MAX_PARTICLE2),	// 頂点データ用に確保するバッファサイズ(バイト単位)
												D3DUSAGE_WRITEONLY,							// 頂点バッファの使用法　
												FVF_VERTEX_3D,								// 使用する頂点フォーマット
												D3DPOOL_MANAGED,							// リソースのバッファを保持するメモリクラスを指定
												&g_pD3DVtxBuffParticle,						// 頂点バッファインターフェースへのポインタ
												NULL)))										// NULLに設定
	{
        return E_FAIL;
	}

	{//頂点バッファの中身を埋める
		VERTEX_3D *pVtx;

		// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
		g_pD3DVtxBuffParticle->Lock(0, 0, (void**)&pVtx, 0);

		for(int nCntParticle = 0; nCntParticle < MAX_PARTICLE+MAX_PARTICLE2; nCntParticle++, pVtx += 4)
		{
			// 頂点座標の設定
			pVtx[0].pos = D3DXVECTOR3(-PARTICLE_SIZE_X / 2, -PARTICLE_SIZE_Y / 2, 0.0f);
			pVtx[1].pos = D3DXVECTOR3(PARTICLE_SIZE_X / 2, -PARTICLE_SIZE_Y / 2, 0.0f);
			pVtx[2].pos = D3DXVECTOR3(-PARTICLE_SIZE_X / 2, PARTICLE_SIZE_Y / 2, 0.0f);
			pVtx[3].pos = D3DXVECTOR3(PARTICLE_SIZE_X / 2, PARTICLE_SIZE_Y / 2, 0.0f);

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
		g_pD3DVtxBuffParticle->Unlock();
	}

	return S_OK;
}

//=============================================================================
// 頂点座標の設定
//=============================================================================
void SetVertexParticle(int nIdxParticle, float fSizeX, float fSizeY)
{
	{//頂点バッファの中身を埋める
		VERTEX_3D *pVtx;

		// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
		g_pD3DVtxBuffParticle->Lock(0, 0, (void**)&pVtx, 0);

		pVtx += (nIdxParticle * 4);

		// 頂点座標の設定
		pVtx[0].pos = D3DXVECTOR3(-fSizeX / 2, -fSizeY / 2, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(-fSizeX / 2, fSizeY / 2, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(fSizeX / 2, -fSizeY / 2, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(fSizeX / 2, fSizeY / 2, 0.0f);

		// 頂点データをアンロックする
		g_pD3DVtxBuffParticle->Unlock();
	}
}

//=============================================================================
// 頂点カラーの設定
//=============================================================================
void SetColorParticle(int nIdxParticle, D3DXCOLOR col)
{
	{//頂点バッファの中身を埋める
		VERTEX_3D *pVtx;

		// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
		g_pD3DVtxBuffParticle->Lock(0, 0, (void**)&pVtx, 0);

		pVtx += (nIdxParticle * 4);

		// 頂点座標の設定
		pVtx[0].col =
		pVtx[1].col =
		pVtx[2].col =
		pVtx[3].col = col;

		// 頂点データをアンロックする
		g_pD3DVtxBuffParticle->Unlock();
	}
}

//=============================================================================
// 頂点情報の作成
//=============================================================================
int ParticleManager::SetParticle(D3DXVECTOR3 pos, D3DXVECTOR3 move, D3DXCOLOR col, float fSizeX, float fSizeY, int nLife, int num)
{
	int nIdxParticle = -1;

	if (num == 1)
	{
		for (int nCntParticle = 0; nCntParticle < MAX_PARTICLE; nCntParticle++)
		{
			if (!g_aParticle[nCntParticle].bUse)
			{
				g_aParticle[nCntParticle].pos = pos;
				g_aParticle[nCntParticle].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
				g_aParticle[nCntParticle].scale = D3DXVECTOR3(0.1f, 0.1f, 0.1f);
				g_aParticle[nCntParticle].move = move;
				g_aParticle[nCntParticle].col = col;
				g_aParticle[nCntParticle].fSizeX = fSizeX;
				g_aParticle[nCntParticle].fSizeY = fSizeY;
				g_aParticle[nCntParticle].nLife = nLife;
				g_aParticle[nCntParticle].bUse = true;

				// 頂点座標の設定
				SetVertexParticle(nCntParticle, fSizeX, fSizeY);

				nIdxParticle = nCntParticle;

				break;
			}
		}
	}
	else
	{
		for (int nCntParticle2 = 0; nCntParticle2 < MAX_PARTICLE2; nCntParticle2++)
		{
			if (!g_aParticle2[nCntParticle2].bUse)
			{
				g_aParticle2[nCntParticle2].pos = pos;
				g_aParticle2[nCntParticle2].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
				g_aParticle2[nCntParticle2].scale = D3DXVECTOR3(0.1f, 0.1f, 0.1f);
				g_aParticle2[nCntParticle2].move = move;
				g_aParticle2[nCntParticle2].col = col;
				g_aParticle2[nCntParticle2].fSizeX = fSizeX;
				g_aParticle2[nCntParticle2].fSizeY = fSizeY;
				g_aParticle2[nCntParticle2].nLife = nLife;
				g_aParticle2[nCntParticle2].bUse = true;

				// 頂点座標の設定
				SetVertexParticle(nCntParticle2, fSizeX, fSizeY);

				nIdxParticle = nCntParticle2;

				break;
			}
		}
	}

	return nIdxParticle;
}
