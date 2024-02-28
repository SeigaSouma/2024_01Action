//=============================================================================
// 
// ゲーム評価処理 [gamerating.cpp]
// Author : 相馬靜雅
// 
//=============================================================================
#include "gamerating.h"
#include "game.h"
#include "gamemanager.h"
#include "manager.h"
#include "calculation.h"

//==========================================================================
// 定数定義
//==========================================================================
namespace
{
	const std::string TEXT_STANDARDS = "data\\TEXT\\gamerating\\ratingStandards.txt";		// 評価基準の外部ファイル名
	const std::map<std::string, int> PRIORITY_RANK =
	{
		{"RankS", 3}, // Sランクの優先順位
		{"RankA", 2}, // Aランクの優先順位
		{"RankB", 1}  // Bランクの優先順位
	};
	const int POINT_RANK[] =
	{
		5, 6, 7
	};
	const int CONDITION_S = 7;	// Sランク条件
	const int CONDITION_A = 5;	// Aランク条件
}

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================
std::map<std::string, std::map<std::string, CGameRating::sRating>> CGameRating::m_RatingStandards = {};	// 評価基準

//==========================================================================
// コンストラクタ
//==========================================================================
CGameRating::CGameRating()
{
	// 値のクリア
	m_RatingInfo = sRating();	// 評価情報
}

//==========================================================================
// デストラクタ
//==========================================================================
CGameRating::~CGameRating()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CGameRating* CGameRating::Create()
{
	// メモリ確保
	CGameRating* pRating = DEBUG_NEW CGameRating;

	if (pRating != nullptr)
	{
		// 初期化処理
		if (FAILED(pRating->Init()))
		{
			return nullptr;
		}
	}

	return pRating;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CGameRating::Init()
{
	

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CGameRating::Uninit()
{
	delete this;
}

//==========================================================================
// 更新処理
//==========================================================================
void CGameRating::Update()
{
	
}

//==========================================================================
// テキスト読み込み処理
//==========================================================================
void CGameRating::ReadText()
{
	// ファイルを開く
	std::ifstream File(TEXT_STANDARDS);
	if (!File.is_open())
	{
		return;
	}

	// リセット
	m_RatingStandards.clear();

	// データ読み込み
	std::string line;
	while (std::getline(File, line))
	{
		// コメントはスキップ
		if (line.empty() || 
			line[0] == '#')
		{
			continue;
		}

		// ストリーム作成
		std::istringstream lineStream(line);

		// ステージとランク
		std::string stage, rank;

		// 評価基準
		float time;
		int damage;
		int dead;

		// 評価基準追加
		if (lineStream >> stage >> rank >> time >> damage >> dead)
		{
			m_RatingStandards[stage][rank] = sRating(time, damage, dead);

			// ランク割り当て
			if (rank == "RankB") {
				m_RatingStandards[stage][rank].rating = RATING::RATING_B;
			}
			else if (rank == "RankA") {
				m_RatingStandards[stage][rank].rating = RATING::RATING_A;
			}
			else if (rank == "RankS") {
				m_RatingStandards[stage][rank].rating = RATING::RATING_S;
			}
		}
	}

	// ファイルを閉じる
	File.close();
}

//==========================================================================
// 総合ランクポイント割り出し
//==========================================================================
int CGameRating::CalculateOverrallRankPoint(RATING allRank)
{
	return POINT_RANK[allRank];
}

//==========================================================================
// ステージ毎のランク割り出し
//==========================================================================
CGameRating::RATING CGameRating::CalculateRank(RATING timeRank, RATING dmgRank, RATING deadRank)
{
	int allpoint = static_cast<int>(timeRank + dmgRank + deadRank + RATINGTYPE_MAX);

	if (allpoint >= CONDITION_S)
	{
		return RATING::RATING_S;
	}
	if (allpoint >= CONDITION_A)
	{
		return RATING::RATING_A;
	}

	return RATING::RATING_B;
}

//==========================================================================
// クリアタイムのランク割り出し
//==========================================================================
CGameRating::RATING CGameRating::CalculateClearTimeRank(const float time)
{

	std::string stageName = "Stage" + std::to_string(CGame::GetInstance()->GetGameManager()->GetNowStage());

	// 評価の最高値保持
	RATING maxRating = RATING::RATING_B;
	int maxPriority = 0;

	for (const auto& rankInfo : m_RatingStandards[stageName]) 
	{
		const sRating& standard = rankInfo.second;

		// 優先順位割り出し
		int priority = PRIORITY_RANK.at(rankInfo.first);

		// 基準チェック
		if (time <= standard.clearTime && 
			priority >= maxPriority)
		{
			maxPriority = priority;
			maxRating = rankInfo.second.rating;
		}
	}

	return maxRating;
}

//==========================================================================
// 被ダメージのランク割り出し
//==========================================================================
CGameRating::RATING CGameRating::CalculateRecieveDamageRank(const int damage)
{
	std::string stageName = "Stage" + std::to_string(CGame::GetInstance()->GetGameManager()->GetNowStage());

	// 評価の最高値保持
	RATING maxRating = RATING::RATING_B;
	int maxPriority = 0;

	for (const auto& rankInfo : m_RatingStandards[stageName])
	{
		const sRating& standard = rankInfo.second;

		// 優先順位割り出し
		int priority = PRIORITY_RANK.at(rankInfo.first);

		// 基準チェック
		if (damage <= standard.receiveDamage &&
			priority >= maxPriority)
		{
			maxPriority = priority;
			maxRating = rankInfo.second.rating;
		}
	}

	return maxRating;
}

//==========================================================================
// 死亡回数のランク割り出し
//==========================================================================
CGameRating::RATING CGameRating::CalculateNumDeadRank(const int dead)
{
	std::string stageName = "Stage" + std::to_string(CGame::GetInstance()->GetGameManager()->GetNowStage());

	// 評価の最高値保持
	RATING maxRating = RATING::RATING_B;
	int maxPriority = 0;

	for (const auto& rankInfo : m_RatingStandards[stageName])
	{
		const sRating& standard = rankInfo.second;

		// 優先順位割り出し
		int priority = PRIORITY_RANK.at(rankInfo.first);

		// 基準チェック
		if (dead <= standard.numDead &&
			priority >= maxPriority)
		{
			maxPriority = priority;
			maxRating = rankInfo.second.rating;
		}
	}

	return maxRating;
}