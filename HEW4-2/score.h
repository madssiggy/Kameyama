#pragma once

#include "main.h"

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitScore(void);
void UninitScore(void);
void UpdateScore(void);
void DrawScore(void);

HRESULT MakeVertexScore(LPDIRECT3DDEVICE9 pDevice);
void SetTextureScore(int nIdx, int nNumber);

void Add_Score(int num);
int GetScore();