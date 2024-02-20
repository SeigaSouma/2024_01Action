//=============================================================================
// 
//  �X�g�[���S�[�����G�w�b�_�[ [enemy_stonegolem.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _ENEMYSTONEGOLEM_H_
#define _ENEMYSTONEGOLEM_H_	// ��d�C���N���[�h�h�~

#include "enemy.h"

//==========================================================================
// �N���X��`
//==========================================================================
// �X�g�[���S�[�����G�N���X
class CEnemyGolem : public CEnemy
{
public:

	//=============================
	// �񋓌^��`
	//=============================
	//���[�V������
	enum MOTION
	{
		MOTION_DEF = 0,			// �j���[�g����
		MOTION_WALK,			// �ړ�
		MOTION_ATTACK_NORMAL,	// �ʏ�U��
		MOTION_ATTACK_STRONG,	// ���U��
		MOTION_DMG,				// �_���[�W
		MOTION_DOWN,			// �_�E��
		MOTION_KNOCKBACK,		// ����
		MOTION_FADEOUT,			// �t�F�[�h�A�E�g
		MOTION_ATTACK_SIDESWIPE,	// ���ガ
		MOTION_ATTACK_SIDESWIPE2,	// ���ガ2
		MOTION_ATTACK_UPSWIPE2,		// �c�ガ2
		MOTION_ATTACK_FLINCH_UPSWIPE,	// �ʏ�U��
		MOTION_ATTACK_FLINCH_SIDESWIPE,	// �ʏ�U��
		MOTION_MAX
	};

	CEnemyGolem(int nPriority = mylib_const::ENEMY_PRIORITY);
	~CEnemyGolem();

	// �I�[�o�[���C�h���ꂽ�֐�
	HRESULT Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;
	void Kill() override;


private:


	//=============================
	// �֐����X�g
	//=============================

	//=============================
	// �����o�֐�
	//=============================

	// ���̑��֐�
	void MotionSet() override;	// ���[�V�����̐ݒ�
	void AttackAction(CMotion::AttackInfo ATKInfo, int nCntATK) override;		// �U��������
	void AttackInDicision(CMotion::AttackInfo* pATKInfo, int nCntATK) override;	// �U�����蒆����

	//=============================
	// �����o�ϐ�
	//=============================
};




// �c�ガ(4�i��)
class CEnemyGolemUpSwipe2 : public CEnemyProximity
{
public:
	CEnemyGolemUpSwipe2() : CEnemyProximity(350.0f)
	{
		m_bWillDirectlyTrans = true;
	}

	// ���[�V�����C���f�b�N�X�؂�ւ�
	virtual void ChangeMotionIdx(CEnemy* boss) override
	{
		m_nIdxMotion = CEnemyGolem::MOTION_ATTACK_UPSWIPE2;
		CEnemyAttack::ChangeMotionIdx(boss);

		// ���ڑJ�ڂ���
		m_bWillDirectlyTrans = true;
	}
};


// 3�i�ڂ̋���
class CFlinch_GolemSideSwipe : public CEnemyFlinch
{
public:
	CFlinch_GolemSideSwipe()
	{
		m_bCreateFirstTime = true;
	}

	// ���[�V�����C���f�b�N�X�؂�ւ�
	virtual void ChangeMotionIdx(CEnemy* boss) override
	{
		m_nIdxMotion = CEnemyGolem::MOTION::MOTION_ATTACK_FLINCH_SIDESWIPE;
		CEnemyFlinch::ChangeMotionIdx(boss);
	}
};

// ���ガ(3�i��)
class CEnemyGolemSideSwipe2 : public CEnemyProximity
{
public:
	CEnemyGolemSideSwipe2() : CEnemyProximity(350.0f)
	{
		m_bWillDirectlyTrans = true;

		// ���ލU���ɐݒ�
		SetFlinchAction(DEBUG_NEW CFlinch_GolemSideSwipe());
	}

	// ���[�V�����C���f�b�N�X�؂�ւ�
	virtual void ChangeMotionIdx(CEnemy* boss) override
	{
		m_nIdxMotion = CEnemyGolem::MOTION_ATTACK_SIDESWIPE2;
		CEnemyAttack::ChangeMotionIdx(boss);

		// ���ڑJ�ڂ���
		m_bWillDirectlyTrans = true;

		if (UtilFunc::Transformation::Random(0, 1) == 0)
		{
			// �A���U���ݒ�
			SetChainAttack(DEBUG_NEW CEnemyGolemUpSwipe2());
		}
	}
};

// ���ガ�U��(2�i��)
class CEnemyGolemSideSwipe : public CEnemyProximity
{
public:
	CEnemyGolemSideSwipe() : CEnemyProximity(400.0f)
	{ 
		m_bWillDirectlyTrans = true;
	}

	// ���[�V�����C���f�b�N�X�؂�ւ�
	virtual void ChangeMotionIdx(CEnemy* boss) override
	{
		m_nIdxMotion = CEnemyGolem::MOTION_ATTACK_SIDESWIPE;
		CEnemyAttack::ChangeMotionIdx(boss);

		// ���ڑJ�ڂ���
		m_bWillDirectlyTrans = true;

		if (UtilFunc::Transformation::Random(0, 1) == 0)
		{
			// �A���U���ݒ�
			SetChainAttack(DEBUG_NEW CEnemyGolemSideSwipe2());
		}
	}
};



// 1�i�ڂ̋���
class CFlinch_GolemUPSwipe : public CEnemyFlinch
{
public:
	CFlinch_GolemUPSwipe()
	{
		m_bCreateFirstTime = true;
	}

	// ���[�V�����C���f�b�N�X�؂�ւ�
	virtual void ChangeMotionIdx(CEnemy* boss) override
	{
		m_nIdxMotion = CEnemyGolem::MOTION::MOTION_ATTACK_FLINCH_UPSWIPE;
		CEnemyFlinch::ChangeMotionIdx(boss);
	}
};

// �U��グ�U��(1�i��)
class CEnemyGolemUPSwipe : public CEnemyProximity
{
public:
	CEnemyGolemUPSwipe() : CEnemyProximity(400.0f)
	{
		m_bWillDirectlyTrans = true;

		// ���ލU���ɐݒ�
		SetFlinchAction(DEBUG_NEW CFlinch_GolemUPSwipe());
	}

	// ���[�V�����C���f�b�N�X�؂�ւ�
	virtual void ChangeMotionIdx(CEnemy* boss) override
	{
		m_nIdxMotion = CEnemyGolem::MOTION_ATTACK_NORMAL;
		CEnemyAttack::ChangeMotionIdx(boss);

		// ���ڑJ�ڂ���
		m_bWillDirectlyTrans = true;

		// �A���U���ݒ�
		SetChainAttack(DEBUG_NEW CEnemyGolemSideSwipe());
	}
};


// ���U��
class CGolemStrongAttack : public CEnemyStrongAttack
{
public:
	CGolemStrongAttack() : CEnemyStrongAttack(400.0f)
	{
		m_bWillDirectlyTrans = true;
		m_bSetAngleNotAttacking = true;	// �U������O�������킹�t���O
	}

	// ���[�V�����C���f�b�N�X�؂�ւ�
	virtual void ChangeMotionIdx(CEnemy* boss) override
	{
		m_nIdxMotion = CEnemyGolem::MOTION::MOTION_ATTACK_STRONG;
		CEnemyStrongAttack::ChangeMotionIdx(boss);

		// ���ڑJ�ڂ���
		m_bWillDirectlyTrans = true;
	}
};

#endif