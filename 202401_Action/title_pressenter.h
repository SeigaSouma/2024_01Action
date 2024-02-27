//=============================================================================
// 
// �^�C�g���G���^�[ [title_pressenter.h]
// Author : ���n�Ή�
// 
//=============================================================================

#ifndef _TITLE_PRESSENTER_H_
#define _TITLE_PRESSENTER_H_		// ��d�C���N���[�h�h�~�̃}�N�����`����

#include "object2D.h"
class CTutorialCheckShould;

//==========================================================================
// �N���X��`
//==========================================================================
// �^�C�g���G���^�[
class CTitle_PressEnter : public CObject2D
{
public:

	CTitle_PressEnter(float fadetime, int nPriority = 8);
	~CTitle_PressEnter() {}
	
	//  �I�[�o�[���C�h���ꂽ�֐�
	HRESULT Init() override;
	void Update() override;

	static CTitle_PressEnter* Create(float fadetime);	// ��������

private:

	//=============================
	// �񋓌^��`
	//=============================
	enum eState
	{
		STATE_NONE = 0, // �Ȃɂ��Ȃ�
		STATE_FADEIN,	// �t�F�[�h�C��
		STATE_MAX
	};

	//=============================
	// �֐��|�C���^
	//=============================
	typedef void(CTitle_PressEnter::* STATE_FUNC)();
	static STATE_FUNC m_StateFunc[];

	//=============================
	// �����o�֐�
	//=============================
	void StateNone();		// �Ȃɂ��Ȃ�
	void StateFadeIn();		// �t�F�[�h�C��

	//=============================
	// �����o�ϐ�
	//=============================
	eState m_state;		// ���
	float m_fStateTime;	// ��ԃJ�E���^�[
	const float m_fFadeOutTime;	// �t�F�[�h�ɂ����鎞��
	CTutorialCheckShould* m_pTutorialCheck;	// �`���[�g���A����邩�m�F
};

#endif