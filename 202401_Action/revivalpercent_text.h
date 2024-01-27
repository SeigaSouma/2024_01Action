//=============================================================================
// 
// 復活確率テキスト処理 [revivalpercent_text.h]
// Author : 相馬靜雅
// 
//=============================================================================

#ifndef _REVIVALPERCENT_TEXT_H_
#define _REVIVALPERCENT_TEXT_H_		// 二重インクルード防止のマクロを定義する

class CMultiNumber;

#include "object2D.h"

//==========================================================================
// クラス定義
//==========================================================================
// 復活確率テキスト画面
class CRevivalPercentText : public CObject2D
{
public:

	CRevivalPercentText(float fadetime, int nPriority = 8);
	~CRevivalPercentText();
	
	//  オーバーライドされた関数
	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;

	static CRevivalPercentText* Create(const float fadetime);	// 生成処理

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
	typedef void(CRevivalPercentText::* STATE_FUNC)(void);
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
	CMultiNumber* m_apNumber;	// 数字のオブジェクト
	const float m_fFadeTime;	// フェードにかかる時間


};

#endif