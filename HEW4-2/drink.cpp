#include "Drink.h"
#include "myDirect3D.h"
#include "input.h"
#include "texture.h"
#include "debugproc.h"
#include "camera.h"
#include "debugproc.h"
#include "player.h"
#include "shadow.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************

#define	VALUE_MOVE		(1.0f)							// 移動量
#define	VALUE_ROTATE	(D3DX_PI * 0.03f)				// 回転量

#define	Drink_WIDTH		(15.0f)						// 地面の幅(X方向)
#define	Drink_DEPTH		(15.0f)						// 地面の奥行(Z方向)
#define	Drink_HEIGHT	(15.0f)						// 地面の高さ(Y方向)

#define Drink_ANGLE		(8)
#define Drink_NUM		(5)

#define SAFE_RELEASE(p) {if(p){(p)->Release();(p)=NULL;}}//安全に解放する
//*****************************************************************************
// グローバル変数
//*****************************************************************************
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffDrink = NULL;	// 頂点バッファへのポインタ
struct DRINK
{
	D3DXMATRIX				g_mtxWorldDrink;		// ワールドマトリックス
	D3DXVECTOR3				g_posDrink;				// 地面の位置
	D3DXVECTOR3				g_rotDrink;				// 地面の向き(回転)
	D3DXVECTOR3				g_sclDrink;				// 地面の大きさ(スケール)
	int						g_sdId;					// 影ID
	bool					isUse=true;				// ドリンクの使用判定
};

DRINK Drink[Drink_NUM];
static bool Hit = false;

static LPDIRECT3DDEVICE9		pDevice;
static DWORD					dwNumMaterials = 0;
static LPD3DXMESH				pMesh = NULL;				//メッシュ
static D3DMATERIAL9*			pMaterials = NULL;			//マテリアル
static LPDIRECT3DTEXTURE9*		pTextures = NULL;			//テクスチャ

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void Collision_Drink();

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT Drink_Initialize(void)
{
	pDevice = GetD3DDevice();

	// Xファイルからメッシュをロードする 
	LPD3DXBUFFER pD3DXMtrlBuffer = NULL;

	if (FAILED(D3DXLoadMeshFromX("drink.x", D3DXMESH_SYSTEMMEM,
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
	for (int i = 0; i < Drink_NUM; i++)
	{
		Drink[i].g_posDrink = D3DXVECTOR3(-300.0f + 50.0f*i, 20.0f, 500.0f);
		Drink[i].g_rotDrink = D3DXVECTOR3(D3DX_PI / 6, 0.0f, 0.0f);
		Drink[i].g_sclDrink = D3DXVECTOR3(7.0f, 7.0f, 7.0f);
	
		//影を作成する
		Drink[i].g_sdId = Shadow_Create(D3DXVECTOR3(Drink[i].g_posDrink.x,Drink[i].g_posDrink.y-20.0f,Drink[i].g_posDrink.z),
			D3DXVECTOR3(0.5f, 0.5f, 0.5f));

	}

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void Drink_Finalize(void)
{
	if (g_pVtxBuffDrink != NULL)
	{// 頂点バッファの開放
		g_pVtxBuffDrink->Release();
		g_pVtxBuffDrink = NULL;
	}

	SAFE_RELEASE(pMesh);
	SAFE_RELEASE(pDevice);
}

//=============================================================================
// 更新処理
//=============================================================================
void Drink_Update(void)
{
	Collision_Drink();
	// Y軸左回転
	for (int i = 0; i < Drink_NUM; i++)
	{
		if(Drink[i].isUse) Drink[i].g_rotDrink.y -= VALUE_ROTATE;
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void Drink_Draw(void)
{
	//デバイスが無いなら終了
	if (NULL == pDevice) { return; }

	D3DXMATRIXA16 mtxScl, mtxRot, mtxTranslate;
	for (int i = 0; i < Drink_NUM; i++)
	{
		if (Drink[i].isUse)
		{
			// ワールドマトリックスの初期化
			D3DXMatrixIdentity(&Drink[i].g_mtxWorldDrink);

			// スケール反転
			D3DXMatrixScaling(&mtxScl, Drink[i].g_sclDrink.x, Drink[i].g_sclDrink.y, Drink[i].g_sclDrink.z);
			D3DXMatrixMultiply(&Drink[i].g_mtxWorldDrink, &Drink[i].g_mtxWorldDrink, &mtxScl);

			// 回転を反転
			D3DXMatrixRotationYawPitchRoll(&mtxRot, Drink[i].g_rotDrink.y, Drink[i].g_rotDrink.x, Drink[i].g_rotDrink.z);
			D3DXMatrixMultiply(&Drink[i].g_mtxWorldDrink, &Drink[i].g_mtxWorldDrink, &mtxRot);

			// 移動を反転
			D3DXMatrixTranslation(&mtxTranslate, Drink[i].g_posDrink.x, Drink[i].g_posDrink.y, Drink[i].g_posDrink.z);
			D3DXMatrixMultiply(&Drink[i].g_mtxWorldDrink, &Drink[i].g_mtxWorldDrink, &mtxTranslate);

			pDevice->SetTransform(D3DTS_WORLD, &Drink[i].g_mtxWorldDrink);

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
		}
	}
}

// ドリンクの当たり判定
void Collision_Drink()
{
	D3DXVECTOR3 Player = GetPlayer_Pos();
	D3DXVECTOR3 dst1(Player.x, Player.y, Player.z);

	Hit = false;
	for (int i = 0; i < Drink_NUM; i++)
	{
		if (Drink[i].isUse)
		{
			D3DXVECTOR3 dst2(Drink[i].g_posDrink.x, Drink[i].g_posDrink.y, Drink[i].g_posDrink.z);
			D3DXVECTOR3 distance;
			distance = dst2 - dst1;
			float length = D3DXVec3Length(&distance);
			float size = 15;
			if (length < size)
			{
				Hit = true;
				Drink[i].isUse = false;

				//影の解放処理
				Shadow_Release(Drink[i].g_sdId);
				return;
			}
		}
	}
	return;
}

bool Get_DrinkHit()
{
	return Hit;
}