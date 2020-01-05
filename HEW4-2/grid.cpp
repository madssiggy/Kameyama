
#include "grid.h"
#include "myDirect3D.h"
#include "input.h"
#include "texture.h"
#include "debugproc.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************

#define	VALUE_MOVE		(5.0f)							// 移動量
#define	VALUE_ROTATE	(D3DX_PI * 0.02f)				// 回転量

#define	Grid_WIDTH		(50.0f)						// 地面の幅(X方向)
#define	Grid_DEPTH		(50.0f)						// 地面の奥行(Z方向)
#define	Grid_HEIGHT		(50.0f)						// 地面の高さ(Y方向)

#define Grid_AREA_X		100
#define Grid_AREA_Y		100
#define Grid_LINE		1

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT MakeVertexGrid(LPDIRECT3DDEVICE9 pDevice);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffGrid = NULL;	// 頂点バッファへのポインタ

D3DXMATRIX				g_mtxWorldGrid;		// ワールドマトリックス
D3DXVECTOR3				g_posGrid;				// 地面の位置

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT Grid_Initialize(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetD3DDevice();

	// 頂点情報の作成
	MakeVertexGrid(pDevice);

	// 位置の初期設定
	g_posGrid = D3DXVECTOR3(-50.0f*(Grid_AREA_X-1), -90.0f, 50.0f*(Grid_AREA_Y-1));

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void Grid_Finalize(void)
{
	if (g_pVtxBuffGrid != NULL)
	{// 頂点バッファの開放
		g_pVtxBuffGrid->Release();
		g_pVtxBuffGrid = NULL;
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void Grid_Update(void)
{
	
}

//=============================================================================
// 描画処理
//=============================================================================
void Grid_Draw(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetD3DDevice();
	D3DXMATRIX mtxScl, mtxRot, mtxTranslate;

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&g_mtxWorldGrid);

	// 移動を反転
	D3DXMatrixTranslation(&mtxTranslate, g_posGrid.x, g_posGrid.y, g_posGrid.z);
	D3DXMatrixMultiply(&g_mtxWorldGrid, &g_mtxWorldGrid, &mtxTranslate);

	pDevice->SetTransform(D3DTS_WORLD, &g_mtxWorldGrid);

	pDevice->SetStreamSource(0, g_pVtxBuffGrid, 0, sizeof(VERTEX_3D));

	pDevice->SetFVF(FVF_VERTEX_3D);

	pDevice->SetTexture(0, NULL);

	pDevice->DrawPrimitive(D3DPT_LINELIST, 0, ((Grid_LINE + 1) * 2)*(Grid_AREA_X*Grid_AREA_Y));
}

//=============================================================================
// 頂点の作成
//=============================================================================
HRESULT MakeVertexGrid(LPDIRECT3DDEVICE9 pDevice)
{
	if (FAILED(pDevice->CreateVertexBuffer(sizeof(VERTEX_3D)* ((Grid_LINE + 1) * 4) * (Grid_AREA_X*Grid_AREA_Y), //VRAMにメモリを作る
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&g_pVtxBuffGrid,
		NULL)))
	{
		return E_FAIL;
	}

	{
		VERTEX_3D *pVtx;

		//Lock,unLockをあまりしないのがパフォーマンスを上げる方法
		g_pVtxBuffGrid->Lock(0, 0, (void**)&pVtx, 0); //VRAMにあったメモリをシステム上に持ってきてくれる

		//横の大きさ
		int cou = 0;
		for (int y=0; y < Grid_AREA_Y; y++)
		{
			//縦の大きさ
			for (int x=0; x < Grid_AREA_X; x++)
			{
				//グリッドの線の数
				for (int i = 0; i <= Grid_LINE; i++)
				{
					float l_sp = 100.0f / Grid_LINE;

					// ポジション・大きさ
					pVtx[0 + cou * 4].pos = D3DXVECTOR3(-Grid_WIDTH + 100.0f*x, Grid_HEIGHT, Grid_DEPTH - l_sp*i - 100.0f*y);
					pVtx[1 + cou * 4].pos = D3DXVECTOR3(Grid_WIDTH + 100.0f*x, Grid_HEIGHT, Grid_DEPTH - l_sp*i - 100.0f*y);
					pVtx[2 + cou * 4].pos = D3DXVECTOR3(-Grid_WIDTH + l_sp*i + 100.0f*x, Grid_HEIGHT, Grid_DEPTH - 100.0f*y);
					pVtx[3 + cou * 4].pos = D3DXVECTOR3(-Grid_WIDTH + l_sp*i + 100.0f*x, Grid_HEIGHT, -Grid_DEPTH - 100.0f*y);
							 
					// 法線ベクトル
					pVtx[0 + cou * 4].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
					pVtx[1 + cou * 4].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
					pVtx[2 + cou * 4].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
					pVtx[3 + cou * 4].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
							 
					// 色
					pVtx[0 + cou * 4].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
					pVtx[1 + cou * 4].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
					pVtx[2 + cou * 4].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
					pVtx[3 + cou * 4].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

					// 回っている回数
					cou++;
				}
			}
		}
		g_pVtxBuffGrid->Unlock(); //VRAMにメモリを返す(重い処理)
	}

	return S_OK;
}

