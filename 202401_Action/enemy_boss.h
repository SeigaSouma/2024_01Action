//=============================================================================
// 
//  �{�X�w�b�_�[ [enemy_boss.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _ENEMYBOSS_H_
#define _ENEMYBOSS_H_	// ��d�C���N���[�h�h�~

#include "enemy.h"

// �O���錾
class CHP_GaugeBoss;
class CEnemyState;
class CBossAttack;

//==========================================================================
// �N���X��`
//==========================================================================
// �{�X�N���X��`
class CEnemyBoss : public CEnemy
{
public:

	//=============================
	// �񋓌^��`
	//=============================
	//���[�V������
	enum MOTION
	{
		MOTION_DEF = 0,			// �j���[�g�������[�V����
		MOTION_WALK,			// �ړ����[�V����
		MOTION_OVERHEADATK,		// �U�艺�낵
		MOTION_SIDESWIPE,		// ���Ȃ�
		MOTION_HANDSLAP,		// �n���h�X���b�v
		MOTION_LAUNCHBALLAST,	// ���I��΂�
		MOTION_ROLLING,			// ���[�����O
		MOTION_DOWN,			// �_�E�����[�V����
		MOTION_BACKSTEP,		// �o�b�N�X�e�b�v
		MOTION_BACKSTEP_SMALL,	// ���o�b�N�X�e�b�v
		MOTION_FADEOUT,			// �t�F�[�h�A�E�g
		MOTION_MAX
	};


	CEnemyBoss(int nPriority = mylib_const::ENEMY_PRIORITY);
	~CEnemyBoss();

	// �I�[�o�[���C�h���ꂽ�֐�
	HRESULT Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;
	void Kill() override;
	void NormalHitResponse() override;	// �q�b�g���̔���
	void CounterHitResponse() override;	// �q�b�g���̔���

	bool SmallStep();	// ���X�e�b�v

	

	/*CEnemyState* GetATKState() { return m_pATKState; }
	CEnemyState* GetNextATKState() { return m_pNextATKState; }*/
private:


	//=============================
	// �֐����X�g
	//=============================

	//=============================
	// �����o�֐�
	//=============================
	// ��Ԋ֐�
	virtual void StateDown() override;	// �_�E�����


	// ���̑��֐�
	void MotionSet() override;	// ���[�V�����̐ݒ�
	void AttackAction(CMotion::AttackInfo ATKInfo, int nCntATK) override;		// �U��������
	void AttackInDicision(CMotion::AttackInfo* pATKInfo, int nCntATK) override;	// �U�����蒆����

	//=============================
	// �����o�ϐ�
	//=============================
	float m_fActTime;		// �s���J�E���^�[
	CHP_GaugeBoss *m_pBossHPGauge;	// �{�X��HP�Q�[�W

};



//=============================
// �{�X�X�e�[�g
//=============================
// �X�e�b�v
class CBossStep : public CEnemyBeforeAction
{
public:

	CBossStep() {}
	//
	//virtual void Action(CEnemy* enemy) override;		// �s��
	//virtual void Attack(CEnemy* enemy) override {}	// �U������

	// ���[�V�����C���f�b�N�X�؂�ւ�
	virtual void ChangeMotionIdx(CEnemy* enemy) override
	{
		m_nIdxMotion = CEnemyBoss::MOTION_BACKSTEP;
		CEnemyBeforeAction::ChangeMotionIdx(enemy);
	}
};

// ���X�e�b�v
class CBossStep_Small : public CBossStep
{
public:

	CBossStep_Small() {}

	// ���[�V�����C���f�b�N�X�؂�ւ�
	virtual void ChangeMotionIdx(CEnemy* enemy) override
	{
		m_nIdxMotion = CEnemyBoss::MOTION_BACKSTEP_SMALL;
		CEnemyBeforeAction::ChangeMotionIdx(enemy);
	}
};
//
//// �U��
//class CBossAttack : public CEnemyState
//{
//public:
//
//	CBossAttack() : m_bWillDirectlyTrans(true)
//	{
//		m_bCreateFirstTime = true;
//	}
//
//	virtual void Action(CEnemy* enemy) override = 0;	// �s��
//	virtual void Attack(CEnemy* enemy) override;	// �U������
//
//
//	// ���[�V�����C���f�b�N�X�؂�ւ�
//	virtual void ChangeMotionIdx(CEnemy* enemy) override
//	{
//		CEnemyState::ChangeMotionIdx(enemy);
//	}
//
//	// �J�ڑO����
//	virtual void BeforeTransitionProcess(CEnemy* enemy) 
//	{
//		enemy->RotationTarget();
//		enemy->ActChase(1.0f, 600.0f);
//	}	// �J�ڑO����
//
//	bool IsDirectlyTrans() { return m_bWillDirectlyTrans; }	// ���ڑJ�ڃt���O�擾
//
//
//protected:
//	bool m_bWillDirectlyTrans;	// ���ڑJ�ڃt���O
//};
//
//// �ߐڍU��
//class CBossProximity : public CEnemyAttack
//{
//public:
//
//	CBossProximity() {}
//	
//	virtual void Action(CEnemy* enemy) override;	// �s��
//	virtual void ChangeMotionIdx(CEnemy* enemy) override = 0;	// ���[�V�����C���f�b�N�X�؂�ւ�
//
//};
//
//// �������U��
//class CBossRemote : public CEnemyAttack
//{
//public:
//	CBossRemote() {}
//
//	virtual void Action(CEnemy* enemy) override;	// �s��
//	virtual void ChangeMotionIdx(CEnemy* enemy) override = 0;	// ���[�V�����C���f�b�N�X�؂�ւ�
//};



