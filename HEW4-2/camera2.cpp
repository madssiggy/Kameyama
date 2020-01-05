
#include "camera2.h"
#include "myDirect3D.h"
#include "input.h"
#include "debugproc.h" 

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	CAM_POS_V_X		(0.0f)					// カメラの視点初期位置(X座標)
#define	CAM_POS_V_Y		(100.0f)				// カメラの視点初期位置(Y座標)
#define	CAM_POS_V_Z		(-200.0f)				// カメラの視点初期位置(Z座標)
#define	CAM_POS_R_X		(0.0f)					// カメラの注視点初期位置(X座標)
#define	CAM_POS_R_Y		(0.0f)					// カメラの注視点初期位置(Y座標)
#define	CAM_POS_R_Z		(0.0f)					// カメラの注視点初期位置(Z座標)
#define	VIEW_ANGLE		(D3DXToRadian(45.0f))							// ビュー平面の視野角
#define	VIEW_ASPECT		((float)SCREEN_WIDTH / (float)SCREEN_HEIGHT)	// ビュー平面のアスペクト比
#define	VIEW_NEAR_Z		(10.0f)											// ビュー平面のNearZ値
#define	VIEW_FAR_Z		(1000.0f)										// ビュー平面のFarZ値

#define VALUE_MOVE_Camera2 1.0
#define	VALUE_ROTATE_Camera2	(D3DX_PI * 0.02f)				// 回転量

//*****************************************************************************
// カメラの構造体
//*****************************************************************************
typedef struct
{
	D3DXVECTOR3 posV;			// 視点
	D3DXVECTOR3 posR;			// 注視点
	D3DXVECTOR3 vecU;			// 上方向ベクトル
	D3DXVECTOR3 rot;
	D3DXMATRIX mtxProjection;	// プロジェクションマトリックス
	D3DXMATRIX mtxView;			// ビューマトリックス
	float fDstance;

} Camera2;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
Camera2 g_Camera2;		// カメラ情報

//*****************************************************************************
// グローバル変数
//*****************************************************************************
float fVecX, fVecZ;

//=============================================================================
// カメラの初期化処理
//=============================================================================
void Camera2_Initialize(void)
{
	g_Camera2.posV = D3DXVECTOR3(CAM_POS_V_X, CAM_POS_V_Y, CAM_POS_V_Z);
	g_Camera2.posR = D3DXVECTOR3(CAM_POS_R_X, CAM_POS_R_X, CAM_POS_R_X);
	g_Camera2.vecU = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	g_Camera2.fDstance = 20.0f;

	fVecX = g_Camera2.posV.x - g_Camera2.posR.x;
	fVecZ = g_Camera2.posV.z - g_Camera2.posR.z;
	g_Camera2.fDstance = sqrtf(fVecX * fVecX + fVecZ * fVecZ);
}

//=============================================================================
// カメラの終了処理
//=============================================================================
void Camera2_Finalize(void)
{
}

