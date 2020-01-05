/*「Ground.cpp」=============================================
　・地面を作るファイルだよ

 　 制作概要：HEW2年次作品「ペダル」
　　製作者：バイクのやつ　	(-"-)taiki okahara
=============================================================*/

#include "myDirect3D.h"
#include "input.h"
#include "texture.h"
#include "debugproc.h"
#include <iostream>
#include <time.h>
#include "ground.h"
#include <stdlib.h>
#include "player.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define PLATE_SIZE			(40.0f)							//プレートの大きさ

#define	PLATE_NUM			(20)							//プレートの数

#define GROUND_NUM			(75)							//ブロックの総数

#define GROUND_SIZE		((PLATE_NUM) * (PLATE_SIZE))		//ブロックの幅

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
float MakeSlope(int slope_type, int x, int z, int height);
//*****************************************************************************
// グローバル変数
//*****************************************************************************

static const int		PrimCount = (PLATE_NUM * PLATE_NUM) * 2 + (PLATE_NUM - 1) * 4;	//ポリゴン数
static const int		NumIndex = (PLATE_NUM + 1) * 2 * PLATE_NUM + (PLATE_NUM - 1) * 2;	//頂点インデックス数
static const int		NumVertex = (PLATE_NUM + 1) * (PLATE_NUM + 1);						//頂点数

static D3DXVECTOR3	p_before_left;
static D3DXVECTOR3	p_before_right;

static int		CntGround = 0;//インスタンスに要素番号を付随するための番号

//=============================================================================
// GROUNDクラス
//=============================================================================																			
class GROUND
{
public:
	D3DXMATRIX				g_mtxWorldGround;		// ワールドマトリックス
	D3DXVECTOR3				g_posGround;				// 地面の位置
	D3DXVECTOR3				g_rotGround;				// 地面の向き(回転)
	D3DXVECTOR3				g_sclGround;				// 地面の大きさ(スケール)

	//グラウンドを生成する関数
	void	SetGround_Pos(int ground_type,float ground_center_distance, int curve_radius,int slope_type, int height);

	bool	isUse;			//使っているか使っていないか（保険）

	//Max,Min座標
	float	MinX;
	float	MinZ;
	float	MaxX;
	float	MaxZ;


	VERTEX_3D *LookVtx;//ポインタのポインタ
	int		  *LookIdxNum;//インデックスのポインタ

	LPDIRECT3DVERTEXBUFFER9	g_pVtxBuffGround;// 頂点バッファへのポインタ
	LPDIRECT3DINDEXBUFFER9	g_pIdxBuffGround;// インデックスバッファへのポインタ

	GROUND();
	~GROUND();
	int		ground_number;			//　ブロックの要素番号
private:
	int		ground_radius;			//　回転角
	int		ground_curve_length;	//　カーブの長さ
	int		ground_type;			//　ブロックの種類
	void	MakeIdx();
	void	MakeVertexGround(float ground_center_distance, int slope_type, int slope_height);
};
GROUND::GROUND() {

	LookVtx = new VERTEX_3D[NumVertex];	//　VRAMから持ってくるVtx情報の容量確保
	LookIdxNum = new int[NumIndex];		//	VRAMから持ってくるIdx情報の容量確保

	//バッファポインタNULL初期化
	g_pVtxBuffGround = NULL;
	g_pIdxBuffGround = NULL;

	ground_number = CntGround;			//グラウンドの要素番号をCntGroundをインクリメントして代入
	CntGround++;

	ground_radius = 0;
	ground_curve_length = 0;
	isUse = false;			//SetPos関数が呼び出されるまでTrueにしない
}
GROUND::~GROUND() {

	delete[] LookVtx;
	delete[] LookIdxNum;
	if (g_pIdxBuffGround != NULL)
	{// 頂点バッファの開放
		g_pIdxBuffGround->Release();
		g_pIdxBuffGround = NULL;
	}

	if (g_pVtxBuffGround != NULL)
	{// 頂点バッファの開放
		g_pVtxBuffGround->Release();
		g_pVtxBuffGround = NULL;
	}
}
GROUND	ground[GROUND_NUM];	//グラウンドをGROUN_NUM分生成

typedef enum
{
	TYPE_STRAIGHT,		//直線
	TYPE_CURVE,			//90°カーブ
	TYPE_SOFT_CURVE,	//なめらかなカーブ
	TYPE_STAIRS,
	TYPE_MAX
}TypeIndex;

typedef enum
{
	NORMAL,		//通常
	UP_DOWN,	//上り、下り坂
	LEFT_RIGHT,	//左か右が高く傾く
	STAIRS,
	C_U_D1,		//なめらかな上り、下り坂
	C_U_D2,
	C_U_D3,
	C_U_D4,
	SAMPLE,

}SlopeIndex;


