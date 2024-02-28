//=============================================================================
// 
//  総合戦果ヘッダー [battleresult_Overall.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _BATTLERESULT_OVERALL_H_
#define _BATTLERESULT_OVERALL_H_	// 二重インクルード防止

#include "object2d.h"
#include "gamerating.h"
#include "multinumber.h"

//==========================================================================
// クラス定義
//==========================================================================
// 総合戦果クラス
class CBattleResult_Overall : public CObject2D
{
public:

	//=============================
	// 列挙型定義
	//=============================
	// 状態
	enum STATE
	{
		STATE_NONE = 0,	// なにもなし
		STATE_FADEIN,	// フェードイン
		STATE_WAIT_POINT,	// ポイント入場待ち
		STATE_FADEIN_POINT,	// ポイントフェードイン
		STATE_FADEOUT,	// フェードアウト
		STATE_MAX
	};

	CBattleResult_Overall(int nPriority = 8);
	~CBattleResult_Overall();

	// オーバーライドされた関数
	HRESULT Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;

	void SetState(STATE state);
	void AllSetting();	// 全て設定
	void Kill();	// 削除
	bool IsCompleteStaging() { return m_bCompleteStaging; }
	static CBattleResult_Overall* Create(CGameRating::RATING rank, int prevPoint, int currentPoint);	// 生成処理

private:

	//=============================
	// 列挙型定義
	//=============================
	// 数字
	enum NUMBERTYPE
	{
		NUMBERTYPE_PREV = 0,	// 前回
		NUMBERTYPE_CURRENT,		// 今回
		NUMBERTYPE_MAX
	};

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
	typedef void(CBattleResult_Overall::* STATE_FUNC)();
	static STATE_FUNC m_StateFuncList[];	// 状態関数のリスト

	//=============================
	// メンバ関数
	//=============================
	// 状態
	void StateNone();		// なにもなし
	void StateFadeIn();		// フェードイン
	void StateRankWait();	// ランク入場待ち
	void StateFadeIn_Point();		// ポイントフェードイン
	void StateFadeOut();	// フェードアウト

	// 生成
	void CreatePoint();		// ポイント差分生成

	//=============================
	// メンバ変数
	//=============================
	int m_nPoint[NUMBERTYPE::NUMBERTYPE_MAX];				// ポイント
	CMultiNumber* m_pDiffPoint[NUMBERTYPE::NUMBERTYPE_MAX];		// ポイント
	STATE m_state;	// 状態
	float m_fStateTimer;	// 状態タイマー
	CGameRating::RATING m_OverallRating;	// ランク
	bool m_bCompleteStaging;	// 演出完了フラグ
};


#endif