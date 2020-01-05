
#pragma once

#include <Windows.h>
#define DIRECTINPUT_VERSION (0x0800)
#include <dinput.h>

using namespace std;
/* game pad情報 */


// JoyConインデックス
#define LEFT_JOYCON 0
#define RIGHT_JOYCON 1

#define BUTTON_UP		0x00000001l	// 方向キー上(.IY<0)
#define BUTTON_DOWN		0x00000002l	// 方向キー下(.IY>0)
#define BUTTON_LEFT		0x00000004l	// 方向キー左(.IX<0)
#define BUTTON_RIGHT	0x00000008l	// 方向キー右(.IX>0)
#define BUTTON_A		0x00000010l	// Ａボタン(.rgbButtons[0]&0x80)
#define BUTTON_B		0x00000020l	// Ｂボタン(.rgbButtons[1]&0x80)
#define BUTTON_C		0x00000040l	// Ｃボタン(.rgbButtons[2]&0x80)
#define BUTTON_X		0x00000080l	// Ｘボタン(.rgbButtons[3]&0x80)
#define BUTTON_Y		0x00000100l	// Ｙボタン(.rgbButtons[4]&0x80)
#define BUTTON_Z		0x00000200l	// Ｚボタン(.rgbButtons[5]&0x80)
#define BUTTON_L		0x00000400l	// Ｌボタン(.rgbButtons[6]&0x80)
#define BUTTON_R		0x00000800l	// Ｒボタン(.rgbButtons[7]&0x80)
#define BUTTON_START	0x00001000l	// ＳＴＡＲＴボタン(.rgbButtons[8]&0x80)
#define BUTTON_M		0x00002000l	// Ｍボタン(.rgbButtons[9]&0x80)
#define GAMEPADMAX		2			// 同時に接続するジョイパッドの最大数をセット

// Joyconスティック
#define JOYCON_STICK_DOWN 0x00004000l  
#define JOYCON_STICK_UP 0x00008000l  
#define JOYCON_STICK_RIGHT 0x00016000l  
#define JOYCON_STICK_LEFT 0x00032000l 

// JoyCon左
#define JOYCON_DOWN 0x00000001l  
#define JOYCON_UP 0x00000002l 
#define JOYCON_RIGHT 0x00000004l 
#define JOYCON_LEFT 0x00000008l 
#define JOYCON_SR_LEFT 0x00000010l 
#define JOYCON_SL_LEFT 0x00000020l 
#define JOYCON_L 0x00000040l 
#define JOYCON_ZL 0x00000080l 
#define JOYCON_MIN 0x000000100l
#define JOYCON_L3  0x00000800l 
#define JOYCON_SCREENSHOT 0x00002000l 

// JoyCon右
#define JOYCON_Y 0x00000001l 
#define JOYCON_X 0x00000002l 
#define JOYCON_B 0x00000004l  
#define JOYCON_A 0x00000008l 
#define JOYCON_SR_RIGHT 0x000000010l  
#define JOYCON_SL_RIGHT 0x000000020l  
#define JOYCON_R  0x000000040l 
#define JOYCON_ZR  0x000000080l 
#define JOYCON_PLUS 0x000000200l
#define JOYCON_R3 0x000000400l 
#define JOYCON_HOME   0x0000001000l 


bool Keyboard_Initialize(HINSTANCE hInstance, HWND hWnd);
void Keyboard_Finalize(void);
void Keyboard_Update(void);

bool Keyboard_IsPress(int nKey);
bool Keyboard_IsTrigger(int nKey);
bool Keyboard_IsRelease(int nKey);

//---------------------------- game pad
bool GamePad_Initialize(HINSTANCE hInstance, HWND hWnd);
void GamePad_Finalize(void);
void GamePad_Update(void);

BOOL GamePad_IsPress(int padNo, DWORD button);
BOOL GamePad_IsTrigger(int padNo, DWORD button);

class JoyCon {

private:

	// 加速度
	DWORD	 Accelerometer;
	// ジャイロ(x,y,z)
	DWORD Gyro[3];
	// ボタン状態
	DWORD State;
	// ボタン状態（Trigger）
	DWORD Trigger;

public:

	LPDIRECTINPUTDEVICE8 Device = NULL;

	bool Initialize(HINSTANCE hInstance, HWND hWnd);

	void Finalize();

	void Update();

	BOOL IsPress(DWORD button);

	BOOL IsTrigger(DWORD button);

	int GetTrigger();

	// 加速度設定
	void SetAccelerometer(DWORD data);
	// ジャイロ設定
	void SetGyro(DWORD x, DWORD y, DWORD z);

	// GetジャイロX
	int GetGyro_X();
	// GetジャイロY
	int GetGyro_Y();
	// GetジャイロZ
	int GetGyro_Z();
	// Get加速度
	int GetAccelerometer();
};


// JoyCon
extern JoyCon joycon[GAMEPADMAX];

bool isRotation();
