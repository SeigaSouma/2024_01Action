//=============================================================================
// 
// タイトルエンター [title_pressenter.h]
// Author : 相馬靜雅
// 
//=============================================================================

#ifndef _TITLE_PRESSENTER_H_
#define _TITLE_PRESSENTER_H_		// 二重インクルード防止のマクロを定義する

#include "object2D.h"
class CTutorialCheckShould;

//==========================================================================
// クラス定義
//==========================================================================
// タイトルエンター
class CTitle_PressEnter : public CObject2D
{
public:

	CTitle_PressEnter(float fadetime, int nPriority = 8);
	~CTitle_PressEnter() {}
	
	//  オーバーライドされた関数
	HRESULT Init() override;
	void Update() override;

	static CTitle_PressEnter* Create(float fadetime);	// 生成処理

private:

	//=============================
	// 列挙型定義
	//=============================
	enum eState
	{
		STATE_NONE = 0, // なにもなし
		STATE_FADEIN,	// フェードイン
		STATE_MAX
	};

	//=============================
	// 関数ポインタ
	//=============================
	typedef void(CTitle_PressEnter::* STATE_FUNC)();
	static STATE_FUNC m_StateFunc[];

	//=============================
	// メンバ関数
	//=============================
	void StateNone();		// なにもなし
	void StateFadeIn();		// フェードイン

	//=============================
	// メンバ変数
	//=============================
	eState m_state;		// 状態
	float m_fStateTime;	// 状態カウンター
	const float m_fFadeOutTime;	// フェードにかかる時間
	CTutorialCheckShould* m_pTutorialCheck;	// チュートリアルやるか確認
};

#endif