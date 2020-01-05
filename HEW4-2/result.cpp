
#include "sprite.h"
#include "input.h"
#include "scene.h"
#include "texture.h"
#include "fade.h"
#include "RankSystem.h"
#include "sound.h"

void Result_Initialize(void)
{
	InitSound(GetHWND());
	PlaySound(SOUND_LABEL_BGM_RESULT);
	Fade_Start(false, 90, D3DCOLOR_RGBA(0, 0, 0, 0));
}

void Result_Finalize(void)
{
	UninitSound();
}

void Result_Update(void)
{
	if (Keyboard_IsTrigger(DIK_SPACE)) 
	{
		Scene_Change(SCENE_INDEX_TITLE);
	}

	if (Keyboard_IsTrigger(DIK_G))
	{
		Scene_Change(SCENE_INDEX_GAME);
	}

	// ÉXÉRÉAÇÃì«Ç›éÊÇË
	BainariRead();
}

void Result_Draw(void)
{
	Sprite_Draw(TEXTURE_INDEX_RESULT, 0.0f, 0.0f);
}
