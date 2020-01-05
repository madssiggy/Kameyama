//=============================================================================
//
// �p�[�e�B�N������ [particle.cpp]
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
// �}�N����`
//*****************************************************************************
#define	TEXTURE_PARTICLE	"asset/texture/effect000.jpg"	// �ǂݍ��ރe�N�X�`���t�@�C����
#define	PARTICLE_SIZE_X		(50.0f)							// �r���{�[�h�̕�
#define	PARTICLE_SIZE_Y		(50.0f)							// �r���{�[�h�̍���
#define	VALUE_MOVE_PARTICLE	(5.0f)							// �ړ����x

#define	MAX_PARTICLE		(40)							// �r���{�[�h�ő吔
#define	MAX_PARTICLE2		(720)							// �r���{�[�h�ő吔

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT MakeVertexParticle(LPDIRECT3DDEVICE9 pDevice);
void SetVertexParticle(int nIdxParticle, float fSizeX, float fSizeY);
void SetColorParticle(int nIdxParticle, D3DXCOLOR col);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static LPDIRECT3DTEXTURE9		g_pD3DTextureParticle = NULL;	// �e�N�X�`���ւ̃|�C���^
static LPDIRECT3DVERTEXBUFFER9 g_pD3DVtxBuffParticle = NULL;	// ���_�o�b�t�@�C���^�[�t�F�[�X�ւ̃|�C���^

static D3DXMATRIX				g_mtxWorldParticle;				// ���[���h�}�g���b�N�X
static D3DXMATRIX				g_mtxWorldParticle2;			// ���[���h�}�g���b�N�X

static PARTICLE					g_aParticle[MAX_PARTICLE];		// �p�[�e�B�N�����[�N
static PARTICLE					g_aParticle2[MAX_PARTICLE2];	// �p�[�e�B�N�����[�N

static D3DXVECTOR3				g_posBase;						// �r���{�[�h�����ʒu
static float					g_fWidthBase = 5.0f;			// ��̕�
static float					g_fHeightBase = 10.0f;			// ��̍���
static bool						g_bPause = false;				// �|�[�YON/OFF
static float					g_roty = 0.0f;					// �ړ�����
static float					g_spd = 0.0f;					// �ړ��X�s�[�h

