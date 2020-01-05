
#include "input.h"

#define	NUM_KEY_MAX			(256)

// game pad用設定値
#define DEADZONE		2500			// 各軸の25%を無効ゾーンとする
#define RANGE_MAX		1000			// 有効範囲の最大値
#define RANGE_MIN		-1000			// 有効範囲の最小値

static bool initialize(HINSTANCE hInstance);
static void finalize(void);

LPDIRECTINPUT8			g_pInput = NULL;
JoyCon joycon[GAMEPADMAX];
//キーボード
static LPDIRECTINPUTDEVICE8	g_pDevKeyboard = NULL;
static BYTE					g_aKeyState[NUM_KEY_MAX];
static BYTE					g_aKeyStateTrigger[NUM_KEY_MAX];
static BYTE					g_aKeyStateRelease[NUM_KEY_MAX];

//ゲームパッド
static LPDIRECTINPUTDEVICE8	g_pGamePad[GAMEPADMAX] = { NULL, NULL};// パッドデバイス
static DWORD				g_padState[GAMEPADMAX];	// パッド情報（複数対応）
static DWORD				g_padTrigger[GAMEPADMAX];
static int					g_padCount = 0;			// 検出したパッドの数
// Joycon数


bool initialize(HINSTANCE hInstance)
{
	if(g_pInput == NULL) {

		if(FAILED(DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&g_pInput, NULL))) {

			return false;
		}
	}

	return true;
}

void finalize(void)
{
	if(g_pInput != NULL) {
		g_pInput->Release();
		g_pInput = NULL;
	}
}

bool Keyboard_Initialize(HINSTANCE hInstance, HWND hWnd)
{
	if( !initialize(hInstance) ) {

		MessageBox(hWnd, "DirectInputオブジェクトが作れねぇ！", "警告！", MB_ICONWARNING);
		return false;
	}

	if(FAILED(g_pInput->CreateDevice(GUID_SysKeyboard, &g_pDevKeyboard, NULL)))
	{
		MessageBox(hWnd, "キーボードがねぇ！", "警告！", MB_ICONWARNING);
		return false;
	}

	if(FAILED(g_pDevKeyboard->SetDataFormat(&c_dfDIKeyboard)))
	{
		MessageBox(hWnd, "キーボードのデータフォーマットを設定できませんでした。", "警告！", MB_ICONWARNING);
		return false;
	}
	//													 DISCL_BACKGROUND | DISCL_EXCLUSIVE	
	if(FAILED(g_pDevKeyboard->SetCooperativeLevel(hWnd, (DISCL_FOREGROUND | DISCL_NONEXCLUSIVE))))
	{
		MessageBox(hWnd, "キーボードの協調モードを設定できませんでした。", "警告！", MB_ICONWARNING);
		return false;
	}

	g_pDevKeyboard->Acquire();

	return true;
}

void Keyboard_Finalize(void)
{
	if(g_pDevKeyboard != NULL)
	{
		g_pDevKeyboard->Unacquire();

		g_pDevKeyboard->Release();
		g_pDevKeyboard = NULL;
	}

	finalize();
}

void Keyboard_Update(void)
{
	BYTE aKeyState[NUM_KEY_MAX];

	if(SUCCEEDED(g_pDevKeyboard->GetDeviceState(sizeof(aKeyState), aKeyState)))
	{
		for(int nCnKey = 0; nCnKey < NUM_KEY_MAX; nCnKey++)
		{
			g_aKeyStateTrigger[nCnKey] = (g_aKeyState[nCnKey] ^ aKeyState[nCnKey]) & aKeyState[nCnKey];
			g_aKeyStateRelease[nCnKey] = (g_aKeyState[nCnKey] ^ aKeyState[nCnKey]) & g_aKeyState[nCnKey];

			g_aKeyState[nCnKey] = aKeyState[nCnKey];
		}
	}
	else
	{
		g_pDevKeyboard->Acquire();
	}
}

bool Keyboard_IsPress(int nKey)
{
	return (g_aKeyState[nKey] & 0x80) ? true : false;
}

