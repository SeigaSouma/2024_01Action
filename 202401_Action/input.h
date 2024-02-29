//=============================================================================
//
// 入力処理 [input.h]
// Author : 相馬靜雅
//
//=============================================================================

#ifndef _INPUT_H_	//このマクロ定義がされていなかったら
#define _INPUT_H_	//二重インクルード防止のマクロを定義する

#include "main.h"
#include "constans.h"
#pragma comment(lib, "xinput.lib")	//コントローラーに必要

//==========================================================================
// マクロ定義
//==========================================================================
#define NUM_KEY_MAX	(256)	// キーの最大数

//==========================================================================
// クラス定義
//==========================================================================
// 入力クラス
class CInput
{
public:
	CInput();
	virtual ~CInput();

	virtual HRESULT Init(HINSTANCE hInstance, HWND hWnd);
	virtual void Uninit();
	virtual void Update() = 0;

protected:
	static int nNumAll;					// 総数
	static LPDIRECTINPUT8 m_pInput;		// DirectInputオブジェクトへのポインタ
	LPDIRECTINPUTDEVICE8 m_pDevice;		// 入力デバイスへのポインタ
};

// キーボードクラス
class CInputKeyboard : public CInput
{
public:

	CInputKeyboard();
	~CInputKeyboard();

	HRESULT Init(HINSTANCE hInstance, HWND hWnd);
	void Uninit();
	void Update();
	bool GetPress(int nKey);
	bool GetTrigger(int nKey);
	bool GetRelease(int nKey);
	bool GetRepeat(int nKey, int nPattern);

private:
	int m_nPatternRepeat[NUM_KEY_MAX];		// リピ―トの間隔
	int m_nCntKeyRepeat[NUM_KEY_MAX];		// リピート用カウント
	BYTE m_aKeyState[NUM_KEY_MAX];			// プレス情報
	BYTE m_aKeyStateTrigger[NUM_KEY_MAX];	// トリガー情報
	BYTE m_aKeyStateRelease[NUM_KEY_MAX];	// リリース情報
	BYTE m_aKeyStateRepeat[NUM_KEY_MAX];	// リピート情報
};

// ジョイパッドクラス
class CInputGamepad
{
public:

	// 列挙型定義
	typedef enum
	{
		BUTTON_UP = 0,
		BUTTON_DOWN,
		BUTTON_LEFT,
		BUTTON_RIGHT,
		BUTTON_START,
		BUTTON_BACK,
		BUTTON_LSTICKPUSH,
		BUTTON_RSTICKPUSH,
		BUTTON_LB,
		BUTTON_RB,
		BUTTON_LT,
		BUTTON_RT,
		BUTTON_A,
		BUTTON_B,
		BUTTON_X,
		BUTTON_Y,
		BUTTON_LX,
		BUTTON_LY,
		BUTTON_18,
		BUTTON_19,
		BUTTON_MAX
	}BUTTON;

	// 列挙型定義
	enum STICK
	{
		STICK_X = 0,
		STICK_Y,
		STICK_MAX
	};

	typedef enum
	{
		VIBRATION_STATE_NONE = 0,	//何もしてない状態
		VIBRATION_STATE_ENEMYHIT,	//ダメージ状態
		VIBRATION_STATE_DMG,		//ダメージ状態
		VIBRATION_STATE_ITEM,		//アイテム
		VIBRATION_STATE_RESPAWN,	// リスポーン

	}VIBRATION_STATE;

	CInputGamepad();
	~CInputGamepad();

	HRESULT Init(HINSTANCE hInstance, HWND hWnd);
	void Uninit();
	void Update();

	// ボタン系
	bool GetPress(BUTTON nKey, int nCntPlayer);
	bool GetTrigger(BUTTON nKey, int nCntPlayer);
	bool GetRepeat(BUTTON nKey, int nCntPlayer);
	bool GetRelease(int nKey, int nCntPlayer);

	// トリガー系
	bool GetPressLT(int nCntPlayer);	// LTのプレス判定
	bool GetPressRT(int nCntPlayer);	// RTのプレス判定
	bool GetTriggerLT(int nCntPlayer);	// LTのトリガー判定
	bool GetTriggerRT(int nCntPlayer);	// RTのトリガー判定

