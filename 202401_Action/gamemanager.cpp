//=============================================================================
// 
// ゲームマネージャ処理 [gamemanager.cpp]
//  Author : 相馬靜雅
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
#include "skilltree.h"
#include "skilltree_ability.h"
#include "skilltree_behavior.h"
#include "gallery.h"
#include "torch.h"


//==========================================================================
// 定数定義
//==========================================================================
namespace
{
	const int POINT_WAVECLEAR = 5;		// ウェーブクリアのポイント
}

//==========================================================================
// コンストラクタ
//==========================================================================
CGameManager::CGameManager()
{
	// 値のクリア
	m_SceneType = SCENE_MAIN;	// シーンの種類
	m_bEndRush = false;			// ラッシュが終了したか
	m_bControll = false;		// 操作できるか
	m_bEndNormalStage = false;	// 通常ステージが終了したか
	m_nNowStage = 0;			// 現在のステージ
	m_nNumStage = 0;			// ステージの総数
	m_nPrevPoint = 0;			// 前回のポイント
	m_pSkilltreeAbillity = nullptr;	// スキルツリー能力のポインタ
	m_PrevSkillIconMastering.clear();	// 前回のスキルアイコンの習得状況
	m_p_PrevSkillIcon.clear();			// 前回のスキルアイコン
}

//==========================================================================
// デストラクタ
//==========================================================================
CGameManager::~CGameManager()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CGameManager* CGameManager::Create(void)
{
	// メモリ確保
	CGameManager* pManager = DEBUG_NEW CGameManager;

	if (pManager != NULL)
	{// メモリの確保が出来ていたら

		// 初期化処理
		HRESULT hr = pManager->Init();

		if (FAILED(hr))
		{// 失敗していたら
			return NULL;
		}
	}

	return pManager;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CGameManager::Init(void)
{
	m_bControll = true;			// 操作できるか
	m_bEndNormalStage = false;	// 通常ステージが終了したか

#if _DEBUG
	m_nNowStage = 0;			// 現在のステージ
#else
	m_nNowStage = 0;			// 現在のステージ
#endif

	m_SceneType = SCENE_MAIN;	// シーンの種類

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CGameManager::Uninit(void)
{
	if (m_pSkilltreeAbillity != nullptr)
	{
		m_pSkilltreeAbillity->Uninit();
		m_pSkilltreeAbillity = nullptr;
	}
}

//==========================================================================
// 更新処理
//==========================================================================
void CGameManager::Update(void)
{
	if (CGame::GetInstance()->GetEnemyBase()->GetNumStage() - 1 <= m_nNowStage)
	{// 総ステージ数を超えたら
		m_nNowStage = CGame::GetInstance()->GetEnemyBase()->GetNumStage() - 1;

		// 通常ステージが終了判定
		m_bEndNormalStage = true;
	}

	// 操作状態
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

	case SCENE_SKILLTREE:		// スキルツリー
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

	case SCENE_REASPAWN:			// 復活
		m_bControll = false;
		SceneReaspawn();
		break;

	default:
		break;
	}


	// テキストの描画
	CManager::GetInstance()->GetDebugProc()->Print(
		"---------------- ゲームマネージャ情報 ----------------\n"
		"【今のモード】[%d]\n", m_SceneType);


	if (m_SceneType == SCENE_TRANSITION)
	{// 遷移中

		// 遷移なしフェードの状態取得
		CInstantFade::STATE fadestate = CManager::GetInstance()->GetInstantFade()->GetState();

		if (fadestate == CInstantFade::STATE_FADECOMPLETION)
		{// 完了した瞬間

			// カメラ取得
			CCamera *pCamera = CManager::GetInstance()->GetCamera();
			if (pCamera != NULL)
			{
				pCamera->SetEnableFollow(true);
			}

			// 観衆設定
			CGallery::SetGallery();

			// 松明設定
			CTorch::SetTorch();

			if (!m_bEndNormalStage)
			{// 通常ステージが終わっていなかったら
				SetEnemy();
			}
			else
			{// ボスステージ
				SetBoss();
			}

		}
	}

}

//==========================================================================
// ゲームクリア時の設定
//==========================================================================
void CGameManager::GameClearSettings(void)
{
	// クリアテキスト生成
	CStageClearText::Create(MyLib::Vector3(640.0f, 360.0f, 0.0f));

	// 転移ビーコン生成
	CTransferBeacon::Create(CTransferBeacon::TRANSTYPE_ENHANCE);

	// プレイヤー取得
	CListManager<CPlayer> playerList = CPlayer::GetListObj();
	CPlayer* pPlayer = playerList.GetData(0);

	// クリアポイント追加
	pPlayer->GetSkillPoint()->AddPoint(POINT_WAVECLEAR);

	// 前回のポイント保存
	m_nPrevPoint = pPlayer->GetSkillPoint()->GetPoint();

	// 前回の習得状況保存
	m_PrevSkillIconMastering = CSkillTree::GetInstance()->GetMastering();

	// 観衆のリスト取得
	CListManager<CGallery> galleryList = CGallery::GetList();
	CGallery* pGallery = nullptr;
	while (galleryList.ListLoop(&pGallery))
	{
		pGallery->SetState(CGallery::STATE_CLEARHEAT);
	}

}

//==========================================================================
// 強化シーン
//==========================================================================
void CGameManager::SceneEnhance(void)
{
	// 遷移なしフェードの状態取得
	CInstantFade::STATE fadestate = CManager::GetInstance()->GetInstantFade()->GetState();

	if (fadestate != CInstantFade::STATE_FADECOMPLETION)
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
	}

	// ステージ切り替え
	CGame::GetInstance()->GetStage()->ChangeStage("data\\TEXT\\stage\\info.txt");

	// マップ切り替え
	MyMap::ChangeMap("data\\TEXT\\map\\map_enhance.txt");

	// プレイヤー取得
	CListManager<CPlayer> playerList = CPlayer::GetListObj();
	CPlayer* pPlayer = nullptr;

	// リストループ
	while (playerList.ListLoop(&pPlayer))
	{
		pPlayer->SetPosition(0.0f);
	}

	// 転移ビーコン生成
	CTransferBeacon::Create(CTransferBeacon::TRANSTYPE_GAMEMAIN);

	// スキルツリーオブジェクト生成
	CSkillTree_Obj::Create();

	// スキルツリー能力生成
	if (m_pSkilltreeAbillity != nullptr)
	{
		m_pSkilltreeAbillity->Uninit();
		m_pSkilltreeAbillity = nullptr;
	}
	m_pSkilltreeAbillity = CSkillTree_Ability::Create();
}

