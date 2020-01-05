//=============================================================================
//
// �X�s�[�h���C��
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
// �}�N����`
//*****************************************************************************
#define	TEXTURE_LINE		"asset/texture/effect000.jpg"	// �ǂݍ��ރe�N�X�`���t�@�C����
#define	LINE_SIZE_X			(50.0f)							// �r���{�[�h�̕�
#define	LINE_SIZE_Y			(50.0f)							// �r���{�[�h�̍���
#define	VALUE_MOVE_LINE		(5.0f)							// �ړ����x

#define	MAX_LINE			(15)							// �r���{�[�h�ő吔
#define	MAX_LINE2			(2000)							// �r���{�[�h�ő吔


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT MakeVertexLine(LPDIRECT3DDEVICE9 pDevice);
void SetVertexLine(int nIdxLine, float fSizeX, float fSizeY);
void SetColorLine(int nIdxLine, D3DXCOLOR col);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static LPDIRECT3DTEXTURE9		g_pD3DTextureLine = NULL;	// �e�N�X�`���ւ̃|�C���^
static LPDIRECT3DVERTEXBUFFER9 g_pD3DVtxBuffLine = NULL;	// ���_�o�b�t�@�C���^�[�t�F�[�X�ւ̃|�C���^

static D3DXMATRIX				g_mtxWorldLine;				// ���[���h�}�g���b�N�X
static D3DXMATRIX				g_mtxWorldLine2;			// ���[���h�}�g���b�N�X

static PARTICLE					g_aLine[MAX_LINE];			// �p�[�e�B�N�����[�N
static PARTICLE					g_aLine2[MAX_LINE2];		// �p�[�e�B�N�����[�N

static D3DXVECTOR3				g_posBase;						// �r���{�[�h�����ʒu
static float					g_fWidthBase = 5.0f;			// ��̕�
static float					g_fHeightBase = 10.0f;			// ��̍���
static float					g_roty = 0.0f;					// �ړ�����
static float					g_spd = 0.0f;					// �ړ��X�s�[�h

