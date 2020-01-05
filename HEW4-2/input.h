
#pragma once

#include <Windows.h>
#define DIRECTINPUT_VERSION (0x0800)
#include <dinput.h>

using namespace std;
/* game pad��� */


// JoyCon�C���f�b�N�X
#define LEFT_JOYCON 0
#define RIGHT_JOYCON 1

#define BUTTON_UP		0x00000001l	// �����L�[��(.IY<0)
#define BUTTON_DOWN		0x00000002l	// �����L�[��(.IY>0)
#define BUTTON_LEFT		0x00000004l	// �����L�[��(.IX<0)
#define BUTTON_RIGHT	0x00000008l	// �����L�[�E(.IX>0)
#define BUTTON_A		0x00000010l	// �`�{�^��(.rgbButtons[0]&0x80)
#define BUTTON_B		0x00000020l	// �a�{�^��(.rgbButtons[1]&0x80)
#define BUTTON_C		0x00000040l	// �b�{�^��(.rgbButtons[2]&0x80)
#define BUTTON_X		0x00000080l	// �w�{�^��(.rgbButtons[3]&0x80)
#define BUTTON_Y		0x00000100l	// �x�{�^��(.rgbButtons[4]&0x80)
#define BUTTON_Z		0x00000200l	// �y�{�^��(.rgbButtons[5]&0x80)
#define BUTTON_L		0x00000400l	// �k�{�^��(.rgbButtons[6]&0x80)
#define BUTTON_R		0x00000800l	// �q�{�^��(.rgbButtons[7]&0x80)
#define BUTTON_START	0x00001000l	// �r�s�`�q�s�{�^��(.rgbButtons[8]&0x80)
#define BUTTON_M		0x00002000l	// �l�{�^��(.rgbButtons[9]&0x80)
#define GAMEPADMAX		2			// �����ɐڑ�����W���C�p�b�h�̍ő吔���Z�b�g

// Joycon�X�e�B�b�N
#define JOYCON_STICK_DOWN 0x00004000l  
#define JOYCON_STICK_UP 0x00008000l  
#define JOYCON_STICK_RIGHT 0x00016000l  
#define JOYCON_STICK_LEFT 0x00032000l 

// JoyCon��
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

// JoyCon�E
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

	// �����x
	DWORD	 Accelerometer;
	// �W���C��(x,y,z)
	DWORD Gyro[3];
	// �{�^�����
	DWORD State;
	// �{�^����ԁiTrigger�j
	DWORD Trigger;

public:

	LPDIRECTINPUTDEVICE8 Device = NULL;

	bool Initialize(HINSTANCE hInstance, HWND hWnd);

	void Finalize();

	void Update();

	BOOL IsPress(DWORD button);

	BOOL IsTrigger(DWORD button);

	int GetTrigger();

	// �����x�ݒ�
	void SetAccelerometer(DWORD data);
	// �W���C���ݒ�
	void SetGyro(DWORD x, DWORD y, DWORD z);

	// Get�W���C��X
	int GetGyro_X();
	// Get�W���C��Y
	int GetGyro_Y();
	// Get�W���C��Z
	int GetGyro_Z();
	// Get�����x
	int GetAccelerometer();
};


// JoyCon
extern JoyCon joycon[GAMEPADMAX];

bool isRotation();
