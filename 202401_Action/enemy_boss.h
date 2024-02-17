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

	bool SmallStep();	// 小ステップ

	

	/*CEnemyState* GetATKState() { return m_pATKState; }
	CEnemyState* GetNextATKState() { return m_pNextATKState; }*/
private:


	//=============================
	// 関数リスト
	//=============================

	//=============================
	// メンバ関数
	//=============================
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
	CHP_GaugeBoss *m_pBossHPGauge;	// ボスのHPゲージ

};



//=============================
// ボスステート
//=============================
// ステップ
class CBossStep : public CEnemyBeforeAction
{
public:

	CBossStep() {}
	//
	//virtual void Action(CEnemy* enemy) override;		// 行動
	//virtual void Attack(CEnemy* enemy) override {}	// 攻撃処理

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
//
//// 攻撃
//class CBossAttack : public CEnemyState
//{
//public:
//
//	CBossAttack() : m_bWillDirectlyTrans(true)
//	{
//		m_bCreateFirstTime = true;
//	}
//
//	virtual void Action(CEnemy* enemy) override = 0;	// 行動
//	virtual void Attack(CEnemy* enemy) override;	// 攻撃処理
//
//
//	// モーションインデックス切り替え
//	virtual void ChangeMotionIdx(CEnemy* enemy) override
//	{
//		CEnemyState::ChangeMotionIdx(enemy);
//	}
//
//	// 遷移前処理
//	virtual void BeforeTransitionProcess(CEnemy* enemy) 
//	{
//		enemy->RotationTarget();
//		enemy->ActChase(1.0f, 600.0f);
//	}	// 遷移前処理
//
//	bool IsDirectlyTrans() { return m_bWillDirectlyTrans; }	// 直接遷移フラグ取得
//
//
//protected:
//	bool m_bWillDirectlyTrans;	// 直接遷移フラグ
//};
//
//// 近接攻撃
//class CBossProximity : public CEnemyAttack
//{
//public:
//
//	CBossProximity() {}
//	
//	virtual void Action(CEnemy* enemy) override;	// 行動
//	virtual void ChangeMotionIdx(CEnemy* enemy) override = 0;	// モーションインデックス切り替え
//
//};
//
//// 遠距離攻撃
//class CBossRemote : public CEnemyAttack
//{
//public:
//	CBossRemote() {}
//
//	virtual void Action(CEnemy* enemy) override;	// 行動
//	virtual void ChangeMotionIdx(CEnemy* enemy) override = 0;	// モーションインデックス切り替え
//};



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
class CBossSideSwipeCombo : public CEnemyProximity
{
public:
	CBossSideSwipeCombo() : CEnemyProximity(600.0f) { m_bWillDirectlyTrans = true; }
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

// 振り下ろし
class CBossOverHead : public CEnemyProximity
{
public:
	CBossOverHead() : CEnemyProximity(600.0f) { m_bWillDirectlyTrans = true; }

	//virtual void BeforeAttack(CEnemy* enemy) override;	// 攻撃前処理

	// モーションインデックス切り替え
	virtual void ChangeMotionIdx(CEnemy* enemy) override
	{
		m_nIdxMotion = CEnemyBoss::MOTION_OVERHEADATK;
		CEnemyAttack::ChangeMotionIdx(enemy);
		m_bWillDirectlyTrans = true;
	}
};


// ハンドスラップ
class CBossHandSlap : public CEnemyProximity
{
public:
	CBossHandSlap() : CEnemyProximity(600.0f) { m_bWillDirectlyTrans = true; }

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