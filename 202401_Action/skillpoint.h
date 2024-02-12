//=============================================================================
// 
//  �X�L���|�C���g�w�b�_�[ [skillpoint.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _SKILLPOINT_H_
#define _SKILLPOINT_H_	// ��d�C���N���[�h�h�~

class CMultiNumber;

#include "object2D.h"

//==========================================================================
// �N���X��`
//==========================================================================
// �X�L���|�C���g�N���X��`
class CSkillPoint : public CObject2D
{
public:

	enum STATE
	{
		STATE_ARRIVE = 0,		// ����
		STATE_SLIDEIN,			// �X���C�h�C��
		STATE_SLIDECOMPLETION,	// �X���C�h����
		STATE_SLIDEOUT,			// �X���C�h�A�E�g
		STATE_WAIT,				// �ҋ@
		STATE_ENHANCE,			// ����
		STATE_MAX
	};

	CSkillPoint(int nPriority = 9);
	~CSkillPoint();

	HRESULT Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;

	void Kill();

	// �|�C���g�֘A
	void SetPoint(int nValue);					// �|�C���g�ݒ�
	int GetPoint() const { return m_nPoint; }	// �|�C���g�擾
	void AddPoint();							// �|�C���g�ǉ�
	void AddPoint(int nValue);					// �|�C���g�ǉ�
	void SubPoint();							// �|�C���g�폜
	void SubPoint(int nValue);					// �|�C���g�폜

	// ���̑�
	void SetSlideIn();	// �X���C�h�C���ݒ�
	void SetState(STATE state);	// ��Ԑݒ�

	static CSkillPoint* Create();	// ��������

private:

	//=============================
	// �֐����X�g
	//=============================
	typedef void(CSkillPoint::* STATE_FUNC)();
	static STATE_FUNC m_StateFunc[];	// ��Ԋ֐�

	//=============================
	// �����o�֐�
	//=============================
	// ��Ԋ֐�
	void StateArrive();				// ����
	void StateSlideIn();			// �X���C�h�C��
	void StateSlideCompletion();	// �X���C�h����
	void StateSlideOut();			// �X���C�h�A�E�g
	void StateWait();				// �ҋ@
	void StateEnhance();			// ����

	//=============================
	// �����o�ϐ�
	//=============================
	int m_nPoint;				// �|�C���g
	int m_nVisualValue;			// �����ڏ�̐��l
	float m_fStateTime;			// ��ԃ^�C�}�[
	float m_fSlideTime;			// �X���C�h�^�C�}�[
	STATE m_State;				// ���
	CMultiNumber* m_apNumber;	// �����̃I�u�W�F�N�g
};



#endif