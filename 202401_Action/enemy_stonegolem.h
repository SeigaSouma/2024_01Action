//=============================================================================
// 
//  ストーンゴーレム敵ヘッダー [enemy_stonegolem.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _ENEMYSTONEGOLEM_H_
#define _ENEMYSTONEGOLEM_H_	// 二重インクルード防止

#include "enemy.h"

//==========================================================================
// クラス定義
//==========================================================================
// ストーンゴーレム敵クラス
class CEnemyGolem : public CEnemy
{
public:

	//=============================
	// 列挙型定義
	//=============================
	//モーション列挙
	enum MOTION
	{
		MOTION_DEF = 0,			// ニュートラル
		MOTION_WALK,			// 移動
		MOTION_ATTACK_NORMAL,	// 通常攻撃
		MOTION_ATTACK_STRONG,	// 強攻撃
		MOTION_DMG,				// ダメージ
		MOTION_DOWN,			// ダウン
		MOTION_KNOCKBACK,		// やられ
		MOTION_FADEOUT,			// フェードアウト
		MOTION_ATTACK_NORMAL2,	// 通常攻撃2
		MOTION_MAX
	};

	CEnemyGolem(int nPriority = mylib_const::ENEMY_PRIORITY);
	~CEnemyGolem();

	// オーバーライドされた関数
	HRESULT Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;
	void Kill() override;


private:


	//=============================
	// 関数リスト
	//=============================

	//=============================
	// メンバ関数
	//=============================

	// その他関数
	void MotionSet() override;	// モーションの設定
	void AttackAction(CMotion::AttackInfo ATKInfo, int nCntATK) override;		// 攻撃時処理
	void AttackInDicision(CMotion::AttackInfo* pATKInfo, int nCntATK) override;	// 攻撃判定中処理

	//=============================
	// メンバ変数
	//=============================
};



// 横薙ぎ攻撃
class CEnemyGolemSideSwipe : public CEnemyProximity
{
public:
	CEnemyGolemSideSwipe() : CEnemyProximity(350.0f)
	{ 
		m_bWillDirectlyTrans = true;
	}

	// モーションインデックス切り替え
	virtual void ChangeMotionIdx(CEnemy* boss) override
	{
		m_nIdxMotion = CEnemyGolem::MOTION_ATTACK_NORMAL2;
		CEnemyAttack::ChangeMotionIdx(boss);

		// 直接遷移する
		m_bWillDirectlyTrans = true;
	}
};


// 振り上げ攻撃
class CEnemyGolemUPSwipe : public CEnemyProximity
{
public:
	CEnemyGolemUPSwipe() : CEnemyProximity(350.0f)
	{
		m_bWillDirectlyTrans = true;
	}

	// モーションインデックス切り替え
	virtual void ChangeMotionIdx(CEnemy* boss) override
	{
		m_nIdxMotion = CEnemyGolem::MOTION_ATTACK_NORMAL;
		CEnemyAttack::ChangeMotionIdx(boss);

		// 直接遷移する
		m_bWillDirectlyTrans = true;

		// 連続攻撃設定
		SetChainAttack(DEBUG_NEW CEnemyGolemSideSwipe());
	}
};


#endif