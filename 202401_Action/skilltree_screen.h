//=============================================================================
// 
// スキルツリー画面処理 [skilltree_screen.h]
// Author : 相馬靜雅
// 
//=============================================================================

#ifndef _SKILLTREE_SCREEN_H_
#define _SKILLTREE_SCREEN_H_		// 二重インクルード防止のマクロを定義する

#include "object2D.h"

//==========================================================================
// クラス定義
//==========================================================================
// スキルツリー画面定義
class CSkillTree_Screen : public CObject2D
{
public:

	CSkillTree_Screen(int nPriority = 8);
	~CSkillTree_Screen();

	
	//  オーバーライドされた関数
	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
	void SetVtx(void);
	static CSkillTree_Screen* Create(void);

private:

	//=============================
	// 列挙型定義
	//=============================
	// 状態列挙
	enum eState
	{
		STATE_NONE = 0,	// なにもなし
		STATE_FADEIN,	// フェードイン
		STATE_FADEOUT,	// フェードアウト
		STATE_MAX
	};

	//=============================
	// 関数リスト
	//=============================
	typedef void(CSkillTree_Screen::*STATE_FUNC)(void);	// 状態処理のリスト
	static STATE_FUNC m_StateFuncList[];

	//=============================
	// メンバ関数
	//=============================
	void StateNone(void);	// 何もない状態
	void StateFadeIn(void);	// フェードイン状態
	void StateFadeOut(void);// フェードアウト状態

	// メンバ変数
	float m_fStateTime;	// 状態遷移カウンター
	eState m_state;		// 状態

};

#endif