//==========================================================================
// 
//  ûKG(U) [enemy_practice_SA.cpp]
//  Author : nèÎë
// 
//==========================================================================
#include "enemy_practice_SA.h"

//==========================================================================
// RXgN^
//==========================================================================
CEnemyPractice_SA::CEnemyPractice_SA() : CEnemyPractice()
{

}

//==========================================================================
// fXgN^
//==========================================================================
CEnemyPractice_SA::~CEnemyPractice_SA()
{

}

//==========================================================================
// ú»
//==========================================================================
HRESULT CEnemyPractice_SA::Init()
{
	//ú»
	CEnemyPractice::Init();

	m_pAtkPattern.push_back(DEBUG_NEW CAttack_PracticeStrongAttack());	// ­U

	// UØèÖ¦
	ChangeATKState(m_pAtkPattern[0]);

	return S_OK;
}
