//=============================================================================
// 
// 死亡テキスト処理 [deadtext.h]
// Author : 相馬靜雅
// 
//=============================================================================

#ifndef _DEAD_TEXT_H_
#define _DEAD_TEXT_H_		// 二重インクルード防止のマクロを定義する

#include "object2D.h"

//==========================================================================
// クラス定義
//==========================================================================
// 死亡画面
class CDeadText : public CObject2D
{
public:

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

	CDeadText(float fadein, float fadeout, int nPriority = 8);
	~CDeadText();
	
	//  オーバーライドされた関数
	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;

	void SetState(eState state) { m_state = state; }	// 状態設定
	static CDeadText* Create(const float fadein, const float fadeout);		// 生成処理

private:

	//=============================
	// 関数ポインタ
	//=============================
	typedef void(CDeadText::* STATE_FUNC)(void);
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
	const float m_fFadeInTime;	// フェードにかかる時間
	const float m_fFadeOutTime;	// フェードにかかる時間


};

#endif