//==========================================================================
// 復活
//==========================================================================
void CGameManager::SceneReaspawn(void)
{
	// 遷移なしフェードの状態取得
	CInstantFade::STATE fadestate = CManager::GetInstance()->GetInstantFade()->GetState();
	if (fadestate != CInstantFade::STATE_FADECOMPLETION)
	{// 完了してない
		return;
	}

	// プレイヤー取得
	CListManager<CPlayer> playerList = CPlayer::GetListObj();
	CPlayer* pPlayer = playerList.GetData(0);

	// アイコン毎の情報取得
	std::vector<CSkillTree_Icon::sSkillIcon> iconInfo = CSkillTree::GetInstance()->GetIconInfo();

	// 習得済み能力割り当て
	for (const auto& info : iconInfo)
	{
		if (info.mastering != CSkillTree_Icon::MASTERING_DONE)
		{
			continue;
		}
		CAbillityStrategy* pAbillity = CAbillityStrategy::CreateInstance(info, pPlayer);
		pAbillity->BindAbillity();

		delete pAbillity;
		pAbillity = nullptr;
	}

	// 前回の習得状況設定
	CSkillTree::GetInstance()->SetMastering(m_PrevSkillIconMastering);

	// 前回のポイント+お情けポイント0設定
	pPlayer->GetSkillPoint()->SetPoint(m_nPrevPoint + 1);


	// 強化シーン処理
	SceneEnhance();
}

//==========================================================================
// ボス設定
//==========================================================================
void CGameManager::SetBoss(void)
{
	// BGMストップ
	CManager::GetInstance()->GetSound()->StopSound(CSound::LABEL_BGM_GAME);

	// 種類設定
	m_SceneType = SCENE_BOSS;

	// リセット処理
	CGame::GetInstance()->ResetBeforeBoss();

	// シーンのリセット
	CManager::GetInstance()->GetScene()->ResetScene();


	// プレイヤー取得
	CListManager<CPlayer> playerList = CPlayer::GetListObj();
	CPlayer* pPlayer = nullptr;

	// リストループ
	while (playerList.ListLoop(&pPlayer))
	{
		// 位置設定
		pPlayer->SetPosition(D3DXVECTOR3(0.0f, 0.0f, -2000.0f));
	}

	// カメラの情報取得
	CCamera* pCamera = CManager::GetInstance()->GetCamera();
	pCamera->ResetBoss();

	// 黒フレーム侵入
	//CManager::GetInstance()->GetBlackFrame()->SetState(CBlackFrame::STATE_IN);

	// 敵の再配置
	CEnemyManager* pEnemyManager = CGame::GetInstance()->GetEnemyManager();
	if (pEnemyManager != NULL)
	{
		// 敵の再配置
		pEnemyManager->SetStageBoss();
	}
}

//==========================================================================
// 敵設定
//==========================================================================
void CGameManager::SetEnemy(void)
{

	// ステージ切り替え
	CGame::GetInstance()->GetStage()->ChangeStage("data\\TEXT\\stage\\info.txt");

	// マップ切り替え
	MyMap::ChangeMap("data\\TEXT\\map\\info.txt");

	// プレイヤー取得
	CListManager<CPlayer> playerList = CPlayer::GetListObj();
	CPlayer* pPlayer = nullptr;

	// リストループ
	int i = 0;
	while (playerList.ListLoop(&pPlayer))
	{
		// 位置設定
		pPlayer->SetPosition(MyLib::Vector3(-500.0f + i * 250.0f, 0.0f, 0.0f));
		i++;
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

//==========================================================================
// ステージの加算
//==========================================================================
void CGameManager::AddNowStage(void)
{
	// 加算
	m_nNowStage++;

	if (CGame::GetInstance()->GetEnemyBase()->GetNumStage() <= m_nNowStage)
	{// 総ステージ数を超えたら
		m_nNowStage = CGame::GetInstance()->GetEnemyBase()->GetNumStage();

		// 通常ステージが終了判定
		m_bEndNormalStage = true;
	}

}

//==========================================================================
// 現在のステージ取得
//==========================================================================
int CGameManager::GetNowStage(void)
{
	return m_nNowStage;
}

//==========================================================================
// シーンの種類設定
//==========================================================================
void CGameManager::SetType(SceneType type)
{
	m_SceneType = type;
}

//==========================================================================
// シーンの種類取得
//==========================================================================
CGameManager::SceneType CGameManager::GetType(void)
{
	return m_SceneType;
}
