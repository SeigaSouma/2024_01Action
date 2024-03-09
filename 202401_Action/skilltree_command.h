//=============================================================================
// 
// スキルツリーコマンド処理 [skilltree_command.h]
// Author : 相馬靜雅
// 
//=============================================================================

#ifndef _SKILLTREE_COMMAND_H_
#define _SKILLTREE_COMMAND_H_		// 二重インクルード防止のマクロを定義する

#include "object2D.h"

//==========================================================================
// クラス定義
//==========================================================================
// スキルツリーコマンドクラス定義
class CSkillTree_Command : public CObject
{
public:

	CSkillTree_Command(int nPriority = 8, const LAYER layer = LAYER::LAYER_2D);
	~CSkillTree_Command();
	
	//  オーバーライド関数
	HRESULT Init() override;
	void Uninit() override;
	void Update() override {}
	void Draw() override {}

	void SetAlpha(float alpha);	// 不透明度設定
	static CSkillTree_Command* Create();
private:

	//=============================
	// 列挙型定義
	//=============================
	enum ICONTYPE
	{
		ICONTYPE_BACK = 0,	// もどる
		ICONTYPE_GET,	// 習得
		ICONTYPE_RESET,	// リセット
		ICONTYPE_MAX
	};

	//=============================
	// メンバ変数
	//=============================
	CObject2D* m_pIcon[ICONTYPE_MAX];	// アイコンのオブジェクト
};

#endif