int cnt=0;
int cnt2 = 1;
//=============================================================================
// ����������
//=============================================================================
HRESULT SpeedlineManager::InitSpeedline(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetD3DDevice();

	// ���_���̍쐬
	MakeVertexLine(pDevice);

	// �e�N�X�`���̓ǂݍ���
	D3DXCreateTextureFromFile(pDevice,						// �f�o�C�X�ւ̃|�C���^
		TEXTURE_LINE,			// �t�@�C���̖��O
		&g_pD3DTextureLine);	// �ǂݍ��ރ������[

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
// �I������
//=============================================================================
void SpeedlineManager::UninitSpeedline(void)
{
	if (g_pD3DTextureLine != NULL)
	{// �e�N�X�`���̊J��
		g_pD3DTextureLine->Release();
		g_pD3DTextureLine = NULL;
	}

	if (g_pD3DVtxBuffLine != NULL)
	{// ���_�o�b�t�@�̊J��
		g_pD3DVtxBuffLine->Release();
		g_pD3DVtxBuffLine = NULL;
	}
}

//=============================================================================
// �X�V����
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

	DebugProc_Print((char *)"�J�E���g : %d\n",cnt2);


	DebugProc_Print((char *)"�p�[�e�B�N��x : %f\n", g_aLine[0].pos.x);
	DebugProc_Print((char *)"�p�[�e�B�N��y : %f\n", g_aLine[0].pos.y);
	DebugProc_Print((char *)"�p�[�e�B�N��z : %f\n", g_aLine[0].pos.z);
	DebugProc_Print((char *)"�p�[�e�B�N��2x : %f\n", g_aLine2[0].pos.x);
	DebugProc_Print((char *)"�p�[�e�B�N��2y : %f\n", g_aLine2[0].pos.y);
	DebugProc_Print((char *)"�p�[�e�B�N��2z : %f\n", g_aLine2[0].pos.z);


	if (cnt++ % cnt2 == 0)
	{
		// �X�V����
		for (int nCntLine = 0; nCntLine < MAX_LINE; nCntLine++)
		{
			// �g�p��
			if (g_aLine[nCntLine].bUse)
			{
				// �ړ�
				g_aLine[nCntLine].pos.x += g_aLine[nCntLine].move.x;
				g_aLine[nCntLine].pos.z += g_aLine[nCntLine].move.z;
				// �d�͂Ȃǂ��ł���
				g_aLine[nCntLine].pos.y += g_aLine[nCntLine].move.y;

				// ����������
				g_aLine[nCntLine].nLife--;

				// �������Ȃ��Ȃ�����
				if (g_aLine[nCntLine].nLife <= 0)
				{
					// �\������Ȃ��Ȃ�
					g_aLine[nCntLine].bUse = false;
				}
				else
				{
					// �F�̐ݒ�
					SetColorLine(nCntLine, g_aLine[nCntLine].col);

					// �c���𐶐�
					SpeedlineManager::SetSpeedline(g_aLine[nCntLine].pos, D3DXVECTOR3(0.0f, 0.0f, 0.0f), g_aLine[nCntLine].col,
						g_aLine[nCntLine].fSizeX, g_aLine[nCntLine].fSizeY, g_aLine[nCntLine].nLife, 2);
				}
			}
		}
		for (int nCntLine2 = 0; nCntLine2 < MAX_LINE2; nCntLine2++)
		{
			// �g�p��
			if (g_aLine2[nCntLine2].bUse)
			{
				// ����������
				g_aLine2[nCntLine2].nLife -= 4;

				// �������Ȃ��Ȃ�����
				if (g_aLine2[nCntLine2].nLife <= 0)
				{
					// �\������Ȃ��Ȃ�
					g_aLine2[nCntLine2].bUse = false;
				}
				else
				{
					// ���l�ݒ�
					//g_aLine2[nCntLine2].col.a -= 0.01f;
					if (g_aLine2[nCntLine2].col.a < 0.0f)
					{
						g_aLine2[nCntLine2].col.a = 0.0f;
					}

					// �F�̐ݒ�
					SetColorLine(MAX_LINE + nCntLine2, g_aLine2[nCntLine2].col);
				}
			}
		}

		// �p�[�e�B�N������
		if (rand() % 10 == 0)
		{
			D3DXVECTOR3 pos;
			D3DXVECTOR3 move;
			float fAngle;
			int nLife;
			float fSize;

			// �|�W�V����
			pos.x = GetPlayer_Pos().x + 5.0f * cosf(GetPlayer_Rot().y)*(float)(rand() % 10 - 5.0f);
			pos.y = GetPlayer_Pos().y + 40.0f *(float)(rand() % 10) / 10;
			pos.z = GetPlayer_Pos().z + 20.0f * sinf(GetPlayer_Rot().y)*(float)(rand() % 10 - 5.0f);

			// ��������Ă���ǂ��ɔ�Ԃ��H
			move.x = -sinf(GetPlayer_Rot().y);
			move.y = 0.0f;
			move.z = -cosf(GetPlayer_Rot().y);

			// �����_���Ŗ��̐ݒ�
			nLife = 200;

			// �p�[�e�B�N���̑傫��
			fSize = (float)(rand() % 10 + 5.0f);

			// �r���{�[�h�̐ݒ�								�@�@ ��    ��    ��
			SpeedlineManager::SetSpeedline(pos, move, D3DXCOLOR(1.0f, 0.0f, 0.0f, 0.5f), fSize, fSize, nLife, 1);
		}
	}
}