//	[SetGround_Pos関数]	ブロックの初期設定をする関数
//【第一引数：ブロック種類】【第二引数：回転角度】【第三引数：回転軸距離】【第四引数：カーブの長さ】
void GROUND::SetGround_Pos(int g_type, float ground_center_distance, int curve_length, int s_type,int height) {

	isUse = true;
	ground_type = g_type;					//ブロックの種類を代入
	ground_curve_length = curve_length;


	// 位置・回転・スケールの初期設定
	g_rotGround = D3DXVECTOR3(0, 0, 0);		//回転角。基本的にはいじらない
	g_sclGround = D3DXVECTOR3(1.0f, 1.0f, 1.0f);//大きさ


	//　前のブロックの回転角度とカーブの長さから設置する角度を計算する
	if (ground_number == 0)	//最初のブロックは例外処理
	{
		ground_radius = ground[ground_number].ground_radius + ground[ground_number].ground_curve_length;
	}
	else
	{
		ground_radius = ground[ground_number - 1].ground_radius + ground[ground_number - 1].ground_curve_length;
	}

	MakeVertexGround(ground_center_distance, s_type, height);

}
//=============================================================================
// 頂点の作成
//=============================================================================
void GROUND::MakeVertexGround(float ground_center_distance, int slope_type, int slope_height)
{	
	//頂点バッファの中身を埋める
	//Ground_Type_Vertx(radi, height);

	LPDIRECT3DDEVICE9 pDevice = GetD3DDevice();

	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * NumVertex, //VRAMにメモリを作る
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&g_pVtxBuffGround,
		NULL);

	VERTEX_3D *pVtx;

	//Lock,unLockをあまりしないのがパフォーマンスを上げる方法
	g_pVtxBuffGround->Lock(0, 0, (void**)&pVtx, 0); //VRAMにあったメモリをシステム上に持ってきてくれる

	D3DXVECTOR3 base_position;// = g_posGround + D3DXVECTOR3(GROUND_SIZE / 2, 0.0f, -GROUND_SIZE / 2);//ブロックの真ん中となる座標
	D3DXVECTOR3	rotation_pos;	//回転の中心となる座標
	D3DXVECTOR3 trans;

	//色、法線は基準値のまま
	for (int z = 0; z < PLATE_NUM + 1; z++) {
		for (int x = 0; x < PLATE_NUM + 1; x++) {
			pVtx[(PLATE_NUM + 1)* z + x].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
			pVtx[(PLATE_NUM + 1)* z + x].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		}
	}

	switch (ground_type)
	{

		//　直線 ==================================================================================
	case TYPE_STRAIGHT:


		if (ground_radius > 0)
		{
			g_posGround = p_before_left;

			base_position = g_posGround;//左

			for (int z = 0; z < PLATE_NUM + 1; z++) {
				for (int x = 0; x < PLATE_NUM + 1; x++) {

					rotation_pos = D3DXVECTOR3((PLATE_NUM - x) * PLATE_SIZE, 0.0f, z  * PLATE_SIZE);//回転する中心からの距離

					//縁を作るよ
					if (x == 0 || x == 20) {
						rotation_pos.y += 0.0f;
					}
					else if (x == 1 || x == 19) {
						rotation_pos.y += -2.0f;
					}
					else {
						rotation_pos.y += -15.0f;
					}

					//座標作成
					pVtx[(PLATE_NUM + 1)* z + x].pos = D3DXVECTOR3(
						base_position.x + rotation_pos.x * cos(D3DX_PI * ground_radius / 180) - (rotation_pos.z * sin(D3DX_PI * ground_radius / 180)),	//加法定理より　x' = cosθ - sinθ
						base_position.y + rotation_pos.y,
						base_position.z + rotation_pos.z * cos(D3DX_PI * ground_radius / 180) + rotation_pos.x * sin(D3DX_PI * ground_radius / 180));	//加法定理より　y' = cosθ + sinθ

					//テクスチャ位置
					pVtx[(PLATE_NUM + 1)* z + x].tex = D3DXVECTOR2((PLATE_SIZE * x) / GROUND_SIZE, (PLATE_SIZE * z) / GROUND_SIZE);
					//バッファをVRAMからメモリにコピー
					LookVtx[(PLATE_NUM + 1)* z + x] = pVtx[(PLATE_NUM + 1)* z + x];
				}
			}

			p_before_left = this->LookVtx[440].pos;
			p_before_right = this->LookVtx[420].pos;

		}
		else if (ground_radius <= 0)
		{
			g_posGround = p_before_right;

			base_position = g_posGround;//ブロックの真ん中となる座標

			for (int z = 0; z < PLATE_NUM + 1; z++) {
				for (int x = 0; x < PLATE_NUM + 1; x++) {

					rotation_pos = D3DXVECTOR3(-x * PLATE_SIZE, 0.0f, z  * PLATE_SIZE);//回転する中心からの距離

					//縁を作るよ
					if (x == 0 || x == 20) {
						rotation_pos.y += 0.0f;
					}
					else if (x == 1 || x == 19) {
						rotation_pos.y += -2.0f;
					}
					else {
						rotation_pos.y += -15.0f;
					}

					//座標作成
					pVtx[(PLATE_NUM + 1)* z + x].pos = D3DXVECTOR3(
						base_position.x + rotation_pos.x * cos(D3DX_PI * ground_radius / 180) - (rotation_pos.z * sin(D3DX_PI * ground_radius / 180)),	//加法定理より　x' = cosθ - sinθ
						base_position.y + rotation_pos.y + MakeSlope(slope_type,x,z, slope_height),
						base_position.z + rotation_pos.z * cos(D3DX_PI * ground_radius / 180) + rotation_pos.x * sin(D3DX_PI * ground_radius / 180));	//加法定理より　y' = cosθ + sinθ

					//テクスチャ位置
					pVtx[(PLATE_NUM + 1)* z + x].tex = D3DXVECTOR2((PLATE_SIZE * x) / GROUND_SIZE, (PLATE_SIZE * z) / GROUND_SIZE);
					//バッファをVRAMからメモリにコピー
					LookVtx[(PLATE_NUM + 1)* z + x] = pVtx[(PLATE_NUM + 1)* z + x];
				}
			}

			p_before_left = this->LookVtx[440].pos;
			p_before_right = this->LookVtx[420].pos;
		}
		else
		{
			MessageBox(NULL, "ブロック生成に誤りがありました。(else)", NULL, MB_OK);
		}

		break;

		//　直角のようなカーブ ====================================================================
	case TYPE_CURVE:

		if (ground_radius >= 0)
		{
			g_posGround = p_before_left;

			if (ground_curve_length >= 0)
			{

				base_position = g_posGround;//左下にする
				//90°カーブの場合中心とする点を回転軸に置く		
				for (int z = 0; z < PLATE_NUM + 1; z++) {
					for (int x = 0; x < PLATE_NUM + 1; x++) {


						//縁を作るよ
						if (x == 0 || x == 20) {
							base_position.y = g_posGround.y;
						}
						else if (x == 1 || x == 19) {
							base_position.y = g_posGround.y - 2.0f;
						}
						else
						{
							base_position.y = g_posGround.y - 15.0f;
						}

						pVtx[(PLATE_NUM + 1)* z + x].pos = D3DXVECTOR3(
							base_position.x + cos((z* D3DX_PI / (PLATE_NUM * 180 / ground_curve_length)) + D3DX_PI * ground_radius / 180) * (PLATE_SIZE * (PLATE_NUM - x) + 1),
							base_position.y + MakeSlope(slope_type, x, z, slope_height),
							base_position.z + sin((z* D3DX_PI / (PLATE_NUM * 180 / ground_curve_length)) + D3DX_PI * ground_radius / 180) * (PLATE_SIZE * (PLATE_NUM - x) + 1));

						pVtx[(PLATE_NUM + 1)* z + x].tex = D3DXVECTOR2((PLATE_SIZE * x) / GROUND_SIZE, (PLATE_SIZE * z) / GROUND_SIZE);

						LookVtx[(PLATE_NUM + 1)* z + x] = pVtx[(PLATE_NUM + 1)* z + x];
					}
				}

				p_before_left = this->LookVtx[440].pos;
				p_before_right = this->LookVtx[420].pos;
			}
			else if (ground_curve_length < 0)
			{
				base_position = g_posGround + D3DXVECTOR3(GROUND_SIZE * cos(D3DX_PI * ground_radius / 180), 0.0f, GROUND_SIZE * sin(D3DX_PI * ground_radius / 180));
				//90°カーブの場合中心とする点を回転軸に置く	
			
				for (int z = 0; z < PLATE_NUM + 1; z++) {
					for (int x = 0; x < PLATE_NUM + 1; x++) {


						//縁を作るよ
						if (x == 0 || x == 20) {
							base_position.y = g_posGround.y;
						}
						else if (x == 1 || x == 19) {
							base_position.y = g_posGround.y - 2.0f;
						}
						else
						{
							base_position.y = g_posGround.y - 15.0f;
						}

						pVtx[(PLATE_NUM + 1)* z + x].pos = D3DXVECTOR3(
							base_position.x + cos((z* D3DX_PI / (PLATE_NUM * 180 / -ground_curve_length)) - D3DX_PI * ground_radius / 180) * -(PLATE_SIZE * x + 1),
							base_position.y + MakeSlope(slope_type, x, z, slope_height),
							base_position.z + sin((z* D3DX_PI / (PLATE_NUM * 180 / -ground_curve_length)) - D3DX_PI * ground_radius / 180) * (PLATE_SIZE * x + 1));

						pVtx[(PLATE_NUM + 1)* z + x].tex = D3DXVECTOR2((PLATE_SIZE * x) / GROUND_SIZE, (PLATE_SIZE * z) / GROUND_SIZE);

						LookVtx[(PLATE_NUM + 1)* z + x] = pVtx[(PLATE_NUM + 1)* z + x];
					}
				}

				p_before_left = this->LookVtx[440].pos;
				p_before_right = this->LookVtx[420].pos;
			}

		}
		else if (ground_radius < 0)
		{
			g_posGround = p_before_right;
			if (ground_curve_length >= 0)
			{

				base_position = g_posGround + D3DXVECTOR3(-GROUND_SIZE * cos(D3DX_PI * ground_radius/180), 0.0f, -GROUND_SIZE * sin(D3DX_PI * ground_radius/ 180));//左下にする
				//90°カーブの場合中心とする点を回転軸に置く

				for (int z = 0; z < PLATE_NUM + 1; z++) {
					for (int x = 0; x < PLATE_NUM + 1; x++) {


						//縁を作るよ
						if (x == 0 || x == 20) {
							base_position.y = g_posGround.y;
						}
						else if (x == 1 || x == 19) {
							base_position.y = g_posGround.y - 2.0f;
						}
						else
						{
							base_position.y = g_posGround.y - 15.0f;
						}

						pVtx[(PLATE_NUM + 1)* z + x].pos = D3DXVECTOR3(
							base_position.x + cos((z* D3DX_PI / (PLATE_NUM * 180 / ground_curve_length)) + D3DX_PI * ground_radius / 180) * (PLATE_SIZE * (PLATE_NUM - x) + 1),
							base_position.y + MakeSlope(slope_type, x, z, slope_height),
							base_position.z + sin((z* D3DX_PI / (PLATE_NUM * 180 / ground_curve_length)) + D3DX_PI * ground_radius / 180) * (PLATE_SIZE * (PLATE_NUM - x) + 1));

						pVtx[(PLATE_NUM + 1)* z + x].tex = D3DXVECTOR2((PLATE_SIZE * x) / GROUND_SIZE, (PLATE_SIZE * z) / GROUND_SIZE);

						LookVtx[(PLATE_NUM + 1)* z + x] = pVtx[(PLATE_NUM + 1)* z + x];
					}
				}

				p_before_left = this->LookVtx[440].pos;
				p_before_right = this->LookVtx[420].pos;
			}
			else if (ground_curve_length < 0)
			{
				base_position = g_posGround;
				//90°カーブの場合中心とする点を回転軸に置く
				
				for (int z = 0; z < PLATE_NUM + 1; z++) {
					for (int x = 0; x < PLATE_NUM + 1; x++) {


						//縁を作るよ
						if (x == 0 || x == 20) {
							base_position.y = g_posGround.y;
						}
						else if (x == 1 || x == 19) {
							base_position.y = g_posGround.y - 2.0f;
						}
						else
						{
							base_position.y = g_posGround.y - 15.0f;
						}

						pVtx[(PLATE_NUM + 1)* z + x].pos = D3DXVECTOR3(
							base_position.x + cos((z* D3DX_PI / (PLATE_NUM * 180 / -ground_curve_length)) - D3DX_PI * ground_radius / 180) * -(PLATE_SIZE * x + 1),
							base_position.y + MakeSlope(slope_type, x, z, slope_height),
							base_position.z + sin((z* D3DX_PI / (PLATE_NUM * 180 / -ground_curve_length)) - D3DX_PI * ground_radius / 180) * (PLATE_SIZE * x + 1));

						pVtx[(PLATE_NUM + 1)* z + x].tex = D3DXVECTOR2((PLATE_SIZE * x) / GROUND_SIZE, (PLATE_SIZE * z) / GROUND_SIZE);

						LookVtx[(PLATE_NUM + 1)* z + x] = pVtx[(PLATE_NUM + 1)* z + x];

					}
				}

				p_before_left = this->LookVtx[440].pos;
				p_before_right = this->LookVtx[420].pos;
			}
		}
		else
		{
			MessageBox(NULL, "ブロック生成に誤りがありました。(else)", NULL, MB_OK);
		}

		break;

		//　なめらかなカーブ ======================================================================
	case TYPE_SOFT_CURVE:



		if (ground_radius >= 0)
		{
			g_posGround = p_before_left;

			if (ground_curve_length >= 0)
			{
				base_position = g_posGround + D3DXVECTOR3(-ground_center_distance, 0.0f, 0.0f);//ブロックの真ん中となる座標
				//カーブの場合中心とする点を回転軸に置く
				for (int z = 0; z < PLATE_NUM + 1; z++) {
					for (int x = 0; x < PLATE_NUM + 1; x++) {


						//縁を作るよ
						if (x == 0 || x == 20) {
							base_position.y = g_posGround.y;
						}
						else if (x == 1 || x == 19) {
							base_position.y = g_posGround.y - 2.0f;
						}
						else
						{
							base_position.y = g_posGround.y - 15.0f;
						}

						trans.x = base_position.x + cos((z* D3DX_PI / (PLATE_NUM * (180 / ground_curve_length)))) * ((PLATE_SIZE * (PLATE_NUM - x) + 1 + ground_center_distance));
						trans.y = base_position.y + MakeSlope(slope_type, x, z, slope_height);
						trans.z = base_position.z + sin((z* D3DX_PI / (PLATE_NUM * (180 / ground_curve_length)))) * ((PLATE_SIZE * (PLATE_NUM - x) + 1 + ground_center_distance));


						pVtx[(PLATE_NUM + 1)* z + x].pos = D3DXVECTOR3(
							g_posGround.x + cos(D3DX_PI * ground_radius / 180) * (trans.x - g_posGround.x) - (sin(D3DX_PI * ground_radius / 180) * (trans.z - g_posGround.z)),
							g_posGround.y,
							g_posGround.z + cos(D3DX_PI * ground_radius / 180) * (trans.z - g_posGround.z) + (sin(D3DX_PI * ground_radius / 180) * (trans.x - g_posGround.x)));
						/*pVtx[(PLATE_NUM + 1)* z + x].pos = D3DXVECTOR3(
							trans.x,
							trans.y,
							trans.z);*/


						pVtx[(PLATE_NUM + 1)* z + x].tex = D3DXVECTOR2((PLATE_SIZE * x) / GROUND_SIZE, (PLATE_SIZE * z) / GROUND_SIZE);

						LookVtx[(PLATE_NUM + 1)* z + x] = pVtx[(PLATE_NUM + 1)* z + x];
					}
				}

				p_before_left = this->LookVtx[440].pos;
				p_before_right = this->LookVtx[420].pos;
			}
			else if (ground_curve_length < 0)
			{
				base_position = g_posGround + D3DXVECTOR3(GROUND_SIZE * cos(D3DX_PI * ground_radius / 180) + ground_center_distance, 0.0f, GROUND_SIZE * sin(D3DX_PI * ground_radius / 180));
				
				for (int z = 0; z < PLATE_NUM + 1; z++) {
					for (int x = 0; x < PLATE_NUM + 1; x++) {


						//縁を作るよ
						if (x == 0 || x == 20) {
							base_position.y = g_posGround.y;
						}
						else if (x == 1 || x == 19) {
							base_position.y = g_posGround.y - 2.0f;
						}
						else
						{
							base_position.y = g_posGround.y - 15.0f;
						}

						trans.x = base_position.x + cos((z* D3DX_PI / (PLATE_NUM * (180 / -ground_curve_length)))) * (-(PLATE_SIZE * x + ground_center_distance));
						trans.y = base_position.y + MakeSlope(slope_type, x, z, slope_height);
						trans.z = base_position.z + sin((z* D3DX_PI / (PLATE_NUM * (180 / -ground_curve_length)))) * ((PLATE_SIZE * x + ground_center_distance));


						pVtx[(PLATE_NUM + 1)* z + x].pos = D3DXVECTOR3(
							base_position.x - ground_center_distance + cos(D3DX_PI * ground_radius / 180) * (trans.x - base_position.x + ground_center_distance) - sin(D3DX_PI * ground_radius / 180) * (trans.z - base_position.z),
							trans.y,
							base_position.z + cos(D3DX_PI * ground_radius / 180) * (trans.z - base_position.z) + sin(D3DX_PI * ground_radius / 180) * (trans.x - base_position.x + ground_center_distance));


						pVtx[(PLATE_NUM + 1)* z + x].tex = D3DXVECTOR2((PLATE_SIZE * x) / GROUND_SIZE, (PLATE_SIZE * z) / GROUND_SIZE);

						LookVtx[(PLATE_NUM + 1)* z + x] = pVtx[(PLATE_NUM + 1)* z + x];
					}
				}

				p_before_left = this->LookVtx[440].pos;
				p_before_right = this->LookVtx[420].pos;
			}
			else
			{
				MessageBox(NULL, "ブロック生成に誤りがありました。(else)", NULL, MB_OK);
			}
		}
		else if (ground_radius < 0)
		{
			g_posGround = p_before_right;

			if (ground_curve_length >= 0)

			{
				base_position = g_posGround + D3DXVECTOR3(-GROUND_SIZE * cos(D3DX_PI * ground_radius / 180) - ground_center_distance, 0.0f,-GROUND_SIZE * sin(D3DX_PI * ground_radius / 180));
				
				for (int z = 0; z < PLATE_NUM + 1; z++) {
					for (int x = 0; x < PLATE_NUM + 1; x++) {


						//縁を作るよ
						if (x == 0 || x == 20) {
							base_position.y = g_posGround.y;
						}
						else if (x == 1 || x == 19) {
							base_position.y = g_posGround.y - 2.0f;
						}
						else
						{
							base_position.y = g_posGround.y - 15.0f;
						}

						trans.x = base_position.x + cos((z* D3DX_PI / (PLATE_NUM * (180 / ground_curve_length)))) * ((PLATE_SIZE * (PLATE_NUM - x) + ground_center_distance));
						trans.y = base_position.y + MakeSlope(slope_type, x, z, slope_height);
						trans.z = base_position.z + sin((z* D3DX_PI / (PLATE_NUM * (180 / ground_curve_length)))) * ((PLATE_SIZE * (PLATE_NUM - x) + ground_center_distance));

						
						pVtx[(PLATE_NUM + 1)* z + x].pos = D3DXVECTOR3(
							base_position.x + ground_center_distance + cos(D3DX_PI * ground_radius / 180) * (trans.x - base_position.x - ground_center_distance) - sin(D3DX_PI * ground_radius / 180) * (trans.z - base_position.z),
							trans.y,
							base_position.z + cos(D3DX_PI * ground_radius / 180) * (trans.z - base_position.z) + sin(D3DX_PI * ground_radius / 180) * (trans.x - base_position.x - ground_center_distance));

						/*pVtx[(PLATE_NUM + 1)* z + x].pos = D3DXVECTOR3(
							trans.x,
							trans.y,
							trans.z);*/



						pVtx[(PLATE_NUM + 1)* z + x].tex = D3DXVECTOR2((PLATE_SIZE * x) / GROUND_SIZE, (PLATE_SIZE * z) / GROUND_SIZE);

						LookVtx[(PLATE_NUM + 1)* z + x] = pVtx[(PLATE_NUM + 1)* z + x];
					}
				}

				p_before_left = this->LookVtx[440].pos;
				p_before_right = this->LookVtx[420].pos;
			}
			else if (ground_curve_length < 0)
			{
				base_position = g_posGround + D3DXVECTOR3(-GROUND_SIZE * cos(D3DX_PI * ground_radius / 180) + ground_center_distance, 0.0f, -GROUND_SIZE * sin(D3DX_PI * ground_radius / 180));
			
				for (int z = 0; z < PLATE_NUM + 1; z++) {
					for (int x = 0; x < PLATE_NUM + 1; x++) {


						//縁を作るよ
						if (x == 0 || x == 20) {
							base_position.y = g_posGround.y;
						}
						else if (x == 1 || x == 19) {
							base_position.y = g_posGround.y - 2.0f;
						}
						else
						{
							base_position.y = g_posGround.y - 15.0f;
						}

						trans.x = base_position.x + cos((z* D3DX_PI / (PLATE_NUM * (180 / -ground_curve_length)))) * -((PLATE_SIZE * x + ground_center_distance));
						trans.y = base_position.y + MakeSlope(slope_type, x, z, slope_height);
						trans.z = base_position.z + sin((z* D3DX_PI / (PLATE_NUM * (180 / -ground_curve_length)))) * ((PLATE_SIZE * x + ground_center_distance));


						pVtx[(PLATE_NUM + 1)* z + x].pos = D3DXVECTOR3(
							
							
							g_posGround.x + cos(D3DX_PI * ground_radius / 180) * (trans.x - base_position.x + ground_center_distance) - sin(D3DX_PI * ground_radius / 180) * (trans.z - base_position.z),
							trans.y,
							g_posGround.z + cos(D3DX_PI * ground_radius / 180) * (trans.z - base_position.z) + sin(D3DX_PI * ground_radius / 180) * (trans.x - base_position.x + ground_center_distance));

						/*pVtx[(PLATE_NUM + 1)* z + x].pos = D3DXVECTOR3(
							trans.x,
							trans.y,
							trans.z);*/



						pVtx[(PLATE_NUM + 1)* z + x].tex = D3DXVECTOR2((PLATE_SIZE * x) / GROUND_SIZE, (PLATE_SIZE * z) / GROUND_SIZE);

						LookVtx[(PLATE_NUM + 1)* z + x] = pVtx[(PLATE_NUM + 1)* z + x];
					}
				}

				p_before_left = this->LookVtx[440].pos;
				p_before_right = this->LookVtx[420].pos;
			}
			else
			{
				MessageBox(NULL, "ブロック生成に誤りがありました。(else)", NULL, MB_OK);
			}

			D3DXVECTOR3 trans;
		}
		else
		{
			MessageBox(NULL, "ブロック生成に誤りがありました。(else)", NULL, MB_OK);
		}
		
		break;

		//　階段 ==================================================================================
	case TYPE_STAIRS:


		if (ground_radius > 0)
		{
			g_posGround = p_before_left;

			base_position = g_posGround;//左

			for (int z = 0; z < PLATE_NUM + 1; z++) {
				for (int x = 0; x < PLATE_NUM + 1; x++) {

					rotation_pos = D3DXVECTOR3((PLATE_NUM - x) * PLATE_SIZE, 0.0f, (z - (z%2)) * PLATE_SIZE);//回転する中心からの距離

					//縁を作るよ
					if (x == 0 || x == 20) {
						rotation_pos.y += 0.0f;
					}
					else if (x == 1 || x == 19) {
						rotation_pos.y += -2.0f;
					}
					else {
						rotation_pos.y += -15.0f;
					}

					//座標作成
					pVtx[(PLATE_NUM + 1)* z + x].pos = D3DXVECTOR3(
						base_position.x + rotation_pos.x * cos(D3DX_PI * ground_radius / 180) - (rotation_pos.z * sin(D3DX_PI * ground_radius / 180)),	//加法定理より　x' = cosθ - sinθ
						base_position.y + rotation_pos.y + MakeSlope(slope_type, x, z, slope_height),
						base_position.z + rotation_pos.z * cos(D3DX_PI * ground_radius / 180) + rotation_pos.x * sin(D3DX_PI * ground_radius / 180));	//加法定理より　y' = cosθ + sinθ

					//テクスチャ位置
					pVtx[(PLATE_NUM + 1)* z + x].tex = D3DXVECTOR2((PLATE_SIZE * x) / GROUND_SIZE, (PLATE_SIZE * z) / GROUND_SIZE);
					//バッファをVRAMからメモリにコピー
					LookVtx[(PLATE_NUM + 1)* z + x] = pVtx[(PLATE_NUM + 1)* z + x];
				}
			}

			p_before_left = this->LookVtx[440].pos;
			p_before_right = this->LookVtx[420].pos;

		}
		else if (ground_radius <= 0)
		{
			g_posGround = p_before_right;

			base_position = g_posGround;//ブロックの真ん中となる座標

			for (int z = 0; z < PLATE_NUM + 1; z++) {
				for (int x = 0; x < PLATE_NUM + 1; x++) {

					rotation_pos = D3DXVECTOR3(-x * PLATE_SIZE, 0.0f, (z -(z % 2)) * PLATE_SIZE);//回転する中心からの距離

					//縁を作るよ
					if (x == 0 || x == 20) {
						rotation_pos.y += 0.0f;
					}
					else if (x == 1 || x == 19) {
						rotation_pos.y += -2.0f;
					}
					else {
						rotation_pos.y += -15.0f;
					}

					//座標作成
					pVtx[(PLATE_NUM + 1)* z + x].pos = D3DXVECTOR3(
						base_position.x + rotation_pos.x * cos(D3DX_PI * ground_radius / 180) - (rotation_pos.z * sin(D3DX_PI * ground_radius / 180)),	//加法定理より　x' = cosθ - sinθ
						base_position.y + rotation_pos.y + MakeSlope(slope_type, x, z, slope_height),
						base_position.z + rotation_pos.z * cos(D3DX_PI * ground_radius / 180) + rotation_pos.x * sin(D3DX_PI * ground_radius / 180));	//加法定理より　y' = cosθ + sinθ

					//テクスチャ位置
					pVtx[(PLATE_NUM + 1)* z + x].tex = D3DXVECTOR2((PLATE_SIZE * x) / GROUND_SIZE, (PLATE_SIZE * z) / GROUND_SIZE);
					//バッファをVRAMからメモリにコピー
					LookVtx[(PLATE_NUM + 1)* z + x] = pVtx[(PLATE_NUM + 1)* z + x];
				}
			}

			p_before_left = this->LookVtx[440].pos;
			p_before_right = this->LookVtx[420].pos;
		}
		else
		{
			MessageBox(NULL, "ブロック生成に誤りがありました。(else)", NULL, MB_OK);
		}

		break;


	default:
		MessageBox(NULL, "ブロック生成に誤りがありました。(default)", NULL, MB_OK);
		break;
		
	}


	//ブロックの高さを変更する分岐を入れる

	g_pVtxBuffGround->Unlock(); //VRAMにメモリを返す(重い処理)


	

	if (isUse)
	{
		//ブロック座標の最大、最小値をPointに代入
		float min_Pos_x = g_posGround.x;
		float min_Pos_z = g_posGround.z;
		float max_Pos_x = g_posGround.x;
		float max_Pos_z = g_posGround.z;

		for (int i = 0; i < NumVertex; i++)
		{
			if (LookVtx[i].pos.x < min_Pos_x) {
				min_Pos_x = LookVtx[i].pos.x;
			}
			if (LookVtx[i].pos.z < min_Pos_z) {
				min_Pos_z = LookVtx[i].pos.z;
			}
			if (LookVtx[i].pos.x > max_Pos_x) {
				max_Pos_x = LookVtx[i].pos.x;
			}
			if (LookVtx[i].pos.z > max_Pos_z) {
				max_Pos_z = LookVtx[i].pos.z;
			}
		}

		/*
			Point0:X軸最小値
			Point1:Z軸最小値
			Point2:X軸最大値
			Point3:Z軸最大値

		*/
		MinX = min_Pos_x;
		MinZ = min_Pos_z;
		MaxX = max_Pos_x;
		MaxZ = max_Pos_z;
	}

	MakeIdx();
}

