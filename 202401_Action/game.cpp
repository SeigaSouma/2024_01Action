//=============================================================================
// 
//  �Q�[������ [game.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "manager.h"
#include "game.h"
#include "game_tutorial.h"
#include "renderer.h"
#include "calculation.h"
#include "debugproc.h"
#include "fade.h"
#include "camera.h"
#include "pause.h"
#include "texture.h"

#include "input.h"
#include "player.h"
#include "enemy.h"
#include "score.h"
#include "timer.h"
#include "map.h"
#include "elevation.h"
#include "sound.h"
#include "edit_enemybase.h"
#include "stage.h"
#include "skilltree.h"

#include "enemymanager.h"
#include "player.h"
#include "enemybase.h"
#include "limitarea.h"
#include "particle.h"
#include "myeffekseer.h"
#include "gallery.h"
#include "torch.h"
#include "controlassist.h"
#include "gamerating.h"

//==========================================================================
// �ÓI�����o�ϐ��錾
//==========================================================================
CGame* CGame::m_pThisPtr = nullptr;	// ���g�̃|�C���^

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CGame::CGame()
{
	// �l�̃N���A
	m_pScore = nullptr;				// �X�R�A�̃I�u�W�F�N�g
	m_pTimer = nullptr;				// �^�C�}�[�̃I�u�W�F�N�g
	m_pLimitArea = nullptr;			// �G���A�����̃I�u�W�F�N�g
	m_pEditEnemyBase = nullptr;		// �G�̋��_�G�f�B�^�[
	m_pStage = nullptr;				// �X�e�[�W�̃I�u�W�F�N�g
	m_pGameManager = nullptr;		// �Q�[���}�l�[�W���̃I�u�W�F�N�g
	m_EditType = EDITTYPE_OFF;		// �G�f�B�b�g�̎��
	m_pEnemyBase = nullptr;			// �G�̋��_
	m_pEnemyManager = nullptr;		// �G�}�l�[�W���̃I�u�W�F�N�g
	m_bEdit = false;				// �G�f�B�b�g�̔���
	m_clear = false;				// �N���A����
	m_fMaxRokOnDistance = 0.0f;		// ���b�N�I���̍ő勗��
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CGame::~CGame()
{

}

//==========================================================================
// ��������
//==========================================================================
CGame* CGame::Create(CScene::MODE mode)
{
	if (m_pThisPtr == nullptr)
	{// �܂��������Ă��Ȃ�������

		// �C���X�^���X����
		switch (mode)
		{
		case CScene::MODE_GAME:
			m_pThisPtr = DEBUG_NEW CGame;
			break;

		case CScene::MODE::MODE_GAMETUTORIAL:
			m_pThisPtr = DEBUG_NEW CGameTutorial;
			break;
		}
	}

	return m_pThisPtr;
}

//==========================================================================
// �C���X�^���X�擾
//==========================================================================
CGame* CGame::GetInstance()
{
	return m_pThisPtr;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CGame::Init()
{
	// �G�f�B�b�g����OFF
	m_bEdit = false;

	// ���b�N�I���̍ő勗��
	m_fMaxRokOnDistance = mylib_const::MAX_ROCKONDISTANCE_GAME;

	// �v���C���[�̐��ݒ�
	CManager::GetInstance()->SetNumPlayer(1);

	// ����������
	if (FAILED(CScene::Init()))
	{// ���s�����ꍇ
		return E_FAIL;
	}

	//**********************************
	// �Q�[���}�l�[�W��
	//**********************************
	m_pGameManager = CGameManager::Create(GetMode());

	// ���[�h�ʏ���������
	InitByMode();


	//**********************************
	// �v���C���[
	//**********************************
	// �L��������
	for (int nCntPlayer = 0; nCntPlayer < 1; nCntPlayer++)
	{
		if (CPlayer::Create(nCntPlayer) == nullptr)
		{
			return E_FAIL;
		}
	}
	// �v���C���[�擾
	CListManager<CPlayer> playerList = CPlayer::GetListObj();
	CPlayer* pPlayer = nullptr;

	// ���X�g���[�v
	while (playerList.ListLoop(&pPlayer))
	{
		pPlayer->SetPosition(MyLib::Vector3(0.0f, 10.0f, 0.0f));
		pPlayer->SetRotation(MyLib::Vector3(0.0f, 0.0f, 0.0f));
	}

	// �X�e�[�W
	m_pStage = CStage::Create("data\\TEXT\\stage\\info.txt");

	CManager::GetInstance()->GetCamera()->Reset(CScene::MODE_GAME);

	// �X�L���c���[����
	CSkillTree::Create();
	m_pGameManager->SetPrevEnhance();

	// �N���A�̔���
	SetEnableClear(true);

	// ����
	return S_OK;
}

//==========================================================================
// ���[�h�ʏ�����
//==========================================================================
void CGame::InitByMode()
{

	//**********************************
	// �G�̋��_
	//**********************************
	m_pEnemyBase = CEnemyBase::Create("data\\TEXT\\enemydata\\base.txt");
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

	// �]����ǂݍ���
	CGameRating::ReadText();
}

//==========================================================================
// �I������
//==========================================================================
void CGame::Uninit()
{
	m_pThisPtr = nullptr;

	// �X�R�A�̔j��
	if (m_pScore != nullptr)
	{// �������̊m�ۂ��o���Ă�����

		// �I������
		m_pScore->Uninit();

		// �������̊J��
		delete m_pScore;
		m_pScore = nullptr;
	}

	// �^�C�}�[�̔j��
	if (m_pTimer != nullptr)
	{
		// �I������
		m_pTimer->Uninit();
		delete m_pTimer;
		m_pTimer = nullptr;
	}

	if (m_pEditEnemyBase != nullptr)
	{
		// �I��������
		m_pEditEnemyBase->Uninit();
		delete m_pEditEnemyBase;
		m_pEditEnemyBase = nullptr;
	}

	// �X�e�[�W�̔j��
	if (m_pStage != nullptr)
	{// �������̊m�ۂ��o���Ă�����

		// �I������
		m_pStage->Uninit();
		delete m_pStage;
		m_pStage = nullptr;
	}

	if (m_pGameManager != nullptr)
	{
		// �I������
		m_pGameManager->Uninit();
		delete m_pGameManager;
		m_pGameManager = nullptr;
	}

	// �G�}�l�[�W��
	if (m_pEnemyManager != nullptr)
	{
		m_pEnemyManager->Uninit();
		delete m_pEnemyManager;
		m_pEnemyManager = nullptr;
	}

	// �G�̋��_
	if (m_pEnemyBase != nullptr)
	{
		m_pEnemyBase->Uninit();
		delete m_pEnemyBase;
		m_pEnemyBase = nullptr;
	}

	// �I������
	CScene::Uninit();
}

//==========================================================================
// �X�V����
//==========================================================================
void CGame::Update()
{

	// �Q�[���}�l�[�W��
	if (m_pGameManager != nullptr)
	{
		// �X�V����
		m_pGameManager->Update();
	}

	CManager::GetInstance()->GetDebugProc()->Print(
		"���݂̃��[�h�F�y�Q�[���z\n"
		"�؂�ւ��F�y F �z\n\n");

	// �L�[�{�[�h���擾
	CInputKeyboard *pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();

	// �Q�[���p�b�h���擾
	CInputGamepad *pInputGamepad = CManager::GetInstance()->GetInputGamepad();

	if (m_pScore != nullptr &&
		CManager::GetInstance()->GetEdit() == nullptr &&
		m_pEnemyManager != nullptr)
	{
		// �X�R�A�̍X�V����
		m_pScore->Update();
	}

#if _DEBUG
	if (pInputKeyboard->GetTrigger(DIK_F4))
	{// F4�ŃG�f�B�b�g�؂�ւ�

		// �؂�ւ�
		m_EditType = (EEditType)(((int)m_EditType + 1) % (int)EDITTYPE_MAX);	// �Ǐ]�̎��

		// ���Z�b�g
		EditReset();

		// �G�f�B�b�g����ON
		m_bEdit = true;

		switch (m_EditType)
		{
		case CGame::EDITTYPE_OFF:	// �S���I�t
			// �G�f�B�b�g����OFF
			m_bEdit = false;
			break;

		case EDITTYPE_ENEMYBASE:
			if (m_pEditEnemyBase == nullptr)
			{// nullptr��������

				// �G�f�B�b�g�̐�������
				m_pEditEnemyBase = CEditEnemyBase::Create();
			}
			break;

		}
	}
#endif

	if (GetEnemyManager() != nullptr)
	{// �G�}�l�[�W���̍X�V����
		GetEnemyManager()->Update();
	}

	if (m_pEditEnemyBase != nullptr)
	{// �G�̋��_�G�f�B�^�[�̍X�V����
		m_pEditEnemyBase->Update();
	}

	// �G�̋��_
	if (m_pEnemyBase != nullptr)
	{
		m_pEnemyBase->Update();
	}

	// �X�e�[�W�̍X�V
	if (m_pStage != nullptr)
	{
		m_pStage->Update();
	}


	// �^�C�}�[�X�V
	if (m_pTimer != nullptr &&
		!CManager::GetInstance()->GetPause()->IsPause())
	{
		m_pTimer->Update();
	}

#if _DEBUG

	if (pInputKeyboard->GetTrigger(DIK_F))
	{
		// ���[�h�ݒ�
		CManager::GetInstance()->GetFade()->SetFade(CScene::MODE_RESULT);
	}

#endif

	// �V�[���̍X�V
	CScene::Update();

}

//==========================================================================
// �`�揈��
//==========================================================================
void CGame::Draw()
{

}

//==========================================================================
// �X�R�A�̎擾
//==========================================================================
CScore *CGame::GetScore()
{
	return m_pScore;
}

//==========================================================================
// �X�e�[�W�̎擾
//==========================================================================
CStage *CGame::GetStage()
{
	return m_pStage;
}

//==========================================================================
// �Q�[���}�l�[�W���̎擾
//==========================================================================
CGameManager *CGame::GetGameManager()
{
	return m_pGameManager;
}

//==========================================================================
// �G�}�l�[�W���̎擾
//==========================================================================
CEnemyManager *CGame::GetEnemyManager()
{
	return m_pEnemyManager;
}

//==========================================================================
// �G�̋��_
//==========================================================================
CEnemyBase *CGame::GetEnemyBase()
{
	return m_pEnemyBase;
}

//==========================================================================
// ���Z�b�g����
//==========================================================================
void CGame::ResetBeforeBoss()
{
#if 0
	// �X�e�[�W�̔j��
	if (m_pStage != nullptr)
	{// �������̊m�ۂ��o���Ă�����

		// �I������
		m_pStage->Release();
		delete m_pStage;
		m_pStage = nullptr;
	}

	// �G�̋��_
	if (m_pEnemyBase != nullptr)
	{
		m_pEnemyBase->Uninit();
		delete m_pEnemyBase;
		m_pEnemyBase = nullptr;
	}

	// �G���A����
	if (m_pLimitArea != nullptr)
	{
		m_pLimitArea->Uninit();
		m_pLimitArea = nullptr;
	}

	// �X�e�[�W
	m_pStage = CStage::Create("data\\TEXT\\stage\\boss_info.txt");

	//**********************************
	// �G�̋��_
	//**********************************
	m_pEnemyBase = CEnemyBase::Create("data\\TEXT\\enemydata\\base_boss.txt");
	if (m_pEnemyBase == nullptr)
	{// nullptr��������
		return;
	}
#endif
	// ���b�N�I���̍ő勗��
	m_fMaxRokOnDistance = mylib_const::MAX_ROCKONDISTANCE_BOSS;
}

//==========================================================================
// �G�f�B�^�[���Z�b�g����
//==========================================================================
void CGame::EditReset()
{
	
	if (m_pEditEnemyBase != nullptr)
	{
		// �I��������
		m_pEditEnemyBase->Release();
		m_pEditEnemyBase->Uninit();
		delete m_pEditEnemyBase;
		m_pEditEnemyBase = nullptr;
	}

}