//=============================================================================
// ����������
//=============================================================================
HRESULT ParticleManager::InitParticle(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetD3DDevice();

	// ���_���̍쐬
	MakeVertexParticle(pDevice);

	// �e�N�X�`���̓ǂݍ���
	D3DXCreateTextureFromFile(pDevice,						// �f�o�C�X�ւ̃|�C���^
								TEXTURE_PARTICLE,			// �t�@�C���̖��O
								&g_pD3DTextureParticle);	// �ǂݍ��ރ������[

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
// �I������
//=============================================================================
void ParticleManager::UninitParticle(void)
{
	if(g_pD3DTextureParticle != NULL)
	{// �e�N�X�`���̊J��
		g_pD3DTextureParticle->Release();
		g_pD3DTextureParticle = NULL;
	}

	if(g_pD3DVtxBuffParticle != NULL)
	{// ���_�o�b�t�@�̊J��
		g_pD3DVtxBuffParticle->Release();
		g_pD3DVtxBuffParticle = NULL;
	}
}

//=============================================================================
// �X�V����
//=============================================================================
void ParticleManager::UpdateParticle(void)
{
	// �X�V����
	if (Get_driftcnt() >= 20)
	{
		for (int nCntParticle = 0; nCntParticle < MAX_PARTICLE; nCntParticle++)
		{
			// �g�p��
			if (g_aParticle[nCntParticle].bUse)
			{
				// �ړ�
				g_aParticle[nCntParticle].pos.x += g_aParticle[nCntParticle].move.x+ Get_driftmove().x;
				g_aParticle[nCntParticle].pos.z += g_aParticle[nCntParticle].move.z+ Get_driftmove().z;
				// �d�͂Ȃǂ��ł���
				g_aParticle[nCntParticle].pos.y += g_aParticle[nCntParticle].move.y + Get_driftmove().y;

				g_aParticle[nCntParticle].move.y -= 0.05f*(float)((rand() % 10+5) / 10);

				// ����������
				g_aParticle[nCntParticle].nLife--;

				// �������Ȃ��Ȃ�����
				if (g_aParticle[nCntParticle].nLife <= 0)
				{
					// �\������Ȃ��Ȃ�
					g_aParticle[nCntParticle].bUse = false;
					// ReleaseShadow(g_aParticle[nCntParticle].nIdxShadow);
					// �e�̐������炷
					g_aParticle[nCntParticle].nIdxShadow = -1;
				}
				else
				{
					// ��������������
					if (g_aParticle[nCntParticle].nLife <= 8)
					{
						g_aParticle[nCntParticle].col = D3DXCOLOR(0.0f, 0.7f, 0.7f, g_aParticle[nCntParticle].col.a);
					}

					// �F�̐ݒ�
					SetColorParticle(nCntParticle, g_aParticle[nCntParticle].col);
				}
				// �c���𐶐�
				ParticleManager::SetParticle(g_aParticle[nCntParticle].pos, D3DXVECTOR3(0.0f, 0.0f, 0.0f), g_aParticle[nCntParticle].col,
					g_aParticle[nCntParticle].fSizeX*1.4f, g_aParticle[nCntParticle].fSizeY*1.4f, g_aParticle[nCntParticle].nLife, 2);
			}
		}
		for (int nCntParticle2 = 0; nCntParticle2 < MAX_PARTICLE2; nCntParticle2++)
		{
			// �g�p��
			if (g_aParticle2[nCntParticle2].bUse)
			{
				// �ړ�
				g_aParticle2[nCntParticle2].pos.x += Get_driftmove().x;
				g_aParticle2[nCntParticle2].pos.z += Get_driftmove().z;
				g_aParticle2[nCntParticle2].pos.y += Get_driftmove().y;

				// ����������
				g_aParticle2[nCntParticle2].nLife -= 2;

				// �������Ȃ��Ȃ�����
				if (g_aParticle2[nCntParticle2].nLife <= 0)
				{
					// �\������Ȃ��Ȃ�
					g_aParticle2[nCntParticle2].bUse = false;
					// ReleaseShadow(g_aParticle2[nCntParticle2].nIdxShadow);
					// �e�̐������炷
					g_aParticle2[nCntParticle2].nIdxShadow = -1;
				}
				else
				{
					// ���l�ݒ�
					g_aParticle2[nCntParticle2].col.a -= 0.05f;
					// �T�C�Y�ݒ�
					g_aParticle2[nCntParticle2].fSizeX -= 2.0f;
					g_aParticle2[nCntParticle2].fSizeY -= 2.0f;
					if (g_aParticle2[nCntParticle2].col.a < 0.0f)
					{
						g_aParticle2[nCntParticle2].col.a = 0.0f;
					}

					// �F�̐ݒ�
					SetColorParticle(nCntParticle2, g_aParticle2[nCntParticle2].col);
				}
			}
		}

		// �p�[�e�B�N������
		{
			D3DXVECTOR3 pos;
			D3DXVECTOR3 move;
			float fAngle;
			int nLife;
			float fSize;

			// �|�W�V����
			pos.x = GetPlayer_Pos().x - 25.0f * sinf(GetPlayer_Rot().y);
			pos.y = GetPlayer_Pos().y-10.0f;
			pos.z = GetPlayer_Pos().z - 25.0f * cosf(GetPlayer_Rot().y);

			// ��������Ă���ǂ��ɔ�Ԃ�
			move.x = (1.0f - sinf(GetPlayer_Rot().y))* (float)((rand() % 10 - 5) / 10.0f);
			move.y = (rand() % 30 + 25) / 75.0f;
			move.z = (1.0f - cosf(GetPlayer_Rot().y))* (float)((rand() % 10 - 5) / 10.0f);

			// �����_���Ŗ��̐ݒ�
			nLife = rand() % 20 + 25;

			// �p�[�e�B�N���̑傫��
			fSize = (float)(rand() % 8 + 1.0f);

			// �r���{�[�h�̐ݒ�									    				�@�@�@��	��	�@��	
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
// �`�揈��
//=============================================================================
void ParticleManager::DrawParticle(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetD3DDevice();
	D3DXMATRIX mtxView,mtxScale,mtxTranslate;

	// ���C�e�B���O�𖳌���
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	// ���Z�����ɐݒ�
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);			// ���� = �]����(SRC) + �]����(DEST)
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);		// ���\�[�X�J���[�̎w��
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);			// ���f�X�e�B�l�[�V�����J���[�̎w��
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);				// Z�o�b�t�@�[�̏������݂����Ȃ�

	// �r���[�}�g���b�N�X���擾
	pDevice->GetTransform(D3DTS_VIEW, &mtxView);

	for(int nCntParticle = 0; nCntParticle < MAX_PARTICLE; nCntParticle++)
	{
		if(g_aParticle[nCntParticle].bUse)
		{
			// ���[���h�}�g���b�N�X�̏�����
			D3DXMatrixIdentity(&g_mtxWorldParticle);

			// �t�s������߂�
			D3DXMatrixInverse(&g_mtxWorldParticle, NULL, &mtxView);

			// ���s�ړ��̕��������߂�
			g_mtxWorldParticle._41 = 0.0f;
			g_mtxWorldParticle._42 = 0.0f;
			g_mtxWorldParticle._43 = 0.0f;

			// �X�P�[���𔽉f
			D3DXMatrixScaling(&mtxScale, g_aParticle[nCntParticle].scale.x, g_aParticle[nCntParticle].scale.y, g_aParticle[nCntParticle].scale.z);
			D3DXMatrixMultiply(&g_mtxWorldParticle, &g_mtxWorldParticle, &mtxScale);

			// �ړ��𔽉f
			D3DXMatrixTranslation(&mtxTranslate, g_aParticle[nCntParticle].pos.x, g_aParticle[nCntParticle].pos.y, g_aParticle[nCntParticle].pos.z);
			D3DXMatrixMultiply(&g_mtxWorldParticle, &g_mtxWorldParticle, &mtxTranslate);

			// ���[���h�}�g���b�N�X�̐ݒ�
			pDevice->SetTransform(D3DTS_WORLD, &g_mtxWorldParticle);

			// ���_�o�b�t�@���f�o�C�X�̃f�[�^�X�g���[���Ƀo�C���h
			pDevice->SetStreamSource(0, g_pD3DVtxBuffParticle, 0, sizeof(VERTEX_3D));

			// ���_�t�H�[�}�b�g�̐ݒ�
			pDevice->SetFVF(FVF_VERTEX_3D);

			// �e�N�X�`���̐ݒ�
			pDevice->SetTexture(0, g_pD3DTextureParticle);

			// �|���S���̕`��
			pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, (nCntParticle * 4), NUM_POLYGON);
		}
	}
	for (int nCntParticle2 = 0; nCntParticle2 < MAX_PARTICLE2; nCntParticle2++)
	{
		if (g_aParticle2[nCntParticle2].bUse)
		{
			// ���[���h�}�g���b�N�X�̏�����
			D3DXMatrixIdentity(&g_mtxWorldParticle2);

			// �t�s������߂�
			D3DXMatrixInverse(&g_mtxWorldParticle2, NULL, &mtxView);

			// ���s�ړ��̕��������߂�
			g_mtxWorldParticle2._41 = 0.0f;
			g_mtxWorldParticle2._42 = 0.0f;
			g_mtxWorldParticle2._43 = 0.0f;

			// �X�P�[���𔽉f
			D3DXMatrixScaling(&mtxScale, g_aParticle2[nCntParticle2].scale.x, g_aParticle2[nCntParticle2].scale.y, g_aParticle2[nCntParticle2].scale.z);
			D3DXMatrixMultiply(&g_mtxWorldParticle2, &g_mtxWorldParticle2, &mtxScale);

			// �ړ��𔽉f
			D3DXMatrixTranslation(&mtxTranslate, g_aParticle2[nCntParticle2].pos.x, g_aParticle2[nCntParticle2].pos.y, g_aParticle2[nCntParticle2].pos.z);
			D3DXMatrixMultiply(&g_mtxWorldParticle2, &g_mtxWorldParticle2, &mtxTranslate);

			// ���[���h�}�g���b�N�X�̐ݒ�
			pDevice->SetTransform(D3DTS_WORLD, &g_mtxWorldParticle2);

			// ���_�o�b�t�@���f�o�C�X�̃f�[�^�X�g���[���Ƀo�C���h
			pDevice->SetStreamSource(0, g_pD3DVtxBuffParticle, 0, sizeof(VERTEX_3D));

			// ���_�t�H�[�}�b�g�̐ݒ�
			pDevice->SetFVF(FVF_VERTEX_3D);

			// �e�N�X�`���̐ݒ�
			pDevice->SetTexture(0, g_pD3DTextureParticle);

			// �|���S���̕`��
			pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, (nCntParticle2 * 4), NUM_POLYGON);
		}
	}


	// ���C�e�B���O��L����
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

	// �ʏ�u�����h�ɖ߂�
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);			// ���� = �]����(SRC) + �]����(DEST)
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);		// ���\�[�X�J���[�̎w��
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);	// ���f�X�e�B�l�[�V�����J���[�̎w��
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);				// Z�o�b�t�@�[�̏������݂�����
}