	// スティック系
	bool GetLStickTrigger(STICK XY);	// スティックのトリガー判定
	bool GetRStickTrigger(STICK XY);	// スティックのトリガー判定
	MyLib::Vector3 GetStickMoveL(int nCntPlayer);
	MyLib::Vector3 GetStickMoveR(int nCntPlayer);
	MyLib::Vector3 GetStickPositionRatioL(int nCntPlayer);	// 左スティックの割合取得
	MyLib::Vector3 GetStickPositionRatioR(int nCntPlayer);	// 右スティックの割合取得
	float GetStickRotL(int nCntPlayer);					// スティックの向き取得
	float GetStickRotR(int nCntPlayer);					// スティックの向き取得
	bool IsTipStick() { return m_bLStickTip; }		// スティックが倒れてるかの判定

	// バイブレーション系
	void SetEnableVibration();
	bool GetEnableVibration();
	void SetVibration(VIBRATION_STATE VibState, int nCntPlayer);
	int GetnCntPad();

private:

	void UpdateStickTrigger();		// スティックのトリガー判定
	void UpdateTriggerState(int nCntPlayer, XINPUT_STATE inputstate);	// トリガーの判定処理

	// メンバ変数
	XINPUT_STATE m_aGamepadState[mylib_const::MAX_PLAYER];				// プレス情報
	XINPUT_STATE m_aGamepadStateTrigger[mylib_const::MAX_PLAYER];		// トリガー情報
	XINPUT_STATE m_aGamepadStateRepeat[mylib_const::MAX_PLAYER];		// リピート情報
	XINPUT_STATE m_aGamepadStateRelease[mylib_const::MAX_PLAYER];		// リリース情報
	XINPUT_VIBRATION m_aGamepadStateVib[mylib_const::MAX_PLAYER];		// バイブレーション
	VIBRATION_STATE m_VibrationState[mylib_const::MAX_PLAYER];			// 振動の種類
	int m_nCntVibration[mylib_const::MAX_PLAYER];						// 振動の時間
	int m_nMaxCntVibration[mylib_const::MAX_PLAYER];					// 振動の時間
	int m_nCntPadrepeat;									// リピート用カウント

	// トリガー
	struct sTrigger
	{
		bool bPress;	// プレス判定
		bool bTrigger;	// トリガー判定

		sTrigger() :bPress(false), bTrigger(false) {}
	};
	sTrigger m_StateLT[mylib_const::MAX_PLAYER];	// LTの判定
	sTrigger m_StateRT[mylib_const::MAX_PLAYER];	// RTの判定

	// スティック
	bool m_bLeftStickSelect[STICK_MAX];						// 左スティックの選択判定
	bool m_bLeftStickTrigger[STICK_MAX];					// 左スティックのトリガー判定
	bool m_bRightStickSelect[STICK_MAX];					// 右スティックの選択判定
	bool m_bRightStickTrigger[STICK_MAX];					// 右スティックのトリガー判定
	bool m_bLStickTip;										// 左スティックの傾き判定
	bool m_bVibrationUse;									// バイブを使用するかどうか
};

// マウスクラス
class CInputMouse : public CInput
{
public:

	// 列挙型定義
	typedef enum
	{
		BUTTON_LEFT = 0,
		BUTTON_RIGHT,
		BUTTON_WHEEL,
		BUTTON_SIDE_1,
		BUTTON_SIDE_2
	}BUTTON;

	CInputMouse();
	~CInputMouse();

	HRESULT Init(HINSTANCE hInstance, HWND hWnd);
	void Uninit();
	void Update();
	bool GetPress(BUTTON nKey);
	MyLib::Vector3 GetMouseMove();

private:
	DIMOUSESTATE2 m_MouseState;			//全入力情報の保管
	DIMOUSESTATE2 m_MouseStateTrigger;	//トリガー情報
	DIMOUSESTATE2 m_MouseStateRerease;	//リリース情報
	DIMOUSESTATE2 m_MouseStateRepeat;	//リピート情報
	BYTE aOldState[8];					//前回の入力情報を保存
};

#endif