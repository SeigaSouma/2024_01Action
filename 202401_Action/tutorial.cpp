//=============================================================================
// 
//  �Q�[������ [tutorial.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "manager.h"
#include "tutorial.h"
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
CTutorial* CTutorial::m_pThisPtr = nullptr;	// ���g�̃|�C���^

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CTutorial::CTutorial()
{
	// �l�̃N���A
	m_pLimitArea = nullptr;			// �G���A�����̃I�u�W�F�N�g
	//m_pGameManager = nullptr;		// �Q�[���}�l�[�W���̃I�u�W�F�N�g
	m_pEnemyBase = nullptr;			// �G�̋��_
	m_pEnemyManager = nullptr;		// �G�}�l�[�W���̃I�u�W�F�N�g
	m_bEdit = false;				// �G�f�B�b�g�̔���
	m_clear = false;				// �N���A����
	m_fMaxRokOnDistance = 0.0f;		// ���b�N�I���̍ő勗��
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CTutorial::~CTutorial()
{

}

//==========================================================================
// ��������
//==========================================================================
CTutorial* CTutorial::Create()
{
	if (m_pThisPtr == nullptr)
	{// �܂��������Ă��Ȃ�������

		// �C���X�^���X����
		m_pThisPtr = DEBUG_NEW CTutorial;
	}

	return m_pThisPtr;
}

//==========================================================================
// �C���X�^���X�擾
//==========================================================================
CTutorial* CTutorial::GetInstance()
{
	return m_pThisPtr;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CTutorial::Init()
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
	//m_pGameManager = CTutorialManager::Create();

	//**********************************
	// �G�̋��_
	//**********************************
	m_pEnemyBase = CEnemyBase::Create("data\\TEXT\\enemydata\\base.txt");
	if (m_pEnemyBase == nullptr)
	{// nullptr��������
		return E_FAIL;
	}

	//**********************************
	// �G�}�l�[�W��
	//**********************************
	m_pEnemyManager = CEnemyManager::Create("data\\TEXT\\enemydata\\manager.txt");
	if (m_pEnemyManager == nullptr)
	{// nullptr��������
		return E_FAIL;
	}

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

	// �X�R�A�̐�������
	//m_pScore = CScore::Create(MyLib::Vector3(1000.0f, 50.0f, 0.0f));

	CManager::GetInstance()->GetCamera()->Reset(CScene::MODE_GAME);

	// BGM�Đ�
	//CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL_BGM_GAME);

	/*CLimitArea::sLimitEreaInfo info;
	info.fMaxX = 13000.0f;
	info.fMaxZ = 1200.0f;
	info.fMinX = -1200.0f;
	info.fMinZ = -1200.0f;
	m_pLimitArea = CLimitArea::Create(info);
	m_pLimitArea->SetEnableDisp(false);*/

	// �X�L���c���[����
	CSkillTree::Create();
	//m_pGameManager->SetPrevEnhance();

	// �N���A�̔���
	SetEnableClear(true);


	// ����
	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CTutorial::Uninit()
{
	m_pThisPtr = nullptr;

	

	//if (m_pGameManager != nullptr)
	//{
	//	// �I������
	//	m_pGameManager->Uninit();
	//	delete m_pGameManager;
	//	m_pGameManager = nullptr;
	//}

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
void CTutorial::Update()
{

	//// �Q�[���}�l�[�W��
	//if (m_pGameManager != nullptr)
	//{
	//	// �X�V����
	//	m_pGameManager->Update();
	//}

	CManager::GetInstance()->GetDebugProc()->Print(
		"���݂̃��[�h�F�y�Q�[���z\n"
		"�؂�ւ��F�y F �z\n\n");

	// �L�[�{�[�h���擾
	CInputKeyboard *pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();

	// �Q�[���p�b�h���擾
	CInputGamepad *pInputGamepad = CManager::GetInstance()->GetInputGamepad();


	if (GetEnemyManager() != nullptr)
	{// �G�}�l�[�W���̍X�V����
		GetEnemyManager()->Update();
	}

	// �G�̋��_
	if (m_pEnemyBase != nullptr)
	{
		m_pEnemyBase->Update();
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
void CTutorial::Draw()
{

}
//
////==========================================================================
//// �Q�[���}�l�[�W���̎擾
////==========================================================================
//CTutorialManager *CTutorial::GetGameManager()
//{
//	return m_pGameManager;
//}

//==========================================================================
// �G�}�l�[�W���̎擾
//==========================================================================
CEnemyManager *CTutorial::GetEnemyManager()
{
	return m_pEnemyManager;
}

//==========================================================================
// �G�̋��_
//==========================================================================
CEnemyBase *CTutorial::GetEnemyBase()
{
	return m_pEnemyBase;
}

//==========================================================================
// ���Z�b�g����
//==========================================================================
void CTutorial::ResetBeforeBoss()
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
void CTutorial::EditReset()
{
	
	

}
