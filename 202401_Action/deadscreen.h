//=============================================================================
// 
// 死亡画面処理 [deadscreen.h]
// Author : 相馬靜雅
// 
//=============================================================================

#ifndef _DEAD_SCREEN_H_
#define _DEAD_SCREEN_H_		// 二重インクルード防止のマクロを定義する

#include "object2D.h"

//==========================================================================
// クラス定義
//==========================================================================
// 死亡画面
class CDeadScreen : public CObject2D
{
public:

	CDeadScreen(int nPriority = 8);
	~CDeadScreen();
	
	//  オーバーライドされた関数
	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;

	static CDeadScreen* Create(void);	// 生成処理

private:

	//=============================
	// メンバ関数
	//=============================
	enum eState
	{
		STATE_NONE = 0, // なにもなし
		STATE_FADEIN,	// フェードイン
		STATE_FADEOUT,	// フェードアウト
		STATE_MAX
	};

	//=============================
	// 関数ポインタ
	//=============================
	typedef void(CDeadScreen::* STATE_FUNC)(void);
	static STATE_FUNC m_StateFunc[];

	//=============================
	// メンバ関数
	//=============================
	void StateNone(void);		// なにもなし
	void StateFadeIn(void);		// フェードイン
	void StateFadeOut(void);	// フェードアウト

	//=============================
	// メンバ変数
	//=============================
	eState m_state;		// 状態
	float m_fStateTime;	// 状態カウンター


};

#endif