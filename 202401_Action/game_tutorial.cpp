//=============================================================================
// 
//  �Q�[������ [game_tutorial.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "manager.h"
#include "game_tutorial.h"
#include "tutorialmanager.h"
#include "renderer.h"
#include "calculation.h"
#include "debugproc.h"
#include "fade.h"
#include "camera.h"
#include "input.h"

#include "enemymanager.h"
#include "player.h"
#include "skillpoint.h"
#include "enemybase.h"

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CGameTutorial::CGameTutorial()
{
	// �l�̃N���A

}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CGameTutorial::~CGameTutorial()
{

}

//==========================================================================
// ����������
//==========================================================================
HRESULT CGameTutorial::Init()
{
	
	// ����������
	CGame::Init();
	
	// �v���C���[�擾
	CListManager<CPlayer> playerList = CPlayer::GetListObj();
	CPlayer* pPlayer = nullptr;

	// ���X�g���[�v
	while (playerList.ListLoop(&pPlayer))
	{
		pPlayer->GetSkillPoint()->SetPoint(99);
	}

	// ����
	return S_OK;
}

//==========================================================================
// ���[�h�ʏ�����
//==========================================================================
void CGameTutorial::InitByMode()
{
	//**********************************
	// �G�̋��_
	//**********************************
	m_pEnemyBase = CEnemyBase::Create("data\\TEXT\\enemydata\\base_tutorial.txt");
	if (m_pEnemyBase == nullptr)
	{
		return;
	}

	//**********************************
	// �G�}�l�[�W��
	//**********************************
	m_pEnemyManager = CEnemyManager::Create("data\\TEXT\\enemydata\\manager.txt");
	if (m_pEnemyManager == nullptr)
	{
		return;
	}
}

//==========================================================================
// �I������
//==========================================================================
void CGameTutorial::Uninit()
{
	// �I������
	CGame::Uninit();
}

//==========================================================================
// �X�V����
//==========================================================================
void CGameTutorial::Update()
{
	// �X�V
	CGame::Update();
}

//==========================================================================
// �`�揈��
//==========================================================================
void CGameTutorial::Draw()
{
	// �`��
	CGame::Draw();
}
