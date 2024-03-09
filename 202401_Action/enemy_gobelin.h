//=============================================================================
// 
//  �S�u�����G�w�b�_�[ [enemy_gobelin.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _ENEMYGOBELIN_H_
#define _ENEMYGOBELIN_H_	// ��d�C���N���[�h�h�~

#include "enemy.h"

//==========================================================================
// �N���X��`
//==========================================================================
// �S�u�����G�N���X��`
class CEnemyGobelin : public CEnemy
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
		MOTION_RETURNDOWN,				// �_�E��
		MOTION_KNOCKBACK,		// ����
		MOTION_FADEOUT,			// �t�F�[�h�A�E�g
		MOTION_FLINCH_NORMALATK,			// �t�F�[�h�A�E�g
		MOTION_MAX
	};

	CEnemyGobelin();
	~CEnemyGobelin();

	// �I�[�o�[���C�h���ꂽ�֐�
	HRESULT Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;
	void Kill() override;

private:

	//=============================
	// �����o�֐�
	//=============================
	// ��Ԋ֐�
	virtual void StateDown() override;		// �_�E�����
	//virtual void StateStrongAtk() override;	// ���U�����

	// ���̑��֐�
	void MotionSet() override;	// ���[�V�����̐ݒ�
	void AttackAction(CMotion::AttackInfo ATKInfo, int nCntATK) override;		// �U��������
	void AttackInDicision(CMotion::AttackInfo* pATKInfo, int nCntATK) override;	// �U�����蒆����

};



// �_�E�����A
class CReturnDown_Gobelin : public CEnemyReturnDown
{
public:
	CReturnDown_Gobelin() {}

	// ���[�V�����C���f�b�N�X�؂�ւ�
	virtual void ChangeMotionIdx(CEnemy* boss) override
	{
		m_nIdxMotion = CEnemyGobelin::MOTION::MOTION_RETURNDOWN;
		CEnemyReturnDown::ChangeMotionIdx(boss);
	}
};

// �ʏ�U���̋���
class CFlinch_GobelinNormalATK : public CEnemyFlinch
{
public:
	CFlinch_GobelinNormalATK() 
	{
		m_bCreateFirstTime = true;
	}

	// ���[�V�����C���f�b�N�X�؂�ւ�
	virtual void ChangeMotionIdx(CEnemy* boss) override
	{
		m_nIdxMotion = CEnemyGobelin::MOTION::MOTION_FLINCH_NORMALATK;
		CEnemyFlinch::ChangeMotionIdx(boss);
	}
};

// �ʏ�U��
class CAttack_GobelinNormalAttack : public CEnemyNormalAttack
{
public:
	CAttack_GobelinNormalAttack() 
	{ 
		m_bWillDirectlyTrans = true;

		// ���ލU���ɐݒ�
		SetFlinchAction(DEBUG_NEW CFlinch_GobelinNormalATK());
	}

	// ���[�V�����C���f�b�N�X�؂�ւ�
	virtual void ChangeMotionIdx(CEnemy* boss) override
	{
		m_nIdxMotion = CEnemy::MOTION_ATTACK_NORMAL;
		CEnemyNormalAttack::ChangeMotionIdx(boss);

		// ���ڑJ�ڂ���
		m_bWillDirectlyTrans = true;
	}
};


#endif