//=============================================================================
// インデックスの作成
//=============================================================================
void GROUND::MakeIdx() {

	LPDIRECT3DDEVICE9 pDevice = GetD3DDevice();

	pDevice->CreateIndexBuffer(sizeof(WORD) * NumIndex, //VRAMにメモリを作る
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&g_pIdxBuffGround,
		NULL);
	//インデックスバッファの中身を埋める
	WORD *pIdx;

	g_pIdxBuffGround->Lock(0, 0, (void**)&pIdx, 0);

	int j = 0;
	int i = 0;
	for (int k = 0; k < PLATE_NUM; k++)
	{

		for (j; j <= PLATE_NUM; j++)
		{
			pIdx[(j * 2) + i] = (PLATE_NUM + 1) + j + (PLATE_NUM + 1) * k;
			LookIdxNum[(j * 2) + i] = pIdx[(j * 2) + i];
			pIdx[(j * 2) + i + 1] = j + (PLATE_NUM + 1) * k;
			LookIdxNum[(j * 2) + i + 1] = pIdx[(j * 2) + i + 1];
		}
		if (k == PLATE_NUM - 1)
		{
			break;
		}
		j--;
		pIdx[(j * 2) + i + 2] = j + (PLATE_NUM + 1) * k;
		LookIdxNum[(j * 2) + i + 2] = j + (PLATE_NUM + 1) * k;

		pIdx[(j * 2) + i + 3] = j + (PLATE_NUM + 2) + (PLATE_NUM + 1) * k;
		LookIdxNum[(j * 2) + i + 3] = j + (PLATE_NUM + 2) + (PLATE_NUM + 1) * k;

		i += (2 * (PLATE_NUM) + 4);
		j = 0;
	}
	g_pIdxBuffGround->Unlock();
}

