//=============================================================================
// 
//  ボスヘッダー [enemy_boss.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _ENEMYBOSS_H_
#define _ENEMYBOSS_H_	// 二重インクルード防止

#include "enemy.h"

// 前方宣言
class CHP_GaugeBoss;
class CEnemyState;
class CBossAttack;

//==========================================================================
// クラス定義
//==========================================================================
// ボスクラス定義
class CEnemyBoss : public CEnemy
{
public:

	//=============================
	// 列挙型定義
	//=============================
	//モーション列挙
	enum MOTION
	{
		MOTION_DEF = 0,			// ニュートラルモーション
		MOTION_WALK,			// 移動モーション
		MOTION_OVERHEADATK,		// 振り下ろし
		MOTION_SIDESWIPE,		// 横なぎ
		MOTION_HANDSLAP,		// ハンドスラップ
		MOTION_LAUNCHBALLAST,	// 瓦礫飛ばし
		MOTION_ROLLING,			// ローリング
		MOTION_DOWNWAIT,		// ダウン待機
		MOTION_DOWN,			// ダウン
		MOTION_RETURNDOWN,		// ダウン復帰
		MOTION_BACKSTEP,		// バックステップ
		MOTION_BACKSTEP_SMALL,	// 小バックステップ
		MOTION_FADEOUT,			// フェードアウト
		MOTION_FLINCH_HANDSLAP,	// ハンドスラップ
		MOTION_FLINCH_OVERHEAD,	// 振り下ろしの怯み
		MOTION_KNOCKBACK,		// ノックバック
		MOTION_MAX
	};

	CEnemyBoss(int nPriority = mylib_const::ENEMY_PRIORITY);
	~CEnemyBoss();

	// オーバーライドされた関数
	HRESULT Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;
	void Kill() override;
	void NormalHitResponse() override;	// ヒット時の反応
	void CounterHitResponse() override;	// ヒット時の反応

	bool SmallStep();	// 小ステップ

private:

	//=============================
	// メンバ関数
	//=============================
	// 状態関数
	virtual void StateDown() override;		// ダウン状態
	virtual void StateDead() override;		// 死亡
	virtual void StateFadeOut() override;	// フェードアウト
	virtual void StateDownWait() override;	// ダウン待機状態

	// その他関数
	void MotionSet() override;	// モーションの設定
	void AttackAction(CMotion::AttackInfo ATKInfo, int nCntATK) override;		// 攻撃時処理
	void AttackInDicision(CMotion::AttackInfo* pATKInfo, int nCntATK) override;	// 攻撃判定中処理

	//=============================
	// メンバ変数
	//=============================
	float m_fActTime;		// 行動カウンター
	CHP_GaugeBoss *m_pBossHPGauge;	// ボスのHPゲージ

};



//=============================
// ボスステート
//=============================
// ダウン復帰
class CReturnDown_Boss : public CEnemyReturnDown
{
public:
	CReturnDown_Boss() {}

	// モーションインデックス切り替え
	virtual void ChangeMotionIdx(CEnemy* boss) override
	{
		m_nIdxMotion = CEnemyBoss::MOTION::MOTION_RETURNDOWN;
		CEnemyReturnDown::ChangeMotionIdx(boss);
	}
};

// ステップ
class CBossStep : public CEnemyBeforeAction
{
public:

	CBossStep() {}

	// モーションインデックス切り替え
	virtual void ChangeMotionIdx(CEnemy* enemy) override
	{
		m_nIdxMotion = CEnemyBoss::MOTION_BACKSTEP;
		CEnemyBeforeAction::ChangeMotionIdx(enemy);
	}
};

// 小ステップ
class CBossStep_Small : public CBossStep
{
public:

	CBossStep_Small() {}

	// モーションインデックス切り替え
	virtual void ChangeMotionIdx(CEnemy* enemy) override
	{
		m_nIdxMotion = CEnemyBoss::MOTION_BACKSTEP_SMALL;
		CEnemyBeforeAction::ChangeMotionIdx(enemy);
	}
};


//=============================
// 近接群
//=============================
// 横なぎコンボ
class CBossSideSwipeCombo : public CEnemyProximity
{
public:
	CBossSideSwipeCombo() : CEnemyProximity(600.0f) 
	{ 
		m_bWillDirectlyTrans = true; 
		m_bSetAngleNotAttacking = true;	// 攻撃判定外向き合わせフラグ
	}
	virtual void BeforeAttack(CEnemy* enemy) override;	// 攻撃前処理

