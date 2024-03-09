//=============================================================================
// 
//  練習敵ヘッダー [enemy_practice.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _ENEMYPRACTICE_H_
#define _ENEMYPRACTICE_H_	// 二重インクルード防止

#include "enemy.h"

//==========================================================================
// クラス定義
//==========================================================================
// 練習敵クラス定義
class CEnemyPractice : public CEnemy
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
		MOTION_DOWN,				// ダウン
		MOTION_RETURNDOWN,			// ダウン
		MOTION_KNOCKBACK,			// やられ
		MOTION_FADEOUT,				// フェードアウト
		MOTION_FLINCH_NORMALATK,	// 通常攻撃怯み
		MOTION_MAX
	};

	CEnemyPractice();
	~CEnemyPractice();

	// オーバーライドされた関数
	virtual HRESULT Init() override;
	virtual void Uninit() override;
	virtual void Update() override;
	virtual void Draw() override;
	virtual void Kill() override;
	virtual bool Hit(const int nValue, const MyLib::Vector3& hitpos, CGameManager::AttackType atkType = CGameManager::AttackType::ATTACK_NORMAL) override;
	virtual void NormalHitResponse() override;	// ヒット時の反応
	virtual void CounterHitResponse() override;	// ヒット時の反応

private:

	//=============================
	// メンバ関数
	//=============================
	// 状態関数
	virtual void StateDown() override;		// ダウン状態

	// その他関数
	void MotionSet() override;	// モーションの設定
	void AttackAction(CMotion::AttackInfo ATKInfo, int nCntATK) override;		// 攻撃時処理
	void AttackInDicision(CMotion::AttackInfo* pATKInfo, int nCntATK) override;	// 攻撃判定中処理

};



// ダウン復帰
class CReturnDown_Practice : public CEnemyReturnDown
{
public:
	CReturnDown_Practice() {}

	// モーションインデックス切り替え
	virtual void ChangeMotionIdx(CEnemy* boss) override
	{
		m_nIdxMotion = CEnemyPractice::MOTION::MOTION_RETURNDOWN;
		CEnemyReturnDown::ChangeMotionIdx(boss);
	}
};

// 通常攻撃
class CAttack_PracticeNormalAttack : public CEnemyNormalAttack
{
public:
	CAttack_PracticeNormalAttack() 
	{ 
		m_bWillDirectlyTrans = true;

		// 向き合わせなし
		m_bSetAngleBeforeAttack = false;
		m_bSetAngleNotAttacking = false;
	}

	void Action(CEnemy* boss) override;	// 行動のオーバーライド

	// モーションインデックス切り替え
	virtual void ChangeMotionIdx(CEnemy* boss) override
	{
		m_nIdxMotion = CEnemy::MOTION_ATTACK_NORMAL;
		CEnemyNormalAttack::ChangeMotionIdx(boss);

		// 直接遷移する
		m_bWillDirectlyTrans = true;
	}
};

// 強攻撃
class CAttack_PracticeStrongAttack : public CEnemyStrongAttack
{
public:
	CAttack_PracticeStrongAttack()
	{
		m_bWillDirectlyTrans = true;

		// 向き合わせなし
		m_bSetAngleBeforeAttack = false;
		m_bSetAngleNotAttacking = false;
	}

	void Action(CEnemy* boss) override;	// 行動のオーバーライド

	// モーションインデックス切り替え
	virtual void ChangeMotionIdx(CEnemy* boss) override
	{
		m_nIdxMotion = CEnemy::MOTION_ATTACK_STRONG;
		CEnemyStrongAttack::ChangeMotionIdx(boss);

		// 直接遷移する
		m_bWillDirectlyTrans = true;
	}
};


#endif