//=============================================================================
// 坂生成関数
//=============================================================================
float MakeSlope(int slope_type, int x,int z, int height) {

	switch (slope_type)
	{
	case NORMAL:
		return 0.0f;

	case UP_DOWN:
		return  1.0f * (z%(PLATE_NUM + 1)) * (height / PLATE_NUM);
				
	case STAIRS:
		return -(z - !(z % 2)) * height / PLATE_NUM;

	case C_U_D1:
		return (height * sin(D3DX_PI * z/(2 * PLATE_NUM)));

	case C_U_D2:
		return (-height + (height * sin(D3DX_PI * z/(2 * PLATE_NUM) + D3DX_PI/2)));

	case C_U_D3:
		return (height * sin(D3DX_PI * z/(2 * PLATE_NUM) + D3DX_PI));

	case C_U_D4:
		return (height + (height * sin(D3DX_PI * z / (2 * PLATE_NUM) + D3DX_PI *3/2)));

	case SAMPLE:
		return (-40 * sin(D3DX_PI * height/180) + sin((D3DX_PI * z / (18 * PLATE_NUM)) + (D3DX_PI * height / 180)) * 40);

	default:
		return 0.0f;
	}
}


//=============================================================================
// 初期化処理
//=============================================================================
void Ground_Initialize(void)
{
	

	
	ground[0].LookVtx[420].pos = D3DXVECTOR3(400, 0, -400);
	ground[0].LookVtx[440].pos = D3DXVECTOR3(-400, 0, -400);
	p_before_right = ground[0].LookVtx[420].pos;
	p_before_left = ground[0].LookVtx[440].pos;

	//ground生成
	
	{
		//ground[0].SetGround_Pos(TYPE_STRAIGHT, 0, 0, SAMPLE, 0);
		//ground[1].SetGround_Pos(TYPE_STRAIGHT, 0, 0, SAMPLE, 10);
		//ground[2].SetGround_Pos(TYPE_STRAIGHT, 0, 0, SAMPLE, 20);
		//ground[3].SetGround_Pos(TYPE_STRAIGHT, 0, 0, SAMPLE, 30);


		ground[0].SetGround_Pos(TYPE_STRAIGHT, 0, 0, UP_DOWN, -60);
		ground[1].SetGround_Pos(TYPE_STRAIGHT, 0, 0, UP_DOWN, -60);
		ground[2].SetGround_Pos(TYPE_STRAIGHT, 0, 0, NORMAL, 0);
		ground[3].SetGround_Pos(TYPE_STRAIGHT, 0, 0, UP_DOWN, 80);

		ground[4].SetGround_Pos(TYPE_STRAIGHT, 0, 0, UP_DOWN, 80);
		ground[5].SetGround_Pos(TYPE_STRAIGHT, 0, 0, UP_DOWN, 80);

		ground[6].SetGround_Pos(TYPE_CURVE, 0, 60, NORMAL, 0);

		ground[7].SetGround_Pos(TYPE_STRAIGHT, 0, 0, NORMAL, 0);
		ground[8].SetGround_Pos(TYPE_STRAIGHT, 0, 0, NORMAL, 0);
		ground[9].SetGround_Pos(TYPE_STRAIGHT, 0, 0, NORMAL, 0);


		ground[10].SetGround_Pos(TYPE_SOFT_CURVE, 1200, -20, SAMPLE, 0);
		ground[11].SetGround_Pos(TYPE_SOFT_CURVE, 1200, -20, SAMPLE, 10);
		ground[12].SetGround_Pos(TYPE_SOFT_CURVE, 1200, -20, SAMPLE, 20);
		ground[13].SetGround_Pos(TYPE_SOFT_CURVE, 1200, -20, SAMPLE, 30);
		ground[14].SetGround_Pos(TYPE_SOFT_CURVE, 1200, -20, SAMPLE, 40);
		ground[15].SetGround_Pos(TYPE_SOFT_CURVE, 1200, -20, SAMPLE, 50);
		ground[16].SetGround_Pos(TYPE_SOFT_CURVE, 1200, -20, SAMPLE, 60);
		ground[17].SetGround_Pos(TYPE_SOFT_CURVE, 1200, -20, SAMPLE, 70);
		ground[18].SetGround_Pos(TYPE_SOFT_CURVE, 1200, -20, SAMPLE, 80);

		ground[19].SetGround_Pos(TYPE_SOFT_CURVE, 1200, 30, NORMAL, 0);

		ground[20].SetGround_Pos(TYPE_STRAIGHT, 0, 0, NORMAL, 0);
		ground[21].SetGround_Pos(TYPE_STRAIGHT, 0, 0, NORMAL, 0);
		ground[22].SetGround_Pos(TYPE_STRAIGHT, 0, 0, UP_DOWN, 10);

		ground[23].SetGround_Pos(TYPE_CURVE, 0, 90, NORMAL, 0);

		ground[24].SetGround_Pos(TYPE_STRAIGHT, 0, 0, NORMAL, 0);
		ground[25].SetGround_Pos(TYPE_CURVE, 0, -90, NORMAL, 0);

		ground[26].SetGround_Pos(TYPE_STRAIGHT, 0, 0, NORMAL, 0);
		ground[27].SetGround_Pos(TYPE_STRAIGHT, 0, 0, UP_DOWN, 20);

		ground[28].SetGround_Pos(TYPE_SOFT_CURVE, 1300, -45, NORMAL, 0);
		ground[29].SetGround_Pos(TYPE_SOFT_CURVE, 1300, -45, NORMAL, 0);

		ground[30].SetGround_Pos(TYPE_STRAIGHT, 0, 0, NORMAL, 0);
		ground[31].SetGround_Pos(TYPE_STRAIGHT, 0, 0, NORMAL, 0);

		ground[32].SetGround_Pos(TYPE_CURVE, 0, 90, NORMAL, 0);

		ground[33].SetGround_Pos(TYPE_STRAIGHT, 0, 0, UP_DOWN, -60);
		ground[34].SetGround_Pos(TYPE_STRAIGHT, 0, 0, NORMAL, 0);
		ground[35].SetGround_Pos(TYPE_STRAIGHT, 0, 0, UP_DOWN, 10);
		ground[36].SetGround_Pos(TYPE_STRAIGHT, 0, 0, UP_DOWN, 60);

		ground[37].SetGround_Pos(TYPE_CURVE, 0, -90, NORMAL, 0);

		ground[38].SetGround_Pos(TYPE_STRAIGHT, 0, 0, UP_DOWN, -40);
		ground[39].SetGround_Pos(TYPE_STRAIGHT, 0, 0, UP_DOWN, -40);

		ground[40].SetGround_Pos(TYPE_SOFT_CURVE, 1200, -45, UP_DOWN, -40);
		ground[41].SetGround_Pos(TYPE_SOFT_CURVE, 1200, -45, UP_DOWN, -40);

		ground[42].SetGround_Pos(TYPE_STRAIGHT, 0, 0, NORMAL, 0);
	
		ground[43].SetGround_Pos(TYPE_CURVE, 0, 90, NORMAL, 0);

		ground[44].SetGround_Pos(TYPE_STRAIGHT, 0, 0, NORMAL, 0);

		ground[45].SetGround_Pos(TYPE_CURVE, 0, 90, NORMAL, 0);

		ground[46].SetGround_Pos(TYPE_STAIRS, 0, 0, STAIRS, 200);
		ground[47].SetGround_Pos(TYPE_STRAIGHT, 0, 0, NORMAL, 0);
		ground[48].SetGround_Pos(TYPE_STRAIGHT, 0, 0, NORMAL, 0);
		ground[49].SetGround_Pos(TYPE_STRAIGHT, 0, 0, NORMAL,0);

		ground[50].SetGround_Pos(TYPE_SOFT_CURVE,1000, -36, UP_DOWN, -10);
		ground[51].SetGround_Pos(TYPE_SOFT_CURVE,1000, -36, UP_DOWN, 20);
		ground[52].SetGround_Pos(TYPE_SOFT_CURVE,1000, -36, NORMAL, 0);
		ground[53].SetGround_Pos(TYPE_SOFT_CURVE,1000, -36, NORMAL, 0);
		ground[54].SetGround_Pos(TYPE_SOFT_CURVE,1000, -36, NORMAL, 0);

		ground[55].SetGround_Pos(TYPE_STRAIGHT, 0, 0, NORMAL, 0);
		ground[56].SetGround_Pos(TYPE_STRAIGHT, 0, 0, NORMAL, 0);
		ground[57].SetGround_Pos(TYPE_STRAIGHT, 0, 0, NORMAL, 0);
		ground[58].SetGround_Pos(TYPE_STRAIGHT, 0, 0, NORMAL, 0);
		ground[59].SetGround_Pos(TYPE_STRAIGHT, 0, 0, NORMAL, 0);
		ground[60].SetGround_Pos(TYPE_STRAIGHT, 0, 0, NORMAL, 0);
		ground[61].SetGround_Pos(TYPE_STRAIGHT, 0, 0, NORMAL, 0);
		ground[62].SetGround_Pos(TYPE_STRAIGHT, 0, 0, NORMAL, 0);


		ground[63].SetGround_Pos(TYPE_SOFT_CURVE, 400, -90, NORMAL, 0);

		ground[64].SetGround_Pos(TYPE_SOFT_CURVE, 400, 45, NORMAL, 0);
		ground[65].SetGround_Pos(TYPE_SOFT_CURVE, 400, 45, NORMAL, 0);
		ground[66].SetGround_Pos(TYPE_SOFT_CURVE, 400, 45, NORMAL, 0);
		ground[67].SetGround_Pos(TYPE_SOFT_CURVE, 400, 45, UP_DOWN, 80);

		ground[68].SetGround_Pos(TYPE_CURVE, 0, -90, NORMAL, 0);

		ground[69].SetGround_Pos(TYPE_STRAIGHT, 0, 0, NORMAL, 0);
		ground[70].SetGround_Pos(TYPE_STRAIGHT, 0, 0, UP_DOWN, 105);

		ground[71].SetGround_Pos(TYPE_SOFT_CURVE, 200, -45, NORMAL, 0);
		ground[72].SetGround_Pos(TYPE_SOFT_CURVE, 200, -45, NORMAL, 0);


	}
}

