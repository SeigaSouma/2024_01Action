//=============================================================================
// 
// スキルツリーカーソル処理 [skilltree_cursor.h]
// Author : 相馬靜雅
// 
//=============================================================================

#ifndef _SKILLTREE_CURSOR_H_
#define _SKILLTREE_CURSOR_H_		// 二重インクルード防止のマクロを定義する

#include "object2D.h"

//==========================================================================
// クラス定義
//==========================================================================
// スキルツリーカーソルクラス定義
class CSkillTree_Cursor : public CObject2D
{
public:

	CSkillTree_Cursor(int nPriority = 8);
	~CSkillTree_Cursor();
	
	//  オーバーライド関数
	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
	void SetVtx(void) override;

	static CSkillTree_Cursor* Create(int nIdx);
private:

	int m_nMyPlayerIdx;		// プレイヤーインデックス番号
};

#endif