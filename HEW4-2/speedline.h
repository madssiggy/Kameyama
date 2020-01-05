//=============================================================================
//
// スピードライン
// Author : 
//
//=============================================================================
#pragma once


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
class SpeedlineManager
{
public:
	HRESULT InitSpeedline(void);
	void UninitSpeedline(void);
	void UpdateSpeedline(void);
	void DrawSpeedline(void);

	int SetSpeedline(D3DXVECTOR3 pos, D3DXVECTOR3 move, D3DXCOLOR col, float fSizeX, float fSizeY, int nLife, int num);
};
