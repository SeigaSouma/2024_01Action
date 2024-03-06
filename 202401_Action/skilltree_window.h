//=============================================================================
// 
// スキルツリーウィンドウ処理 [skilltree_window.h]
// Author : 相馬靜雅
// 
//=============================================================================

#ifndef _SKILLTREE_WINDOW_H_
#define _SKILLTREE_WINDOW_H_		// 二重インクルード防止のマクロを定義する

#include "object2D.h"

//==========================================================================
// クラス定義
//==========================================================================
// スキルツリーウィンドウ定義
class CSkillTree_Window : public CObject2D
{
public:

	CSkillTree_Window(int nPriority = 8);
	~CSkillTree_Window();

	
	//  オーバーライドされた関数
	HRESULT Init() override;

	static CSkillTree_Window* Create();

};

#endif