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

	CRevivalPercentText(float fadetime, int nPriority = 8);
	~CRevivalPercentText();
	
	//  オーバーライドされた関数
	HRESULT Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;

	void Kill();
	void SetState(eState state) { m_state = state; }	// 状態設定
	static CRevivalPercentText* Create(int respawnpercent, const float fadetime);	// 生成処理

private:

	//=============================
	// 関数ポインタ
	//=============================
	typedef void(CRevivalPercentText::* STATE_FUNC)();
	static STATE_FUNC m_StateFunc[];

	//=============================
	// メンバ関数
	//=============================
	// 状態系
	void StateNone();		// なにもなし
	void StateFadeIn();		// フェードイン
	void StateFadeOut();	// フェードアウト

	// その他関数
	void SetNumberObj(int number);

	//=============================
	// メンバ変数
	//=============================
	eState m_state;		// 状態
	float m_fStateTime;	// 状態カウンター
	CMultiNumber* m_apNumber;	// 数字のオブジェクト
	const float m_fFadeTime;	// フェードにかかる時間


};

#endif