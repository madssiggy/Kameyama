#include "myDirect3D.h"
#include "texture.h"
#include "player.h"
#include "debugproc.h"

#define SAFE_RELEASE_SKYDOME(p) {if(p){(p)->Release();(p)=NULL;}}//���S�ɉ������

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************

static LPDIRECT3DVERTEXBUFFER9			g_pVtxBuffSkydome = NULL;	// ���_�o�b�t�@�ւ̃|�C���^

static D3DXMATRIX						g_mtxWorldSkydome;			// ���[���h�}�g���b�N�X
static D3DXVECTOR3						g_posSkydome;				// �v���C���[�̈ʒu
static D3DXVECTOR3						g_rotSkydome;				// �v���C���[�̌���(��])
static D3DXVECTOR3						g_sclSkydome;				// �v���C���[�̑傫��(�X�P�[��)
static D3DXVECTOR3						g_normalSkydome;				// �v���C���[�̖@��

static LPDIRECT3DDEVICE9		pDevice;
static DWORD					dwNumMaterials = 0;
static LPD3DXMESH				pMesh = NULL;				//���b�V��
static D3DMATERIAL9*			pMaterials = NULL;			//�}�e���A��
static LPDIRECT3DTEXTURE9*		pTextures = NULL;			//�e�N�X�`��


HRESULT Skydome_Initialize(void){


	pDevice = GetD3DDevice();

	// X�t�@�C�����烁�b�V�������[�h���� 
	LPD3DXBUFFER pD3DXMtrlBuffer = NULL;

	if (FAILED(D3DXLoadMeshFromX("Sky3.x", D3DXMESH_SYSTEMMEM,
		pDevice, NULL, &pD3DXMtrlBuffer, NULL,
		&dwNumMaterials, &pMesh)))
	{
		MessageBox(NULL, "X�t�@�C���̓ǂݍ��݂Ɏ��s���܂���", NULL, MB_OK);
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
				MessageBox(NULL, "�e�N�X�`���̓ǂݍ��݂Ɏ��s���܂���", NULL, MB_OK);
			}
		}
	}
	pD3DXMtrlBuffer->Release();


	// �ʒu�E��]�E�X�P�[���̏����ݒ�
	g_posSkydome = D3DXVECTOR3(0.0f, -150000.0f, 0.0f);
	g_rotSkydome = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_sclSkydome = D3DXVECTOR3(150000.0f,150000.0f, 150000.0f);
	g_normalSkydome = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void Skydome_Finalize(void)
{
	if (g_pVtxBuffSkydome != NULL)
	{// ���_�o�b�t�@�̊J��
		g_pVtxBuffSkydome->Release();
		g_pVtxBuffSkydome = NULL;
	}

	SAFE_RELEASE_SKYDOME(pMesh);
	SAFE_RELEASE_SKYDOME(pDevice);
}

//=============================================================================
// �`�揈��
//=============================================================================
void Skydome_Draw(void)
{
	//�f�o�C�X�������Ȃ�I��
	if (NULL == pDevice) { return; }

	D3DXMATRIXA16 mtxScl, mtxRot, mtxTranslate;

	// ���[���h�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&g_mtxWorldSkydome);

	// �X�P�[�����]
	D3DXMatrixScaling(&mtxScl, g_sclSkydome.x, g_sclSkydome.y, g_sclSkydome.z);
	D3DXMatrixMultiply(&g_mtxWorldSkydome, &g_mtxWorldSkydome, &mtxScl);

	// ��]�𔽓]
	D3DXMatrixRotationYawPitchRoll(&mtxRot, g_rotSkydome.y, g_rotSkydome.x, g_rotSkydome.z);
	D3DXMatrixMultiply(&g_mtxWorldSkydome, &g_mtxWorldSkydome, &mtxRot);

	// �ړ��𔽓]
	D3DXMatrixTranslation(&mtxTranslate, g_posSkydome.x, g_posSkydome.y, g_posSkydome.z);
	D3DXMatrixMultiply(&g_mtxWorldSkydome, &g_mtxWorldSkydome, &mtxTranslate);

	pDevice->SetTransform(D3DTS_WORLD, &g_mtxWorldSkydome);

	//���_�V�F�[�_���Z�b�g(�g��Ȃ��ꍇ��NULL)
	GetD3DDevice()->SetVertexShader(NULL);

	//���_�t�H�[�}�b�g���Z�b�g
	GetD3DDevice()->SetFVF(pMesh->GetFVF());

	//�����ɏ������e������
	for (DWORD i = 0; i < dwNumMaterials; i++)
	{
		pDevice->SetMaterial(&pMaterials[i]);
		pDevice->SetTexture(0, pTextures[i]);
		pMesh->DrawSubset(i);
	}

	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);		// ���C�g��L��
	pDevice->SetRenderState(D3DRS_AMBIENT, 0x00aaaaaa);	// ������ݒ�
	pDevice->SetRenderState(D3DRS_SPECULARENABLE, TRUE);// ���ʔ��˗L��
	//pDevice->SetRenderState(D3DRS_ZENABLE, TRUE);	// Z�o�b�t�@�[��L��
}


void Skydome_Update(void)
{
	//g_posSkydome = D3DXVECTOR3(GetPlayer_Pos().x, -10000.0f, GetPlayer_Pos().z - 10000);

	//g_sclSkydome += D3DXVECTOR3(100,100,100);
	//g_posSkydome.y += -100.0f;
	//DebugProc_Print((char *)"�y�y%f�z�z", g_sclSkydome.x);
}