//=============================================================================
// カメラの更新処理
//=============================================================================
void Camera2_Update(void)
{

	if (Keyboard_IsPress(DIK_A))
	{
		if (Keyboard_IsPress(DIK_W))
		{// 左奥移動
			g_Camera2.posV.x += sinf(-D3DX_PI * 0.75f - g_Camera2.rot.y) * VALUE_MOVE_Camera2;
			g_Camera2.posV.z -= cosf(-D3DX_PI * 0.75f - g_Camera2.rot.y) * VALUE_MOVE_Camera2;
		}
		else if (Keyboard_IsPress(DIK_S))
		{// 左手前移動
			g_Camera2.posV.x += sinf(-D3DX_PI * 0.25f - g_Camera2.rot.y) * VALUE_MOVE_Camera2;
			g_Camera2.posV.z -= cosf(-D3DX_PI * 0.25f - g_Camera2.rot.y) * VALUE_MOVE_Camera2;
		}
		else
		{// 左移動
			g_Camera2.posV.x += sinf(-D3DX_PI * 0.50f - g_Camera2.rot.y) * VALUE_MOVE_Camera2;
			g_Camera2.posV.z -= cosf(-D3DX_PI * 0.50f - g_Camera2.rot.y) * VALUE_MOVE_Camera2;
		}

		g_Camera2.posR.z = g_Camera2.posV.z + cosf(g_Camera2.rot.y) * g_Camera2.fDstance;
		g_Camera2.posR.x = g_Camera2.posV.x + sinf(g_Camera2.rot.y) * g_Camera2.fDstance;
	}
	else if (Keyboard_IsPress(DIK_D))
	{
		if (Keyboard_IsPress(DIK_W))
		{// 左奥移動
			g_Camera2.posV.x += sinf(D3DX_PI * 0.75f - g_Camera2.rot.y) * VALUE_MOVE_Camera2;
			g_Camera2.posV.z -= cosf(D3DX_PI * 0.75f - g_Camera2.rot.y) * VALUE_MOVE_Camera2;
		}
		else if (Keyboard_IsPress(DIK_S))
		{// 左手前移動
			g_Camera2.posV.x += sinf(D3DX_PI * 0.25f - g_Camera2.rot.y) * VALUE_MOVE_Camera2;
			g_Camera2.posV.z -= cosf(D3DX_PI * 0.25f - g_Camera2.rot.y) * VALUE_MOVE_Camera2;
		}
		else
		{// 左移動
			g_Camera2.posV.x += sinf(D3DX_PI * 0.50f - g_Camera2.rot.y) * VALUE_MOVE_Camera2;
			g_Camera2.posV.z -= cosf(D3DX_PI * 0.50f - g_Camera2.rot.y) * VALUE_MOVE_Camera2;
		}

		g_Camera2.posR.x = g_Camera2.posV.x + sinf(g_Camera2.rot.y) * g_Camera2.fDstance;
		g_Camera2.posR.z = g_Camera2.posV.z + cosf(g_Camera2.rot.y) * g_Camera2.fDstance;
	}
	else if (Keyboard_IsPress(DIK_W))
	{
	g_Camera2.posV.x += sinf(D3DX_PI * 1.0f - g_Camera2.rot.y) * VALUE_MOVE_Camera2;
	g_Camera2.posV.z -= cosf(D3DX_PI * 1.0f - g_Camera2.rot.y) * VALUE_MOVE_Camera2;

	g_Camera2.posR.x = g_Camera2.posV.x + sinf(g_Camera2.rot.y) * g_Camera2.fDstance;
	g_Camera2.posR.z = g_Camera2.posV.z + cosf(g_Camera2.rot.y) * g_Camera2.fDstance;
	}

	else if (Keyboard_IsPress(DIK_S))
	{
	g_Camera2.posV.x += sinf(D3DX_PI * 0.0f - g_Camera2.rot.y) * VALUE_MOVE_Camera2;
	g_Camera2.posV.z -= cosf(D3DX_PI * 0.0f - g_Camera2.rot.y) * VALUE_MOVE_Camera2;

	g_Camera2.posR.x = g_Camera2.posV.x + sinf(g_Camera2.rot.y) * g_Camera2.fDstance;
	g_Camera2.posR.z = g_Camera2.posV.z + cosf(g_Camera2.rot.y) * g_Camera2.fDstance;

	}

	if (Keyboard_IsPress(DIK_Z))
	{// Y軸左回転
	g_Camera2.rot.y += VALUE_MOVE_Camera2/10;
	if (g_Camera2.rot.y > D3DX_PI)
	{
	g_Camera2.rot.y -= D3DX_PI * 2.0f;
	}

	g_Camera2.posV.x = g_Camera2.posR.x - sinf(g_Camera2.rot.y) * g_Camera2.fDstance;
	g_Camera2.posV.z = g_Camera2.posR.z - cosf(g_Camera2.rot.y) * g_Camera2.fDstance;

	}

	else if (Keyboard_IsPress(DIK_C))
	{// Y軸左回転
	g_Camera2.rot.y -= VALUE_MOVE_Camera2/10;
	if (g_Camera2.rot.y < D3DX_PI)
	{
	g_Camera2.rot.y += D3DX_PI * 2.0f;
	}

	g_Camera2.posV.x = g_Camera2.posR.x - sinf(g_Camera2.rot.y) * g_Camera2.fDstance;
	g_Camera2.posV.z = g_Camera2.posR.z - cosf(g_Camera2.rot.y) * g_Camera2.fDstance;

	}

	if (Keyboard_IsPress(DIK_Y))
	{
	g_Camera2.posV.y += VALUE_MOVE_Camera2;
	}

	if (Keyboard_IsPress(DIK_N))
	{
	g_Camera2.posV.y -= VALUE_MOVE_Camera2;
	}

	if (Keyboard_IsPress(DIK_Q))
	{// X軸右回転
	g_Camera2.rot.y -= VALUE_MOVE_Camera2/10;

	if (g_Camera2.rot.y < -D3DX_PI)
	{
	g_Camera2.rot.y += D3DX_PI * 2.0f;
	}

	g_Camera2.posR.x = g_Camera2.posV.x + sinf(g_Camera2.rot.y) * g_Camera2.fDstance;
	g_Camera2.posR.z = g_Camera2.posV.z + cosf(g_Camera2.rot.y) * g_Camera2.fDstance;
	}

	if (Keyboard_IsPress(DIK_E))
	{// X軸右回転
	g_Camera2.rot.y += VALUE_MOVE_Camera2/10;

	if (g_Camera2.rot.y > D3DX_PI)
	{
	g_Camera2.rot.y -= D3DX_PI * 2.0f;
	}

	g_Camera2.posR.x = g_Camera2.posV.x + sinf(g_Camera2.rot.y) * g_Camera2.fDstance;
	g_Camera2.posR.z = g_Camera2.posV.z + cosf(g_Camera2.rot.y) * g_Camera2.fDstance;
	}

	if (Keyboard_IsPress(DIK_T))
	{// X軸右回転
	g_Camera2.posR.y += VALUE_MOVE_Camera2;
	}

	if (Keyboard_IsPress(DIK_B))
	{// X軸右回転
	g_Camera2.posR.y -= VALUE_MOVE_Camera2;
	}

	if (Keyboard_IsPress(DIK_U))
	{
		g_Camera2.posV.y -= VALUE_MOVE_Camera2;
	
		g_Camera2.posV.z += VALUE_MOVE_Camera2;

		g_Camera2.posR.z -= VALUE_MOVE_Camera2;
	}

	if (Keyboard_IsPress(DIK_M))
	{
		g_Camera2.posV.y += VALUE_MOVE_Camera2;
	
		g_Camera2.posV.z -= VALUE_MOVE_Camera2;

		g_Camera2.posR.z += VALUE_MOVE_Camera2;
	}

	if (Keyboard_IsPress(DIK_SPACE))
	{
	g_Camera2.posV = D3DXVECTOR3(CAM_POS_V_X, CAM_POS_V_Y, CAM_POS_V_Z);
	g_Camera2.posR = D3DXVECTOR3(CAM_POS_R_X, CAM_POS_R_Y, CAM_POS_R_Z);
	g_Camera2.rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	fVecX = g_Camera2.posV.x - g_Camera2.posR.x;
	fVecZ = g_Camera2.posV.z - g_Camera2.posR.z;
	g_Camera2.fDstance = sqrtf(fVecX * fVecX + fVecZ * fVecZ);

	}



	/*
	PrintDebugProc((char *)"*** ３Ｄポリゴン操作 ***\n");
	PrintDebugProc((char *)"位置 [%f : %f : %f]\n", g_posField.x, g_posField.y, g_posField.z);
	PrintDebugProc((char *)"前移動 : Ｗ\n");
	PrintDebugProc((char *)"後移動 : Ｓ\n");
	*/

	DebugProc_Print((char *)"*** カメラ操作 ***\n");
	DebugProc_Print((char *)"位置R [%f : %f : %f]\n", g_Camera2.posR.x, g_Camera2.posR.y, g_Camera2.posR.z);
	DebugProc_Print((char *)"位置V [%f : %f : %f]\n", g_Camera2.posV.x, g_Camera2.posV.y, g_Camera2.posV.z);

}