namespace UtilFunc 
{
	namespace Transformation
	{
		int Random(int nMinNum, int nMaxNum);
	}
}

//=============================
// �ߐڌQ
//=============================
// ���Ȃ��R���{
class CBossSideSwipeCombo : public CEnemyProximity
{
public:
	CBossSideSwipeCombo() : CEnemyProximity(600.0f) { m_bWillDirectlyTrans = true; }
	virtual void BeforeAttack(CEnemy* enemy) override;	// �U���O����

	// ���[�V�����C���f�b�N�X�؂�ւ�
	virtual void ChangeMotionIdx(CEnemy* enemy) override
	{
		m_nIdxMotion = CEnemyBoss::MOTION_SIDESWIPE;
		CEnemyAttack::ChangeMotionIdx(enemy);
		m_bWillDirectlyTrans = true;

		// �U���O�s���t���O
		m_bBeforeAttackAction = true;
		if (UtilFunc::Transformation::Random(0, 2) == 0)
		{
			m_bBeforeAttackAction = false;
		}
	}

	// �J�ڑO����
	virtual void BeforeTransitionProcess(CEnemy* enemy) override
	{
		// ���ލs����ݒ�
		enemy->ChangeATKState(DEBUG_NEW CBossStep_Small());
		enemy->GetATKState()->ChangeMotionIdx(enemy);
	}
};

// �U�艺�낵
class CBossOverHead : public CEnemyProximity
{
public:
	CBossOverHead() : CEnemyProximity(600.0f) { m_bWillDirectlyTrans = true; }

	//virtual void BeforeAttack(CEnemy* enemy) override;	// �U���O����

	// ���[�V�����C���f�b�N�X�؂�ւ�
	virtual void ChangeMotionIdx(CEnemy* enemy) override
	{
		m_nIdxMotion = CEnemyBoss::MOTION_OVERHEADATK;
		CEnemyAttack::ChangeMotionIdx(enemy);
		m_bWillDirectlyTrans = true;
	}
};


// �n���h�X���b�v
class CBossHandSlap : public CEnemyProximity
{
public:
	CBossHandSlap() : CEnemyProximity(600.0f) { m_bWillDirectlyTrans = true; }

	// ���[�V�����C���f�b�N�X�؂�ւ�
	virtual void ChangeMotionIdx(CEnemy* enemy) override
	{
		m_nIdxMotion = CEnemyBoss::MOTION_HANDSLAP;
		CEnemyAttack::ChangeMotionIdx(enemy);
		m_bWillDirectlyTrans = true;
	}
};

// ���[�����O
class CBossRolling : public CEnemyProximity
{
public:
	CBossRolling() : CEnemyProximity(600.0f) { m_bWillDirectlyTrans = false; }

	// �s��
	virtual void Action(CEnemy* enemy) override
	{
		Attack(enemy);
	}

	// ���[�V�����C���f�b�N�X�؂�ւ�
	virtual void ChangeMotionIdx(CEnemy* enemy) override
	{
		m_nIdxMotion = CEnemyBoss::MOTION_ROLLING;
		CEnemyAttack::ChangeMotionIdx(enemy);
		m_bWillDirectlyTrans = false;
	}

	// �J�ڑO����
	virtual void BeforeTransitionProcess(CEnemy* enemy) override 
	{
		// ���ލs����ݒ�
		enemy->ChangeATKState(DEBUG_NEW CBossStep());
		enemy->GetATKState()->ChangeMotionIdx(enemy);
	}

};

//=============================
// �������Q
//=============================
// ���I��΂�
class CBossLaunchBallast : public CEnemyRemote
{
public:
	CBossLaunchBallast() { m_bWillDirectlyTrans = true; }

	// ���[�V�����C���f�b�N�X�؂�ւ�
	virtual void ChangeMotionIdx(CEnemy* enemy) override
	{
		m_nIdxMotion = CEnemyBoss::MOTION_LAUNCHBALLAST;
		CEnemyAttack::ChangeMotionIdx(enemy);
		m_bWillDirectlyTrans = true;
	}
};


#endif