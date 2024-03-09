//=============================================================================
// 
//  ���K�G�w�b�_�[ [enemy_practice.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _ENEMYPRACTICE_H_
#define _ENEMYPRACTICE_H_	// ��d�C���N���[�h�h�~

#include "enemy.h"

//==========================================================================
// �N���X��`
//==========================================================================
// ���K�G�N���X��`
class CEnemyPractice : public CEnemy
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
		MOTION_DOWN,				// �_�E��
		MOTION_RETURNDOWN,			// �_�E��
		MOTION_KNOCKBACK,			// ����
		MOTION_FADEOUT,				// �t�F�[�h�A�E�g
		MOTION_FLINCH_NORMALATK,	// �ʏ�U������
		MOTION_MAX
	};

	CEnemyPractice();
	~CEnemyPractice();

	// �I�[�o�[���C�h���ꂽ�֐�
	virtual HRESULT Init() override;
	virtual void Uninit() override;
	virtual void Update() override;
	virtual void Draw() override;
	virtual void Kill() override;
	virtual bool Hit(const int nValue, const MyLib::Vector3& hitpos, CGameManager::AttackType atkType = CGameManager::AttackType::ATTACK_NORMAL) override;
	virtual void NormalHitResponse() override;	// �q�b�g���̔���
	virtual void CounterHitResponse() override;	// �q�b�g���̔���

private:

	//=============================
	// �����o�֐�
	//=============================
	// ��Ԋ֐�
	virtual void StateDown() override;		// �_�E�����

	// ���̑��֐�
	void MotionSet() override;	// ���[�V�����̐ݒ�
	void AttackAction(CMotion::AttackInfo ATKInfo, int nCntATK) override;		// �U��������
	void AttackInDicision(CMotion::AttackInfo* pATKInfo, int nCntATK) override;	// �U�����蒆����

};



// �_�E�����A
class CReturnDown_Practice : public CEnemyReturnDown
{
public:
	CReturnDown_Practice() {}

	// ���[�V�����C���f�b�N�X�؂�ւ�
	virtual void ChangeMotionIdx(CEnemy* boss) override
	{
		m_nIdxMotion = CEnemyPractice::MOTION::MOTION_RETURNDOWN;
		CEnemyReturnDown::ChangeMotionIdx(boss);
	}
};

// �ʏ�U��
class CAttack_PracticeNormalAttack : public CEnemyNormalAttack
{
public:
	CAttack_PracticeNormalAttack() 
	{ 
		m_bWillDirectlyTrans = true;

		// �������킹�Ȃ�
		m_bSetAngleBeforeAttack = false;
		m_bSetAngleNotAttacking = false;
	}

	void Action(CEnemy* boss) override;	// �s���̃I�[�o�[���C�h

	// ���[�V�����C���f�b�N�X�؂�ւ�
	virtual void ChangeMotionIdx(CEnemy* boss) override
	{
		m_nIdxMotion = CEnemy::MOTION_ATTACK_NORMAL;
		CEnemyNormalAttack::ChangeMotionIdx(boss);

		// ���ڑJ�ڂ���
		m_bWillDirectlyTrans = true;
	}
};

// ���U��
class CAttack_PracticeStrongAttack : public CEnemyStrongAttack
{
public:
	CAttack_PracticeStrongAttack()
	{
		m_bWillDirectlyTrans = true;

		// �������킹�Ȃ�
		m_bSetAngleBeforeAttack = false;
		m_bSetAngleNotAttacking = false;
	}

	void Action(CEnemy* boss) override;	// �s���̃I�[�o�[���C�h

	// ���[�V�����C���f�b�N�X�؂�ւ�
	virtual void ChangeMotionIdx(CEnemy* boss) override
	{
		m_nIdxMotion = CEnemy::MOTION_ATTACK_STRONG;
		CEnemyStrongAttack::ChangeMotionIdx(boss);

		// ���ڑJ�ڂ���
		m_bWillDirectlyTrans = true;
	}
};


#endif