//=============================================================================
// 描画処理
//=============================================================================
void Ground_Draw(void)
{
	
	LPDIRECT3DDEVICE9 pDevice = GetD3DDevice();
	D3DXMATRIX mtxScl, mtxRot, mtxTranslate;

	for (int i = 0; i < GROUND_NUM; i++)
	{

		// ワールドマトリックスの初期化
		D3DXMatrixIdentity(&ground[i].g_mtxWorldGround);

		// スケール反転
		D3DXMatrixScaling(&mtxScl, ground[i].g_sclGround.x, ground[i].g_sclGround.y, ground[i].g_sclGround.z);
		D3DXMatrixMultiply(&ground[i].g_mtxWorldGround, &ground[i].g_mtxWorldGround, &mtxScl);

		// 移動を反転
		D3DXMatrixTranslation(&mtxTranslate, ground[i].g_posGround.x, ground[i].g_posGround.y, ground[i].g_posGround.z);
		//D3DXMatrixMultiply(&ground[i].g_mtxWorldGround, &ground[i].g_mtxWorldGround, &mtxTranslate);

		// 回転を反転
		D3DXMatrixRotationYawPitchRoll(&mtxRot, ground[i].g_rotGround.y, ground[i].g_rotGround.x, ground[i].g_rotGround.z);
		D3DXMatrixMultiply(&ground[i].g_mtxWorldGround, &ground[i].g_mtxWorldGround, &mtxRot);



		pDevice->SetTransform(D3DTS_WORLD, &ground[i].g_mtxWorldGround);
		pDevice->SetStreamSource(0, ground[i].g_pVtxBuffGround, 0, sizeof(VERTEX_3D));

		//インデックスバッファのセット
		pDevice->SetIndices(ground[i].g_pIdxBuffGround);

		pDevice->SetFVF(FVF_VERTEX_3D);
		
		pDevice->SetTexture(0, Texture_GetTexture(TEXTURE_INDEX_FIELD01));//テクスチャを変える場合はここの第二引数を変更する
		//pDevice->SetRenderState(, D3DCULL_CCW);
		pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, NumVertex, 0, PrimCount);
	}

	
}

