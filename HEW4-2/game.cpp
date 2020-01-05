#include <time.h>
#include "myDirect3D.h"
#include "main.h"
#include "input.h"
#include "camera.h"
#include "camera2.h"
#include "light.h"
#include "debugproc.h"
#include "texture.h"
#include "sprite.h"
#include "score.h"
#include "meter.h"
#include "ground.h"
#include "scene.h"
#include "fade.h"
#include "Player.h"
#include "grid.h"
#include "drink.h"
#include "particle.h"
#include "speedline.h"
#include "shadow.h"
#include "skydome.h"
#include "RankSystem.h"
#include "back_ground.h"
#include "sound.h"

typedef enum PHASE_INDEX
{
	PHASE_INDEX_FADE,
	PHASE_INDEX_PLAYER_IN,
	PHASE_INDEX_PLAYER_MUTEKI,
	PHASE_INDEX_PLAYER_NORMAL,
	PHASE_INDEX_STAGE_CLEAR,
	PHASE_INDEX_END,

	PHASE_INDEX_MAX
};

static PHASE_INDEX g_Phase;
ParticleManager particle;
SpeedlineManager line;

static int g_GameFrameCount;

static int g_Score = 0;// 点数
static int g_KillCount = 0;


static bool Game_EndCheck(void);



void Game_Initialize(void)
{
	// カメラの初期化処理
	Camera_Initialize();
	//Camera2_Initialize();

	// ライトの初期化処理
	Light_Initialize();

	//グリッドの初期化処理
	//Grid_Initialize();

	// スカイドーム
	Skydome_Initialize();

	// バックグラウンド
	Back_Initialize();

	// 影
	Shadow_Initialize();

	// プレイヤー
	Player_Initialize();

	// グラウンド
	Ground_Initialize();

	// ドリンク
	Drink_Initialize();

	// スコア
	InitScore();

	// パーティクル
	particle.InitParticle();

	// スピードライン
	line.InitSpeedline();

	// メーター
	Meter_Initialize();

	InitSound(GetHWND());
	PlaySound(SOUND_LABEL_BGM_GAME);
}

void Game_Finalize(void)
{
	UninitSound();
	// カメラの終了処理
	Camera_Finalize();
	//Camera2_Finalize();

	// ライトの終了処理
	Light_Finalize();

	// グリッドの終了
	//Grid_Finalize();

	// スカイドーム
	Skydome_Finalize();

	// バックグラウンド
	Back_Finalize();

	// 影
	Shadow_Finalize();

	// プレイヤー
	Player_Finalize();

	// グラウンド
	//Ground_Finalize();

	// ドリンク
	Drink_Finalize();

	// スコア
	UninitScore();

	// パーティクル
	particle.UninitParticle();

	// スピードライン
	line.UninitSpeedline();

	// メーター
	Meter_Finalize();

	// スコア書き込み
	BainariWrite();
}

void Game_Update(void)
{
	// カメラの更新処理
	Camera_Update();
	//Camera2_Update();

	// ライトの更新処理
	Light_Update();

	//グリッド線
	//Grid_Update();

	// スカイドーム
	Skydome_Update();

	// バックグラウンド
	Back_Update();

	// 影
	Shadow_Update();

	// プレイヤー
	Player_Update();

	// グラウンド
	//Ground_Update();

	// ドリンク
	Drink_Update();

	// スコア
	UpdateScore();	

	// パーティクル
	particle.UpdateParticle();

	// スピードライン
	line.UpdateSpeedline();

	// メーター
	Meter_Update();

	if (Keyboard_IsTrigger(DIK_SPACE))
	{
		Scene_Change(SCENE_INDEX_RESULT);
	}

}

void Game_Draw(void)
{
	// カメラの設定
	Camera_SetCamera();
	//Camera2_SetCamera();

	//グリッドの描画
	//Grid_Draw();

	// スカイドーム
	Skydome_Draw();

	// バックグラウンド
	Back_Draw();

	// グラウンド
	Ground_Draw();

	// 影
	Shadow_Draw();

	// プレイヤー
	Player_Draw();

	// ドリンク
	Drink_Draw();

	// スコア
	DrawScore();

	// パーティクル
	particle.DrawParticle();

	// スピードライン
	line.DrawSpeedline();

	// メーター
	Meter_Draw();
}



bool Game_EndCheck(void)
{

}



void Game_AddScore(int score)
{
	//g_Score += score;
}

void Game_AddKillCount()
{
	//g_KillCount += 1;
}
