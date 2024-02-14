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
#include "enemy.h"
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
#include "skilltree.h"
#include "skilltree_ability.h"
#include "skilltree_behavior.h"
#include "gallery.h"
#include "torch.h"
#include "limitarea.h"
#include "calculation.h"
#include "fog.h"
#include "controlassist.h"


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
	m_nPrevPoint = 0;			// �O��̃|�C���g
	m_pSkilltreeAbillity = nullptr;	// �X�L���c���[�\�͂̃|�C���^
	m_PrevSkillIconMastering.clear();	// �O��̃X�L���A�C�R���̏K����
	m_p_PrevSkillIcon.clear();			// �O��̃X�L���A�C�R��
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
CGameManager* CGameManager::Create()
{
	// �������m��
	CGameManager* pManager = DEBUG_NEW CGameManager;

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

//==========================================================================
// ����������
//==========================================================================
HRESULT CGameManager::Init()
{
	m_bControll = true;			// ����ł��邩
	m_bEndNormalStage = false;	// �ʏ�X�e�[�W���I��������

#if _DEBUG
	m_nNowStage = 0;			// ���݂̃X�e�[�W
#else
	m_nNowStage = 0;			// ���݂̃X�e�[�W
#endif

	m_SceneType = SCENE_MAIN;	// �V�[���̎��

	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CGameManager::Uninit()
{
	if (m_pSkilltreeAbillity != nullptr)
	{
		m_pSkilltreeAbillity->Uninit();
		m_pSkilltreeAbillity = nullptr;
	}
}

//==========================================================================
// �X�V����
//==========================================================================
void CGameManager::Update()
{
	//if (CGame::GetInstance()->GetEnemyBase()->GetNumStage() - 1 <= m_nNowStage)
	//{// ���X�e�[�W���𒴂�����
	//	m_nNowStage = CGame::GetInstance()->GetEnemyBase()->GetNumStage() - 1;

	//	// �ʏ�X�e�[�W���I������
	//	m_bEndNormalStage = true;
	//}

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
		SceneTransition();
		break;

	case SCENE_REASPAWN:			// ����
		m_bControll = false;
		SceneReaspawn();
		break;

	default:
		break;
	}


	// �e�L�X�g�̕`��
	CManager::GetInstance()->GetDebugProc()->Print(
		"---------------- �Q�[���}�l�[�W����� ----------------\n"
		"�y���[�h�z[%d]\n"
		"�y�X�e�[�W�z[%d]\n", m_SceneType, m_nNowStage);
}

//==========================================================================
// �Q�[���N���A���̐ݒ�
//==========================================================================
void CGameManager::GameClearSettings()
{
	// �N���A�e�L�X�g����
	CStageClearText::Create(MyLib::Vector3(640.0f, 360.0f, 0.0f));

	// �]�ڃr�[�R������
	CTransferBeacon::Create(CTransferBeacon::TRANSTYPE_ENHANCE, MyLib::Vector3(0.0f, 200.0f, 0.0f));

	// �v���C���[�擾
	CListManager<CPlayer> playerList = CPlayer::GetListObj();
	CPlayer* pPlayer = playerList.GetData(0);

	// �N���A�|�C���g�ǉ�
	CSkillPoint* pSkillPoint = pPlayer->GetSkillPoint();
	pSkillPoint->AddPoint(POINT_WAVECLEAR);
	pSkillPoint->SetSlideIn();

	// �O��̃|�C���g�ۑ�
	m_nPrevPoint = pSkillPoint->GetPoint();

	// �O��̏K���󋵕ۑ�
	m_PrevSkillIconMastering = CSkillTree::GetInstance()->GetMastering();

	// �ϏO�̃��X�g�擾
	CListManager<CGallery> galleryList = CGallery::GetList();
	CGallery* pGallery = nullptr;
	while (galleryList.ListLoop(&pGallery))
	{
		pGallery->SetState(CGallery::STATE_CLEARHEAT);
	}

}

//==========================================================================
// ���C���J�ڒ�
//==========================================================================
void CGameManager::SceneTransition()
{
	// �J�ڂȂ��t�F�[�h�̏�Ԏ擾
	CInstantFade::STATE fadestate = CManager::GetInstance()->GetInstantFade()->GetState();

	if (fadestate == CInstantFade::STATE_FADECOMPLETION)
	{// ���������u��

		// �J�����擾
		CCamera* pCamera = CManager::GetInstance()->GetCamera();
		if (pCamera != NULL)
		{
			pCamera->SetEnableFollow(true);
		}

		// �ϏO�ݒ�
		CGallery::SetGallery();

		// �����ݒ�
		CTorch::SetTorch();

		// �G���A�������擾
		CListManager<CLimitArea> limitareaList = CLimitArea::GetListObj();
		CLimitArea* pLimitArea = nullptr;
		while (limitareaList.ListLoop(&pLimitArea))
		{
			pLimitArea->Kill();
		}

		// �G�t�F�N�g�S�Ē�~
		CMyEffekseer::GetInstance()->StopAll();
		CMyEffekseer::GetInstance()->StopAll();

		// �t�H�O���Z�b�g
		MyFog::ToggleFogFrag(false);

		// �o
		CMyEffekseer::GetInstance()->SetEffect(
			CMyEffekseer::EFKLABEL_BGFIRE,
			MyLib::Vector3(0.0f, 0.0f, 0.0f),
			0.0f, 0.0f, 100.0f, false);

		// �v���C���[�擾
		CListManager<CPlayer> playerList = CPlayer::GetListObj();
		CPlayer* pPlayer = playerList.GetData(0);

		// �N���A�|�C���g�ǉ�
		CSkillPoint* pSkillPoint = pPlayer->GetSkillPoint();
		pSkillPoint->SetState(CSkillPoint::STATE_WAIT);


		if (!m_bEndNormalStage)
		{// �ʏ�X�e�[�W���I����Ă��Ȃ�������
			SetEnemy();

			// ����⏕����
			CControlAssist* pAssist = CControlAssist::Create();
			pAssist->ResetText();
			pAssist->SetText(CControlAssist::CONTROLTYPE_ROCKON);
			pAssist->SetText(CControlAssist::CONTROLTYPE_COUNTER);
		}
		else
		{// �{�X�X�e�[�W
			SetBoss();
		}

	}
}

//==========================================================================
// �����V�[��
//==========================================================================
void CGameManager::SceneEnhance()
{
	// �J�ڂȂ��t�F�[�h�̏�Ԏ擾
	CInstantFade::STATE fadestate = CManager::GetInstance()->GetInstantFade()->GetState();

	if (fadestate != CInstantFade::STATE_FADECOMPLETION)
	{// �������ĂȂ�
		return;
	}

	// �ϏO�폜
	CListManager<CGallery> galleryList = CGallery::GetList();
	CGallery* pGallery = nullptr;
	while (galleryList.ListLoop(&pGallery))
	{
		pGallery->Kill();
	}

	// �����폜
	CListManager<CTorch> torchList = CTorch::GetList();
	CTorch* pTorch = nullptr;
	while (torchList.ListLoop(&pTorch))
	{
		pTorch->Kill();
	}

	// �J�������Z�b�g
	CCamera* pCamera = CManager::GetInstance()->GetCamera();
	if (pCamera != NULL)
	{
		pCamera->Reset(CScene::MODE_GAME);
		pCamera->SetStateCameraV(DEBUG_NEW CStateCameraV_Enhance());
	}

	// �G�t�F�N�g�S�Ē�~
	CMyEffekseer::GetInstance()->StopAll();

	// ����⏕�폜
	CControlAssist* pAssist = CControlAssist::GetInstance();
	if (pAssist != nullptr)
	{
		pAssist->ResetText();
	}


	// �X�e�[�W���Z
	AddNowStage();

	// �O��̃|�C���g�ۑ�
	m_nPrevPoint = CPlayer::GetListObj().GetData(0)->GetSkillPoint()->GetPoint();

	// �X�e�[�W�؂�ւ�
	CGame::GetInstance()->GetStage()->ChangeStage("data\\TEXT\\stage\\info.txt");

	// �}�b�v�؂�ւ�
	MyMap::ChangeMap("data\\TEXT\\map\\map_enhance.txt");

	// �v���C���[�擾
	CListManager<CPlayer> playerList = CPlayer::GetListObj();
	CPlayer* pPlayer = nullptr;

	// ���X�g���[�v
	while (playerList.ListLoop(&pPlayer))
	{
		pPlayer->SetPosition(0.0f);
		pPlayer->GetSkillPoint()->SetState(CSkillPoint::STATE_ENHANCE);
	}

	// �G�t�F�N�g�S�Ē�~
	CMyEffekseer::GetInstance()->StopAll();

	// �]�ڃr�[�R������
	CTransferBeacon::Create(CTransferBeacon::TRANSTYPE_GAMEMAIN);

	// �X�L���c���[�I�u�W�F�N�g����
	CSkillTree_Obj::Create();

	// �X�L���c���[�\�͐���
	if (m_pSkilltreeAbillity != nullptr)
	{
		m_pSkilltreeAbillity->Uninit();
		m_pSkilltreeAbillity = nullptr;
	}
	m_pSkilltreeAbillity = CSkillTree_Ability::Create();

	CLimitArea::sLimitEreaInfo info;
	info.fMaxX = 600.0f;
	info.fMaxZ = 1050.0f;
	info.fMinX = -600.0f;
	info.fMinZ = -1000.0f;
	CLimitArea* pArea = CLimitArea::Create(info);
	pArea->SetEnableDisp(false);

	MyFog::SetFogparam(D3DXCOLOR(1.0f, 0.95f, 0.9f, 1.0f), info.fMaxZ, 3000.0f, D3DFOG_LINEAR);
	MyFog::ToggleFogFrag(true);
}

//==========================================================================
// ����
//==========================================================================
void CGameManager::SceneReaspawn()
{
	// �J�ڂȂ��t�F�[�h�̏�Ԏ擾
	CInstantFade::STATE fadestate = CManager::GetInstance()->GetInstantFade()->GetState();
	if (fadestate != CInstantFade::STATE_FADECOMPLETION)
	{// �������ĂȂ�
		return;
	}

	// �G�̃��X�g�擾
	CListManager<CEnemy> enemyList = CEnemy::GetListObj();
	CEnemy* pEnemy = nullptr;

	// ���X�g���[�v
	while (enemyList.ListLoop(&pEnemy))
	{
		pEnemy->Kill();
		pEnemy->Uninit();
	}

	// �v���C���[����
	CPlayer* pPlayer = CPlayer::Create(0);

	// �A�C�R�����̏��擾
	std::vector<CSkillTree_Icon::sSkillIcon> iconInfo = CSkillTree::GetInstance()->GetIconInfo();

	// �K���ςݔ\�͊��蓖��
	for (int i = 0; i < static_cast<int>(m_PrevSkillIconMastering.size()); i++)
	{
		if (m_PrevSkillIconMastering[i] != CSkillTree_Icon::MASTERING_DONE)
		{
			continue;
		}
		CAbillityStrategy* pAbillity = CAbillityStrategy::CreateInstance(iconInfo[i], pPlayer);
		pAbillity->BindAbillity();

		delete pAbillity;
		pAbillity = nullptr;
	}

	// �O��̏K���󋵐ݒ�
	CSkillTree::GetInstance()->SetMastering(m_PrevSkillIconMastering);

	// �O��̃|�C���g+����|�C���g0�ݒ�
	pPlayer->GetSkillPoint()->SetPoint(m_nPrevPoint + 1);

	// ���݂̃X�e�[�W
	m_nNowStage--;
	if (m_nNowStage <= 0)
	{
		m_nNowStage = 0;
	}

	if (CGame::GetInstance()->GetEnemyBase()->GetNumStage() <= m_nNowStage)
	{// ���X�e�[�W���𒴂�����

		// �ʏ�X�e�[�W���I������
		m_bEndNormalStage = true;
	}
	else
	{
		m_bEndNormalStage = false;
	}

	// �G�̍Ĕz�u
	CEnemyManager* pEnemyManager = CGame::GetInstance()->GetEnemyManager();

	// �ύX���ɂ���
	pEnemyManager->SetEnableChangeStage(true);

	// �����V�[���ɐ؂�ւ�
	m_SceneType = SCENE_ENHANCE;

	// �����V�[������
	SceneEnhance();
}

//==========================================================================
// �{�X�ݒ�
//==========================================================================
void CGameManager::SetBoss()
{
	// BGM�X�g�b�v
	CManager::GetInstance()->GetSound()->StopSound(CSound::LABEL_BGM_GAME);

	// ��ސݒ�
	m_SceneType = SCENE_BOSS;

	// ���Z�b�g����
	CGame::GetInstance()->ResetBeforeBoss();

	// �V�[���̃��Z�b�g
	CManager::GetInstance()->GetScene()->ResetScene();


	// �v���C���[�擾
	CListManager<CPlayer> playerList = CPlayer::GetListObj();
	CPlayer* pPlayer = nullptr;

	// ���X�g���[�v
	while (playerList.ListLoop(&pPlayer))
	{
		// �ʒu�ݒ�
		pPlayer->SetPosition(D3DXVECTOR3(0.0f, 0.0f, -1000.0f));
	}

	// �J�����̏��擾
	CCamera* pCamera = CManager::GetInstance()->GetCamera();
	pCamera->ResetBoss();

	// ���t���[���N��
	//CManager::GetInstance()->GetBlackFrame()->SetState(CBlackFrame::STATE_IN);

	// �G�̍Ĕz�u
	CEnemyManager* pEnemyManager = CGame::GetInstance()->GetEnemyManager();
	if (pEnemyManager != NULL)
	{
		// �G�̍Ĕz�u
		pEnemyManager->SetStageBoss();
	}
}

//==========================================================================
// �G�ݒ�
//==========================================================================
void CGameManager::SetEnemy()
{

	// �X�e�[�W�؂�ւ�
	CGame::GetInstance()->GetStage()->ChangeStage("data\\TEXT\\stage\\info.txt");

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
		pPlayer->SetPosition(0.0f);
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
	CEnemyManager *pEnemyManager = CGame::GetInstance()->GetEnemyManager();
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
void CGameManager::AddNowStage()
{
	// ���Z
	m_nNowStage++;

	if (CGame::GetInstance()->GetEnemyBase()->GetNumStage() <= m_nNowStage)
	{// ���X�e�[�W���𒴂�����

		// �ʏ�X�e�[�W���I������
		m_bEndNormalStage = true;
	}

}

//==========================================================================
// ���݂̃X�e�[�W�擾
//==========================================================================
int CGameManager::GetNowStage()
{
	return m_nNowStage;
}

//==========================================================================
// �O��̋������e�ݒ�
//==========================================================================
void CGameManager::SetPrevEnhance()
{
	// �O��̏K���󋵕ۑ�
	m_PrevSkillIconMastering = CSkillTree::GetInstance()->GetMastering();
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
CGameManager::SceneType CGameManager::GetType()
{
	return m_SceneType;
}
