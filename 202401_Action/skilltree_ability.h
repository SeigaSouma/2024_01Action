//=============================================================================
// 
// スキルツリー能力処理 [skilltree_ability.h]
// Author : 相馬靜雅
// 
//=============================================================================

#ifndef _SKILLTREE_ABILITY_H_
#define _SKILLTREE_ABILITY_H_		// 二重インクルード防止のマクロを定義する

//==========================================================================
// クラス定義
//==========================================================================
// スキルツリー能力
class CSkillTree_Ability
{
public:

	//=============================
	// 列挙型定義
	//=============================
	// カテゴリー
	enum eSkillCategory
	{
		SKILLCATEGORY_BUFF = 0,	// バフ
		SKILLCATEGORY_UNLOCK,	// アンロック
		SKILLCATEGORY_MAX
	};


	//=============================
	// 構造体定義
	//=============================
	// スキルのステージ
	struct sSkillStage
	{
		int stage;	// 段階

		// デフォルトコンストラクタ
		sSkillStage() : stage(0) {}

		// パラメータつきコンストラクタ
		sSkillStage(int stage)
			: stage(stage) {}

		
		// JSONからの読み込み
		void from_json(const json& j)
		{
			j.at("stage").get_to(stage);	// 段階
		}

		// JSONへの書き込み
		void to_json(json& j) const
		{
			j = json
			{
				{"stage", stage},	// 段階
			};
		}
	};

	// スキルの種類
	struct sSkillType
	{
		std::string typeName;	// 種類の名前
		std::vector<sSkillStage> stage;	// スキルのステージ

		// デフォルトコンストラクタ
		sSkillType() {}

		// パラメータつきコンストラクタ
		sSkillType(const std::string& name, const std::vector<sSkillStage>& stagelist)
			: typeName(name), stage(stagelist) {}

		
		// JSONからの読み込み
		void from_json(const json& j)
		{
			j.at("typeName").get_to(typeName);	// 種類の名前

			// スキルのステージ
			for (const auto& type : j.at("skillstage"))
			{
				sSkillStage loacalstage;
				loacalstage.from_json(type);
				stage.push_back(loacalstage);
			}
		}

		// JSONへの書き込み
		void to_json(json& j) const
		{
			j = json
			{
				{"typeName", typeName},	// 種類の名前
			};

			j["skillstage"] = json::array(); // 空の配列を作成

			// スキルのステージ
			for (const auto& type : stage)
			{
				json jsonData;
				type.to_json(jsonData);

				j["skillstage"].emplace_back(jsonData);
			}
		}

	};

	// スキルのカテゴリー
	struct sSkillCategory
	{
		eSkillCategory categoryType;;	// カテゴリーの列挙
		std::vector<sSkillType> type;	// スキルの種類

		// デフォルトコンストラクタ
		sSkillCategory() : categoryType(SKILLCATEGORY_BUFF) {}

		// パラメータつきコンストラクタ
		sSkillCategory(eSkillCategory type, const std::vector<sSkillType>& typelist)
			: categoryType(type), type(typelist) {}


		// JSONからの読み込み
		void from_json(const json& j)
		{
			j.at("categorytype").get_to(categoryType);	// カテゴリーの列挙

			// スキルの種類
			for (const auto& category : j.at("skilltypes"))
			{
				sSkillType loacaltype;
				loacaltype.from_json(category);
				type.push_back(loacaltype);
			}
		}

		// JSONへの書き込み
		void to_json(json& j) const
		{
			j = json
			{
				{"categorytype", categoryType},	// カテゴリーの列挙
			};

			j["skilltypes"] = json::array(); // 空の配列を作成

			// スキルの種類
			for (const auto& category : type)
			{
				json jsonData;
				category.to_json(jsonData);

				j["skilltypes"].emplace_back(jsonData);
			}
		}
	};

	// JSONからの読み込み
	void from_json(const json& j)
	{
		// スキルのカテゴリー
		for (const auto& ability : j.at("skillcategorys"))
		{
			sSkillCategory loacalcategory;
			loacalcategory.from_json(ability);
			m_SkillCategory.push_back(loacalcategory);
		}
	}

	// JSONへの書き込み
	void to_json(json& j) const
	{

		j["skillcategorys"] = json::array(); // 空の配列を作成

		// スキルのカテゴリー
		for (const auto& ability : m_SkillCategory)
		{
			json jsonData;
			ability.to_json(jsonData);

			j["skillcategorys"].emplace_back(jsonData);
		}
	}
	

	CSkillTree_Ability();
	~CSkillTree_Ability();
	
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	void LoadJson(void);	// Jsonからのロード
	void SaveJson(void);	// Jsonへのセーブ

	static CSkillTree_Ability* GetInstance() { return m_pThisPtr; }	// インスタンス取得
	static CSkillTree_Ability* Create(void);	// 生成処理

private:

	//=============================
	// メンバ変数
	//=============================
	std::vector<sSkillCategory> m_SkillCategory;
	static CSkillTree_Ability* m_pThisPtr;	// 自身のポインタ
};

#endif