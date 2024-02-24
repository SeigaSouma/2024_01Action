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
	// 戦果ランク
	enum RANK
	{
		RANK_B = 0,	// Bランク
		RANK_A,		// Aランク
		RANK_S,		// Sランク
		RANK_MAX
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
	// メンバ関数
	//=============================
	void CreateRank();		// クリアランク生成
	void CreateClearTime();	// クリアタイム生成
	void CreateDamage();	// 被ダメージ生成
	void CreateDead();		// 死亡回数生成

	//=============================
	// メンバ変数
	//=============================
	CObject2D* m_pRank[CGameRating::RATINGTYPE::RATINGTYPE_MAX];		// 種類ごとのランク
	CMultiNumber* m_pNumber[CGameRating::RATINGTYPE::RATINGTYPE_MAX];	// 種類ごとの数字
	CMultiNumber* m_pClearTime[3];	// 種類ごとの数字
	CMultiNumber* m_pDamage;	// 種類ごとの数字
	CMultiNumber* m_pDead;	// 種類ごとの数字
};


#endif