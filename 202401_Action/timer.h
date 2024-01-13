//=============================================================================
// 
//  タイマーヘッダー [timer.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _TIMER_H_
#define _TIMER_H_	// 二重インクルード防止

#include "main.h"

//==========================================================================
// 前方宣言
//==========================================================================
class CMultiNumber;
class CObject2D;
class CObjectCircleGauge2D;

//==========================================================================
// 定数定義
//==========================================================================
namespace
{
	const int NUM_CIRCLE = 10;	// 円の数
	const float START_TIME = 120;	// 初期時間
}

//==========================================================================
// クラス定義
//==========================================================================
// タイマークラス定義
class CTimer
{
public:

	enum eState
	{
		STATE_WAIT = 0,		// 待機
		STATE_APPEARANCE,	// 出現
		STATE_ADD_LITTLE,	// 少し加算
		STATE_ADJ,			// 調整
		STATE_MAX
	};

	CTimer(int nPriority = 6);
	~CTimer();

	static CTimer *Create(void);
	static CTimer *Create(MyLib::Vector3 pos);
	static CTimer *GetInstance(void) { return m_pTimer; }

	HRESULT Init(MyLib::Vector3 pos);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	float GetTime(void) { return m_fTime; }

	CTimer::eState GetState(void) { return m_state; }
	void SetState(eState state) { m_state = state; }

private:

	// 関数リスト
	typedef void(CTimer::*STATE_FUNC)(void);
	static STATE_FUNC m_StateFuncList[];	// 状態関数リスト

	// メンバ関数
	void StateWait(void);
	void StatAppearance(void);
	void StatAddLittle(void);
	void StateAdjustment(void);

	// メンバ変数
	eState m_state;					// 状態
	float m_fStateTime;				// 状態時間
	float m_fTime;					// 時間
	MyLib::Vector3 m_pos;				// 位置
	MyLib::Vector3 m_posOrigin;		// 元の位置
	bool m_bAddTime;	// タイマー加算のフラグ
	int m_nProgress;	// 進行状況
	CObjectCircleGauge2D *m_apCircle[NUM_CIRCLE];	// 数字がわりの円
	static CTimer *m_pTimer;	// 自身のポインタ
};



#endif