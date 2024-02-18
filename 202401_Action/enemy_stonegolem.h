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
		MOTION_ATTACK_NORMAL2,	// �ʏ�U��2
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



// ���ガ�U��
class CEnemyGolemSideSwipe : public CEnemyProximity
{
public:
	CEnemyGolemSideSwipe() : CEnemyProximity(350.0f)
	{ 
		m_bWillDirectlyTrans = true;
	}

	// ���[�V�����C���f�b�N�X�؂�ւ�
	virtual void ChangeMotionIdx(CEnemy* boss) override
	{
		m_nIdxMotion = CEnemyGolem::MOTION_ATTACK_NORMAL2;
		CEnemyAttack::ChangeMotionIdx(boss);

		// ���ڑJ�ڂ���
		m_bWillDirectlyTrans = true;
	}
};


// �U��グ�U��
class CEnemyGolemUPSwipe : public CEnemyProximity
{
public:
	CEnemyGolemUPSwipe() : CEnemyProximity(350.0f)
	{
		m_bWillDirectlyTrans = true;
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


#endif