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
		MOTION_LAUNCHBALLAST,	// ���I��΂�
		MOTION_ROLLING,			// ���[�����O
		MOTION_KNOCKBACK,		// ���ꃂ�[�V����
		MOTION_FADEOUT,			// �t�F�[�h�A�E�g
		MOTION_MAX
	};


	CEnemyBoss(int nPriority = mylib_const::ENEMY_PRIORITY);
	~CEnemyBoss();

	// �I�[�o�[���C�h���ꂽ�֐�
	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
	void Kill(void) override;
	void NormalHitResponse() override;	// �q�b�g���̔���
	void CounterHitResponse() override;	// �q�b�g���̔���

	// �U����Ԑ؂�ւ�
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



	void ActChase(void);		// �ǂ��|��

	void PerformAttack();		// �U�����s����
	void DrawingRandomAction();	// �U�������_�����I

	bool IsCatchUp() { return m_bCatchUp; }

	CBossState* GetNextATKState() { return m_pNextATKState; }
private:


	//=============================
	// �֐����X�g
	//=============================

	//=============================
	// �����o�֐�
	//=============================
	// �s���֐�
	void ActionSet(void) override;		// �s���̐ݒ�
	void UpdateAction(void) override;	// �s���X�V
	void ActWait(void);					// �ҋ@

	// ���̑��֐�
	void MotionSet(void) override;	// ���[�V�����̐ݒ�
	void RotationTarget(void);		// �^�[�Q�b�g�̕�������
	void AttackAction(CMotion::AttackInfo ATKInfo, int nCntATK) override;		// �U��������
	void AttackInDicision(CMotion::AttackInfo ATKInfo, int nCntATK) override;	// �U�����蒆����

	//=============================
	// �����o�ϐ�
	//=============================
	float m_fActTime;		// �s���J�E���^�[
	bool m_bCatchUp;	// �ǂ���������
	CHP_GaugeBoss *m_pBossHPGauge;	// �{�X��HP�Q�[�W
	CBossState* m_pATKState;		// ���̍s���|�C���^
	CBossState* m_pNextATKState;	// ���̍s���|�C���^

	std::vector<CBossAttack*> m_pAtkPattern;
};



// �{�X�X�e�[�g
class CBossState
{
public:
	virtual void Action(CEnemyBoss* boss) = 0;	// �s��
	virtual void Attack(CEnemyBoss* boss) = 0;	// �U������

	// ���[�V�����C���f�b�N�X�؂�ւ�
	virtual void ChangeMotionIdx(CEnemyBoss* boss) = 0;
};

class CBossStep : public CBossState
{
public:
	// �s��
	virtual void Action(CEnemyBoss* boss) override
	{
		// �ǂ��|����
		boss->ActChase();

		if (boss->IsCatchUp())
		{
			boss->ChangeATKState(boss->GetNextATKState());
		}
	}
};


// �U��
class CBossAttack : public CBossState
{
public:

	CBossAttack() : m_nIdxMotion(0) {}

	virtual void Action(CEnemyBoss* boss) override = 0;	// �s��


	virtual void Attack(CEnemyBoss* boss) override;	// �U������

	// ���[�V�����C���f�b�N�X�؂�ւ�
	virtual void ChangeMotionIdx(CEnemyBoss* boss) override
	{
		// �h���N���X�ŃC���f�b�N�X�ݒ肳��Ă�O��
		boss->SetMotion(m_nIdxMotion);
	}

protected:
	int m_nIdxMotion;
};

// �ߐڍU��
class CBossProximity : public CBossAttack
{
public:

	CBossProximity() {}
	
	virtual void Action(CEnemyBoss* boss) override;	// �s��

	// ���[�V�����C���f�b�N�X�؂�ւ�
	virtual void ChangeMotionIdx(CEnemyBoss* boss) override = 0;
};

// �������U��
class CBossRemote : public CBossAttack
{
public:
	CBossRemote() {}

	virtual void Action(CEnemyBoss* boss) override;	// �s��

	// ���[�V�����C���f�b�N�X�؂�ւ�
	virtual void ChangeMotionIdx(CEnemyBoss* boss) override = 0;
};




// ���Ȃ��R���{
class CBossSideSwipeCombo : public CBossProximity
{
public:
	CBossSideSwipeCombo() {}
	
	// ���[�V�����C���f�b�N�X�؂�ւ�
	virtual void ChangeMotionIdx(CEnemyBoss* boss) override
	{
		m_nIdxMotion = CEnemyBoss::MOTION_SIDESWIPE;
		CBossAttack::ChangeMotionIdx(boss);
	}
};

// �U�艺�낵
class CBossOverHead : public CBossProximity
{
public:
	CBossOverHead() {}

	// ���[�V�����C���f�b�N�X�؂�ւ�
	virtual void ChangeMotionIdx(CEnemyBoss* boss) override
	{
		m_nIdxMotion = CEnemyBoss::MOTION_OVERHEADATK;
		CBossAttack::ChangeMotionIdx(boss);
	}
};

// ���[�����O
class CBossRolling : public CBossProximity
{
public:
	CBossRolling() {}

	// ���[�V�����C���f�b�N�X�؂�ւ�
	virtual void ChangeMotionIdx(CEnemyBoss* boss) override
	{
		m_nIdxMotion = CEnemyBoss::MOTION_ROLLING;
		CBossAttack::ChangeMotionIdx(boss);
	}
};

// ���I��΂�
class CBossLaunchBallast : public CBossRemote
{
public:
	CBossLaunchBallast() {}

	// ���[�V�����C���f�b�N�X�؂�ւ�
	virtual void ChangeMotionIdx(CEnemyBoss* boss) override
	{
		m_nIdxMotion = CEnemyBoss::MOTION_LAUNCHBALLAST;
		CBossAttack::ChangeMotionIdx(boss);
	}
};


#endif