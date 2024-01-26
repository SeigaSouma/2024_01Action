//=============================================================================
// 
// スキルツリーアイコン処理 [skilltree_icon.h]
// Author : 相馬靜雅
// 
//=============================================================================

#ifndef _SKILLTREE_ICON_H_
#define _SKILLTREE_ICON_H_		// 二重インクルード防止のマクロを定義する

#include "object2D.h"
#include "skilltree_ability.h"
#include "listmanager.h"

class CAbillityStrategy;

//==========================================================================
// クラス定義
//==========================================================================
// スキルツリーアイコン定義
class CSkillTree_Icon : public CObject2D
{
public:
	//=============================
	// 列挙型定義
	//=============================
	// 習得列挙
	enum eMastering
	{
		MASTERING_YET = 0,		// 未習得
		MASTERING_DONE,			// 習得済み
		MASTERING_POSSIBLE,		// 習得可能
		MASTERING_SAMPLEDONE,	// お試し習得
		MASTERING_MAX
	};

	// JSONからの読み込み
	void from_json(const json& j)
	{
		j.at("mastering").get_to(m_Mastering);
	}

	// JSONへの書き込み
	void to_json(json& j) const
	{
		j = json
		{
			{"mastering", m_Mastering},
		};
	}

	// 状態列挙
	enum eState
	{
		STATE_NONE = 0,	// なにもない
		STATE_MAX
	};

	/**
	@brief	スキルアイコン
	*/
	struct sSkillIcon
	{
		int ID;				// 自分のID
		int parentID;		// 親のID
		int texID;			// テクスチャインデックス
		int needpoint;		// 必要ポイント
		CSkillTree_Ability::eSkillCategory skillCategory;	// スキルのカテゴリー
		int skillType;		// スキルの種類
		int skillStage;		// スキルのステージ
		MyLib::Vector3 pos;	// 位置
		eMastering mastering;	// 習得状態

		// デフォルトコンストラクタ
		sSkillIcon() : ID(0), parentID(0), texID(0), needpoint(0), pos(), skillCategory(), skillType(0), skillStage(0), mastering() {}

		// パラメータつきコンストラクタ
		sSkillIcon(int nID,int nParentID, int nTexID, int nNeedPoint, const MyLib::Vector3& position, const CSkillTree_Ability::eSkillCategory& category, int type, int stage, const eMastering& master)
			: ID(nID), parentID(nParentID), texID(nTexID), needpoint(nNeedPoint), pos(position), skillCategory(category), skillType(type), skillStage(stage), mastering(master) {}

		// JSONからの読み込み
		void from_json(const json& j)
		{
			j.at("ID").get_to(ID);
			j.at("parentID").get_to(parentID);
			j.at("texID").get_to(texID);
			j.at("needpoint").get_to(needpoint);
			j.at("pos").get_to(pos);
			j.at("skillcategory").get_to(skillCategory);
			j.at("skilltype").get_to(skillType);
			j.at("skillstage").get_to(skillStage);
		}

		// JSONへの書き込み
		void to_json(json& j) const
		{
			j = json
			{
				{"ID", ID},
				{"parentID", parentID},
				{"texID", texID},
				{"needpoint", needpoint},
				{"pos", pos},
				{"skillcategory", skillCategory},
				{"skilltype", skillType},
				{"skillstage", skillStage},
			};
		}
	};

	CSkillTree_Icon(int nPriority = 8);
	~CSkillTree_Icon();
	
	//  オーバーライドされた関数
	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
	void SetVtx(void) override;

	void SetIconInfo(sSkillIcon iconinfo);	// アイコン情報設定
	sSkillIcon GetIconInfo(void);			// アイコン情報取得
	void SetMastering(eMastering mastering) { m_Mastering = mastering; }	// 習得状況設定
	CSkillTree_Icon::eMastering GetMastering(void) { return m_Mastering; }	// 習得状態取得

	void BindStartAvillity(void);			// 初期能力割り当て
	bool BindAvillity(void);				// 能力割り当て
	static CSkillTree_Icon* Create(sSkillIcon iconinfo);
	static CListManager<CSkillTree_Icon> GetListObj(void) { return m_List; }	// リスト取得

private:

	//=============================
	// 関数リスト
	//=============================
	typedef void(CSkillTree_Icon::*STATE_FUNC)(void);	// 状態処理のリスト
	static STATE_FUNC m_StateFuncList[];

	//=============================
	// メンバ関数
	//=============================
	void StateNone(void);	// 何もない状態
	HRESULT ReadTexture(void);

	//=============================
	// メンバ変数
	//=============================
	float m_fStateTime;			// 状態遷移カウンター
	eState m_state;				// 状態
	eMastering m_Mastering;		// 習得状態
	sSkillIcon m_SkillIconInfo;	// スキルアイコン情報
	static std::vector<int> m_nTexIdx;	// テクスチャインデックス番号
	static bool m_bLoadComplete;		// ロード完了のフラグ
	CAbillityStrategy* m_pAbillity;		// 能力のオブジェクト
	static CListManager<CSkillTree_Icon> m_List;	// リスト

};

#endif