//=============================================================================
// ���_���̍쐬
//=============================================================================
HRESULT MakeVertexParticle(LPDIRECT3DDEVICE9 pDevice)
{
	// �I�u�W�F�N�g�̒��_�o�b�t�@�𐶐�
    if( FAILED( pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * NUM_VERTEX * (MAX_PARTICLE+MAX_PARTICLE2),	// ���_�f�[�^�p�Ɋm�ۂ���o�b�t�@�T�C�Y(�o�C�g�P��)
												D3DUSAGE_WRITEONLY,							// ���_�o�b�t�@�̎g�p�@�@
												FVF_VERTEX_3D,								// �g�p���钸�_�t�H�[�}�b�g
												D3DPOOL_MANAGED,							// ���\�[�X�̃o�b�t�@��ێ����郁�����N���X���w��
												&g_pD3DVtxBuffParticle,						// ���_�o�b�t�@�C���^�[�t�F�[�X�ւ̃|�C���^
												NULL)))										// NULL�ɐݒ�
	{
        return E_FAIL;
	}

	{//���_�o�b�t�@�̒��g�𖄂߂�
		VERTEX_3D *pVtx;

		// ���_�f�[�^�͈̔͂����b�N���A���_�o�b�t�@�ւ̃|�C���^���擾
		g_pD3DVtxBuffParticle->Lock(0, 0, (void**)&pVtx, 0);

		for(int nCntParticle = 0; nCntParticle < MAX_PARTICLE+MAX_PARTICLE2; nCntParticle++, pVtx += 4)
		{
			// ���_���W�̐ݒ�
			pVtx[0].pos = D3DXVECTOR3(-PARTICLE_SIZE_X / 2, -PARTICLE_SIZE_Y / 2, 0.0f);
			pVtx[1].pos = D3DXVECTOR3(PARTICLE_SIZE_X / 2, -PARTICLE_SIZE_Y / 2, 0.0f);
			pVtx[2].pos = D3DXVECTOR3(-PARTICLE_SIZE_X / 2, PARTICLE_SIZE_Y / 2, 0.0f);
			pVtx[3].pos = D3DXVECTOR3(PARTICLE_SIZE_X / 2, PARTICLE_SIZE_Y / 2, 0.0f);

			// �@���̐ݒ�
			pVtx[0].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
			pVtx[1].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
			pVtx[2].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
			pVtx[3].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

			// ���ˌ��̐ݒ�
			pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

			// �e�N�X�`�����W�̐ݒ�
			pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
			pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
			pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
			pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);
		}

		// ���_�f�[�^���A�����b�N����
		g_pD3DVtxBuffParticle->Unlock();
	}

	return S_OK;
}

