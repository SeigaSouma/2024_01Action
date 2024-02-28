//=============================================================================
// 
// �Q�[���}�l�[�W������ [tutorialmanager.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "tutorialmanager.h"
#include "game.h"
#include "camera.h"
#include "manager.h"
#include "limitarea.h"
#include "MyEffekseer.h"
#include "fog.h"
#include "gallery.h"
#include "torch.h"
#include "controlassist.h"
#include "map.h"
#include "player.h"
#include "transferBeacon.h"
#include "skillpoint.h"
#include "skilltree.h"
#include "skilltree_obj.h"
#include "skilltree_ability.h"
#include "skilltree_behavior.h"
#include "stagename.h"
#include "enemy.h"
#include "enemymanager.h"

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CTutorialManager::CTutorialManager()
{
	// �l�̃N���A
	
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CTutorialManager::~CTutorialManager()
{

}

//==========================================================================
// ����������
//==========================================================================
HRESULT CTutorialManager::Init()
{
	CGameManager::Init();
	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CTutorialManager::Uninit()
{

	CGameManager::Uninit();
}

//==========================================================================
// �X�V����
//==========================================================================
void CTutorialManager::Update()
{
	CGameManager::Update();
}

//==========================================================================
// ���C���J�ڒ�
//==========================================================================
void CTutorialManager::SceneTransition()
{
	// �J�ڂȂ��t�F�[�h�̏�Ԏ擾
	CInstantFade::STATE fadestate = CManager::GetInstance()->GetInstantFade()->GetState();

	if (fadestate == CInstantFade::STATE_FADECOMPLETION ||
		!m_bGameStart)
	{// ���������u��

		// BGM�X�g�b�v
		CManager::GetInstance()->GetSound()->StopSound();
		CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL_BGM_GAME);

		// �Q�[���J�n���̃t���O
		m_bGameStart = true;

		// �J�����擾
		CCamera* pCamera = CManager::GetInstance()->GetCamera();
		if (pCamera != NULL)
		{
			pCamera->SetEnableFollow(true);
		}

		// �G���A�������擾
		CListManager<CLimitArea> limitareaList = CLimitArea::GetListObj();
		CLimitArea* pLimitArea = nullptr;
		while (limitareaList.ListLoop(&pLimitArea))
		{
			pLimitArea->Kill();
		}

		// �G�t�F�N�g�S�Ē�~
		CMyEffekseer::GetInstance()->StopAll();

		// �t�H�O���Z�b�g
		MyFog::ToggleFogFrag(false);

		// �o
		CMyEffekseer::GetInstance()->SetEffect(
			CMyEffekseer::EFKLABEL_BGFIRE,
			MyLib::Vector3(0.0f, 0.0f, 0.0f),
			0.0f, 0.0f, 100.0f, false);

		// �ϏO�ݒ�
		CGallery::SetGallery();

		// �����ݒ�
		CTorch::SetTorch();

		// �v���C���[�擾
		CListManager<CPlayer> playerList = CPlayer::GetListObj();
		CPlayer* pPlayer = playerList.GetData(0);

		// �N���A�|�C���g�ǉ�
		if (pPlayer != nullptr)
		{
			CSkillPoint* pSkillPoint = pPlayer->GetSkillPoint();
			pSkillPoint->SetState(CSkillPoint::STATE_WAIT);
		}

		// ����⏕����
		CControlAssist* pAssist = CControlAssist::Create();
		pAssist->ResetText();
		pAssist->SetText(CControlAssist::CONTROLTYPE_ROCKON);
		pAssist->SetText(CControlAssist::CONTROLTYPE_CHANGETARGET);
		pAssist->SetText(CControlAssist::CONTROLTYPE_ATTACK_NORMAL);
		pAssist->SetText(CControlAssist::CONTROLTYPE_AVOID);

		// �G�z�u
		SetEnemy();

		m_SceneType = SceneType::SCENE_MAIN;

		// �]�ڃr�[�R������
		CTransferBeacon::Create(CTransferBeacon::TRANSTYPE_ENHANCE, MyLib::Vector3(0.0f, 200.0f, 0.0f));

		// �ύX���ɂ���
		CEnemyManager* pEnemyManager = CGame::GetInstance()->GetEnemyManager();
		pEnemyManager->SetEnableChangeStage(true);
	}
}

//==========================================================================
// �����V�[��
//==========================================================================
void CTutorialManager::SceneEnhance()
{
	// �J�ڂȂ��t�F�[�h�̏�Ԏ擾
	CInstantFade::STATE fadestate = CManager::GetInstance()->GetInstantFade()->GetState();

	if (fadestate != CInstantFade::STATE_FADECOMPLETION &&
		m_bGameStart)
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

	// �G�̃��X�g�擾
	CListManager<CEnemy> enemyList = CEnemy::GetListObj();
	CEnemy* pEnemy = nullptr;

	// �S�폜
	while (enemyList.ListLoop(&pEnemy))
	{
		pEnemy->Kill();
		pEnemy->Uninit();
	}

	// �Q�[���J�n���̃t���O
	m_bGameStart = true;


	// BGM�X�g�b�v
	CManager::GetInstance()->GetSound()->StopSound();
	CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL::LABEL_BGM_ENHANCE);
	CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL::LABEL_BGM_ENHANCE_WIND);

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

	// �X�e�[�W������
	CStageName::Create();

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
void CTutorialManager::SceneReaspawn()
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
	pPlayer->GetSkillPoint()->SetPoint(99);

	// ���݂̃X�e�[�W
	m_nNowStage--;
	if (m_nNowStage <= -1)
	{
		m_nNowStage = -1;
	}

	// �ʏ�X�e�[�W���I������
	m_bEndNormalStage = true;

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
// �G�ݒ�
//==========================================================================
void CTutorialManager::SetEnemy()
{
	// �}�b�v�؂�ւ�
	MyMap::ChangeMap("data\\TEXT\\map\\info.txt");

	// �v���C���[�擾
	CListManager<CPlayer> playerList = CPlayer::GetListObj();
	CPlayer* pPlayer = nullptr;

	// ���X�g���[�v
	while (playerList.ListLoop(&pPlayer))
	{
		// �ʒu�ݒ�
		pPlayer->SetPosition(MyLib::Vector3(0.0f, 0.0f, 0.0f));
		pPlayer->SetRotation(MyLib::Vector3(0.0f, D3DX_PI, 0.0f));
		pPlayer->SetRotDest(D3DX_PI);
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

