//=============================================================================
// 
// ���S�}�l�[�W������ [deadmanager.h]
// Author : ���n�Ή�
// 
//=============================================================================

#ifndef _DEAD_MANAGER_H_
#define _DEAD_MANAGER_H_		// ��d�C���N���[�h�h�~�̃}�N�����`����

#include "object.h"

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

	static CDeadManager* GetInstance(void);	// �C���X�^���X�擾
	static CDeadManager* Create(void);		// ��������

private:

	//=============================
	// �����o�֐�
	//=============================

	//=============================
	// �����o�ϐ�
	//=============================
	float m_fTime;		// �J�E���^�[
	bool m_bSetScreen;	// �X�N���[���̃Z�b�g�t���O
	bool m_bSetText;	// �e�L�X�g�̃Z�b�g�t���O
	bool m_bSetRevivalText;	// �����m�������̃Z�b�g�t���O
	static CDeadManager* m_pThisPtr;	// ���g�̃|�C���^

};

#endif