//=============================================================================
// �`�揈��
//=============================================================================
void SpeedlineManager::DrawSpeedline(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetD3DDevice();
	D3DXMATRIX mtxView, mtxScale, mtxTranslate;

	// ���C�e�B���O�𖳌���
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	// ���Z�����ɐݒ�
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);			// ���� = �]����(SRC) + �]����(DEST)
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);		// ���\�[�X�J���[�̎w��
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);			// ���f�X�e�B�l�[�V�����J���[�̎w��
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);				// Z�o�b�t�@�[�̏������݂����Ȃ�

	// �r���[�}�g���b�N�X���擾
	pDevice->GetTransform(D3DTS_VIEW, &mtxView);

	for (int nCntLine = 0; nCntLine < MAX_LINE; nCntLine++)
	{
		if (g_aLine[nCntLine].bUse)
		{
			// ���[���h�}�g���b�N�X�̏�����
			D3DXMatrixIdentity(&g_mtxWorldLine);

			// �t�s������߂�
			D3DXMatrixInverse(&g_mtxWorldLine, NULL, &mtxView);

			// ���s�ړ��̕��������߂�
			g_mtxWorldLine._41 = 0.0f;
			g_mtxWorldLine._42 = 0.0f;
			g_mtxWorldLine._43 = 0.0f;

			// �X�P�[���𔽉f
			D3DXMatrixScaling(&mtxScale, g_aLine[nCntLine].scale.x, g_aLine[nCntLine].scale.y, g_aLine[nCntLine].scale.z);
			D3DXMatrixMultiply(&g_mtxWorldLine, &g_mtxWorldLine, &mtxScale);

			// �ړ��𔽉f
			D3DXMatrixTranslation(&mtxTranslate, g_aLine[nCntLine].pos.x, g_aLine[nCntLine].pos.y, g_aLine[nCntLine].pos.z);
			D3DXMatrixMultiply(&g_mtxWorldLine, &g_mtxWorldLine, &mtxTranslate);

			// ���[���h�}�g���b�N�X�̐ݒ�
			pDevice->SetTransform(D3DTS_WORLD, &g_mtxWorldLine);

			// ���_�o�b�t�@���f�o�C�X�̃f�[�^�X�g���[���Ƀo�C���h
			pDevice->SetStreamSource(0, g_pD3DVtxBuffLine, 0, sizeof(VERTEX_3D));

			// ���_�t�H�[�}�b�g�̐ݒ�
			pDevice->SetFVF(FVF_VERTEX_3D);

			// �e�N�X�`���̐ݒ�
			pDevice->SetTexture(0, g_pD3DTextureLine);

			// �|���S���̕`��
			pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, (nCntLine * 4), NUM_POLYGON);
		}
	}
	for (int nCntLine2 = 0; nCntLine2 < MAX_LINE2; nCntLine2++)
	{
		if (g_aLine2[nCntLine2].bUse)
		{
			// ���[���h�}�g���b�N�X�̏�����
			D3DXMatrixIdentity(&g_mtxWorldLine2);

			// �t�s������߂�
			D3DXMatrixInverse(&g_mtxWorldLine2, NULL, &mtxView);

			// ���s�ړ��̕��������߂�
			g_mtxWorldLine2._41 = 0.0f;
			g_mtxWorldLine2._42 = 0.0f;
			g_mtxWorldLine2._43 = 0.0f;

			// �X�P�[���𔽉f
			D3DXMatrixScaling(&mtxScale, g_aLine2[nCntLine2].scale.x, g_aLine2[nCntLine2].scale.y, g_aLine2[nCntLine2].scale.z);
			D3DXMatrixMultiply(&g_mtxWorldLine2, &g_mtxWorldLine2, &mtxScale);

			// �ړ��𔽉f
			D3DXMatrixTranslation(&mtxTranslate, g_aLine2[nCntLine2].pos.x, g_aLine2[nCntLine2].pos.y, g_aLine2[nCntLine2].pos.z);
			D3DXMatrixMultiply(&g_mtxWorldLine2, &g_mtxWorldLine2, &mtxTranslate);

			// ���[���h�}�g���b�N�X�̐ݒ�
			pDevice->SetTransform(D3DTS_WORLD, &g_mtxWorldLine2);

			// ���_�o�b�t�@���f�o�C�X�̃f�[�^�X�g���[���Ƀo�C���h
			pDevice->SetStreamSource(0, g_pD3DVtxBuffLine, 0, sizeof(VERTEX_3D));

			// ���_�t�H�[�}�b�g�̐ݒ�
			pDevice->SetFVF(FVF_VERTEX_3D);

			// �e�N�X�`���̐ݒ�
			pDevice->SetTexture(0, g_pD3DTextureLine);

			// �|���S���̕`��
			pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, (nCntLine2 * 4), NUM_POLYGON);
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
HRESULT MakeVertexLine(LPDIRECT3DDEVICE9 pDevice)
{
	// �I�u�W�F�N�g�̒��_�o�b�t�@�𐶐�
	if (FAILED(pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * NUM_VERTEX * (MAX_LINE + MAX_LINE2),	// ���_�f�[�^�p�Ɋm�ۂ���o�b�t�@�T�C�Y(�o�C�g�P��)
		D3DUSAGE_WRITEONLY,							// ���_�o�b�t�@�̎g�p�@�@
		FVF_VERTEX_3D,								// �g�p���钸�_�t�H�[�}�b�g
		D3DPOOL_MANAGED,							// ���\�[�X�̃o�b�t�@��ێ����郁�����N���X���w��
		&g_pD3DVtxBuffLine,						// ���_�o�b�t�@�C���^�[�t�F�[�X�ւ̃|�C���^
		NULL)))										// NULL�ɐݒ�
	{
		return E_FAIL;
	}

	{//���_�o�b�t�@�̒��g�𖄂߂�
		VERTEX_3D *pVtx;

		// ���_�f�[�^�͈̔͂����b�N���A���_�o�b�t�@�ւ̃|�C���^���擾
		g_pD3DVtxBuffLine->Lock(0, 0, (void**)&pVtx, 0);

		for (int nCntLine = 0; nCntLine < MAX_LINE + MAX_LINE2; nCntLine++, pVtx += 4)
		{
			// ���_���W�̐ݒ�
			pVtx[0].pos = D3DXVECTOR3(-LINE_SIZE_X / 2, -LINE_SIZE_Y / 2, 0.0f);
			pVtx[1].pos = D3DXVECTOR3(LINE_SIZE_X / 2, -LINE_SIZE_Y / 2, 0.0f);
			pVtx[2].pos = D3DXVECTOR3(-LINE_SIZE_X / 2, LINE_SIZE_Y / 2, 0.0f);
			pVtx[3].pos = D3DXVECTOR3(LINE_SIZE_X / 2, LINE_SIZE_Y / 2, 0.0f);

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
		g_pD3DVtxBuffLine->Unlock();
	}

	return S_OK;
}

//=============================================================================
// ���_���W�̐ݒ�
//=============================================================================
void SetVertexLine(int nIdxLine, float fSizeX, float fSizeY)
{
	{//���_�o�b�t�@�̒��g�𖄂߂�
		VERTEX_3D *pVtx;

		// ���_�f�[�^�͈̔͂����b�N���A���_�o�b�t�@�ւ̃|�C���^���擾
		g_pD3DVtxBuffLine->Lock(0, 0, (void**)&pVtx, 0);

		pVtx += (nIdxLine * 4);

		// ���_���W�̐ݒ�
		pVtx[0].pos = D3DXVECTOR3(-fSizeX / 2, -fSizeY / 2, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(-fSizeX / 2, fSizeY / 2, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(fSizeX / 2, -fSizeY / 2, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(fSizeX / 2, fSizeY / 2, 0.0f);

		// ���_�f�[�^���A�����b�N����
		g_pD3DVtxBuffLine->Unlock();
	}
}

//=============================================================================
// ���_�J���[�̐ݒ�
//=============================================================================
void SetColorLine(int nIdxLine, D3DXCOLOR col)
{
	{//���_�o�b�t�@�̒��g�𖄂߂�
		VERTEX_3D *pVtx;

		// ���_�f�[�^�͈̔͂����b�N���A���_�o�b�t�@�ւ̃|�C���^���擾
		g_pD3DVtxBuffLine->Lock(0, 0, (void**)&pVtx, 0);

		pVtx += (nIdxLine * 4);

		// ���_���W�̐ݒ�
		pVtx[0].col =
			pVtx[1].col =
			pVtx[2].col =
			pVtx[3].col = col;

		// ���_�f�[�^���A�����b�N����
		g_pD3DVtxBuffLine->Unlock();
	}
}

//=============================================================================
// ���_���̍쐬
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

				// ���_���W�̐ݒ�
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

				// ���_���W�̐ݒ�
				SetVertexLine(MAX_LINE + nCntLine2, fSizeX, fSizeY);

				nIdxLine = nCntLine2;

				break;
			}
		}
	}

	return nIdxLine;
}
