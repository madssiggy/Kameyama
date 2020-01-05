#include "scene.h"
#include "texture.h"
#include <d3d9.h>
#include "sprite.h"
#include "player.h"

static float rotation = 0.0f;

void Meter_Initialize(void)
{
}

void Meter_Finalize(void)
{
}

void Meter_Update(void)
{
	rotation = GetMeter_Pos() / MAXSPEED * D3DX_PI * 3.0f / 2.0f - D3DX_PI;
}

void Meter_Draw(void)
{
	// メーター
	Sprite_Draw(TEXTURE_INDEX_METER, 128.0f, 350.0f, 0.0f, 0.0f, 256.0f, 256.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);
	// ニードル
	Sprite_Draw(TEXTURE_INDEX_NEEDLE, 258.0f, 480.0f, 0.0f, 0.0f, 20.0f, 100.0f, 10.0f, 70.0f, 1.0f, 1.0f, rotation);
}