bool Keyboard_IsTrigger(int nKey)
{
	return (g_aKeyStateTrigger[nKey] & 0x80) ? true: false;
}

bool Keyboard_IsRelease(int nKey)
{
	return (g_aKeyStateRelease[nKey] & 0x80) ? true: false;
}

//---------------------------------------- コールバック関数
BOOL CALLBACK SearchGamePadCallback(LPDIDEVICEINSTANCE lpddi, LPVOID)
{
	HRESULT result;

	result = g_pInput->CreateDevice(lpddi->guidInstance, &joycon[g_padCount++].Device, NULL);
	return DIENUM_CONTINUE;	// 次のデバイスを列挙
}
//
//bool GamePad_Initialize(HINSTANCE hInstance, HWND hWnd)
//{
//	if (!initialize(hInstance)) {
//
//		MessageBox(hWnd, "DirectInputオブジェクトが作れねぇ！", "警告！", MB_ICONWARNING);
//		return false;
//	}
//
//	HRESULT		result;
//	int			i;
//
//	g_padCount = 0;
//	// ジョイパッドを探す
//	g_pInput->EnumDevices(DI8DEVCLASS_GAMECTRL, (LPDIENUMDEVICESCALLBACK)SearchGamePadCallback, NULL, DIEDFL_ATTACHEDONLY);
//	// セットしたコールバック関数が、パッドを発見した数だけ呼ばれる。
//
//	for (i = 0; i<g_padCount; i++) 
//	{
//		// ジョイスティック用のデータ・フォーマットを設定
//		result = g_pGamePad[i]->SetDataFormat(&c_dfDIJoystick);
//		if (FAILED(result))
//			return false; // データフォーマットの設定に失敗
//
//		// モードを設定（フォアグラウンド＆非排他モード）
//		//		result = pGamePad[i]->SetCooperativeLevel(hWindow, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
//		//		if ( FAILED(result) )
//		//			return false; // モードの設定に失敗
//
//		// 軸の値の範囲を設定
//		// X軸、Y軸のそれぞれについて、オブジェクトが報告可能な値の範囲をセットする。
//		// (max-min)は、最大10,000(?)。(max-min)/2が中央値になる。
//		// 差を大きくすれば、アナログ値の細かな動きを捕らえられる。(パッドの性能による)
//		DIPROPRANGE				diprg;
//		ZeroMemory(&diprg, sizeof(diprg));
//		diprg.diph.dwSize = sizeof(diprg);
//		diprg.diph.dwHeaderSize = sizeof(diprg.diph);
//		diprg.diph.dwHow = DIPH_BYOFFSET;
//		diprg.lMin = RANGE_MIN;
//		diprg.lMax = RANGE_MAX;
//		// X軸の範囲を設定
//		diprg.diph.dwObj = DIJOFS_X;
//		g_pGamePad[i]->SetProperty(DIPROP_RANGE, &diprg.diph);
//		// Y軸の範囲を設定
//		diprg.diph.dwObj = DIJOFS_Y;
//		g_pGamePad[i]->SetProperty(DIPROP_RANGE, &diprg.diph);
//
//		// 各軸ごとに、無効のゾーン値を設定する。
//		// 無効ゾーンとは、中央からの微少なジョイスティックの動きを無視する範囲のこと。
//		// 指定する値は、10000に対する相対値(2000なら20パーセント)。
//		DIPROPDWORD				dipdw;
//		dipdw.diph.dwSize = sizeof(DIPROPDWORD);
//		dipdw.diph.dwHeaderSize = sizeof(dipdw.diph);
//		dipdw.diph.dwHow = DIPH_BYOFFSET;
//		dipdw.dwData = DEADZONE;
//		//X軸の無効ゾーンを設定
//		dipdw.diph.dwObj = DIJOFS_X;
//		g_pGamePad[i]->SetProperty(DIPROP_DEADZONE, &dipdw.diph);
//		//Y軸の無効ゾーンを設定
//		dipdw.diph.dwObj = DIJOFS_Y;
//		g_pGamePad[i]->SetProperty(DIPROP_DEADZONE, &dipdw.diph);
//
//		//ジョイスティック入力制御開始
//		g_pGamePad[i]->Acquire();
//	}
//
//	return true;
//}
//
//void GamePad_Finalize(void)
//{
//	for (int i = 0; i<GAMEPADMAX; i++) {
//		if (g_pGamePad[i])
//		{
//			g_pGamePad[i]->Unacquire();
//			g_pGamePad[i]->Release();
//		}
//	}
//
//	finalize();
//}
//
//void GamePad_Update(void)
//{
//	HRESULT			result;
//	DIJOYSTATE2		dijs;
//	int				i;
//
//	for (i = 0; i<g_padCount; i++)
//	{
//		DWORD lastPadState;
//		lastPadState = g_padState[i];
//		g_padState[i] = 0x00000000l;	// 初期化
//
//		result = g_pGamePad[i]->Poll();	// ジョイスティックにポールをかける
//		if (FAILED(result)) {
//			result = g_pGamePad[i]->Acquire();
//			while (result == DIERR_INPUTLOST)
//				result = g_pGamePad[i]->Acquire();
//		}
//
//		result = g_pGamePad[i]->GetDeviceState(sizeof(DIJOYSTATE), &dijs);	// デバイス状態を読み取る
//		if (result == DIERR_INPUTLOST || result == DIERR_NOTACQUIRED) {
//			result = g_pGamePad[i]->Acquire();
//			while (result == DIERR_INPUTLOST)
//				result = g_pGamePad[i]->Acquire();
//		}
//
//		// ３２の各ビットに意味を持たせ、ボタン押下に応じてビットをオンにする
//		//* y-axis (forward)
//		if (dijs.lY < 0)					g_padState[i] |= BUTTON_UP;
//		//* y-axis (backward)
//		if (dijs.lY > 0)					g_padState[i] |= BUTTON_DOWN;
//		//* x-axis (left)
//		if (dijs.lX < 0)					g_padState[i] |= BUTTON_LEFT;
//		//* x-axis (right)
//		if (dijs.lX > 0)					g_padState[i] |= BUTTON_RIGHT;
//		//* Ａボタン
//		if (dijs.rgbButtons[0] & 0x80)	g_padState[i] |= BUTTON_A;
//		//* Ｂボタン
//		if (dijs.rgbButtons[1] & 0x80)	g_padState[i] |= BUTTON_B;
//		//* Ｃボタン
//		if (dijs.rgbButtons[2] & 0x80)	g_padState[i] |= BUTTON_C;
//		//* Ｘボタン
//		if (dijs.rgbButtons[3] & 0x80)	g_padState[i] |= BUTTON_X;
//		//* Ｙボタン
//		if (dijs.rgbButtons[4] & 0x80)	g_padState[i] |= BUTTON_Y;
//		//* Ｚボタン
//		if (dijs.rgbButtons[5] & 0x80)	g_padState[i] |= BUTTON_Z;
//		//* Ｌボタン
//		if (dijs.rgbButtons[6] & 0x80)	g_padState[i] |= BUTTON_L;
//		//* Ｒボタン
//		if (dijs.rgbButtons[7] & 0x80)	g_padState[i] |= BUTTON_R;
//		//* ＳＴＡＲＴボタン
//		if (dijs.rgbButtons[8] & 0x80)	g_padState[i] |= BUTTON_START;
//		//* Ｍボタン
//		if (dijs.rgbButtons[9] & 0x80)	g_padState[i] |= BUTTON_M;
//
//		// Trigger設定
//		g_padTrigger[i] = ((lastPadState ^ g_padState[i])	// 前回と違っていて
//			& g_padState[i]);					// しかも今ONのやつ
//
//	}
//}
//
//BOOL GamePad_IsPress(int padNo, DWORD button)
//{
//	return (button & g_padState[padNo]);
//}
//
//BOOL GamePad_IsTrigger(int padNo, DWORD button)
//{
//	return (button & g_padTrigger[padNo]);
//}

