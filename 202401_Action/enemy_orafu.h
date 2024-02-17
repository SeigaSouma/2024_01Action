//=============================================================================
// 
//  �I���t�G�w�b�_�[ [enemy_orafu.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _ENEMYORAFU_H_
#define _ENEMYORAFU_H_	// ��d�C���N���[�h�h�~

#include "enemy.h"

//==========================================================================
// �N���X��`
//==========================================================================
// �I���t�G�N���X��`
class CEnemyOrafu : public CEnemy
{
public:

	//=============================
	// �񋓌^��`
	//=============================
	//���[�V������
	enum MOTION
	{
		MOTION_DEF = 0,		// �j���[�g�������[�V����
		MOTION_WALK,		// �ړ����[�V����
		MOTION_PUNCH,		// �p���`���[�V����
		MOTION_DMG,			// �_���[�W
		MOTION_KNOCKBACK,	// ���ꃂ�[�V����
	};

	CEnemyOrafu(int nPriority = mylib_const::ENEMY_PRIORITY);
	~CEnemyOrafu();

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



#endif