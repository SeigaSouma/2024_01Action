//=============================================================================
// 
// スキルツリーライン処理 [skilltree_line.h]
// Author : 相馬靜雅
// 
//=============================================================================

#ifndef _SKILLTREE_LINE_H_
#define _SKILLTREE_LINE_H_		// 二重インクルード防止のマクロを定義する

class CSkillTree_Icon;

#include "object2D.h"
#include "skilltree_ability.h"
#include "listmanager.h"

//==========================================================================
// クラス定義
//==========================================================================
// スキルツリーライン定義
class CSkillTree_Line : public CObject2D
{
public:

	/**
	@brief	スキルライン
	*/
	struct sSkillLine
	{
		
		MyLib::Vector3 mypos;		// 自分の位置
		MyLib::Vector3 parentpos;	// 親の位置

		// デフォルトコンストラクタ
		sSkillLine() : mypos(), parentpos() {}

		// パラメータつきコンストラクタ
		sSkillLine(const MyLib::Vector3& myposition, const MyLib::Vector3& parentposition) : mypos(myposition), parentpos(parentposition) {}
	};

	CSkillTree_Line(int nPriority = 7);
	~CSkillTree_Line();
	
	//  オーバーライドされた関数
	HRESULT Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;
	void SetVtx() override;

	void SetSkillLineInfo(const MyLib::Vector3& myposition, const MyLib::Vector3& parentposition);
	static CListManager<CSkillTree_Line> GetListObj() { return m_List; }	// リスト取得
	static CSkillTree_Line* Create(const MyLib::Vector3& myposition, const MyLib::Vector3& parentposition);
	static CSkillTree_Line* Create(CSkillTree_Icon* myIcon, CSkillTree_Icon* parentIcon);
private:

	sSkillLine m_SkillLine;	// スキルライン
	CSkillTree_Icon* m_pMyIcon;	// 自分のアイコン
	CSkillTree_Icon* m_pParentIcon;	// 親のアイコン
	static CListManager<CSkillTree_Line> m_List;	// リスト
};

#endif