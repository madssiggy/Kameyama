//=============================================================================
//
// 背景 [back_ground.cpp]
//
//=============================================================================
#include "back_ground.h"
#include "myDirect3D.h"
#include "camera.h"
#include "input.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_BUILDING			"asset/MODEL/TheMountain.x"	// 読み込むモデル名
#define TEXTURE_FILENAME		"asset/texture/ground_grass.jpg"

//*****************************************************************************
// クラス定義
//*****************************************************************************

class OBJECT
{
public:

	D3DXMATRIX			mtxWorldModel;	// ワールドマトリックス
	D3DXVECTOR3			posModel;		// 現在の位置
	D3DXVECTOR3			rotModel;		// 現在の向き
	D3DXVECTOR3			rotDestModel;	// 目的の向き
	D3DXVECTOR3			sclModel;		// 大きさ		

	//int					idxShadow;

	OBJECT();
	~OBJECT();

private:

};

OBJECT::OBJECT()
{
	// 位置・向き・移動量の初期設定
	posModel = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	rotModel = D3DXVECTOR3(0.0f, D3DX_PI * 0 / 180, 0.0f);
	rotDestModel = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	sclModel = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
}

OBJECT::~OBJECT()
{
}

//*****************************************************************************
// グローバル変数
//*****************************************************************************

static LPDIRECT3DTEXTURE9	g_pTextureModel;	// テクスチャへのポインタ
static LPD3DXMESH			g_pMeshModel;		// メッシュ情報へのポインタ
static LPD3DXBUFFER			g_pBuffMatModel;	// マテリアル情報へのポインタ
static DWORD				g_nNumMatModel;		// マテリアル情報の総数

OBJECT g_Object[MAX_BG];

float sclUpdate = 0.01f;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT Back_Initialize()
{
	LPDIRECT3DDEVICE9 pDevice = GetD3DDevice();

	g_pTextureModel = NULL;
	g_pMeshModel = NULL;
	g_pBuffMatModel = NULL;

	// テクスチャの読み込み
	if (FAILED(D3DXCreateTextureFromFile(pDevice,// デバイスへのポインタ
		TEXTURE_FILENAME,	// ファイルの名前
		&g_pTextureModel))) // 読み込むメモリー
	{
		MessageBox(NULL, "テクスチャの読み込みに失敗しました", NULL, MB_OK);
	}

	// Xファイルの読み込み
	if (FAILED(D3DXLoadMeshFromX(MODEL_BUILDING,
		D3DXMESH_SYSTEMMEM,
		pDevice,
		NULL,
		&g_pBuffMatModel,
		NULL,
		&g_nNumMatModel,
		&g_pMeshModel)))
	{
		MessageBox(NULL, "Xファイルの読み込みに失敗しました", NULL, MB_OK);
		return E_FAIL;
	}

	g_Object[0].posModel = D3DXVECTOR3(-30000.0f, -5000.0f, 30000.0f);
	g_Object[0].sclModel = D3DXVECTOR3(0.5f, 0.5f, 0.5f);
	
	g_Object[1].posModel = D3DXVECTOR3(30000.0f, -5000.0f, 30000.0f);
	g_Object[1].sclModel = D3DXVECTOR3(0.5f, 0.5f, 0.5f);

	g_Object[2].posModel = D3DXVECTOR3(0.0f, -5000.0f, 50000.0f);
	g_Object[2].sclModel = D3DXVECTOR3(0.5f, 0.5f, 0.5f);

	g_Object[3].posModel = D3DXVECTOR3(-30000.0f, -5000.0f, -30000.0f);
	g_Object[3].sclModel = D3DXVECTOR3(0.5f, 0.5f, 0.5f);

	g_Object[4].posModel = D3DXVECTOR3(30000.0f, -5000.0f, -30000.0f);
	g_Object[4].sclModel = D3DXVECTOR3(0.5f, 0.5f, 0.5f);

	g_Object[5].posModel = D3DXVECTOR3(0.0f, -5000.0f, -50000.0f);
	g_Object[5].sclModel = D3DXVECTOR3(0.5f, 0.5f, 0.5f);
	
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void Back_Finalize(void)
{
	//影の解放処理

	if (g_pTextureModel != NULL)
	{// テクスチャの開放
		g_pTextureModel->Release();
		g_pTextureModel = NULL;
	}

	if (g_pMeshModel != NULL)
	{// メッシュの開放
		g_pMeshModel->Release();
		g_pMeshModel = NULL;
	}

	if (g_pBuffMatModel != NULL)
	{// マテリアルの開放
		g_pBuffMatModel->Release();
		g_pBuffMatModel = NULL;
	}
}


//=============================================================================
// 描画処理
//=============================================================================
void Back_Draw(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetD3DDevice();
	D3DXMATRIX mtxScl, mtxRot, mtxTranslate;
	D3DXMATERIAL *pD3DXMat;
	D3DMATERIAL9 matDef;

	for (int i = 0; i < MAX_BG; i++)
	{
		// ワールドマトリックスの初期化
		D3DXMatrixIdentity(&g_Object[i].mtxWorldModel);

		// スケール反転
		D3DXMatrixScaling(&mtxScl, g_Object[i].sclModel.x, g_Object[i].sclModel.y, g_Object[i].sclModel.z);
		D3DXMatrixMultiply(&g_Object[i].mtxWorldModel, &g_Object[i].mtxWorldModel, &mtxScl);

		// 回転を反映
		D3DXMatrixRotationYawPitchRoll(&mtxRot, g_Object[i].rotModel.y, g_Object[i].rotModel.x, g_Object[i].rotModel.z);
		D3DXMatrixMultiply(&g_Object[i].mtxWorldModel, &g_Object[i].mtxWorldModel, &mtxRot);

		// 移動を反映
		D3DXMatrixTranslation(&mtxTranslate, g_Object[i].posModel.x, g_Object[i].posModel.y, g_Object[i].posModel.z);
		D3DXMatrixMultiply(&g_Object[i].mtxWorldModel, &g_Object[i].mtxWorldModel, &mtxTranslate);

		// ワールドマトリックスの設定
		pDevice->SetTransform(D3DTS_WORLD, &g_Object[i].mtxWorldModel);

		// 現在のマテリアルを取得
		pDevice->GetMaterial(&matDef);

		// マテリアル情報に対するポインタを取得

		pD3DXMat = (D3DXMATERIAL*)g_pBuffMatModel->GetBufferPointer();


		for (int nCntMat = 0; nCntMat < (int)g_nNumMatModel; nCntMat++)
		{
			// マテリアルの設定
			pDevice->SetMaterial(&pD3DXMat[nCntMat].MatD3D);

			// テクスチャの設定
			pDevice->SetTexture(0, g_pTextureModel);

			// 描画
			g_pMeshModel->DrawSubset(nCntMat);
		}

		// マテリアルをデフォルトに戻す
		pDevice->SetMaterial(&matDef);
	}

	//pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);		// ライトを有効
	//pDevice->SetRenderState(D3DRS_AMBIENT, 0x00aaaaaa);	// 環境光を設定
	//pDevice->SetRenderState(D3DRS_SPECULARENABLE, TRUE);// 鏡面反射有効

}

void Back_Update(void) {

	//for (int i = 0; i < 1; i++)
	//{
	//	g_Object[i].sclModel.x += sclUpdate;
	//	g_Object[i].sclModel.y += sclUpdate;
	//	g_Object[i].sclModel.z += sclUpdate;
	//}
}