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

	// �s����
	enum ACTION
	{
		ACTION_CHASE = 0,	// �Ǐ]
		ACTION_PROXIMITY,	// �ߐڍU��
		ACTION_WAIT,		// �ҋ@
		ACTION_MAX
	};

	CEnemyCookie(int nPriority = mylib_const::ENEMY_PRIORITY);
	~CEnemyCookie();

	// �I�[�o�[���C�h���ꂽ�֐�
	HRESULT Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;
	void Kill() override;

	void SetAction(ACTION action) { m_Action = action; }	// �A�N�V�����ݒ�

private:


	//=============================
	// �֐����X�g
	//=============================
	typedef void(CEnemyCookie::*ACT_FUNC)();
	static ACT_FUNC m_ActFuncList[];	// �s���֐����X�g

	//=============================
	// �����o�֐�
	//=============================
	// �s���֐�
	void ActionSet() override;		// �s���̐ݒ�
	void UpdateAction() override;	// �s���X�V
	void ActChase();				// �ǂ��|��
	void ActAttackProximity();		// �ߐڍU��
	void ActWait();					// �ҋ@

	// �s�����֐�
	void ChaseNormal();		// �����ǂ��|��
	void AttackPunch();		// �p���`�U��

	// ���̑��֐�
	void MotionSet() override;	// ���[�V�����̐ݒ�
	void RotationTarget();		// �^�[�Q�b�g�̕�������
	void AttackAction(CMotion::AttackInfo ATKInfo, int nCntATK) override;		// �U��������
	void AttackInDicision(CMotion::AttackInfo* pATKInfo, int nCntATK) override;	// �U�����蒆����

	//=============================
	// �����o�ϐ�
	//=============================
	ACTION m_Action;		// �s��
	float m_fActTime;		// �s���J�E���^�[
	bool m_bCatchUp;	// �ǂ���������
};



#endif