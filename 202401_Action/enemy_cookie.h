//=============================================================================
// 
//  �N�b�L�[�G�w�b�_�[ [enemy_cookie.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _ENEMYCOOKIE_H_
#define _ENEMYCOOKIE_H_	// ��d�C���N���[�h�h�~

#include "enemy.h"

//==========================================================================
// �N���X��`
//==========================================================================
// �N�b�L�[�G�N���X��`
class CEnemyCookie : public CEnemy
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
		MOTION_PUNCH,			// �p���`���[�V����
		MOTION_DMG,			// �_���[�W
		MOTION_KNOCKBACK,		// ���ꃂ�[�V����
	};

	CEnemyCookie(int nPriority = mylib_const::ENEMY_PRIORITY);
	~CEnemyCookie();

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