//==========================================================================
// 
//  ���K�G(�U��)���� [enemy_practice_SA.cpp]
//  Author : ���n�Ή�
// 
//==========================================================================
#include "enemy_practice_SA.h"

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CEnemyPractice_SA::CEnemyPractice_SA(int nPriority) : CEnemyPractice(nPriority)
{

}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CEnemyPractice_SA::~CEnemyPractice_SA()
{

}

//==========================================================================
// ����������
//==========================================================================
HRESULT CEnemyPractice_SA::Init()
{
	//����������
	CEnemyPractice::Init();

	m_pAtkPattern.push_back(DEBUG_NEW CAttack_PracticeStrongAttack());	// ���U��

	// �U���؂�ւ�
	ChangeATKState(m_pAtkPattern[0]);

	return S_OK;
}