bool JoyCon::Initialize(HINSTANCE hInstance, HWND hWnd)
{

	if (!initialize(hInstance)) {

		MessageBox(hWnd, "DirectInput Object Not Found ", "WARNING！", MB_ICONWARNING);
		return false;
	}

	HRESULT		result;

	g_pInput->EnumDevices(DI8DEVCLASS_GAMECTRL, (LPDIENUMDEVICESCALLBACK)SearchGamePadCallback, NULL, DIEDFL_ATTACHEDONLY);

	result = Device->SetDataFormat(&c_dfDIJoystick);

	for (int i = 0; i < GAMEPADMAX; i++) {

		if (FAILED(result))
			return false;

		DIPROPRANGE				diprg;
		ZeroMemory(&diprg, sizeof(diprg));
		diprg.diph.dwSize = sizeof(diprg);
		diprg.diph.dwHeaderSize = sizeof(diprg.diph);
		diprg.diph.dwHow = DIPH_BYOFFSET;
		diprg.lMin = RANGE_MIN;
		diprg.lMax = RANGE_MAX;

		// X軸の範囲を設定
		diprg.diph.dwObj = DIJOFS_X;
		joycon[i].Device->SetProperty(DIPROP_RANGE, &diprg.diph);
		// Y軸の範囲を設定
		diprg.diph.dwObj = DIJOFS_Y;
		joycon[i].Device->SetProperty(DIPROP_RANGE, &diprg.diph);

		// Rz軸の範囲を設定
		diprg.diph.dwObj = DIJOFS_RZ;
		joycon[i].Device->SetProperty(DIPROP_RANGE, &diprg.diph);

		// Slider1の範囲を設定
		diprg.diph.dwObj = DIJOFS_SLIDER(0);
		joycon[i].Device->SetProperty(DIPROP_RANGE, &diprg.diph);

		// Slider2の範囲を設定
		diprg.diph.dwObj = DIJOFS_SLIDER(1);
		joycon[i].Device->SetProperty(DIPROP_RANGE, &diprg.diph);

		DIPROPDWORD				dipdw;
		dipdw.diph.dwSize = sizeof(DIPROPDWORD);
		dipdw.diph.dwHeaderSize = sizeof(dipdw.diph);
		dipdw.diph.dwHow = DIPH_BYOFFSET;
		dipdw.dwData = DEADZONE;
		//X軸の無効ゾーンを設定
		dipdw.diph.dwObj = DIJOFS_X;
		joycon[i].Device->SetProperty(DIPROP_DEADZONE, &dipdw.diph);
		//Y軸の無効ゾーンを設定
		dipdw.diph.dwObj = DIJOFS_Y;
		joycon[i].Device->SetProperty(DIPROP_DEADZONE, &dipdw.diph);

		// Rz軸の無効ゾーンを設定
		dipdw.diph.dwObj = DIJOFS_RZ;
		joycon[i].Device->SetProperty(DIPROP_DEADZONE, &dipdw.diph);

		// Slider1の無効ゾーンを設定
		dipdw.diph.dwObj = DIJOFS_SLIDER(0);
		joycon[i].Device->SetProperty(DIPROP_DEADZONE, &dipdw.diph);

		// Slider2の無効ゾーンを設定
		dipdw.diph.dwObj = DIJOFS_SLIDER(1);
		joycon[i].Device->SetProperty(DIPROP_DEADZONE, &dipdw.diph);

		joycon[i].Device->Acquire();
	}

	return true;
}

