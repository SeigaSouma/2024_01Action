//=============================================================================
// 
//  ���U���g�}�l�[�W���w�b�_�[ [resultmanager.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _RESULTMANAGER_H_
#define _RESULTMANAGER_H_	// ��d�C���N���[�h�h�~

#include "main.h"
#include "constans.h"

// �O���錾
class CEnemy;
class CBoss;

//==========================================================================
// �N���X��`
//==========================================================================
// ���U���g�}�l�[�W���N���X��`
class CResultManager
{
public:

	CResultManager();
	~CResultManager();

	HRESULT Init();
	void Uninit();
	void Update();
	void Reset();	// ���Z�b�g

	void AddScore(int nValue);			// �X�R�A���Z
	int GetBaseScore();			// ���̃X�R�A�̎擾����
	int GetToatalScore();			// �ŏI�X�R�A�̎擾����
	static CResultManager *Create();	// ��������
	
private:
	int m_nBaseScore;	// ���̃X�R�A
	int m_nToatalScore;	// �ŏI�X�R�A
};



#endif