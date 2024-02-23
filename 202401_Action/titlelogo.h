//=============================================================================
// 
// �^�C�g�����S���� [titlelogo.h]
// Author : ���n�Ή�
// 
//=============================================================================

#ifndef _TITLELOGO_H_
#define _TITLELOGO_H_		// ��d�C���N���[�h�h�~�̃}�N�����`����

#include "object2D.h"


//==========================================================================
// �N���X��`
//==========================================================================
// �^�C�g�����S
class CTitleLogo : public CObject2D
{
public:

	//=============================
	// �񋓌^��`
	//=============================
	enum eState
	{
		STATE_NONE = 0, // �Ȃɂ��Ȃ�
		STATE_FADEIN,	// �t�F�[�h�C��
		STATE_MAX
	};

	CTitleLogo(float fadetime, int nPriority = 8);
	~CTitleLogo();
	
	//  �I�[�o�[���C�h���ꂽ�֐�
	HRESULT Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;

	void SetState(eState state) { m_state = state; }	// ��Ԑݒ�
	static CTitleLogo* Create(float fadetime);	// ��������

private:

	//=============================
	// �֐��|�C���^
	//=============================
	typedef void(CTitleLogo::* STATE_FUNC)();
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

};

#endif