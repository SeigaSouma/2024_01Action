//=============================================================================
// 
// �Q�[���}�l�[�W������ [gamemanager.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "gamemanager.h"
#include "game.h"
#include "debugproc.h"
#include "manager.h"
#include "renderer.h"
#include "instantfade.h"
#include "player.h"
#include "camera.h"
#include "sound.h"
#include "blackframe.h"
#include "enemybase.h"
#include "enemymanager.h"
#include "stage.h"
#include "map.h"
#include "transferBeacon.h"
#include "stagecleartext.h"
#include "skillpoint.h"
#include "skilltree_obj.h"


//==========================================================================
// �萔��`
//==========================================================================
namespace
{
	const int POINT_WAVECLEAR = 5;		// �E�F�[�u�N���A�̃|�C���g
}

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CGameManager::CGameManager()
{
	// �l�̃N���A
	m_SceneType = SCENE_MAIN;	// �V�[���̎��
	m_bEndRush = false;			// ���b�V�����I��������
	m_bControll = false;		// ����ł��邩
	m_bEndNormalStage = false;	// �ʏ�X�e�[�W���I��������
	m_nNowStage = 0;			// ���݂̃X�e�[�W
	m_nNumStage = 0;			// �X�e�[�W�̑���
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CGameManager::~CGameManager()
{

}

//==========================================================================
// ��������
//==========================================================================
CGameManager *CGameManager::Create(void)
{
	// �����p�̃I�u�W�F�N�g
	CGameManager *pManager = NULL;

	if (pManager == NULL)
	{// NULL��������

		// �������̊m��
		pManager = DEBUG_NEW CGameManager;

		if (pManager != NULL)
		{// �������̊m�ۂ��o���Ă�����

			// ����������
			HRESULT hr = pManager->Init();

			if (FAILED(hr))
			{// ���s���Ă�����
				return NULL;
			}
		}

		return pManager;
	}

	return NULL;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CGameManager::Init(void)
{
	m_bControll = true;			// ����ł��邩
	m_bEndNormalStage = false;	// �ʏ�X�e�[�W���I��������
	m_nNowStage = 0;			// ���݂̃X�e�[�W
	m_SceneType = SCENE_MAIN;	// �V�[���̎��

	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CGameManager::Uninit(void)
{
	
}

//==========================================================================
// �X�V����
//==========================================================================
void CGameManager::Update(void)
{
	if (CGame::GetEnemyBase()->GetNumStage() - 1 <= m_nNowStage)
	{// ���X�e�[�W���𒴂�����
		m_nNowStage = CGame::GetEnemyBase()->GetNumStage() - 1;

		// �ʏ�X�e�[�W���I������
		m_bEndNormalStage = true;
	}

	// ������
	switch (m_SceneType)
	{
	case CGameManager::SCENE_MAIN:
		m_bControll = true;
		break;

	case CGameManager::SCENE_MAINCLEAR:
		m_bControll = true;
		break;

	case CGameManager::SCENE_ENHANCE:
		m_bControll = true;
		SceneEnhance();
		break;

	case SCENE_SKILLTREE:		// �X�L���c���[
		m_bControll = false;
		break;

	case CGameManager::SCENE_BOSS:
		m_bControll = true;
		break;

	case CGameManager::SCENE_TRANSITIONWAIT:
		m_bControll = false;
		break;

	case CGameManager::SCENE_TRANSITION:
		m_bControll = false;
		break;

	default:
		break;
	}


	// �e�L�X�g�̕`��
	CManager::GetInstance()->GetDebugProc()->Print(
		"---------------- �Q�[���}�l�[�W����� ----------------\n"
		"�y���̃��[�h�z[%d]\n", m_SceneType);


	if (m_SceneType == SCENE_TRANSITION)
	{// �J�ڒ�

		// �J�ڂȂ��t�F�[�h�̏�Ԏ擾
		CInstantFade::STATE fadestate = CManager::GetInstance()->GetInstantFade()->GetState();

		if (fadestate == CInstantFade::STATE_FADECOMPLETION)
		{// ���������u��

			// �J�����擾
			CCamera *pCamera = CManager::GetInstance()->GetCamera();
			if (pCamera != NULL)
			{
				pCamera->SetEnableFollow(true);
			}

			if (!m_bEndNormalStage)
			{// �ʏ�X�e�[�W���I����Ă��Ȃ�������
				SetEnemy();
			}
			else
			{// �{�X�X�e�[�W
				SetBoss();
			}

		}
	}

}

//==========================================================================
// �Q�[���N���A���̐ݒ�
//==========================================================================
void CGameManager::GameClearSettings(void)
{
	// �N���A�e�L�X�g����
	CStageClearText::Create(MyLib::Vector3(640.0f, 360.0f, 0.0f));

	// �]�ڃr�[�R������
	CTransferBeacon::Create(CTransferBeacon::TRANSTYPE_ENHANCE);

	// �v���C���[�擾
	CListManager<CPlayer> playerList = CPlayer::GetListObj();
	CPlayer* pPlayer = nullptr;

	// ���X�g���[�v
	while (playerList.ListLoop(&pPlayer))
	{
		pPlayer->GetSkillPoint()->AddPoint(POINT_WAVECLEAR);
	}
}

//==========================================================================
// �����V�[��
//==========================================================================
void CGameManager::SceneEnhance(void)
{
	// �J�ڂȂ��t�F�[�h�̏�Ԏ擾
	CInstantFade::STATE fadestate = CManager::GetInstance()->GetInstantFade()->GetState();

	if (fadestate != CInstantFade::STATE_FADECOMPLETION)
	{// �������ĂȂ�
		return;
	}

	// �J�������Z�b�g
	CCamera* pCamera = CManager::GetInstance()->GetCamera();
	if (pCamera != NULL)
	{
		pCamera->Reset(CScene::MODE_GAME);
	}

	// �X�e�[�W�؂�ւ�
	CGame::GetStage()->ChangeStage("data\\TEXT\\stage\\info.txt");

	// �}�b�v�؂�ւ�
	MyMap::ChangeMap("data\\TEXT\\map\\map_enhance.txt");

	// �v���C���[�擾
	CListManager<CPlayer> playerList = CPlayer::GetListObj();
	CPlayer* pPlayer = nullptr;

	// ���X�g���[�v
	while (playerList.ListLoop(&pPlayer))
	{
		pPlayer->SetPosition(0.0f);
	}

	// �]�ڃr�[�R������
	CTransferBeacon::Create(CTransferBeacon::TRANSTYPE_GAMEMAIN);

	// �X�L���c���[�I�u�W�F�N�g����
	CSkillTree_Obj::Create();
}

//==========================================================================
// �{�X�ݒ�
//==========================================================================
void CGameManager::SetBoss(void)
{
	// BGM�X�g�b�v
	CManager::GetInstance()->GetSound()->StopSound(CSound::LABEL_BGM_GAME);

	// ��ސݒ�
	m_SceneType = SCENE_BOSS;

	// ���Z�b�g����
	CGame::ResetBeforeBoss();

	// �V�[���̃��Z�b�g
	CManager::GetInstance()->GetScene()->ResetScene();


	// �v���C���[�擾
	CListManager<CPlayer> playerList = CPlayer::GetListObj();
	CPlayer* pPlayer = nullptr;

	// ���X�g���[�v
	int i = 0;
	while (playerList.ListLoop(&pPlayer))
	{
		// �ʒu�ݒ�
		pPlayer->SetPosition(D3DXVECTOR3(-500.0f + i * 50.0f, 5000.0f, 0.0f));
		i++;
	}

	// �J�����̏��擾
	CCamera* pCamera = CManager::GetInstance()->GetCamera();
	pCamera->ResetBoss();

	// ���t���[���N��
	//CManager::GetInstance()->GetBlackFrame()->SetState(CBlackFrame::STATE_IN);

	// �G�̍Ĕz�u
	CEnemyManager* pEnemyManager = CGame::GetEnemyManager();
	if (pEnemyManager != NULL)
	{
		// �G�̍Ĕz�u
		pEnemyManager->SetStageBoss();
	}
}

//==========================================================================
// �G�ݒ�
//==========================================================================
void CGameManager::SetEnemy(void)
{

	// �X�e�[�W�؂�ւ�
	CGame::GetStage()->ChangeStage("data\\TEXT\\stage\\info.txt");

	// �}�b�v�؂�ւ�
	MyMap::ChangeMap("data\\TEXT\\map\\info.txt");

	// �v���C���[�擾
	CListManager<CPlayer> playerList = CPlayer::GetListObj();
	CPlayer* pPlayer = nullptr;

	// ���X�g���[�v
	int i = 0;
	while (playerList.ListLoop(&pPlayer))
	{
		// �ʒu�ݒ�
		pPlayer->SetPosition(MyLib::Vector3(-500.0f + i * 250.0f, 0.0f, 0.0f));
		i++;
	}

	// �J�����̏��擾
	CCamera *pCamera = CManager::GetInstance()->GetCamera();
	if (pCamera != NULL)
	{
		pCamera->Reset(CScene::MODE_GAME);
	}

	// ��ސݒ�
	m_SceneType = SCENE_MAIN;

	// �G�̍Ĕz�u
	CEnemyManager *pEnemyManager = CGame::GetEnemyManager();
	if (pEnemyManager != NULL)
	{
		// �G�̍Ĕz�u
		pEnemyManager->SetStageEnemy();

		// �ύX������Ȃ�����
		pEnemyManager->SetEnableChangeStage(false);
	}
}

//==========================================================================
// �X�e�[�W�̉��Z
//==========================================================================
void CGameManager::AddNowStage(void)
{
	// ���Z
	m_nNowStage++;

	if (CGame::GetEnemyBase()->GetNumStage() <= m_nNowStage)
	{// ���X�e�[�W���𒴂�����
		m_nNowStage = CGame::GetEnemyBase()->GetNumStage();

		// �ʏ�X�e�[�W���I������
		m_bEndNormalStage = true;
	}

}

//==========================================================================
// ���݂̃X�e�[�W�擾
//==========================================================================
int CGameManager::GetNowStage(void)
{
	return m_nNowStage;
}

//==========================================================================
// �V�[���̎�ސݒ�
//==========================================================================
void CGameManager::SetType(SceneType type)
{
	m_SceneType = type;
}

//==========================================================================
// �V�[���̎�ގ擾
//==========================================================================
CGameManager::SceneType CGameManager::GetType(void)
{
	return m_SceneType;
}
