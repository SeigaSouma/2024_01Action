//=============================================================================
// 
// ���S�}�l�[�W������ [deadmanager.h]
// Author : ���n�Ή�
// 
//=============================================================================

#ifndef _DEAD_MANAGER_H_
#define _DEAD_MANAGER_H_		// ��d�C���N���[�h�h�~�̃}�N�����`����

#include "object.h"

class CDeadScreen;	// �X�N���[��
class CDeadText;		// ����
class CRevivalPercentText;	// ���X�|�[���m��

//==========================================================================
// �N���X��`
//==========================================================================
// ���S�}�l�[�W��
class CDeadManager : public CObject
{
public:

	CDeadManager(int nPriority = 8);
	~CDeadManager();
	
	//  �I�[�o�[���C�h���ꂽ�֐�
	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;

	// �����o�֐�
	bool IsRespawnPossible() { return m_bRespawnPossible; }	// �����o���邩�̃t���O�擾
	void SetFadeOut(void);	// �t�F�[�h�A�E�g�ݒ�

	// �ÓI�֐�
	static CDeadManager* GetInstance(void);	// �C���X�^���X�擾
	static CDeadManager* Create(void);		// ��������

private:

	//=============================
	// �����o�֐�
	//=============================
	// ���̑��֐�
	bool DrawingRevival(int winningPercent);	// �������I

	//=============================
	// �����o�ϐ�
	//=============================
	float m_fTime;				// �J�E���^�[
	bool m_bSetScreen;			// �X�N���[���̃Z�b�g�t���O
	bool m_bSetText;			// �e�L�X�g�̃Z�b�g�t���O
	bool m_bSetRevivalText;		// �����m�������̃Z�b�g�t���O
	bool m_bRespawnPossible;	// �����ł��邩�̃t���O

	// ��������I�u�W�F
	CDeadScreen* m_pScreen;	// �X�N���[���̃I�u�W�F
	CDeadText* m_pText;		// �����̃I�u�W�F�N�g
	CRevivalPercentText* m_pRespawnPercent;	// ���X�|�[���m��

	static CDeadManager* m_pThisPtr;	// ���g�̃|�C���^

};

#endif