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

	CDeadScreen(int nPriority = 8);
	~CDeadScreen();
	
	//  �I�[�o�[���C�h���ꂽ�֐�
	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;

	static CDeadScreen* Create(void);	// ��������

private:

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

	//=============================
	// �֐��|�C���^
	//=============================
	typedef void(CDeadScreen::* STATE_FUNC)(void);
	static STATE_FUNC m_StateFunc[];

	//=============================
	// �����o�֐�
	//=============================
	void StateNone(void);		// �Ȃɂ��Ȃ�
	void StateFadeIn(void);		// �t�F�[�h�C��
	void StateFadeOut(void);	// �t�F�[�h�A�E�g

	//=============================
	// �����o�ϐ�
	//=============================
	eState m_state;		// ���
	float m_fStateTime;	// ��ԃJ�E���^�[


};

#endif