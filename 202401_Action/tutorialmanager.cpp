//=============================================================================
// 
// ゲームマネージャ処理 [tutorialmanager.cpp]
//  Author : 相馬靜雅
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
// コンストラクタ
//==========================================================================
CTutorialManager::CTutorialManager()
{
	// 値のクリア
	
}

//==========================================================================
// デストラクタ
//==========================================================================
CTutorialManager::~CTutorialManager()
{

}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CTutorialManager::Init()
{
	CGameManager::Init();
	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CTutorialManager::Uninit()
{

	CGameManager::Uninit();
}

//==========================================================================
// 更新処理
//==========================================================================
void CTutorialManager::Update()
{
	CGameManager::Update();
}

//==========================================================================
// メイン遷移中
//==========================================================================
void CTutorialManager::SceneTransition()
{
	// 遷移なしフェードの状態取得
	CInstantFade::STATE fadestate = CManager::GetInstance()->GetInstantFade()->GetState();

	if (fadestate == CInstantFade::STATE_FADECOMPLETION ||
		!m_bGameStart)
	{// 完了した瞬間

		// BGMストップ
		CManager::GetInstance()->GetSound()->StopSound();
		CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL_BGM_GAME);

		// ゲーム開始時のフラグ
		m_bGameStart = true;

		// カメラ取得
		CCamera* pCamera = CManager::GetInstance()->GetCamera();
		if (pCamera != NULL)
		{
			pCamera->SetEnableFollow(true);
		}

		// エリア制限情報取得
		CListManager<CLimitArea> limitareaList = CLimitArea::GetListObj();
		CLimitArea* pLimitArea = nullptr;
		while (limitareaList.ListLoop(&pLimitArea))
		{
			pLimitArea->Kill();
		}

		// エフェクト全て停止
		CMyEffekseer::GetInstance()->StopAll();

		// フォグリセット
		MyFog::ToggleFogFrag(false);

		// 塵
		CMyEffekseer::GetInstance()->SetEffect(
			CMyEffekseer::EFKLABEL_BGFIRE,
			MyLib::Vector3(0.0f, 0.0f, 0.0f),
			0.0f, 0.0f, 100.0f, false);

		// 観衆設定
		CGallery::SetGallery();

		// 松明設定
		CTorch::SetTorch();

		// プレイヤー取得
		CListManager<CPlayer> playerList = CPlayer::GetListObj();
		CPlayer* pPlayer = playerList.GetData(0);

		// クリアポイント追加
		if (pPlayer != nullptr)
		{
			CSkillPoint* pSkillPoint = pPlayer->GetSkillPoint();
			pSkillPoint->SetState(CSkillPoint::STATE_WAIT);
		}

		// 操作補助生成
		CControlAssist* pAssist = CControlAssist::Create();
		pAssist->ResetText();
		pAssist->SetText(CControlAssist::CONTROLTYPE_ROCKON);
		pAssist->SetText(CControlAssist::CONTROLTYPE_CHANGETARGET);
		pAssist->SetText(CControlAssist::CONTROLTYPE_ATTACK_NORMAL);
		pAssist->SetText(CControlAssist::CONTROLTYPE_AVOID);

		// 敵配置
		SetEnemy();

		m_SceneType = SceneType::SCENE_MAIN;

		// 転移ビーコン生成
		CTransferBeacon::Create(CTransferBeacon::TRANSTYPE_ENHANCE, MyLib::Vector3(0.0f, 200.0f, 0.0f));

		// 変更中にする
		CEnemyManager* pEnemyManager = CGame::GetInstance()->GetEnemyManager();
		pEnemyManager->SetEnableChangeStage(true);
	}
}

