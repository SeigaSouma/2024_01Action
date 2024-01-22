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
	// メンバ関数
	//=============================

	//=============================
	// メンバ変数
	//=============================

};

#endif