//=============================================================================
//
// �w�i [back_ground.cpp]
//
//=============================================================================
#include "back_ground.h"
#include "myDirect3D.h"
#include "camera.h"
#include "input.h"


//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	MODEL_BUILDING			"asset/MODEL/TheMountain.x"	// �ǂݍ��ރ��f����
#define TEXTURE_FILENAME		"asset/texture/ground_grass.jpg"

//*****************************************************************************
// �N���X��`
//*****************************************************************************

class OBJECT
{
public:

	D3DXMATRIX			mtxWorldModel;	// ���[���h�}�g���b�N�X
	D3DXVECTOR3			posModel;		// ���݂̈ʒu
	D3DXVECTOR3			rotModel;		// ���݂̌���
	D3DXVECTOR3			rotDestModel;	// �ړI�̌���
	D3DXVECTOR3			sclModel;		// �傫��		

	//int					idxShadow;

	OBJECT();
	~OBJECT();

private:

};

OBJECT::OBJECT()
{
	// �ʒu�E�����E�ړ��ʂ̏����ݒ�
	posModel = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	rotModel = D3DXVECTOR3(0.0f, D3DX_PI * 0 / 180, 0.0f);
	rotDestModel = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	sclModel = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
}

OBJECT::~OBJECT()
{
}

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************

static LPDIRECT3DTEXTURE9	g_pTextureModel;	// �e�N�X�`���ւ̃|�C���^
static LPD3DXMESH			g_pMeshModel;		// ���b�V�����ւ̃|�C���^
static LPD3DXBUFFER			g_pBuffMatModel;	// �}�e���A�����ւ̃|�C���^
static DWORD				g_nNumMatModel;		// �}�e���A�����̑���

OBJECT g_Object[MAX_BG];

float sclUpdate = 0.01f;

//=============================================================================
// ����������
//=============================================================================
HRESULT Back_Initialize()
{
	LPDIRECT3DDEVICE9 pDevice = GetD3DDevice();

	g_pTextureModel = NULL;
	g_pMeshModel = NULL;
	g_pBuffMatModel = NULL;

	// �e�N�X�`���̓ǂݍ���
	if (FAILED(D3DXCreateTextureFromFile(pDevice,// �f�o�C�X�ւ̃|�C���^
		TEXTURE_FILENAME,	// �t�@�C���̖��O
		&g_pTextureModel))) // �ǂݍ��ރ������[
	{
		MessageBox(NULL, "�e�N�X�`���̓ǂݍ��݂Ɏ��s���܂���", NULL, MB_OK);
	}

	// X�t�@�C���̓ǂݍ���
	if (FAILED(D3DXLoadMeshFromX(MODEL_BUILDING,
		D3DXMESH_SYSTEMMEM,
		pDevice,
		NULL,
		&g_pBuffMatModel,
		NULL,
		&g_nNumMatModel,
		&g_pMeshModel)))
	{
		MessageBox(NULL, "X�t�@�C���̓ǂݍ��݂Ɏ��s���܂���", NULL, MB_OK);
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
// �I������
//=============================================================================
void Back_Finalize(void)
{
	//�e�̉������

	if (g_pTextureModel != NULL)
	{// �e�N�X�`���̊J��
		g_pTextureModel->Release();
		g_pTextureModel = NULL;
	}

	if (g_pMeshModel != NULL)
	{// ���b�V���̊J��
		g_pMeshModel->Release();
		g_pMeshModel = NULL;
	}

	if (g_pBuffMatModel != NULL)
	{// �}�e���A���̊J��
		g_pBuffMatModel->Release();
		g_pBuffMatModel = NULL;
	}
}


//=============================================================================
// �`�揈��
//=============================================================================
void Back_Draw(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetD3DDevice();
	D3DXMATRIX mtxScl, mtxRot, mtxTranslate;
	D3DXMATERIAL *pD3DXMat;
	D3DMATERIAL9 matDef;

	for (int i = 0; i < MAX_BG; i++)
	{
		// ���[���h�}�g���b�N�X�̏�����
		D3DXMatrixIdentity(&g_Object[i].mtxWorldModel);

		// �X�P�[�����]
		D3DXMatrixScaling(&mtxScl, g_Object[i].sclModel.x, g_Object[i].sclModel.y, g_Object[i].sclModel.z);
		D3DXMatrixMultiply(&g_Object[i].mtxWorldModel, &g_Object[i].mtxWorldModel, &mtxScl);

		// ��]�𔽉f
		D3DXMatrixRotationYawPitchRoll(&mtxRot, g_Object[i].rotModel.y, g_Object[i].rotModel.x, g_Object[i].rotModel.z);
		D3DXMatrixMultiply(&g_Object[i].mtxWorldModel, &g_Object[i].mtxWorldModel, &mtxRot);

		// �ړ��𔽉f
		D3DXMatrixTranslation(&mtxTranslate, g_Object[i].posModel.x, g_Object[i].posModel.y, g_Object[i].posModel.z);
		D3DXMatrixMultiply(&g_Object[i].mtxWorldModel, &g_Object[i].mtxWorldModel, &mtxTranslate);

		// ���[���h�}�g���b�N�X�̐ݒ�
		pDevice->SetTransform(D3DTS_WORLD, &g_Object[i].mtxWorldModel);

		// ���݂̃}�e���A�����擾
		pDevice->GetMaterial(&matDef);

		// �}�e���A�����ɑ΂���|�C���^���擾

		pD3DXMat = (D3DXMATERIAL*)g_pBuffMatModel->GetBufferPointer();


		for (int nCntMat = 0; nCntMat < (int)g_nNumMatModel; nCntMat++)
		{
			// �}�e���A���̐ݒ�
			pDevice->SetMaterial(&pD3DXMat[nCntMat].MatD3D);

			// �e�N�X�`���̐ݒ�
			pDevice->SetTexture(0, g_pTextureModel);

			// �`��
			g_pMeshModel->DrawSubset(nCntMat);
		}

		// �}�e���A�����f�t�H���g�ɖ߂�
		pDevice->SetMaterial(&matDef);
	}

	//pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);		// ���C�g��L��
	//pDevice->SetRenderState(D3DRS_AMBIENT, 0x00aaaaaa);	// ������ݒ�
	//pDevice->SetRenderState(D3DRS_SPECULARENABLE, TRUE);// ���ʔ��˗L��

}

void Back_Update(void) {

	//for (int i = 0; i < 1; i++)
	//{
	//	g_Object[i].sclModel.x += sclUpdate;
	//	g_Object[i].sclModel.y += sclUpdate;
	//	g_Object[i].sclModel.z += sclUpdate;
	//}
}