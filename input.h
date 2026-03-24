//=============================================================================
//
//	入力処理 [input.h]
//	Author : SHUMA AIZU
// 
//=============================================================================

#ifndef _INPUT_H_
#define _INPUT_H_

#include "main.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define NUM_KEY_MAX			(256)				// キーの最大数
#define NUM_MOUSE_MAX		(3)					// ボタンの最大数
#define MAX_JOYPAD			(4)					// ジョイパッドの最大数
#define JOYSTICKVALUE_MAX	(32767)				// スティックの値の最大
#define JOYSTICKVALUE_MIN	(-32768)			// スティックの値の最小
#define CUSTOM_DEADZONE		(2048)				// デッドゾーン
#define ZLRTRIGGER			(255)				// ZLZRトリガーの値
#define LEFTTRIGGER			((WORD)0x0400)		// ZLトリガーのビット
#define RIGHTTRIGGER		((WORD)0x0800)		// ZRトリガーのビット

//*****************************************************************************
// ジョイパッドのキーの種類
//*****************************************************************************
typedef enum JOYKEY
{
	JOYKEY_UP = 0,				// 十字キー上
	JOYKEY_DOWN,				// 十字キー下
	JOYKEY_LEFT,				// 十字キー左
	JOYKEY_RIGHT,				// 十字キー右
	JOYKEY_START,				// スタートボタン
	JOYKEY_BACK,				// バックボタン
	JOYKEY_LEFT_THUMB,			// 左スティック押し込み
	JOYKEY_RIGHT_THUMB,			// 右スティック押し込み
	JOYKEY_LEFT_SHOULDER,		// Lボタン
	JOYKEY_RIGHT_SHOULDER,		// Rボタン
	JOYKEY_LEFT_TRIGGER,		// ZLトリガー
	JOYKEY_RIGHT_TRIGGER,		// ZRトリガー
	JOYKEY_A,					// Aボタン
	JOYKEY_B,					// Bボタン
	JOYKEY_X,					// Xボタン
	JOYKEY_Y,					// Yボタン
	JOYKEY_MAX					// 総数
}JOYKEY;

//*****************************************************************************
// ジョイパッドのスティックの種類
//*****************************************************************************
typedef enum JOYSTICK
{
	JOYSTICK_UP = 0,			// 上
	JOYSTICK_DOWN,				// 下
	JOYSTICK_LEFT,				// 左
	JOYSTICK_RIGHT,				// 右
	JOYSTICK_MAX
}JOYSTICK;

//**********************************************************************************
//*** マウスのボタンの種類 ***
//**********************************************************************************
typedef enum MOUSEKEY
{
	MOUSEKEY_LEFT = 0,		// 左クリック
	MOUSEKEY_RIGHT,			// 右クリック
	MOUSEKEY_WHEEL,			// 中クリック
	MOUSEKEY_MAX
}MOUSEKEY;

//*****************************************************************************
// ジョイパッド情報構造体定義
//*****************************************************************************
typedef struct JoypadState
{
	XINPUT_STATE JoykeyState;					// ジョイパッドのプレス情報
	XINPUT_STATE JoykeyStateTrigger;			// ジョイパッドのトリガー情報
	XINPUT_STATE JoykeyStateRelease;			// ジョイパッドのリリース情報
	XINPUT_STATE JoykeyStateRepeat;				// ジョイパッドのリピート情報
	XINPUT_VIBRATION JoypadVibration;			// ジョイパッドの振動情報
	int nVibCounter;							// 振動カウンター
	bool bJoyStickL[JOYSTICK_MAX];				// スティックの情報
	bool bJoyStickR[JOYSTICK_MAX];				// スティックの情報
	int nRepeatKeyCounter[JOYKEY_MAX];			// リピートカウント (ボタン)
	int nRepeatStickCounterL[JOYSTICK_MAX];		// リピートカウント (Lスティック)
	int nRepeatStickCounterR[JOYSTICK_MAX];		// リピートカウント (Rスティック)
}JoypadState;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitKeyboard(HINSTANCE hInstance, HWND hWnd);
void UninitKeyboard(void);
void UpdateKeyboard(void);
bool GetKeyboardPress(int nKey);
bool GetKeyboardTrigger(int nKey);
bool GetKeyboardRelease(int nKey);
bool GetKeyboardRepeat(int nKey);
bool GetKeyboardAny(void);

HRESULT InitJoypad(void);
void UninitJoypad(void);
void UpdateJoypad(void);
bool GetJoypadPress(JOYKEY key, int nIdx = 0);
bool GetJoypadTrigger(JOYKEY key, int nIdx = 0);
bool GetJoypadRelease(JOYKEY key, int nIdx = 0);
bool GetJoypadRepeat(JOYKEY key, int nIdx = 0);
bool GetJoypadAny(int nIdx = 0);
bool GetJoypadLeftTriggePress(int nIdx = 0);
bool GetJoypadRightTriggePress(int nIdx = 0);
bool GetJoypadStickPressL(JOYSTICK stick, int nIdx = 0);
bool GetJoypadStickPressR(JOYSTICK stick, int nIdx = 0);
bool GetJoypadStickRepeatL(JOYSTICK stick, int nIdx = 0);
bool GetJoypadStickRepeatR(JOYSTICK stick, int nIdx = 0);
bool GetJoypadStickLeft(float* pValueH, float* pValueV, int nIdx = 0);
bool GetJoypadStickRight(float* pValueH, float* pValueV, int nIdx = 0);
bool GetJoypadVibration(int nIdx = 0);
void SetJoypadVibration(int nLVibration, int nRVibration, int nVibCounter, int nIdx = 0);
JoypadState* GetJoypadState(int nIdx = 0);
bool GetJoypadControl(void);

HRESULT InitMouse(HWND hWnd);
void UninitMouse(void);
void UpdateMouse(void);
bool GetMousePress(int nButton);
bool GetMouseTrigger(int nButton);
bool GetMouseRelease(int nButton);
POINT GetMousePos(void);

#endif