void JoyCon::Finalize()
{

	if (Device)
	{
		Device->Unacquire();
		Device->Release();
	}

	finalize();
}

void JoyCon::Update()
{
	HRESULT			result;
	DIJOYSTATE2		dijs;

	DWORD lastPadState;
	lastPadState = State;
	State = 0x00000000l;

	result = Device->Poll();

	if (FAILED(result)) {
		result = Device->Acquire();
		while (result == DIERR_INPUTLOST)
			result = Device->Acquire();
	}

	result = Device->GetDeviceState(sizeof(DIJOYSTATE), &dijs);
	if (result == DIERR_INPUTLOST || result == DIERR_NOTACQUIRED) {
		result = Device->Acquire();
		while (result == DIERR_INPUTLOST)
			result = Device->Acquire();
	}

	// JoyCon
	if (dijs.lY > 0) State |= JOYCON_STICK_UP;
	if (dijs.lY < 0) State |= JOYCON_STICK_DOWN;
	if (dijs.lX < 0) State |= JOYCON_STICK_LEFT;
	if (dijs.lX > 0) State |= JOYCON_STICK_RIGHT;

	if (dijs.rgbButtons[0] & 0x80)	State |= JOYCON_DOWN;
	if (dijs.rgbButtons[1] & 0x80)	State |= JOYCON_UP;
	if (dijs.rgbButtons[2] & 0x80)	State |= JOYCON_RIGHT;
	if (dijs.rgbButtons[3] & 0x80)	State |= JOYCON_LEFT;
	if (dijs.rgbButtons[4] & 0x80)State |= JOYCON_SR_LEFT;
	if (dijs.rgbButtons[5] & 0x80)	State |= JOYCON_SL_LEFT;
	if (dijs.rgbButtons[6] & 0x80) State |= JOYCON_L;
	if (dijs.rgbButtons[7] & 0x80)	State |= JOYCON_ZL;
	if (dijs.rgbButtons[8] & 0x80)	State |= JOYCON_MIN;
	if (dijs.rgbButtons[11] & 0x80) State |= JOYCON_L3;
	if (dijs.rgbButtons[13] & 0x80)	State |= JOYCON_SCREENSHOT;
	if (dijs.rgbButtons[16] & 0x80)	State |= JOYCON_Y;
	if (dijs.rgbButtons[17] & 0x80)	State |= JOYCON_X;
	if (dijs.rgbButtons[18] & 0x80)	State |= JOYCON_B;
	if (dijs.rgbButtons[19] & 0x80)	State |= JOYCON_A;
	if (dijs.rgbButtons[20] & 0x80)	State |= JOYCON_SR_RIGHT;
	if (dijs.rgbButtons[21] & 0x80)	State |= JOYCON_SL_RIGHT;
	if (dijs.rgbButtons[22] & 0x80)	State |= JOYCON_R;
	if (dijs.rgbButtons[23] & 0x80)	State |= JOYCON_ZR;
	if (dijs.rgbButtons[25] & 0x80)	State |= JOYCON_PLUS;
	if (dijs.rgbButtons[26] & 0x80)	State |= JOYCON_R3;
	if (dijs.rgbButtons[28] & 0x80)	State |= JOYCON_HOME;

	// 加速度設定
	SetAccelerometer(dijs.lRz);
	// ジャイロ設定
	SetGyro(dijs.lRz, dijs.rglSlider[0], dijs.rglSlider[1]);

	// Trigger設定
	Trigger = ((lastPadState ^ State)	& State);

}

void JoyCon::SetAccelerometer(DWORD data) {
	Accelerometer = data;
}

void JoyCon::SetGyro(DWORD x, DWORD y, DWORD z) {
	Gyro[0] = x;
	Gyro[1] = y;
	Gyro[2] = z;
}

int JoyCon::GetGyro_X() {
	return (int)Gyro[0];
}

int JoyCon::GetGyro_Y() {
	return (int)Gyro[1];
}

int JoyCon::GetGyro_Z() {
	return (int)Gyro[2];
}

int JoyCon::GetAccelerometer() {
	return Accelerometer;
}

int JoyCon::GetTrigger() {
	return Trigger;
}

BOOL JoyCon::IsPress(DWORD button)
{
	return (button & State);
}

BOOL JoyCon::IsTrigger(DWORD button)
{
	return (button & Trigger);
}

bool isRotation() {
	if (joycon[0].GetAccelerometer() > 600 || joycon[0].GetAccelerometer() < -600)
	{
		return true;
	}
	return false;
}