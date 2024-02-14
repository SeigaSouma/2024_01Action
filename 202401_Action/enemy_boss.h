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
class CBossState;
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
		MOTION_DOWN,			// ダウンモーション
		MOTION_BACKSTEP,		// バックステップ
		MOTION_BACKSTEP_SMALL,	// 小バックステップ
		MOTION_FADEOUT,			// フェードアウト
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

	// 攻撃状態切り替え
	void ChangeATKState(CBossState* state);
	void ChangeNextATKState(CBossState* state) { m_pNextATKState = state; }

	void ActChase();		// 追い掛け
	void RotationTarget(float range = 90.0f);	// ターゲットの方を向く
	bool SmallStep();	// 小ステップ

	void PerformAttack();		// 攻撃実行処理
	void DrawingRandomAction();	// 攻撃ランダム抽選
	void ChangeNextAction();	// 次の攻撃へ切り替え

	bool IsCatchUp() { return m_bCatchUp; }	// 追い着き判定
	bool IsInSight() { return m_bInSight; }	// 視界内判定

	CBossState* GetNextATKState() { return m_pNextATKState; }
private:


	//=============================
	// 関数リスト
	//=============================

	//=============================
	// メンバ関数
	//=============================
	// 行動関数
	void ActionSet() override;		// 行動の設定
	void UpdateAction() override;	// 行動更新
	void ActWait();					// 待機

	// 状態関数
	virtual void StateDown() override;	// ダウン状態


	// その他関数
	void MotionSet() override;	// モーションの設定
	void AttackAction(CMotion::AttackInfo ATKInfo, int nCntATK) override;		// 攻撃時処理
	void AttackInDicision(CMotion::AttackInfo* pATKInfo, int nCntATK) override;	// 攻撃判定中処理

	//=============================
	// メンバ変数
	//=============================
	float m_fActTime;		// 行動カウンター
	bool m_bCatchUp;	// 追い着き判定
	bool m_bInSight;	// 視界内判定
	CHP_GaugeBoss *m_pBossHPGauge;	// ボスのHPゲージ
	CBossState* m_pATKState;		// 今の行動ポインタ
	CBossState* m_pNextATKState;	// 次の行動ポインタ

	std::vector<CBossAttack*> m_pAtkPattern;
};



//=============================
// ボスステート
//=============================
class CBossState
{
public:
	CBossState() : m_bCreateFirstTime(false), m_bBeforeAttackAction(false) {}

	virtual void Action(CEnemyBoss* boss) = 0;	// 行動
	virtual void Attack(CEnemyBoss* boss) = 0;	// 攻撃処理
	virtual void ChangeMotionIdx(CEnemyBoss* boss) = 0;	// モーションインデックス切り替え
	virtual void BeforeAttack(CEnemyBoss* boss) { m_bBeforeAttackAction = true; }	// 攻撃前処理

	bool IsCreateFirstTime() { return m_bCreateFirstTime; }	// 初回生成のフラグ

protected:
	bool m_bCreateFirstTime;	// 初回生成のフラグ
	bool m_bBeforeAttackAction;	// 攻撃前行動フラグ
};

// ステップ
class CBossStep : public CBossState
{
public:

	CBossStep() {}
	
	virtual void Action(CEnemyBoss* boss) override;		// 行動
	virtual void Attack(CEnemyBoss* boss) override {}	// 攻撃処理

	// モーションインデックス切り替え
	virtual void ChangeMotionIdx(CEnemyBoss* boss) override
	{
		boss->SetMotion(CEnemyBoss::MOTION_BACKSTEP);
	}
};

// 小ステップ
class CBossStep_Small : public CBossStep
{
public:

	CBossStep_Small() {}

	virtual void Action(CEnemyBoss* boss) override;		// 行動

	// モーションインデックス切り替え
	virtual void ChangeMotionIdx(CEnemyBoss* boss) override
	{
		boss->SetMotion(CEnemyBoss::MOTION_BACKSTEP_SMALL);
	}
};

// 攻撃
class CBossAttack : public CBossState
{
public:

	CBossAttack() : m_nIdxMotion(0), m_bWillDirectlyTrans(true)
	{
		m_bCreateFirstTime = true;
	}

	virtual void Action(CEnemyBoss* boss) override = 0;	// 行動
	virtual void Attack(CEnemyBoss* boss) override;	// 攻撃処理


	// モーションインデックス切り替え
	virtual void ChangeMotionIdx(CEnemyBoss* boss) override
	{
		// 派生クラスでインデックス設定されてる前提
		boss->SetMotion(m_nIdxMotion);
		m_bBeforeAttackAction = false;	// 攻撃前行動フラグ
	}

