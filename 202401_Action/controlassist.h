//=============================================================================
// 
// ����⏕���� [controlassist.h]
// Author : ���n�Ή�
// 
//=============================================================================

#ifndef _CONTROLASSIST_H_
#define _CONTROLASSIST_H_		// ��d�C���N���[�h�h�~�̃}�N�����`����

#include "object2D.h"

//==========================================================================
// �N���X��`
//==========================================================================
// ����⏕�N���X��`
class CControlAssist : public CObject
{
public:

	//=============================
	// �񋓌^��`
	//=============================
	enum CONTROLTYPE
	{
		CONTROLTYPE_DASH = 0,			// �_�b�V��
		CONTROLTYPE_ROCKON,		// ���b�N�I��
		CONTROLTYPE_CHANGETARGET,	// �^�[�Q�b�g�ؑ�
		CONTROLTYPE_ATTACK_NORMAL,	// �ʏ�U��
		CONTROLTYPE_ATTACK_CHARGE,	// �`���[�W�U��
		CONTROLTYPE_GUARD,			// �K�[�h
		CONTROLTYPE_COUNTER,		// ����
		CONTROLTYPE_AVOID,			// ���
		CONTROLTYPE_MAX
	};

	CControlAssist(int nPriority = 0);
	~CControlAssist();

	//  �I�[�o�[���C�h�֐�
	HRESULT Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override {}

	void SetDefaultText();			// �f�t�H���g�e�L�X�g�ݒ�
	void SetText(CONTROLTYPE text);	// �e�L�X�g�ݒ�
	void ResetText();	// �e�L�X�g���Z�b�g

	static CControlAssist* Create();
	static CControlAssist* GetInstance() { return m_pThisPtr; }
private:

	//=============================
	// �����o�ϐ�
	//=============================
	int m_nNumDisp;	// �`�搔
	CObject2D* m_pText[CONTROLTYPE_MAX];	// �e�L�X�g�̃I�u�W�F�N�g
	static CControlAssist* m_pThisPtr;		// ���g�̃|�C���^
};
#endif