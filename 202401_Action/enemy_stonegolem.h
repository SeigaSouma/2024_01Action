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
		MOTION_ATTACK_SIDESWIPE,	// 横薙ぎ
		MOTION_ATTACK_SIDESWIPE2,	// 横薙ぎ2
		MOTION_ATTACK_UPSWIPE2,		// 縦薙ぎ2
		MOTION_ATTACK_FLINCH_UPSWIPE,	// 通常攻撃
		MOTION_ATTACK_FLINCH_SIDESWIPE,	// 通常攻撃
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




// 縦薙ぎ(4段目)
class CEnemyGolemUpSwipe2 : public CEnemyProximity
{
public:
	CEnemyGolemUpSwipe2() : CEnemyProximity(350.0f)
	{
		m_bWillDirectlyTrans = true;
	}

	// モーションインデックス切り替え
	virtual void ChangeMotionIdx(CEnemy* boss) override
	{
		m_nIdxMotion = CEnemyGolem::MOTION_ATTACK_UPSWIPE2;
		CEnemyAttack::ChangeMotionIdx(boss);

		// 直接遷移する
		m_bWillDirectlyTrans = true;
	}
};


// 3段目の怯み
class CFlinch_GolemSideSwipe : public CEnemyFlinch
{
public:
	CFlinch_GolemSideSwipe()
	{
		m_bCreateFirstTime = true;
	}

	// モーションインデックス切り替え
	virtual void ChangeMotionIdx(CEnemy* boss) override
	{
		m_nIdxMotion = CEnemyGolem::MOTION::MOTION_ATTACK_FLINCH_SIDESWIPE;
		CEnemyFlinch::ChangeMotionIdx(boss);
	}
};

// 横薙ぎ(3段目)
class CEnemyGolemSideSwipe2 : public CEnemyProximity
{
public:
	CEnemyGolemSideSwipe2() : CEnemyProximity(350.0f)
	{
		m_bWillDirectlyTrans = true;

		// 怯む攻撃に設定
		SetFlinchAction(DEBUG_NEW CFlinch_GolemSideSwipe());
	}

	// モーションインデックス切り替え
	virtual void ChangeMotionIdx(CEnemy* boss) override
	{
		m_nIdxMotion = CEnemyGolem::MOTION_ATTACK_SIDESWIPE2;
		CEnemyAttack::ChangeMotionIdx(boss);

		// 直接遷移する
		m_bWillDirectlyTrans = true;

		if (UtilFunc::Transformation::Random(0, 1) == 0)
		{
			// 連続攻撃設定
			SetChainAttack(DEBUG_NEW CEnemyGolemUpSwipe2());
		}
	}
};

// 横薙ぎ攻撃(2段目)
class CEnemyGolemSideSwipe : public CEnemyProximity
{
public:
	CEnemyGolemSideSwipe() : CEnemyProximity(400.0f)
	{ 
		m_bWillDirectlyTrans = true;
	}

	// モーションインデックス切り替え
	virtual void ChangeMotionIdx(CEnemy* boss) override
	{
		m_nIdxMotion = CEnemyGolem::MOTION_ATTACK_SIDESWIPE;
		CEnemyAttack::ChangeMotionIdx(boss);

		// 直接遷移する
		m_bWillDirectlyTrans = true;

		if (UtilFunc::Transformation::Random(0, 1) == 0)
		{
			// 連続攻撃設定
			SetChainAttack(DEBUG_NEW CEnemyGolemSideSwipe2());
		}
	}
};



// 1段目の怯み
class CFlinch_GolemUPSwipe : public CEnemyFlinch
{
public:
	CFlinch_GolemUPSwipe()
	{
		m_bCreateFirstTime = true;
	}

	// モーションインデックス切り替え
	virtual void ChangeMotionIdx(CEnemy* boss) override
	{
		m_nIdxMotion = CEnemyGolem::MOTION::MOTION_ATTACK_FLINCH_UPSWIPE;
		CEnemyFlinch::ChangeMotionIdx(boss);
	}
};

// 振り上げ攻撃(1段目)
class CEnemyGolemUPSwipe : public CEnemyProximity
{
public:
	CEnemyGolemUPSwipe() : CEnemyProximity(400.0f)
	{
		m_bWillDirectlyTrans = true;

		// 怯む攻撃に設定
		SetFlinchAction(DEBUG_NEW CFlinch_GolemUPSwipe());
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


// 強攻撃
class CGolemStrongAttack : public CEnemyStrongAttack
{
public:
	CGolemStrongAttack() : CEnemyStrongAttack(400.0f)
	{
		m_bWillDirectlyTrans = true;
		m_bSetAngleNotAttacking = true;	// 攻撃判定外向き合わせフラグ
	}

	// モーションインデックス切り替え
	virtual void ChangeMotionIdx(CEnemy* boss) override
	{
		m_nIdxMotion = CEnemyGolem::MOTION::MOTION_ATTACK_STRONG;
		CEnemyStrongAttack::ChangeMotionIdx(boss);

		// 直接遷移する
		m_bWillDirectlyTrans = true;
	}
};

#endif