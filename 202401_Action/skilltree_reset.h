//=============================================================================
// 
// スキルツリーリセット処理 [skilltree_reset.h]
// Author : 相馬靜雅
// 
//=============================================================================

#ifndef _SKILLTREE_RESET_H_
#define _SKILLTREE_RESET_H_		// 二重インクルード防止のマクロを定義する

#include "object2D.h"

//==========================================================================
// クラス定義
//==========================================================================
// スキルツリーリセット定義
class CSkillTree_Reset : public CObject2D
{
public:

	CSkillTree_Reset(int nPriority = 8);
	~CSkillTree_Reset();
	
	//  オーバーライドされた関数
	HRESULT Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;

	void Kill();
	static CSkillTree_Reset* Create();

private:

	//=============================
	// メンバ関数
	//=============================
	enum SELECT
	{
		SELECT_CANCEL = 0,	// キャンセル
		SELECT_OK,			// OK
		SELECT_MAX
	};

	//=============================
	// メンバ関数
	//=============================
	void CreateSelect();	// 選択肢生成
	void UpdateSelect();	// 選択肢更新

	//=============================
	// メンバ変数
	//=============================
	float m_fFlashTime;	// 点滅時間
	int m_nSelect;	// 選択肢
	
	CObject2D* m_pSelectObj[SELECT_MAX];	// 選択肢のオブジェクト
	static std::vector<int> m_nTexIdx;		// テクスチャのインデックス番号
};

#endif