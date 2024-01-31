//=============================================================================
// 
// ���S�e�L�X�g���� [deadtext.h]
// Author : ���n�Ή�
// 
//=============================================================================

#ifndef _DEAD_TEXT_H_
#define _DEAD_TEXT_H_		// ��d�C���N���[�h�h�~�̃}�N�����`����

#include "object2D.h"

//==========================================================================
// �N���X��`
//==========================================================================
// ���S���
class CDeadText : public CObject2D
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

	CDeadText(float fadein, float fadeout, int nPriority = 8);
	~CDeadText();
	
	//  �I�[�o�[���C�h���ꂽ�֐�
	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;

	void SetState(eState state) { m_state = state; }	// ��Ԑݒ�
	static CDeadText* Create(const float fadein, const float fadeout);		// ��������

private:

	//=============================
	// �֐��|�C���^
	//=============================
	typedef void(CDeadText::* STATE_FUNC)(void);
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
	const float m_fFadeInTime;	// �t�F�[�h�ɂ����鎞��
	const float m_fFadeOutTime;	// �t�F�[�h�ɂ����鎞��


};

#endif