	// モーションインデックス切り替え
	virtual void ChangeMotionIdx(CEnemy* enemy) override
	{
		m_nIdxMotion = CEnemyBoss::MOTION_SIDESWIPE;
		CEnemyAttack::ChangeMotionIdx(enemy);
		m_bWillDirectlyTrans = true;

		// 攻撃前行動フラグ
		m_bBeforeAttackAction = true;
		if (UtilFunc::Transformation::Random(0, 2) == 0)
		{
			m_bBeforeAttackAction = false;
		}
	}

	// 遷移前処理
	virtual void BeforeTransitionProcess(CEnemy* enemy) override
	{
		// 挟む行動を設定
		enemy->ChangeATKState(DEBUG_NEW CBossStep_Small());
		enemy->GetATKState()->ChangeMotionIdx(enemy);
	}
};


// 振り下ろしの怯み
class CFlinch_BossOverHead : public CEnemyFlinch
{
public:
	CFlinch_BossOverHead()
	{
		m_bCreateFirstTime = true;
	}

	// モーションインデックス切り替え
	virtual void ChangeMotionIdx(CEnemy* boss) override
	{
		m_nIdxMotion = CEnemyBoss::MOTION::MOTION_FLINCH_OVERHEAD;
		CEnemyFlinch::ChangeMotionIdx(boss);
	}
};

// 振り下ろし
class CBossOverHead : public CEnemyProximity
{
public:
	CBossOverHead() : CEnemyProximity(600.0f) 
	{ 
		m_bWillDirectlyTrans = true;

		// 怯む攻撃に設定
		SetFlinchAction(DEBUG_NEW CFlinch_BossOverHead());
	}

	// モーションインデックス切り替え
	virtual void ChangeMotionIdx(CEnemy* enemy) override
	{
		m_nIdxMotion = CEnemyBoss::MOTION_OVERHEADATK;
		CEnemyAttack::ChangeMotionIdx(enemy);
		m_bWillDirectlyTrans = true;
	}
};


// ハンドスラップの怯み
class CFlinch_BossHandSlap : public CEnemyFlinch
{
public:
	CFlinch_BossHandSlap()
	{
		m_bCreateFirstTime = true;
	}

	// モーションインデックス切り替え
	virtual void ChangeMotionIdx(CEnemy* boss) override
	{
		m_nIdxMotion = CEnemyBoss::MOTION::MOTION_FLINCH_HANDSLAP;
		CEnemyFlinch::ChangeMotionIdx(boss);
	}
};

// ハンドスラップ
class CBossHandSlap : public CEnemyProximity
{
public:
	CBossHandSlap() : CEnemyProximity(600.0f)
	{
		m_bWillDirectlyTrans = true;
		m_bSetAngleNotAttacking = true;	// 攻撃判定外向き合わせフラグ

		// 怯む攻撃に設定
		SetFlinchAction(DEBUG_NEW CFlinch_BossHandSlap());
	}

	// モーションインデックス切り替え
	virtual void ChangeMotionIdx(CEnemy* enemy) override
	{
		m_nIdxMotion = CEnemyBoss::MOTION_HANDSLAP;
		CEnemyAttack::ChangeMotionIdx(enemy);
		m_bWillDirectlyTrans = true;
	}
};

// ローリング
class CBossRolling : public CEnemyProximity
{
public:
	CBossRolling() : CEnemyProximity(600.0f) { m_bWillDirectlyTrans = false; }

	// 行動
	virtual void Action(CEnemy* enemy) override
	{
		Attack(enemy);
	}

	// モーションインデックス切り替え
	virtual void ChangeMotionIdx(CEnemy* enemy) override
	{
		m_nIdxMotion = CEnemyBoss::MOTION_ROLLING;
		CEnemyAttack::ChangeMotionIdx(enemy);
		m_bWillDirectlyTrans = false;
	}

	// 遷移前処理
	virtual void BeforeTransitionProcess(CEnemy* enemy) override 
	{
		// 挟む行動を設定
		enemy->ChangeATKState(DEBUG_NEW CBossStep());
		enemy->GetATKState()->ChangeMotionIdx(enemy);
	}

};

//=============================
// 遠距離群
//=============================
// 瓦礫飛ばし
class CBossLaunchBallast : public CEnemyRemote
{
public:
	CBossLaunchBallast() { m_bWillDirectlyTrans = true; }

	// モーションインデックス切り替え
	virtual void ChangeMotionIdx(CEnemy* enemy) override
	{
		m_nIdxMotion = CEnemyBoss::MOTION_LAUNCHBALLAST;
		CEnemyAttack::ChangeMotionIdx(enemy);
		m_bWillDirectlyTrans = true;
	}
};


#endif