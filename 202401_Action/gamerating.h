//=============================================================================
// 
// ゲーム評価処理 [gamerating.h]
// Author : 相馬靜雅
// 
//=============================================================================

#ifndef _GAMERATING_H_
#define _GAMERATING_H_		// 二重インクルード防止のマクロを定義する

//==========================================================================
// スキルツリー能力のStrategy
//==========================================================================
class CGameRating
{
public:

	//=============================
	// 列挙型定義
	//=============================
	enum RATING
	{
		RATING_B = 0,	// B評価
		RATING_A,		// A評価
		RATING_S,		// S評価
		RATING_MAX
	};

	enum RATINGTYPE
	{
		RATINGTYPE_TIME = 0,	// クリアタイム
		RATINGTYPE_DMG,			// 被ダメージ
		RATINGTYPE_DEAD,		// 死亡回数
		RATINGTYPE_MAX
	};

	//=============================
	// 構造体定義
	//=============================
	struct sRating
	{
		float clearTime;	// クリア時間
		int receiveDamage;	// 被ダメージ
		int numDead;		// 死亡回数
		RATING rating;		// 評価

		sRating() : clearTime(0.0f), receiveDamage(0), numDead(0), rating() {}
		sRating(float time, int damage, int dead) : clearTime(time), receiveDamage(damage), numDead(dead), rating() {}
	};

	CGameRating();
	~CGameRating();

	HRESULT Init();
	void Uninit();
	void Update();

	void SetReceiveDamage(int damage) { m_RatingInfo.receiveDamage = damage; }	// 被ダメージ設定
	int GetReceiveDamage() { return m_RatingInfo.receiveDamage; }				// 被ダメージ取得
	void AddReceiveDamage(int damage) { m_RatingInfo.receiveDamage += damage; }	// 被ダメージ加算
	void SetNumDead(int num) { m_RatingInfo.numDead = num; }					// 死亡回数設定
	int GetNumDead() { return m_RatingInfo.numDead; }							// 死亡回数取得
	void AddNumDead() { m_RatingInfo.numDead++; }								// 死亡回数加算
	void SetClearTime(float time) { m_RatingInfo.clearTime = time; }			// クリア時間設定
	float GetClearTime() { return m_RatingInfo.clearTime; }						// クリア時間取得
	void AddClearTime(float time) { m_RatingInfo.clearTime += time; }			// クリア時間加算
	RATING GetRating() { return m_RatingInfo.rating; }							// 評価取得
	sRating GetRatingInfo() { return m_RatingInfo; }							// 評価情報取得

	RATING CalculateRank(const sRating& result, const std::map<std::string, sRating>& rankStandards);
	RATING CalculateClearTimeRank(const float time);		// 時間のランク割り出し
	RATING CalculateRecieveDamageRank(const int damage);	// 被ダメージのランク割り出し
	RATING CalculateNumDeadRank(const int dead);			// 死亡回数のランク割り出し


	static CGameRating* Create();		// 生成処理
	static void ReadText();

private:

	//=============================
	// メンバ関数
	//=============================

	//=============================
	// メンバ変数
	//=============================
	sRating m_RatingInfo;	// 評価情報
	static std::map<std::string, std::map<std::string, sRating>> m_RatingStandards;	// 評価基準
};

#endif