	virtual void BeforeTransitionProcess(CEnemyBoss* boss) 
	{
		boss->RotationTarget();
		boss->ActChase();
	}	// 遷移前処理

	bool IsDirectlyTrans() { return m_bWillDirectlyTrans; }	// 直接遷移フラグ取得


protected:
	int m_nIdxMotion;			// モーション番号
	bool m_bWillDirectlyTrans;	// 直接遷移フラグ
};

// 近接攻撃
class CBossProximity : public CBossAttack
{
public:

	CBossProximity() {}
	
	virtual void Action(CEnemyBoss* boss) override;	// 行動
	virtual void ChangeMotionIdx(CEnemyBoss* boss) override = 0;	// モーションインデックス切り替え

};

// 遠距離攻撃
class CBossRemote : public CBossAttack
{
public:
	CBossRemote() {}

	virtual void Action(CEnemyBoss* boss) override;	// 行動
	virtual void ChangeMotionIdx(CEnemyBoss* boss) override = 0;	// モーションインデックス切り替え
};



namespace UtilFunc 
{
	namespace Transformation
	{
		int Random(int nMinNum, int nMaxNum);
	}
}

//=============================
// 近接群
//=============================
// 横なぎコンボ
class CBossSideSwipeCombo : public CBossProximity
{
public:
	CBossSideSwipeCombo() { m_bWillDirectlyTrans = true; }
	virtual void BeforeAttack(CEnemyBoss* boss) override;	// 攻撃前処理

	// モーションインデックス切り替え
	virtual void ChangeMotionIdx(CEnemyBoss* boss) override
	{
		m_nIdxMotion = CEnemyBoss::MOTION_SIDESWIPE;
		CBossAttack::ChangeMotionIdx(boss);
		m_bWillDirectlyTrans = true;

		// 攻撃前行動フラグ
		m_bBeforeAttackAction = true;
		if (UtilFunc::Transformation::Random(0, 2) == 0)
		{
			m_bBeforeAttackAction = false;
		}
	}
};

// 振り下ろし
class CBossOverHead : public CBossProximity
{
public:
	CBossOverHead() { m_bWillDirectlyTrans = true; }

	//virtual void BeforeAttack(CEnemyBoss* boss) override;	// 攻撃前処理

	// モーションインデックス切り替え
	virtual void ChangeMotionIdx(CEnemyBoss* boss) override
	{
		m_nIdxMotion = CEnemyBoss::MOTION_OVERHEADATK;
		CBossAttack::ChangeMotionIdx(boss);
		m_bWillDirectlyTrans = true;
	}

	//// 遷移前処理
	//virtual void BeforeTransitionProcess(CEnemyBoss* boss) override
	//{
	//	// 挟む行動を設定
	//	boss->ChangeATKState(DEBUG_NEW CBossStep_Small());
	//}
};


// ハンドスラップ
class CBossHandSlap : public CBossProximity
{
public:
	CBossHandSlap() { m_bWillDirectlyTrans = true; }

	// モーションインデックス切り替え
	virtual void ChangeMotionIdx(CEnemyBoss* boss) override
	{
		m_nIdxMotion = CEnemyBoss::MOTION_HANDSLAP;
		CBossAttack::ChangeMotionIdx(boss);
		m_bWillDirectlyTrans = true;
	}
};

// ローリング
class CBossRolling : public CBossProximity
{
public:
	CBossRolling() { m_bWillDirectlyTrans = false; }

	// 行動
	virtual void Action(CEnemyBoss* boss) override
	{
		Attack(boss);
	}

	// モーションインデックス切り替え
	virtual void ChangeMotionIdx(CEnemyBoss* boss) override
	{
		m_nIdxMotion = CEnemyBoss::MOTION_ROLLING;
		CBossAttack::ChangeMotionIdx(boss);
		m_bWillDirectlyTrans = false;
	}

	// 遷移前処理
	virtual void BeforeTransitionProcess(CEnemyBoss* boss) override 
	{
		// 挟む行動を設定
		boss->ChangeATKState(DEBUG_NEW CBossStep());
	}

};

//=============================
// 遠距離群
//=============================
// 瓦礫飛ばし
class CBossLaunchBallast : public CBossRemote
{
public:
	CBossLaunchBallast() { m_bWillDirectlyTrans = true; }

	// モーションインデックス切り替え
	virtual void ChangeMotionIdx(CEnemyBoss* boss) override
	{
		m_nIdxMotion = CEnemyBoss::MOTION_LAUNCHBALLAST;
		CBossAttack::ChangeMotionIdx(boss);
		m_bWillDirectlyTrans = true;
	}
};


#endif