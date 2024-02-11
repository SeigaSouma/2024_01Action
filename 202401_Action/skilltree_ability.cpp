//=============================================================================
// 
// スキルツリー能力処理 [skilltree_ability.cpp]
// Author : 相馬靜雅
// 
//=============================================================================
#include "skilltree_ability.h"
#include "renderer.h"
#include "texture.h"
#include "manager.h"
#include "sound.h"
#include "input.h"
#include "calculation.h"

//==========================================================================
// 定数定義
//==========================================================================
namespace
{
	const char* LOADTEXT = "data\\TEXT\\skilltree\\ability.json";
}

//==========================================================================
// 静的メンバ変数定義
//==========================================================================
CSkillTree_Ability* CSkillTree_Ability::m_pThisPtr = nullptr;	// 自身のポインタ

//==========================================================================
// コンストラクタ
//==========================================================================
CSkillTree_Ability::CSkillTree_Ability()
{

}

//==========================================================================
// デストラクタ
//==========================================================================
CSkillTree_Ability::~CSkillTree_Ability()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CSkillTree_Ability* CSkillTree_Ability::Create()
{

	if (m_pThisPtr == NULL)
	{// まだ生成していなかったら

		// インスタンス生成
		m_pThisPtr = DEBUG_NEW CSkillTree_Ability;

		// 初期化処理
		m_pThisPtr->Init();
	}
	else
	{
		// インスタンス取得
		m_pThisPtr->GetInstance();
	}

	return m_pThisPtr;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CSkillTree_Ability::Init()
{
#if 1

	// Jsonから読み込み
	LoadJson();

#else

	m_SkillCategory.push_back(sSkillCategory());

	// 現在扱うカテゴリーのポインタ
	sSkillCategory* pCategory = &m_SkillCategory[SKILLCATEGORY_BUFF];

	// 列挙設定
	pCategory->categoryType = SKILLCATEGORY_BUFF;

	// 種類追加
	pCategory->type.push_back(sSkillType());
	pCategory->type.back().typeName = "Stamina";

	// ステージ追加
	pCategory->type.back().stage.push_back(sSkillStage());
	pCategory->type.back().stage.back().stage = 0;

	// ステージ追加
	pCategory->type.back().stage.push_back(sSkillStage());
	pCategory->type.back().stage.back().stage = 1;

	// Jsonへ書き込み
	SaveJson();
#endif

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CSkillTree_Ability::Uninit()
{
	if (m_pThisPtr != nullptr)
	{
		delete m_pThisPtr;
		m_pThisPtr = nullptr;
	}
}

//==========================================================================
// 更新処理
//==========================================================================
void CSkillTree_Ability::Update()
{
	for (const auto& category : m_SkillCategory)
	{
		category.categoryType;
		for (const auto& type : category.type)
		{
			type.typeName;
			for (const auto& stage : type.stage)
			{
				stage.stage;
			}
		}
	}
}

//==========================================================================
// Jsonからのロード
//==========================================================================
void CSkillTree_Ability::LoadJson()
{
	// ファイルからJSONを読み込む
	std::ifstream file(LOADTEXT);
	if (!file.is_open())
	{
		return;
	}

	nlohmann::json jsonData;
	file >> jsonData;	// jsonデータを与える

	// jsonデータから読み込む
	from_json(jsonData);
}

//==========================================================================
// Jsonへのセーブ
//==========================================================================
void CSkillTree_Ability::SaveJson()
{
	// ファイルにキャラクターのデータを書き込む
	std::ofstream outFile(LOADTEXT);
	if (!outFile.is_open())
	{
		return;
	}

	// 出力データ
	nlohmann::json outputData;
	to_json(outputData);

	// ファイルにデータを書き込む
	outFile << std::setw(4) << outputData << std::endl;
}

//==========================================================================
// 描画処理
//==========================================================================
void CSkillTree_Ability::Draw()
{

}
