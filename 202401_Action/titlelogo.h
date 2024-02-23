//=============================================================================
// 
// タイトルロゴ処理 [titlelogo.h]
// Author : 相馬靜雅
// 
//=============================================================================

#ifndef _TITLELOGO_H_
#define _TITLELOGO_H_		// 二重インクルード防止のマクロを定義する

#include "object2D.h"


//==========================================================================
// クラス定義
//==========================================================================
// タイトルロゴ
class CTitleLogo : public CObject2D
{
public:

	//=============================
	// 列挙型定義
	//=============================
	enum eState
	{
		STATE_NONE = 0, // なにもなし
		STATE_FADEIN,	// フェードイン
		STATE_MAX
	};

	CTitleLogo(float fadetime, int nPriority = 8);
	~CTitleLogo();
	
	//  オーバーライドされた関数
	HRESULT Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;

	void SetState(eState state) { m_state = state; }	// 状態設定
	static CTitleLogo* Create(float fadetime);	// 生成処理

private:

	//=============================
	// 関数ポインタ
	//=============================
	typedef void(CTitleLogo::* STATE_FUNC)();
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

};

#endif