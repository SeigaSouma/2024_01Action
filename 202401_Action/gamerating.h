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

	CGameRating();
	~CGameRating();

	HRESULT Init();
	void Uninit();
	void Update();

	void SetReceiveDamage(int damage) { m_nReceiveDamage = damage; }	// 被ダメージ設定
	int GetReceiveDamage() { return m_nReceiveDamage; }					// 被ダメージ取得
	void AddReceiveDamage(int damage) { m_nReceiveDamage += damage; }	// 被ダメージ加算
	void SetNumDead(int num) { m_nNumDead = num; }						// 死亡回数設定
	int GetNumDead() { return m_nNumDead; }								// 死亡回数取得
	void AddNumDead() { m_nNumDead++; }									// 死亡回数加算
	void SetClearTime(float time) { m_fClearTime = time; }				// クリア時間設定
	float GetClearTime() { return m_fClearTime; }						// クリア時間取得
	void AddClearTime(float time) { m_fClearTime += time; }				// クリア時間加算
	RATING GetRating() { return m_rating; }								// 評価取得

	static CGameRating* Create();		// 生成処理

private:

	//=============================
	// メンバ関数
	//=============================

	//=============================
	// メンバ変数
	//=============================
	int m_nReceiveDamage;	// 被ダメージ
	int m_nNumDead;			// 死亡回数
	float m_fClearTime;		// クリア時間
	RATING m_rating;		// 評価

};

#endif