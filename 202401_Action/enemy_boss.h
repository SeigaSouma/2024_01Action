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
		MOTION_LAUNCHBALLAST,	// 瓦礫飛ばし
		MOTION_ROLLING,			// ローリング
		MOTION_KNOCKBACK,		// やられモーション
		MOTION_FADEOUT,			// フェードアウト
		MOTION_MAX
	};


	CEnemyBoss(int nPriority = mylib_const::ENEMY_PRIORITY);
	~CEnemyBoss();

	// オーバーライドされた関数
	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
	void Kill(void) override;
	void NormalHitResponse() override;	// ヒット時の反応
	void CounterHitResponse() override;	// ヒット時の反応

	// 攻撃状態切り替え
	void ChangeATKState(CBossState* state);
	void ChangeNextATKState(CBossState* state) 
	{ 
		/*delete m_pNextATKState;
		m_pNextATKState = state;*/
	}


	void ChangeATKType(int pattern)
	{
		//m_nIdxAtkPattern
	}



	void ActChase(void);		// 追い掛け

	void PerformAttack();		// 攻撃実行処理
	void DrawingRandomAction();	// 攻撃ランダム抽選

	bool IsCatchUp() { return m_bCatchUp; }

	CBossState* GetNextATKState() { return m_pNextATKState; }
private:


	//=============================
	// 関数リスト
	//=============================

	//=============================
	// メンバ関数
	//=============================
	// 行動関数
	void ActionSet(void) override;		// 行動の設定
	void UpdateAction(void) override;	// 行動更新
	void ActWait(void);					// 待機

	// その他関数
	void MotionSet(void) override;	// モーションの設定
	void RotationTarget(void);		// ターゲットの方を向く
	void AttackAction(CMotion::AttackInfo ATKInfo, int nCntATK) override;		// 攻撃時処理
	void AttackInDicision(CMotion::AttackInfo ATKInfo, int nCntATK) override;	// 攻撃判定中処理

	//=============================
	// メンバ変数
	//=============================
	float m_fActTime;		// 行動カウンター
	bool m_bCatchUp;	// 追い着き判定
	CHP_GaugeBoss *m_pBossHPGauge;	// ボスのHPゲージ
	CBossState* m_pATKState;		// 今の行動ポインタ
	CBossState* m_pNextATKState;	// 次の行動ポインタ

	std::vector<CBossAttack*> m_pAtkPattern;
};



// ボスステート
class CBossState
{
public:
	virtual void Action(CEnemyBoss* boss) = 0;	// 行動
	virtual void Attack(CEnemyBoss* boss) = 0;	// 攻撃処理

	// モーションインデックス切り替え
	virtual void ChangeMotionIdx(CEnemyBoss* boss) = 0;
};

class CBossStep : public CBossState
{
public:
	// 行動
	virtual void Action(CEnemyBoss* boss) override
	{
		// 追い掛ける
		boss->ActChase();

		if (boss->IsCatchUp())
		{
			boss->ChangeATKState(boss->GetNextATKState());
		}
	}
};


// 攻撃
class CBossAttack : public CBossState
{
public:

	CBossAttack() : m_nIdxMotion(0) {}

	virtual void Action(CEnemyBoss* boss) override = 0;	// 行動


	virtual void Attack(CEnemyBoss* boss) override;	// 攻撃処理

	// モーションインデックス切り替え
	virtual void ChangeMotionIdx(CEnemyBoss* boss) override
	{
		// 派生クラスでインデックス設定されてる前提
		boss->SetMotion(m_nIdxMotion);
	}

protected:
	int m_nIdxMotion;
};

// 近接攻撃
class CBossProximity : public CBossAttack
{
public:

	CBossProximity() {}
	
	virtual void Action(CEnemyBoss* boss) override;	// 行動

	// モーションインデックス切り替え
	virtual void ChangeMotionIdx(CEnemyBoss* boss) override = 0;
};

// 遠距離攻撃
class CBossRemote : public CBossAttack
{
public:
	CBossRemote() {}

	virtual void Action(CEnemyBoss* boss) override;	// 行動

	// モーションインデックス切り替え
	virtual void ChangeMotionIdx(CEnemyBoss* boss) override = 0;
};




// 横なぎコンボ
class CBossSideSwipeCombo : public CBossProximity
{
public:
	CBossSideSwipeCombo() {}
	
	// モーションインデックス切り替え
	virtual void ChangeMotionIdx(CEnemyBoss* boss) override
	{
		m_nIdxMotion = CEnemyBoss::MOTION_SIDESWIPE;
		CBossAttack::ChangeMotionIdx(boss);
	}
};

// 振り下ろし
class CBossOverHead : public CBossProximity
{
public:
	CBossOverHead() {}

	// モーションインデックス切り替え
	virtual void ChangeMotionIdx(CEnemyBoss* boss) override
	{
		m_nIdxMotion = CEnemyBoss::MOTION_OVERHEADATK;
		CBossAttack::ChangeMotionIdx(boss);
	}
};

// ローリング
class CBossRolling : public CBossProximity
{
public:
	CBossRolling() {}

	// モーションインデックス切り替え
	virtual void ChangeMotionIdx(CEnemyBoss* boss) override
	{
		m_nIdxMotion = CEnemyBoss::MOTION_ROLLING;
		CBossAttack::ChangeMotionIdx(boss);
	}
};

// 瓦礫飛ばし
class CBossLaunchBallast : public CBossRemote
{
public:
	CBossLaunchBallast() {}

	// モーションインデックス切り替え
	virtual void ChangeMotionIdx(CEnemyBoss* boss) override
	{
		m_nIdxMotion = CEnemyBoss::MOTION_LAUNCHBALLAST;
		CBossAttack::ChangeMotionIdx(boss);
	}
};


#endif