//==========================================================================
// 強化シーン
//==========================================================================
void CTutorialManager::SceneEnhance()
{
	// 遷移なしフェードの状態取得
	CInstantFade::STATE fadestate = CManager::GetInstance()->GetInstantFade()->GetState();

	if (fadestate != CInstantFade::STATE_FADECOMPLETION &&
		m_bGameStart)
	{// 完了してない
		return;
	}

	// 観衆削除
	CListManager<CGallery> galleryList = CGallery::GetList();
	CGallery* pGallery = nullptr;
	while (galleryList.ListLoop(&pGallery))
	{
		pGallery->Kill();
	}

	// 松明削除
	CListManager<CTorch> torchList = CTorch::GetList();
	CTorch* pTorch = nullptr;
	while (torchList.ListLoop(&pTorch))
	{
		pTorch->Kill();
	}

	// カメラリセット
	CCamera* pCamera = CManager::GetInstance()->GetCamera();
	if (pCamera != NULL)
	{
		pCamera->Reset(CScene::MODE_GAME);
		pCamera->SetStateCameraV(DEBUG_NEW CStateCameraV_Enhance());
	}

	// エフェクト全て停止
	CMyEffekseer::GetInstance()->StopAll();

	// 操作補助削除
	CControlAssist* pAssist = CControlAssist::GetInstance();
	if (pAssist != nullptr)
	{
		pAssist->ResetText();
	}

	// 敵のリスト取得
	CListManager<CEnemy> enemyList = CEnemy::GetListObj();
	CEnemy* pEnemy = nullptr;

	// 全削除
	while (enemyList.ListLoop(&pEnemy))
	{
		pEnemy->Kill();
		pEnemy->Uninit();
	}

	// ゲーム開始時のフラグ
	m_bGameStart = true;


	// BGMストップ
	CManager::GetInstance()->GetSound()->StopSound();
	CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL::LABEL_BGM_ENHANCE);
	CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL::LABEL_BGM_ENHANCE_WIND);

	// マップ切り替え
	MyMap::ChangeMap("data\\TEXT\\map\\map_enhance.txt");

	// プレイヤー取得
	CListManager<CPlayer> playerList = CPlayer::GetListObj();
	CPlayer* pPlayer = nullptr;

	// リストループ
	while (playerList.ListLoop(&pPlayer))
	{
		pPlayer->SetPosition(0.0f);
		pPlayer->GetSkillPoint()->SetState(CSkillPoint::STATE_ENHANCE);
	}

	// エフェクト全て停止
	CMyEffekseer::GetInstance()->StopAll();

	// 転移ビーコン生成
	CTransferBeacon::Create(CTransferBeacon::TRANSTYPE_GAMEMAIN);

	// スキルツリーオブジェクト生成
	CSkillTree_Obj::Create();

	// ステージ名生成
	CStageName::Create();

	// スキルツリー能力生成
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
// 復活
//==========================================================================
void CTutorialManager::SceneReaspawn()
{
	// 遷移なしフェードの状態取得
	CInstantFade::STATE fadestate = CManager::GetInstance()->GetInstantFade()->GetState();
	if (fadestate != CInstantFade::STATE_FADECOMPLETION)
	{// 完了してない
		return;
	}

	// 敵のリスト取得
	CListManager<CEnemy> enemyList = CEnemy::GetListObj();
	CEnemy* pEnemy = nullptr;

	// リストループ
	while (enemyList.ListLoop(&pEnemy))
	{
		pEnemy->Kill();
		pEnemy->Uninit();
	}

	// プレイヤー生成
	CPlayer* pPlayer = CPlayer::Create(0);

	// アイコン毎の情報取得
	std::vector<CSkillTree_Icon::sSkillIcon> iconInfo = CSkillTree::GetInstance()->GetIconInfo();

	// 習得済み能力割り当て
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

	// 前回の習得状況設定
	CSkillTree::GetInstance()->SetMastering(m_PrevSkillIconMastering);

	// 前回のポイント+お情けポイント0設定
	pPlayer->GetSkillPoint()->SetPoint(99);

	// 現在のステージ
	m_nNowStage--;
	if (m_nNowStage <= -1)
	{
		m_nNowStage = -1;
	}

	// 通常ステージが終了判定
	m_bEndNormalStage = true;

	// 敵の再配置
	CEnemyManager* pEnemyManager = CGame::GetInstance()->GetEnemyManager();

	// 変更中にする
	pEnemyManager->SetEnableChangeStage(true);

	// 強化シーンに切り替え
	m_SceneType = SCENE_ENHANCE;

	// 強化シーン処理
	SceneEnhance();
}

//==========================================================================
// 敵設定
//==========================================================================
void CTutorialManager::SetEnemy()
{
	// マップ切り替え
	MyMap::ChangeMap("data\\TEXT\\map\\info.txt");

	// プレイヤー取得
	CListManager<CPlayer> playerList = CPlayer::GetListObj();
	CPlayer* pPlayer = nullptr;

	// リストループ
	while (playerList.ListLoop(&pPlayer))
	{
		// 位置設定
		pPlayer->SetPosition(MyLib::Vector3(0.0f, 0.0f, 0.0f));
		pPlayer->SetRotation(MyLib::Vector3(0.0f, D3DX_PI, 0.0f));
		pPlayer->SetRotDest(D3DX_PI);
	}

	// カメラの情報取得
	CCamera *pCamera = CManager::GetInstance()->GetCamera();
	if (pCamera != NULL)
	{
		pCamera->Reset(CScene::MODE_GAME);
	}

	// 種類設定
	m_SceneType = SCENE_MAIN;

	// 敵の再配置
	CEnemyManager *pEnemyManager = CGame::GetInstance()->GetEnemyManager();
	if (pEnemyManager != NULL)
	{
		// 敵の再配置
		pEnemyManager->SetStageEnemy();

		// 変更中じゃなくする
		pEnemyManager->SetEnableChangeStage(false);
	}
}

