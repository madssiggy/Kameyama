#include "myDirect3D.h"
#include "texture.h"
#include "player.h"
#include "debugproc.h"

#define SAFE_RELEASE_SKYDOME(p) {if(p){(p)->Release();(p)=NULL;}}//安全に解放する

//*****************************************************************************
// グローバル変数
//*****************************************************************************

static LPDIRECT3DVERTEXBUFFER9			g_pVtxBuffSkydome = NULL;	// 頂点バッファへのポインタ

static D3DXMATRIX						g_mtxWorldSkydome;			// ワールドマトリックス
static D3DXVECTOR3						g_posSkydome;				// プレイヤーの位置
static D3DXVECTOR3						g_rotSkydome;				// プレイヤーの向き(回転)
static D3DXVECTOR3						g_sclSkydome;				// プレイヤーの大きさ(スケール)
static D3DXVECTOR3						g_normalSkydome;				// プレイヤーの法線

static LPDIRECT3DDEVICE9		pDevice;
static DWORD					dwNumMaterials = 0;
static LPD3DXMESH				pMesh = NULL;				//メッシュ
static D3DMATERIAL9*			pMaterials = NULL;			//マテリアル
static LPDIRECT3DTEXTURE9*		pTextures = NULL;			//テクスチャ


HRESULT Skydome_Initialize(void){


	pDevice = GetD3DDevice();

	// Xファイルからメッシュをロードする 
	LPD3DXBUFFER pD3DXMtrlBuffer = NULL;

	if (FAILED(D3DXLoadMeshFromX("Sky3.x", D3DXMESH_SYSTEMMEM,
		pDevice, NULL, &pD3DXMtrlBuffer, NULL,
		&dwNumMaterials, &pMesh)))
	{
		MessageBox(NULL, "Xファイルの読み込みに失敗しました", NULL, MB_OK);
		return E_FAIL;
	}
	D3DXMATERIAL* d3dxMaterials = (D3DXMATERIAL*)pD3DXMtrlBuffer->GetBufferPointer();
	pMaterials = new D3DMATERIAL9[dwNumMaterials];
	pTextures = new LPDIRECT3DTEXTURE9[dwNumMaterials];

	for (DWORD i = 0; i < dwNumMaterials; i++)
	{
		pMaterials[i] = d3dxMaterials[i].MatD3D;
		pMaterials[i].Ambient = pMaterials[i].Diffuse;
		pTextures[i] = NULL;
		if (d3dxMaterials[i].pTextureFilename != NULL &&
			lstrlen(d3dxMaterials[i].pTextureFilename) > 0)
		{
			if (FAILED(D3DXCreateTextureFromFile(pDevice,
				d3dxMaterials[i].pTextureFilename,
				&pTextures[i])))
			{
				MessageBox(NULL, "テクスチャの読み込みに失敗しました", NULL, MB_OK);
			}
		}
	}
	pD3DXMtrlBuffer->Release();


	// 位置・回転・スケールの初期設定
	g_posSkydome = D3DXVECTOR3(0.0f, -150000.0f, 0.0f);
	g_rotSkydome = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_sclSkydome = D3DXVECTOR3(150000.0f,150000.0f, 150000.0f);
	g_normalSkydome = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void Skydome_Finalize(void)
{
	if (g_pVtxBuffSkydome != NULL)
	{// 頂点バッファの開放
		g_pVtxBuffSkydome->Release();
		g_pVtxBuffSkydome = NULL;
	}

	SAFE_RELEASE_SKYDOME(pMesh);
	SAFE_RELEASE_SKYDOME(pDevice);
}

//=============================================================================
// 描画処理
//=============================================================================
void Skydome_Draw(void)
{
	//デバイスが無いなら終了
	if (NULL == pDevice) { return; }

	D3DXMATRIXA16 mtxScl, mtxRot, mtxTranslate;

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&g_mtxWorldSkydome);

	// スケール反転
	D3DXMatrixScaling(&mtxScl, g_sclSkydome.x, g_sclSkydome.y, g_sclSkydome.z);
	D3DXMatrixMultiply(&g_mtxWorldSkydome, &g_mtxWorldSkydome, &mtxScl);

	// 回転を反転
	D3DXMatrixRotationYawPitchRoll(&mtxRot, g_rotSkydome.y, g_rotSkydome.x, g_rotSkydome.z);
	D3DXMatrixMultiply(&g_mtxWorldSkydome, &g_mtxWorldSkydome, &mtxRot);

	// 移動を反転
	D3DXMatrixTranslation(&mtxTranslate, g_posSkydome.x, g_posSkydome.y, g_posSkydome.z);
	D3DXMatrixMultiply(&g_mtxWorldSkydome, &g_mtxWorldSkydome, &mtxTranslate);

	pDevice->SetTransform(D3DTS_WORLD, &g_mtxWorldSkydome);

	//頂点シェーダをセット(使わない場合はNULL)
	GetD3DDevice()->SetVertexShader(NULL);

	//頂点フォーマットをセット
	GetD3DDevice()->SetFVF(pMesh->GetFVF());

	//ここに処理内容を書く
	for (DWORD i = 0; i < dwNumMaterials; i++)
	{
		pDevice->SetMaterial(&pMaterials[i]);
		pDevice->SetTexture(0, pTextures[i]);
		pMesh->DrawSubset(i);
	}

	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);		// ライトを有効
	pDevice->SetRenderState(D3DRS_AMBIENT, 0x00aaaaaa);	// 環境光を設定
	pDevice->SetRenderState(D3DRS_SPECULARENABLE, TRUE);// 鏡面反射有効
	//pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);	// Zバッファーを有効
}


void Skydome_Update(void)
{
	//g_posSkydome = D3DXVECTOR3(GetPlayer_Pos().x, -10000.0f, GetPlayer_Pos().z - 10000);

	//g_sclSkydome += D3DXVECTOR3(100,100,100);
	//g_posSkydome.y += -100.0f;
	//DebugProc_Print((char *)"【【%f】】", g_sclSkydome.x);
}
