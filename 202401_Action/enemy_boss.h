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
class CBossState;
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

	// �U����Ԑ؂�ւ�
	void ChangeATKState(CBossState* state);
	void ChangeNextATKState(CBossState* state) { m_pNextATKState = state; }

	void ActChase();		// �ǂ��|��
	void RotationTarget(float range = 90.0f);	// �^�[�Q�b�g�̕�������
	bool SmallStep();	// ���X�e�b�v

	void PerformAttack();		// �U�����s����
	void DrawingRandomAction();	// �U�������_�����I
	void ChangeNextAction();	// ���̍U���֐؂�ւ�

	bool IsCatchUp() { return m_bCatchUp; }	// �ǂ���������
	bool IsInSight() { return m_bInSight; }	// ���E������

	CBossState* GetNextATKState() { return m_pNextATKState; }
private:


	//=============================
	// �֐����X�g
	//=============================

	//=============================
	// �����o�֐�
	//=============================
	// �s���֐�
	void ActionSet() override;		// �s���̐ݒ�
	void UpdateAction() override;	// �s���X�V
	void ActWait();					// �ҋ@

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
	bool m_bCatchUp;	// �ǂ���������
	bool m_bInSight;	// ���E������
	CHP_GaugeBoss *m_pBossHPGauge;	// �{�X��HP�Q�[�W
	CBossState* m_pATKState;		// ���̍s���|�C���^
	CBossState* m_pNextATKState;	// ���̍s���|�C���^

	std::vector<CBossAttack*> m_pAtkPattern;
};



//=============================
// �{�X�X�e�[�g
//=============================
class CBossState
{
public:
	CBossState() : m_bCreateFirstTime(false), m_bBeforeAttackAction(false) {}

	virtual void Action(CEnemyBoss* boss) = 0;	// �s��
	virtual void Attack(CEnemyBoss* boss) = 0;	// �U������
	virtual void ChangeMotionIdx(CEnemyBoss* boss) = 0;	// ���[�V�����C���f�b�N�X�؂�ւ�
	virtual void BeforeAttack(CEnemyBoss* boss) { m_bBeforeAttackAction = true; }	// �U���O����

	bool IsCreateFirstTime() { return m_bCreateFirstTime; }	// ���񐶐��̃t���O

protected:
	bool m_bCreateFirstTime;	// ���񐶐��̃t���O
	bool m_bBeforeAttackAction;	// �U���O�s���t���O
};

// �X�e�b�v
class CBossStep : public CBossState
{
public:

	CBossStep() {}
	
	virtual void Action(CEnemyBoss* boss) override;		// �s��
	virtual void Attack(CEnemyBoss* boss) override {}	// �U������

	// ���[�V�����C���f�b�N�X�؂�ւ�
	virtual void ChangeMotionIdx(CEnemyBoss* boss) override
	{
		boss->SetMotion(CEnemyBoss::MOTION_BACKSTEP);
	}
};

// ���X�e�b�v
class CBossStep_Small : public CBossStep
{
public:

	CBossStep_Small() {}

	virtual void Action(CEnemyBoss* boss) override;		// �s��

	// ���[�V�����C���f�b�N�X�؂�ւ�
	virtual void ChangeMotionIdx(CEnemyBoss* boss) override
	{
		boss->SetMotion(CEnemyBoss::MOTION_BACKSTEP_SMALL);
	}
};

// �U��
class CBossAttack : public CBossState
{
public:

	CBossAttack() : m_nIdxMotion(0), m_bWillDirectlyTrans(true)
	{
		m_bCreateFirstTime = true;
	}

	virtual void Action(CEnemyBoss* boss) override = 0;	// �s��
	virtual void Attack(CEnemyBoss* boss) override;	// �U������


	// ���[�V�����C���f�b�N�X�؂�ւ�
	virtual void ChangeMotionIdx(CEnemyBoss* boss) override
	{
		// �h���N���X�ŃC���f�b�N�X�ݒ肳��Ă�O��
		boss->SetMotion(m_nIdxMotion);
		m_bBeforeAttackAction = false;	// �U���O�s���t���O
	}

	virtual void BeforeTransitionProcess(CEnemyBoss* boss) 
	{
		boss->RotationTarget();
		boss->ActChase();
	}	// �J�ڑO����

