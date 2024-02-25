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
#include "fade.h"
#include "controlassist.h"
#include "beforebattle.h"
#include "stagename.h"
#include "afterrespawn_text.h"
#include "gamerating.h"
#include "battleresult.h"


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
	m_bGameStart = false;		// �Q�[���J�n���̃t���O
	m_nNowStage = 0;			// ���݂̃X�e�[�W
	m_nNumStage = 0;			// �X�e�[�W�̑���
	m_nPrevPoint = 0;			// �O��̃|�C���g
	m_pSkilltreeAbillity = nullptr;	// �X�L���c���[�\�͂̃|�C���^
	m_PrevSkillIconMastering.clear();	// �O��̃X�L���A�C�R���̏K����
	m_p_PrevSkillIcon.clear();			// �O��̃X�L���A�C�R��
	m_pGameRating.clear();				// �Q�[���]��
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

	m_SceneType = SCENE_TRANSITION;	// �V�[���̎��

	// �Q�[���]���ǉ�
	m_pGameRating.clear();
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

	for (auto& rating : m_pGameRating)
	{
		rating->Uninit();
		rating = nullptr;
	}
}

//==========================================================================
// �X�V����
//==========================================================================
void CGameManager::Update()
{
	if (m_SceneType != SCENE_SKILLTREE &&
		m_SceneType != SceneType::SCENE_DURING_MAINRESULT)
	{
		// ����⏕����
		CControlAssist* pAssist = CControlAssist::GetInstance();
		if (pAssist)
		{
			pAssist->SetDefaultText();
		}
	}

	// ������
	switch (m_SceneType)
	{
	case CGameManager::SceneType::SCENE_MAIN:
		m_bControll = true;
		m_pGameRating[m_nNowStage]->AddClearTime(CManager::GetInstance()->GetDeltaTime());
		break;

	case CGameManager::SceneType::SCENE_MAINCLEAR:
		m_bControll = true;
		break;

	case SceneType::SCENE_MAINRESULT:
		m_bControll = true;
		break;

	case SceneType::SCENE_DURING_MAINRESULT:
		m_bControll = false;
		break;

	case SceneType::SCENE_BEFOREBATTLE:
		m_bControll = false;
		break;

	case SceneType::SCENE_BATTLESTART:
		m_bControll = false;
		break;

	case CGameManager::SceneType::SCENE_ENHANCE:
		m_bControll = true;
		SceneEnhance();
		break;

	case SceneType::SCENE_SKILLTREE:		// �X�L���c���[
		m_bControll = false;
		break;

	case CGameManager::SceneType::SCENE_BOSS:
		m_bControll = true;
		break;

	case CGameManager::SceneType::SCENE_TRANSITIONWAIT:
		m_bControll = false;
		break;

	case CGameManager::SceneType::SCENE_TRANSITION:
		m_bControll = false;
		SceneTransition();
		break;

	case CGameManager::SceneType::SCENE_REASPAWN:		// ����
		m_bControll = false;
		SceneReaspawn();
		break;

	case SceneType::SCENE_RESULT:
		m_bControll = false;
		break;

	default:
		break;
	}


	// �e�L�X�g�̕`��
	CManager::GetInstance()->GetDebugProc()->Print(
		"---------------- �Q�[���}�l�[�W����� ----------------\n"
		"�y���[�h�z[%d]\n"
		"�y�X�e�[�W�z[%d]\n"
		"�y�N���A���ԁz�u%f�v\n"
		"�y���S�񐔁z�u%d�v\n"
		"�y��_���[�W�z�u%d�v\n"
		, m_SceneType, m_nNowStage, m_pGameRating[m_nNowStage]->GetClearTime(),
		m_pGameRating[m_nNowStage]->GetNumDead(), 
		m_pGameRating[m_nNowStage]->GetReceiveDamage());
}

//==========================================================================
// �Q�[���N���A���̐ݒ�
//==========================================================================
void CGameManager::GameClearSettings()
{
	// �]�ڃr�[�R������
	if (m_nNowStage + 1 < m_nNumStage)
	{
		CTransferBeacon::Create(CTransferBeacon::TRANSTYPE_ENHANCE, MyLib::Vector3(0.0f, 200.0f, 0.0f));
	}
	else
	{
		CTransferBeacon::Create(CTransferBeacon::eTransType::TRANSTYPE_RESULT, MyLib::Vector3(0.0f, 200.0f, 0.0f));
	}

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

}