//=============================================================================
// プレイヤー接触ブロック判定
//=============================================================================
void Ground_Check() {

	D3DXVECTOR3 player_pos = GetPlayer_Pos();
	for (int i = 0; i < GROUND_NUM; i++)
	{
		if (ground[i].isUse)
		{
			//ブロックの最大、最小値の中にプレイヤーがいたら、当たり判定を計算する
			if (ground[i].MinX <= player_pos.x &&
				ground[i].MaxX >= player_pos.x &&
				ground[i].MinZ <= player_pos.z &&
				ground[i].MaxZ >= player_pos.z)
			{
				if (SetGroundHit(i) == false)
					continue;
			}
		}
	}

	return;//エラー検知したほうがいいかも
}

//=============================================================================
// プレイヤーY軸補間（地面の上に載るようにする処理）
//=============================================================================
bool SetGroundHit(int g_num) {

	D3DXVECTOR3 player_botom_position = GetPlayer_Pos();//現在のプレイヤーの位置;
	player_botom_position.y -= 15.0f;//プレイヤー底面

	D3DXVECTOR3 next_player_botom_position = D3DXVECTOR3(player_botom_position.x, 0.0f, player_botom_position.z);//二次元変換
	D3DXVECTOR3 vector1, vector2, vector3;//三角形の三辺をベクトル化


	D3DXVECTOR3 hosen1, hosen2, hosen3;
	for (int i = 0; i < NumIndex - 2; i++)
	{
		vector1 = D3DXVECTOR3(ground[g_num].LookVtx[ground[g_num].LookIdxNum[i]].pos - ground[g_num].LookVtx[ground[g_num].LookIdxNum[i + 1]].pos);
		vector1.y = 0;

		vector2 = D3DXVECTOR3(ground[g_num].LookVtx[ground[g_num].LookIdxNum[i + 1]].pos - ground[g_num].LookVtx[ground[g_num].LookIdxNum[i + 2]].pos);
		vector2.y = 0;

		vector3 = D3DXVECTOR3(ground[g_num].LookVtx[ground[g_num].LookIdxNum[i + 2]].pos - ground[g_num].LookVtx[ground[g_num].LookIdxNum[i]].pos);
		vector3.y = 0;

		if (vector2 == D3DXVECTOR3(0.0f, 0.0f, 0.0f))
		{
			i += 2;
			continue;
		}

		D3DXVec3Cross(&hosen1, &(next_player_botom_position -
			(D3DXVECTOR3(ground[g_num].LookVtx[ground[g_num].LookIdxNum[i]].pos.x, 0.0f, ground[g_num].LookVtx[ground[g_num].LookIdxNum[i]].pos.z))), &vector1);

		D3DXVec3Cross(&hosen2, &(next_player_botom_position -
			(D3DXVECTOR3(ground[g_num].LookVtx[ground[g_num].LookIdxNum[i + 1]].pos.x, 0.0f, ground[g_num].LookVtx[ground[g_num].LookIdxNum[i + 1]].pos.z))), &vector2);



		//プレイヤーがポリゴン面の中にいるか
		if (hosen1.y >= 0 && hosen2.y >= 0 || hosen1.y <= 0 && hosen2.y <= 0)
		{			
			{
				D3DXVec3Cross(
					&hosen3,
					&(next_player_botom_position - (D3DXVECTOR3(ground[g_num].LookVtx[ground[g_num].LookIdxNum[i + 2]].pos.x, 0.0f, ground[g_num].LookVtx[ground[g_num].LookIdxNum[i + 2]].pos.z))),
					&vector3);


				if (hosen2.y >= 0 && hosen3.y >= 0 || hosen2.y <= 0 && hosen3.y <= 0)
				{
					/*if (hosen1.y == 0 || hosen2.y == 0 || hosen3.y == 0)
					{
						break;
					}*/
					{

						D3DXVECTOR3 naiseki1, naiseki2;
						naiseki1 = D3DXVECTOR3(
							player_botom_position.x - ground[g_num].LookVtx[ground[g_num].LookIdxNum[i]].pos.x,
							player_botom_position.y - ground[g_num].LookVtx[ground[g_num].LookIdxNum[i]].pos.y,
							player_botom_position.z - ground[g_num].LookVtx[ground[g_num].LookIdxNum[i]].pos.z);

						naiseki2 = D3DXVECTOR3(
							(player_botom_position.x + GetNormalPlayer_Pos().x) - ground[g_num].LookVtx[ground[g_num].LookIdxNum[i]].pos.x,
							(player_botom_position.y + GetNormalPlayer_Pos().y) - ground[g_num].LookVtx[ground[g_num].LookIdxNum[i]].pos.y,
							(player_botom_position.z + GetNormalPlayer_Pos().z) - ground[g_num].LookVtx[ground[g_num].LookIdxNum[i]].pos.z);

						D3DXVECTOR3 n;//法線ベクトル
						D3DXVECTOR3 AB = GetNormalPlayer_Pos();

						//ベクトル再設定
						vector1 = D3DXVECTOR3(ground[g_num].LookVtx[ground[g_num].LookIdxNum[i + 1]].pos - ground[g_num].LookVtx[ground[g_num].LookIdxNum[i]].pos);
						vector3 = D3DXVECTOR3(ground[g_num].LookVtx[ground[g_num].LookIdxNum[i + 2]].pos - ground[g_num].LookVtx[ground[g_num].LookIdxNum[i]].pos);



						D3DXVec3Cross(&n, &vector1, &vector3);//外積で面法線を計算

						//地面より下にいたら地面上に再設置
						if ((D3DXVec3Dot(&naiseki2, &n)) >= 0.00 && (D3DXVec3Dot(&naiseki1, &n)) <= 0.00 ||
							(D3DXVec3Dot(&naiseki2, &n)) <= 0.00 && (D3DXVec3Dot(&naiseki1, &n)) >= 0.00){

							

							{
								float before_y_pos = player_botom_position.y;
								player_botom_position.y += AB.y * (fabsf(D3DXVec3Dot(&n, &naiseki1) / (fabsf(D3DXVec3Dot(&n, &naiseki1) + fabsf(D3DXVec3Dot(&n, &naiseki2))))));
								
								if (player_botom_position.y != before_y_pos)
								{
									SetPlayer_Pos(player_botom_position.y + 15.0f);
								}								
								else
								{
									SetPlayer_Pos(before_y_pos + 15.0f);
								}
								return 0;//範囲内にいた
							}
						}
					

					}
				}
			}
		}

	}
	SetPlayer_Pos(player_botom_position.y + 15.0f);
	return 1;//範囲内にいなかった
}