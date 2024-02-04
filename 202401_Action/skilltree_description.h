//=============================================================================
// 
// スキルツリー説明文処理 [skilltree_description.h]
// Author : 相馬靜雅
// 
//=============================================================================

#ifndef _SKILLTREE_DESCRIPTION_H_
#define _SKILLTREE_DESCRIPTION_H_		// 二重インクルード防止のマクロを定義する

#include "object2D.h"

//==========================================================================
// クラス定義
//==========================================================================
// スキルツリー説明文定義
class CSkillTree_Description : public CObject2D
{
public:

	CSkillTree_Description(int nPriority = 8);
	~CSkillTree_Description();

	
	//  オーバーライドされた関数
	HRESULT Init(void) override;
	void Update(void) override;

	void SetIdxTex(int nIdx);	// テクスチャのインデックス番号
	static CSkillTree_Description* Create(void);

private:

	//=============================
	// メンバ関数
	//=============================
	HRESULT ReadTexture(void);

	//=============================
	// メンバ変数
	//=============================
	int m_nCurrentIdx;	// 現在のインデックス番号
	static std::vector<int> m_nTexIdx;	// テクスチャインデックス番号
	static bool m_bLoadComplete;		// ロード完了のフラグ
};

#endif