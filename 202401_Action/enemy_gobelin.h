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
		MOTION_KNOCKBACK,		// ����
		MOTION_FADEOUT,			// �t�F�[�h�A�E�g
		MOTION_MAX
	};

	CEnemyGobelin(int nPriority = mylib_const::ENEMY_PRIORITY);
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



#endif