	bool IsDirectlyTrans() { return m_bWillDirectlyTrans; }	// ���ڑJ�ڃt���O�擾


protected:
	int m_nIdxMotion;			// ���[�V�����ԍ�
	bool m_bWillDirectlyTrans;	// ���ڑJ�ڃt���O
};

// �ߐڍU��
class CBossProximity : public CBossAttack
{
public:

	CBossProximity() {}
	
	virtual void Action(CEnemyBoss* boss) override;	// �s��
	virtual void ChangeMotionIdx(CEnemyBoss* boss) override = 0;	// ���[�V�����C���f�b�N�X�؂�ւ�

};

// �������U��
class CBossRemote : public CBossAttack
{
public:
	CBossRemote() {}

	virtual void Action(CEnemyBoss* boss) override;	// �s��
	virtual void ChangeMotionIdx(CEnemyBoss* boss) override = 0;	// ���[�V�����C���f�b�N�X�؂�ւ�
};



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
class CBossSideSwipeCombo : public CBossProximity
{
public:
	CBossSideSwipeCombo() { m_bWillDirectlyTrans = true; }
	virtual void BeforeAttack(CEnemyBoss* boss) override;	// �U���O����

	// ���[�V�����C���f�b�N�X�؂�ւ�
	virtual void ChangeMotionIdx(CEnemyBoss* boss) override
	{
		m_nIdxMotion = CEnemyBoss::MOTION_SIDESWIPE;
		CBossAttack::ChangeMotionIdx(boss);
		m_bWillDirectlyTrans = true;

		// �U���O�s���t���O
		m_bBeforeAttackAction = true;
		if (UtilFunc::Transformation::Random(0, 2) == 0)
		{
			m_bBeforeAttackAction = false;
		}
	}
};

// �U�艺�낵
class CBossOverHead : public CBossProximity
{
public:
	CBossOverHead() { m_bWillDirectlyTrans = true; }

	//virtual void BeforeAttack(CEnemyBoss* boss) override;	// �U���O����

	// ���[�V�����C���f�b�N�X�؂�ւ�
	virtual void ChangeMotionIdx(CEnemyBoss* boss) override
	{
		m_nIdxMotion = CEnemyBoss::MOTION_OVERHEADATK;
		CBossAttack::ChangeMotionIdx(boss);
		m_bWillDirectlyTrans = true;
	}

	//// �J�ڑO����
	//virtual void BeforeTransitionProcess(CEnemyBoss* boss) override
	//{
	//	// ���ލs����ݒ�
	//	boss->ChangeATKState(DEBUG_NEW CBossStep_Small());
	//}
};


// �n���h�X���b�v
class CBossHandSlap : public CBossProximity
{
public:
	CBossHandSlap() { m_bWillDirectlyTrans = true; }

	// ���[�V�����C���f�b�N�X�؂�ւ�
	virtual void ChangeMotionIdx(CEnemyBoss* boss) override
	{
		m_nIdxMotion = CEnemyBoss::MOTION_HANDSLAP;
		CBossAttack::ChangeMotionIdx(boss);
		m_bWillDirectlyTrans = true;
	}
};

// ���[�����O
class CBossRolling : public CBossProximity
{
public:
	CBossRolling() { m_bWillDirectlyTrans = false; }

	// �s��
	virtual void Action(CEnemyBoss* boss) override
	{
		Attack(boss);
	}

	// ���[�V�����C���f�b�N�X�؂�ւ�
	virtual void ChangeMotionIdx(CEnemyBoss* boss) override
	{
		m_nIdxMotion = CEnemyBoss::MOTION_ROLLING;
		CBossAttack::ChangeMotionIdx(boss);
		m_bWillDirectlyTrans = false;
	}

	// �J�ڑO����
	virtual void BeforeTransitionProcess(CEnemyBoss* boss) override 
	{
		// ���ލs����ݒ�
		boss->ChangeATKState(DEBUG_NEW CBossStep());
	}

};

//=============================
// �������Q
//=============================
// ���I��΂�
class CBossLaunchBallast : public CBossRemote
{
public:
	CBossLaunchBallast() { m_bWillDirectlyTrans = true; }

	// ���[�V�����C���f�b�N�X�؂�ւ�
	virtual void ChangeMotionIdx(CEnemyBoss* boss) override
	{
		m_nIdxMotion = CEnemyBoss::MOTION_LAUNCHBALLAST;
		CBossAttack::ChangeMotionIdx(boss);
		m_bWillDirectlyTrans = true;
	}
};


#endif