//=============================================================================
// ���_���W�̐ݒ�
//=============================================================================
void SetVertexParticle(int nIdxParticle, float fSizeX, float fSizeY)
{
	{//���_�o�b�t�@�̒��g�𖄂߂�
		VERTEX_3D *pVtx;

		// ���_�f�[�^�͈̔͂����b�N���A���_�o�b�t�@�ւ̃|�C���^���擾
		g_pD3DVtxBuffParticle->Lock(0, 0, (void**)&pVtx, 0);

		pVtx += (nIdxParticle * 4);

		// ���_���W�̐ݒ�
		pVtx[0].pos = D3DXVECTOR3(-fSizeX / 2, -fSizeY / 2, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(-fSizeX / 2, fSizeY / 2, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(fSizeX / 2, -fSizeY / 2, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(fSizeX / 2, fSizeY / 2, 0.0f);

		// ���_�f�[�^���A�����b�N����
		g_pD3DVtxBuffParticle->Unlock();
	}
}

//=============================================================================
// ���_�J���[�̐ݒ�
//=============================================================================
void SetColorParticle(int nIdxParticle, D3DXCOLOR col)
{
	{//���_�o�b�t�@�̒��g�𖄂߂�
		VERTEX_3D *pVtx;

		// ���_�f�[�^�͈̔͂����b�N���A���_�o�b�t�@�ւ̃|�C���^���擾
		g_pD3DVtxBuffParticle->Lock(0, 0, (void**)&pVtx, 0);

		pVtx += (nIdxParticle * 4);

		// ���_���W�̐ݒ�
		pVtx[0].col =
		pVtx[1].col =
		pVtx[2].col =
		pVtx[3].col = col;

		// ���_�f�[�^���A�����b�N����
		g_pD3DVtxBuffParticle->Unlock();
	}
}

//=============================================================================
// ���_���̍쐬
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

				// ���_���W�̐ݒ�
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

				// ���_���W�̐ݒ�
				SetVertexParticle(nCntParticle2, fSizeX, fSizeY);

				nIdxParticle = nCntParticle2;

				break;
			}
		}
	}

	return nIdxParticle;
}
