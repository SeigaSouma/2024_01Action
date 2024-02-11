//=============================================================================
// 
// ���S��ʏ��� [deadscreen.h]
// Author : ���n�Ή�
// 
//=============================================================================

#ifndef _DEAD_SCREEN_H_
#define _DEAD_SCREEN_H_		// ��d�C���N���[�h�h�~�̃}�N�����`����

#include "object2D.h"

//==========================================================================
// �N���X��`
//==========================================================================
// ���S���
class CDeadScreen : public CObject2D
{
public:

	//=============================
	// �����o�֐�
	//=============================
	enum eState
	{
		STATE_NONE = 0, // �Ȃɂ��Ȃ�
		STATE_FADEIN,	// �t�F�[�h�C��
		STATE_FADEOUT,	// �t�F�[�h�A�E�g
		STATE_MAX
	};

	CDeadScreen(float fadetime, int nPriority = 8);
	~CDeadScreen();
	
	//  �I�[�o�[���C�h���ꂽ�֐�
	HRESULT Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;

	void SetState(eState state) { m_state = state; }	// ��Ԑݒ�
	static CDeadScreen* Create(float fadetime);	// ��������

private:

	//=============================
	// �֐��|�C���^
	//=============================
	typedef void(CDeadScreen::* STATE_FUNC)();
	static STATE_FUNC m_StateFunc[];

	//=============================
	// �����o�֐�
	//=============================
	void StateNone();		// �Ȃɂ��Ȃ�
	void StateFadeIn();		// �t�F�[�h�C��
	void StateFadeOut();	// �t�F�[�h�A�E�g

	//=============================
	// �����o�ϐ�
	//=============================
	eState m_state;		// ���
	float m_fStateTime;	// ��ԃJ�E���^�[
	const float m_fFadeOutTime;	// �t�F�[�h�ɂ����鎞��


};

#endif