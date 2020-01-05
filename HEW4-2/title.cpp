#include "input.h"
#include "main.h"
#include "scene.h"
#include "texture.h"
#include "fade.h"
#include <d3d9.h>
#include "sprite.h"
#include "RankSystem.h"
#include "sound.h"


static int color = 0;
static float size = 0.0f;
static bool size_flag = true;
static bool color_flag = true;


void Title_Initialize(void)
{
	InitSound(GetHWND());
	PlaySound(SOUND_LABEL_BGM_TITLE);
}

void Title_Finalize(void)
{
	UninitSound();
}

void Title_Update(void)
{
	// 透明にする・元に戻す
	if(color_flag)color+=2;
	else color -= 2;
	if (color > 252) color_flag = false;
	if (color < 10) color_flag = true;

	// サイズの拡大縮小
	if(size_flag) size += 0.03f;
	else size -= 0.03f;
	if (size < 0.1f)size_flag = true;
	if (size > 0.3f)size_flag = false;

	if (Keyboard_IsTrigger(DIK_SPACE)) {
		Scene_Change(SCENE_INDEX_GAME);
	}

	
}

void Title_Draw(void)
{
	// タイトルBG
	Sprite_Draw(TEXTURE_INDEX_BG, 0.0f, 0.0f);
	// タイトル文字
	Sprite_Draw(TEXTURE_INDEX_TITLE, 700.0f, 250.0f, 0.0f, 0.0f, 256.0f, 64.0f, 128.0f, 32.0f, 3.5f+size, 3.5f + size, 0.0f);
	// 透過
	Sprite_SetColor(D3DCOLOR_RGBA(255, 255, 255, 255-color));
	// スタートを押して
	Sprite_Draw(TEXTURE_INDEX_PRESS, 500.0f, 420.0f, 0.0f, 0.0f, 256.0f, 64.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);
	// α値を戻す
	Sprite_SetColor(D3DCOLOR_RGBA(255, 255, 255, 255));
}
