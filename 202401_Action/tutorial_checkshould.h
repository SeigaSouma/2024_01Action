//=============================================================================
// 
// チュートリアルをするか確認処理 [tutorial_checkshould.h]
// Author : 相馬靜雅
// 
//=============================================================================

#ifndef _TUTORIAL_CHECKSHOULD_H_
#define _TUTORIAL_CHECKSHOULD_H_		// 二重インクルード防止のマクロを定義する

#include "object2D.h"

//==========================================================================
// クラス定義
//==========================================================================
// チュートリアル確認クラス
class CTutorialCheckShould : public CObject2D
{
public:

	CTutorialCheckShould(int nPriority = 8);
	~CTutorialCheckShould();
	
	//  オーバーライドされた関数
	HRESULT Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;

	void Kill();
	static CTutorialCheckShould* Create();

private:

	//=============================
	// メンバ関数
	//=============================
	enum SELECT
	{
		SELECT_NO = 0,	// そのままゲーム
		SELECT_OK,		// 練習ステージ
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
	int m_nSelect;		// 選択肢
	
	CObject2D* m_pSelectObj[SELECT_MAX];	// 選択肢のオブジェクト
	static std::vector<int> m_nTexIdx;		// テクスチャのインデックス番号
};

#endif