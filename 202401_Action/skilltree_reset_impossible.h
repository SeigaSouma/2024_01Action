//=============================================================================
// 
// スキルツリーリセット不可能処理 [skilltree_reset_impossible.h]
// Author : 相馬靜雅
// 
//=============================================================================

#ifndef _SKILLTREE_RESET_IMPOSSIBLE_H_
#define _SKILLTREE_RESET_IMPOSSIBLE_H_		// 二重インクルード防止のマクロを定義する

#include "object2D.h"

//==========================================================================
// クラス定義
//==========================================================================
// スキルツリーリセット不可能定義
class CSkillTree_ResetImpossible : public CObject2D
{
public:

	//=============================
	// 列挙型定義
	//=============================
	enum ERRORTYPE
	{
		ERRORTYPE_POINT = 0,	// ポイント不足
		ERRORTYPE_NOGET,		// 未習得
		SELECT_MAX
	};

	CSkillTree_ResetImpossible(int nPriority = 9);
	~CSkillTree_ResetImpossible();
	
	//  オーバーライドされた関数
	HRESULT Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;

	static CSkillTree_ResetImpossible* Create(ERRORTYPE error);

private:

	//=============================
	// メンバ関数
	//=============================

	//=============================
	// メンバ変数
	//=============================
	ERRORTYPE m_ErrorType;	// エラーの種類
	float m_fAlphaTime;	// 不透明度タイマー
	static CSkillTree_ResetImpossible* m_pThisPtr;	// 自身のポインタ
};

#endif