//=============================================================================
// カメラの設定処理
//=============================================================================
void Camera2_SetCamera(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetD3DDevice();

	// プロジェクションマトリックスの初期化
	D3DXMatrixIdentity(&g_Camera2.mtxProjection);

	// プロジェクションマトリックスの作成
	D3DXMatrixPerspectiveFovLH(&g_Camera2.mtxProjection,
		VIEW_ANGLE,				// ビュー平面の視野角
		VIEW_ASPECT,			// ビュー平面のアスペクト比
		VIEW_NEAR_Z,			// ビュー平面のNearZ値
		VIEW_FAR_Z);			// ビュー平面のFarZ値

								// プロジェクションマトリックスの設定
	pDevice->SetTransform(D3DTS_PROJECTION, &g_Camera2.mtxProjection);






	D3DXMATRIX mtxProjection;

	D3DXMatrixIdentity(&mtxProjection);


	float w = 1 / tanf(VIEW_ANGLE*VIEW_ASPECT*0.5);
	float h = 1 / tanf(VIEW_ANGLE*0.5);
	float Q = VIEW_FAR_Z / (VIEW_FAR_Z - VIEW_NEAR_Z);

	mtxProjection(0, 0) = w;
	mtxProjection(1, 1) = h;
	mtxProjection(2, 2) = Q;
	mtxProjection(2, 3) = 1.0f;
	mtxProjection(3, 2) = Q * VIEW_NEAR_Z * -1;



	// ビューマトリックスの初期化
	D3DXMatrixIdentity(&g_Camera2.mtxView);

	// ビューマトリックスの作成
	D3DXMatrixLookAtLH(&g_Camera2.mtxView,
		&g_Camera2.posV,		// カメラの視点
		&g_Camera2.posR,		// カメラの注視点
		&g_Camera2.vecU);	// カメラの上方向ベクトル

	D3DXMATRIX mtxView;
	D3DXMatrixIdentity(&mtxView);


	D3DXVECTOR3 axisZ;

	axisZ = g_Camera2.posR - g_Camera2.posV;

	D3DXVec3Normalize(&axisZ, &axisZ);


	D3DXVECTOR3 axisX;

	D3DXVec3Cross(&axisX, &g_Camera2.vecU, &axisZ);


	D3DXVECTOR3 axisY;

	D3DXVec3Cross(&axisY, &axisZ, &axisY);



	mtxView(0, 0) = axisX.x;
	mtxView(1, 0) = axisX.y;
	mtxView(2, 0) = axisX.z;
	mtxView(0, 1) = axisY.x;
	mtxView(1, 1) = axisY.y;
	mtxView(2, 1) = axisY.z;
	mtxView(0, 2) = axisZ.x;
	mtxView(1, 2) = axisZ.y;
	mtxView(2, 2) = axisZ.z;

	mtxView(3, 0) = D3DXVec3Dot(&g_Camera2.posV, &axisX)*-1;
	mtxView(3, 1) = D3DXVec3Dot(&g_Camera2.posV, &axisY)*-1;
	mtxView(3, 2) = D3DXVec3Dot(&g_Camera2.posV, &axisZ)*-1;

	// ビューマトリックスの設定
	pDevice->SetTransform(D3DTS_VIEW, &g_Camera2.mtxView);
}

