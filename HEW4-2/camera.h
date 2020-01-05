#pragma once

#include "main.h"


//*****************************************************************************
// カメラの構造体
//*****************************************************************************
typedef struct
{
	D3DXVECTOR3 posV;			// 視点
	D3DXVECTOR3 posR;			// 注視点
	D3DXVECTOR3 vecU;			// 上方向ベクトル
	D3DXMATRIX mtxProjection;	// プロジェクションマトリックス
	D3DXMATRIX mtxView;			// ビューマトリックス
} CAMERA;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void Camera_Initialize(void);
void Camera_Finalize(void);
void Camera_Update(void);

void Camera_SetCamera(void);

CAMERA *GetCamera(void);