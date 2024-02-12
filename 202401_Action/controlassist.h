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
		CONTROLTYPE_ROCKON = 0,	// ���b�N�I��
		CONTROLTYPE_COUNTER,	// ����
		CONTROLTYPE_MAX
	};

	CControlAssist(int nPriority = 8);
	~CControlAssist();

	//  �I�[�o�[���C�h�֐�
	HRESULT Init() override;
	void Uninit() override;
	void Update() override {}
	void Draw() override {}

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