//==========================================================================
// �Q�[�����U���g�̐ݒ�
//==========================================================================
void CGameManager::GameResultSettings()
{

	CManager::GetInstance()->GetSound()->StopSound(CSound::LABEL::LABEL_BGM_GAME);
	CManager::GetInstance()->GetSound()->StopSound(CSound::LABEL::LABEL_BGM_BOSS);
	CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL::LABEL_BGM_GAMECLEAR);
	CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL::LABEL_BGM_GALLERY);

	// �N���A�e�L�X�g����
	CStageClearText::Create(MyLib::Vector3(640.0f, 360.0f, 0.0f));

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

	if (fadestate == CInstantFade::STATE_FADECOMPLETION || 
		!m_bGameStart)
	{// ���������u��

		// BGM�X�g�b�v
		CManager::GetInstance()->GetSound()->StopSound();
		if (m_nNowStage != 3)
		{
			CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL_BGM_GAME);
		}
		else {
			CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL::LABEL_BGM_BOSS);
		}

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
		CSkillPoint* pSkillPoint = pPlayer->GetSkillPoint();
		pSkillPoint->SetState(CSkillPoint::STATE_WAIT);

		// ����⏕����
		CControlAssist* pAssist = CControlAssist::Create();
		pAssist->ResetText();
		pAssist->SetText(CControlAssist::CONTROLTYPE_ROCKON);
		pAssist->SetText(CControlAssist::CONTROLTYPE_CHANGETARGET);
		pAssist->SetText(CControlAssist::CONTROLTYPE_ATTACK_NORMAL);
		pAssist->SetText(CControlAssist::CONTROLTYPE_AVOID);

		// �G�z�u
		SetEnemy();

		// �퓬�����ɑJ��
		CBeforeBattle::Create(MyLib::Vector3(640.0f, 360.0f, 0.0f));
		SetType(SceneType::SCENE_BEFOREBATTLE);
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
	if (m_bEndNormalStage)
	{
		//CManager::GetInstance()->GetFade()->SetFade(CScene::MODE::MODE_TITLE);
		//CManager::GetInstance()->SetMode(CScene::MODE::MODE_TITLE);
		return;
	}


	// BGM�X�g�b�v
	CManager::GetInstance()->GetSound()->StopSound();
	CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL::LABEL_BGM_ENHANCE);
	CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL::LABEL_BGM_ENHANCE_WIND);

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
	pPlayer->GetSkillPoint()->SetPoint(m_nPrevPoint + 2);

	if (m_pGameRating[m_nNowStage] != nullptr)
	{
		m_pGameRating[m_nNowStage]->AddNumDead();
	}

	// �����񐔉��Z
	m_pGameRating[m_nNowStage]->AddNumDead();

	// �N���A�^�C�����Z�b�g
	m_pGameRating[m_nNowStage]->SetClearTime(0.0f);

	// ���݂̃X�e�[�W
	m_nNowStage--;
	if (m_nNowStage <= -1)
	{
		m_nNowStage = -1;
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

	// ������e�L�X�g����
	CAfterRespawn_Text* pAfterText = CAfterRespawn_Text::Create();
	pAfterText->SetDefaultText();

	// �����V�[������
	SceneEnhance();
}

//==========================================================================
// �{�X�ݒ�
//==========================================================================
void CGameManager::SetBoss()
{
	// ����͎g�p����
#if 0
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
		pPlayer->SetPosition(MyLib::Vector3(0.0f, 0.0f, -1000.0f));
		pPlayer->SetRotation(MyLib::Vector3(0.0f, D3DX_PI, 0.0f));
		pPlayer->SetRotDest(D3DX_PI);
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
#endif
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
	while (playerList.ListLoop(&pPlayer))
	{
		// �ʒu�ݒ�
		pPlayer->SetPosition(MyLib::Vector3(0.0f, 0.0f, -1000.0f));
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

//==========================================================================
// �X�e�[�W�̑����ݒ�
//==========================================================================
void CGameManager::SetNumStage(int nStage)
{ 
	m_nNumStage = nStage;
	
	// �Q�[���]���ǉ�
	for (int i = 0; i < m_nNumStage; i++)
	{
		m_pGameRating.emplace_back();
		m_pGameRating[i] = CGameRating::Create();
	}
}

//==========================================================================
// �X�e�[�W�̉��Z
//==========================================================================
void CGameManager::AddNowStage()
{
	// ���Z
	m_nNowStage++;

	if (m_nNumStage <= m_nNowStage)
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

//==========================================================================
// �Q�[���]���擾
//==========================================================================
CGameRating* CGameManager::GetGameRating()
{
	if (m_pGameRating.empty())
	{
		return nullptr;
	}

	return m_pGameRating[m_nNowStage];
}
