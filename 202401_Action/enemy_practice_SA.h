//=============================================================================
// 
//  ���K�G(���U��)�w�b�_�[ [enemy_practice_SA.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _ENEMYPRACTICE_SA_H_
#define _ENEMYPRACTICE_SA_H_	// ��d�C���N���[�h�h�~

#include "enemy_practice.h"

//==========================================================================
// �N���X��`
//==========================================================================
// ���K�G(�U��)�N���X��`
class CEnemyPractice_SA : public CEnemyPractice
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

	CEnemyPractice_SA(int nPriority = mylib_const::ENEMY_PRIORITY);
	~CEnemyPractice_SA();

	// �I�[�o�[���C�h���ꂽ�֐�
	HRESULT Init() override;

private:

};

#endif