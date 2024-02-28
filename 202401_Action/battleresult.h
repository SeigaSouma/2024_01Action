//=============================================================================
// 
//  戦果ヘッダー [battleresult.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _BATTLERESULT_H_
#define _BATTLERESULT_H_	// 二重インクルード防止

#include "object2d.h"
#include "gamerating.h"
#include "multinumber.h"

class CBattleResult_Overall;

//==========================================================================
// クラス定義
//==========================================================================
// 戦果クラス
class CBattleResult : public CObject2D
{
public:

	//=============================
	// 列挙型定義
	//=============================
	// 状態
	enum STATE
	{
		STATE_FADEIN = 0,	// フェードイン
		STATE_RANKWAIT,		// ランク入場待ち
		STATE_RANKIN,		// ランク入場
		STATE_RANKIN_OVERALL,		// 総合ランク入場
		STATE_RETURNWAIT,	// 押下待機
		STATE_FADEOUT,		// フェードアウト
		STATE_MAX
	};

	CBattleResult(int nPriority = 8);
	~CBattleResult();

	// オーバーライドされた関数
	HRESULT Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;

	void Kill();	// 削除
	static CBattleResult* Create();	// 生成処理

private:

	//=============================
	// 構造体定義
	//=============================
	struct sResultRank
	{
		CObject2D* pObj2D;			// オブジェクト2D
		CGameRating::RATING rating;	// ランク
	};

	//=============================
	// 関数リスト
	//=============================
	typedef void(CBattleResult::* STATE_FUNC)();
	static STATE_FUNC m_StateFuncList[];	// 状態関数のリスト

	//=============================
	// メンバ関数
	//=============================
	// 状態
	void StateFadeIn();		// フェードイン
	void StateRankWait();	// ランク入場待ち
	void StateRankIn();		// ランク入場
	void StateRankIn_OverAll();		// 総合ランク入場
	void StateReturnWait();	// 押下待機
	void StateFadeOut();	// フェードアウト

	// 生成
	void CreateRank();		// クリアランク生成
	void CreateClearTime();	// クリアタイム生成
	void CreateDamage();	// 被ダメージ生成
	void CreateDead();		// 死亡回数生成
	void CreateRankOverall(int prevPoint);	// 総合ランク生成

	// その他
	void AllSetting();	// 全て設定

	//=============================
	// メンバ変数
	//=============================
	sResultRank m_RankInfo[CGameRating::RATINGTYPE::RATINGTYPE_MAX];	// 種類ごとのランク
	CObject2D* m_pRank[CGameRating::RATINGTYPE::RATINGTYPE_MAX];		// 種類ごとのランク
	CMultiNumber* m_pClearTime[3];		// 種類ごとの数字
	CMultiNumber* m_pDamage;			// 種類ごとの数字
	CMultiNumber* m_pDead;				// 種類ごとの数字
	CBattleResult_Overall* m_pOverall;	// 総合
	STATE m_state;	// 状態
	float m_fStateTimer;	// 状態タイマー
	float m_fSurvivalTimer;	// 生存タイマー
	bool m_bCompleteStaging;	// 演出完了フラグ
};


#endif