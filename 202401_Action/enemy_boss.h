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
		MOTION_DOWN,			// �_�E��
		MOTION_RETURNDOWN,		// �_�E�����A
		MOTION_BACKSTEP,		// �o�b�N�X�e�b�v
		MOTION_BACKSTEP_SMALL,	// ���o�b�N�X�e�b�v
		MOTION_FADEOUT,			// �t�F�[�h�A�E�g
		MOTION_FLINCH_HANDSLAP,	// �n���h�X���b�v
		MOTION_FLINCH_OVERHEAD,	// �U�艺�낵�̋���
		MOTION_KNOCKBACK,		// �m�b�N�o�b�N
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
	virtual void StateDown() override;		// �_�E�����
	virtual void StateDead() override;		// ���S
	virtual void StateFadeOut() override;	// �t�F�[�h�A�E�g

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
// �_�E�����A
class CReturnDown_Boss : public CEnemyReturnDown
{
public:
	CReturnDown_Boss() {}

	// ���[�V�����C���f�b�N�X�؂�ւ�
	virtual void ChangeMotionIdx(CEnemy* boss) override
	{
		m_nIdxMotion = CEnemyBoss::MOTION::MOTION_RETURNDOWN;
		CEnemyReturnDown::ChangeMotionIdx(boss);
	}
};

// �X�e�b�v
class CBossStep : public CEnemyBeforeAction
{
public:

	CBossStep() {}

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


//=============================
// �ߐڌQ
//=============================
// ���Ȃ��R���{
class CBossSideSwipeCombo : public CEnemyProximity
{
public:
	CBossSideSwipeCombo() : CEnemyProximity(600.0f) 
	{ 
		m_bWillDirectlyTrans = true; 
		m_bSetAngleNotAttacking = true;	// �U������O�������킹�t���O
	}
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


// �U�艺�낵�̋���
class CFlinch_BossOverHead : public CEnemyFlinch
{
public:
	CFlinch_BossOverHead()
	{
		m_bCreateFirstTime = true;
	}

	// ���[�V�����C���f�b�N�X�؂�ւ�
	virtual void ChangeMotionIdx(CEnemy* boss) override
	{
		m_nIdxMotion = CEnemyBoss::MOTION::MOTION_FLINCH_OVERHEAD;
		CEnemyFlinch::ChangeMotionIdx(boss);
	}
};

// �U�艺�낵
class CBossOverHead : public CEnemyProximity
{
public:
	CBossOverHead() : CEnemyProximity(600.0f) 
	{ 
		m_bWillDirectlyTrans = true;

		// ���ލU���ɐݒ�
		SetFlinchAction(DEBUG_NEW CFlinch_BossOverHead());
	}

	// ���[�V�����C���f�b�N�X�؂�ւ�
	virtual void ChangeMotionIdx(CEnemy* enemy) override
	{
		m_nIdxMotion = CEnemyBoss::MOTION_OVERHEADATK;
		CEnemyAttack::ChangeMotionIdx(enemy);
		m_bWillDirectlyTrans = true;
	}
};


// �n���h�X���b�v�̋���
class CFlinch_BossHandSlap : public CEnemyFlinch
{
public:
	CFlinch_BossHandSlap()
	{
		m_bCreateFirstTime = true;
	}

	// ���[�V�����C���f�b�N�X�؂�ւ�
	virtual void ChangeMotionIdx(CEnemy* boss) override
	{
		m_nIdxMotion = CEnemyBoss::MOTION::MOTION_FLINCH_HANDSLAP;
		CEnemyFlinch::ChangeMotionIdx(boss);
	}
};

// �n���h�X���b�v
class CBossHandSlap : public CEnemyProximity
{
public:
	CBossHandSlap() : CEnemyProximity(600.0f)
	{
		m_bWillDirectlyTrans = true;
		m_bSetAngleNotAttacking = true;	// �U������O�������킹�t���O

		// ���ލU���ɐݒ�
		SetFlinchAction(DEBUG_NEW CFlinch_BossHandSlap());
	}

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