//=============================================================================
// 
//  スキルツリーヘッダー [skilltree.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _SKILLTREE_H_
#define _SKILLTREE_H_	// 二重インクルード防止

#include "object.h"
#include "skilltree_icon.h"

class CSkillTree_Screen;
class CSkillTree_Cursor;
class CSkillTree_Line;
class CSkillTree_Command;
class CSkillTree_Window;
class CSkillTree_Description;
class CSkillTree_Reset;

//==========================================================================
// クラス定義
//==========================================================================
// スキルツリークラス定義
class CSkillTree : public CObject
{
public:

	//=============================
	// 列挙型定義
	//=============================
	// 状態列挙
	enum eState
	{
		STATE_NONE = 0,	// なにもなし
		STATE_FADEIN,	// フェードイン
		STATE_FADEOUT,	// フェードアウト
		STATE_RESET,	// リセット
		STATE_MAX
	};

	CSkillTree(int nPriority = 8);
	~CSkillTree();

	// オーバーライドされた関数
	HRESULT Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;

	void Kill();	// 削除
	void LoadJson();	// Jsonからのロード
	void SaveJson();	// Jsonへのセーブ
	void SetScreen();	// スクリーン上に設定
	void OutScreen();	// スクリーンから捌ける
	void OutReset();	// リセット捌ける

	eState GetState() { return m_state; }	// 状態取得
	void SetMastering(int nIdx, CSkillTree_Icon::eMastering mastering);	// 習得状況設定
	void SetMastering(std::vector<CSkillTree_Icon::eMastering> mastering);	// 習得状況設定
	std::vector<CSkillTree_Icon::eMastering> GetMastering();			// 習得状況取得
	void ResetMastering();												// 習得状況リセット
	std::vector<CSkillTree_Icon*> GetIcon() const;	// アイコン取得
	std::vector<CSkillTree_Icon::sSkillIcon> GetIconInfo() { return m_SkillInfo; }
	CSkillTree_Description* GetDescription() { return m_pDescription; }	// 説明文取得

	static CSkillTree* GetInstance() { return m_pThisPtr; }	// インスタンス取得
	static CSkillTree* Create();

	// JSONからの読み込み
	void from_json(const json& j)
	{
		for (const auto& loadData : j.at("iconinfo"))
		{
			CSkillTree_Icon::sSkillIcon iconinfo;
			iconinfo.from_json(loadData);
			m_SkillInfo.push_back(iconinfo);
		}
	}

	// スキルアイコンのJSONへの書き込み
	void to_json(json& j) const
	{
		j["iconinfo"] = json::array(); // 空の配列を作成

		for (const auto& loadData : m_SkillInfo)
		{
			json iconinfo;
			loadData.to_json(iconinfo);
			j["iconinfo"].emplace_back(iconinfo);
		}
	}
private:

	//=============================
	// 関数リスト
	//=============================
	typedef void(CSkillTree::* STATE_FUNC)();	// 状態処理のリスト
	static STATE_FUNC m_StateFuncList[];

	//=============================
	// メンバ関数
	//=============================
	void StateNone();	// 何もない状態
	void StateFadeIn();	// フェードイン状態
	void StateFadeOut();// フェードアウト状態
	void StateReset();		// リセット状態

	//=============================
	// メンバ変数
	//=============================
	std::vector<CSkillTree_Icon::sSkillIcon> m_SkillInfo;	// スキルアイコン
	std::vector<CSkillTree_Icon*> m_pSkillIcon;				// スキルアイコン

	//=============================
	// メンバ変数
	//=============================
	float m_fAlpha;		// 不透明度
	float m_fStateTime;	// 状態遷移カウンター
	eState m_state;		// 状態
	CSkillTree_Screen* m_pScreen;	// スクリーンのオブジェクト
	CSkillTree_Cursor* m_pCursor;	// カーソルのオブジェクト
	CSkillTree_Command* m_pCommand;	// コマンドのオブジェクト
	CSkillTree_Window* m_pWindow;	// ウィンドウのオブジェクト
	CSkillTree_Description* m_pDescription;	// 説明文のオブジェクト
	CSkillTree_Reset* m_pReset;				// リセット用のオブジェクト
	bool m_bOnScreen;				// スクリーン上にいるかのフラグ
	static CSkillTree* m_pThisPtr;	// 自身のポインタ
};



#endif