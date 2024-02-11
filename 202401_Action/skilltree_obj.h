//=============================================================================
// 
//  �X�L���c���[�I�u�W�F�N�g�w�b�_�[ [skilltree_obj.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _SKILLTREE_OBJ_H_
#define _SKILLTREE_OBJ_H_	// ��d�C���N���[�h�h�~

#include "objectX.h"
#include "listmanager.h"

//==========================================================================
// �N���X��`
//==========================================================================
// �X�L���c���[�I�u�W�F�N�g�N���X
class CSkillTree_Obj : public CObjectX
{
public:

	CSkillTree_Obj(int nPriority = 3);
	~CSkillTree_Obj();

	// �I�[�o�[���C�h���ꂽ�֐�
	virtual HRESULT Init() override;
	virtual void Uninit() override;
	virtual void Update() override;
	virtual void Draw() override;

	// ���̑��֐�
	void StartUp();	// �N������
	void ReStartUp();	// �ċN������

	// �ÓI�֐�
	static CSkillTree_Obj* GetInstance();		// �C���X�^���X�擾
	static CSkillTree_Obj *Create();

private:

	//=============================
	// �񋓌^��`
	//=============================
	enum STATE	// ���
	{
		STATE_NONE = 0,	// �����Ȃ�
		STATE_STARTUP,	// �N��
		STATE_MAX
	};

	//=============================
	// �֐����X�g
	//=============================
	typedef void(CSkillTree_Obj::*STATE_FUNC)();
	static STATE_FUNC m_StateFuncList[];	// ��Ԋ֐��̃��X�g

	//=============================
	// �����o�֐�
	//=============================
	// ��Ԋ֐�
	void StateNone();		// �Ȃɂ��Ȃ�
	void StateStartUp();	// �N��

	// ���̑��֐�
	void CollisionPlayer();	// �v���C���[�Ƃ̓����蔻�菈��

	//=============================
	// �����o�ϐ�
	//=============================
	float m_fStateTime;	// ��ԃJ�E���^�[
	STATE m_state;		// ���
	Effekseer::Handle* m_pWeaponHandle;		// �G�t�F�N�g�n���h���̃|�C���^
	static CSkillTree_Obj* m_pThisPtr;		// ���g�̃|�C���^
};


#endif