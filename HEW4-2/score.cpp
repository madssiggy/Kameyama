
#include "score.h"
#include "myDirect3D.h"
#include "input.h"
#include "debugproc.h"
#include "texture.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************名
#define	SCORE_SIZE_X		Texture_GetWidth(TEXTURE_INDEX_SCORE)							// スコアの数字の幅
#define	SCORE_SIZE_Y		Texture_GetHeight(TEXTURE_INDEX_SCORE)							// スコアの数字の高さ
#define	SCORE_INTERVAL_X	(0.0f)															// スコアの数字の表示間隔

#define	NUM_PLACE			(8)																// スコアの桁数

#define	SCORE_POS_X			(SCREEN_WIDTH - (SCORE_INTERVAL_X + SCORE_SIZE_X) * NUM_PLACE - 10.0f)	// スコアの表示基準位置Ｘ座標
#define	SCORE_POS_Y			(10.0f)																	// スコアの表示基準位置Ｙ座標

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT MakeVertexScore(LPDIRECT3DDEVICE9 pDevice);
void SetTextureScore(int nIdx, int nNumber);

LPDIRECT3DTEXTURE9		g_pTextureScore = NULL;	// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffScore = NULL;	// 頂点バッファへのポインタ
D3DXVECTOR3				g_posScore;				// 位置
D3DXVECTOR3				g_rotScore;				// 向き

int						g_nScore;				// スコア
int                     g_LastScore;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitScore(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetD3DDevice();

	// 位置・向きの初期化
	g_posScore = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_rotScore = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	// スコアの初期化
	g_nScore = 0;

	// 頂点情報の作成
	MakeVertexScore(pDevice);

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitScore(void)
{
	if (g_pTextureScore != NULL)
	{// テクスチャの開放
		g_pTextureScore->Release();
		g_pTextureScore = NULL;
	}

	if (g_pVtxBuffScore != NULL)
	{// 頂点バッファの開放
		g_pVtxBuffScore->Release();
		g_pVtxBuffScore = NULL;
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateScore(void)
{
	for (int nCntPlace = 0; nCntPlace < NUM_PLACE; nCntPlace++)
	{
		int nNumber;

		nNumber = (g_nScore % (int)(powf(10.0f, (float)(NUM_PLACE - nCntPlace)))) / (int)(powf(10.0f, (float)(NUM_PLACE - nCntPlace - 1)));
		SetTextureScore(nCntPlace, nNumber);

		if (Keyboard_IsPress(DIK_UP) || Keyboard_IsPress(DIK_W))
		{
			g_nScore++;
		}
		if (Keyboard_IsPress(DIK_DOWN) || Keyboard_IsPress(DIK_S))
		{
			g_nScore--;
		}
		g_LastScore = g_nScore;
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawScore(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetD3DDevice();

	// 頂点バッファをデバイスのデータストリームにバインド
	pDevice->SetStreamSource(0, g_pVtxBuffScore, 0, sizeof(VERTEX_2D));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_2D);

	// テクスチャの設定
	pDevice->SetTexture(0, Texture_GetTexture(TEXTURE_INDEX_SCORE));

	// ポリゴンの描画
	for (int nCntPlace = 0; nCntPlace < NUM_PLACE; nCntPlace++)
	{
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, (nCntPlace * 4), NUM_POLYGON);
	}
}

//=============================================================================
// 頂点座標の作成
//=============================================================================
HRESULT MakeVertexScore(LPDIRECT3DDEVICE9 pDevice)
{
	// オブジェクトの頂点バッファを生成
	if (FAILED(pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * NUM_VERTEX * NUM_PLACE,	// 頂点データ用に確保するバッファサイズ(バイト単位)
		D3DUSAGE_WRITEONLY,						// 頂点バッファの使用法　
		FVF_VERTEX_2D,							// 使用する頂点フォーマット
		D3DPOOL_MANAGED,						// リソースのバッファを保持するメモリクラスを指定
		&g_pVtxBuffScore,						// 頂点バッファインターフェースへのポインタ
		NULL)))									// NULLに設定
	{
		return E_FAIL;
	}

	{//頂点バッファの中身を埋める
		VERTEX_2D* pVtx;

		// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
		g_pVtxBuffScore->Lock(0, 0, (void**)&pVtx, 0);

		for (int nCntPlace = 0; nCntPlace < NUM_PLACE; nCntPlace++, pVtx += 4)
		{
			// 頂点座標の設定
			pVtx[0].pos = D3DXVECTOR3(SCORE_POS_X + nCntPlace * (SCORE_SIZE_X + SCORE_INTERVAL_X), SCORE_POS_Y, 0.0f);
			pVtx[1].pos = D3DXVECTOR3(SCORE_POS_X + nCntPlace * (SCORE_INTERVAL_X + SCORE_SIZE_X) + SCORE_SIZE_X, SCORE_POS_Y, 0.0f);
			pVtx[2].pos = D3DXVECTOR3(SCORE_POS_X + nCntPlace * (SCORE_SIZE_X + SCORE_INTERVAL_X), SCORE_POS_Y + SCORE_SIZE_Y, 0.0f);
			pVtx[3].pos = D3DXVECTOR3(SCORE_POS_X + nCntPlace * (SCORE_INTERVAL_X + SCORE_SIZE_X) + SCORE_SIZE_X, SCORE_POS_Y + SCORE_SIZE_Y, 0.0f);

			// rhwの設定
			pVtx[0].rhw = 1.0f;
			pVtx[1].rhw = 1.0f;
			pVtx[2].rhw = 1.0f;
			pVtx[3].rhw = 1.0f;

			// 頂点カラーの設定
			pVtx[0].col = D3DCOLOR_RGBA(255, 255, 0, 255);
			pVtx[1].col = D3DCOLOR_RGBA(255, 255, 0, 255);
			pVtx[2].col = D3DCOLOR_RGBA(255, 255, 0, 255);
			pVtx[3].col = D3DCOLOR_RGBA(255, 255, 0, 255);

			// テクスチャ座標の設定
			pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
			pVtx[1].tex = D3DXVECTOR2(0.1f, 0.0f);
			pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
			pVtx[3].tex = D3DXVECTOR2(0.1f, 1.0f);
		}

		// 頂点データをアンロックする
		g_pVtxBuffScore->Unlock();
	}

	return S_OK;

}

//=============================================================================
// テクスチャ座標の設定
//=============================================================================
void SetTextureScore(int nIdx, int nNumber)
{
	VERTEX_2D* pVtx;

	// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
	g_pVtxBuffScore->Lock(0, 0, (void**)&pVtx, 0);

	pVtx += (nIdx * 4);

	// 頂点座標の設定
	pVtx[0].tex = D3DXVECTOR2(nNumber * 0.1f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(nNumber * 0.1f + 0.1f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(nNumber * 0.1f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(nNumber * 0.1f + 0.1f, 1.0f);

	// 頂点データをアンロックする
	g_pVtxBuffScore->Unlock();
}
//===========================================================================
// スコア加算
//===========================================================================
void Add_Score(int num)
{
	g_nScore += num;
}

//============================================================================
// ゲッター
//============================================================================
int GetScore()
{
	return g_LastScore;
}