//=============================================================================
// 
// �����m���e�L�X�g���� [revivalpercent_text.h]
// Author : ���n�Ή�
// 
//=============================================================================

#ifndef _REVIVALPERCENT_TEXT_H_
#define _REVIVALPERCENT_TEXT_H_		// ��d�C���N���[�h�h�~�̃}�N�����`����

class CMultiNumber;

#include "object2D.h"

//==========================================================================
// �N���X��`
//==========================================================================
// �����m���e�L�X�g���
class CRevivalPercentText : public CObject2D
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

	CRevivalPercentText(float fadetime, int nPriority = 8);
	~CRevivalPercentText();
	
	//  �I�[�o�[���C�h���ꂽ�֐�
	HRESULT Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;

	void Kill();
	void SetState(eState state) { m_state = state; }	// ��Ԑݒ�
	static CRevivalPercentText* Create(int respawnpercent, const float fadetime);	// ��������

private:

	//=============================
	// �֐��|�C���^
	//=============================
	typedef void(CRevivalPercentText::* STATE_FUNC)();
	static STATE_FUNC m_StateFunc[];

	//=============================
	// �����o�֐�
	//=============================
	// ��Ԍn
	void StateNone();		// �Ȃɂ��Ȃ�
	void StateFadeIn();		// �t�F�[�h�C��
	void StateFadeOut();	// �t�F�[�h�A�E�g

	// ���̑��֐�
	void SetNumberObj(int number);

	//=============================
	// �����o�ϐ�
	//=============================
	eState m_state;		// ���
	float m_fStateTime;	// ��ԃJ�E���^�[
	CMultiNumber* m_apNumber;	// �����̃I�u�W�F�N�g
	const float m_fFadeTime;	// �t�F�[�h�ɂ����鎞��


};

#endif