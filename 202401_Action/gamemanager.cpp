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
CGameManager *CGameManager::Create(void)
{
	// 生成用のオブジェクト
	CGameManager *pManager = NULL;

	if (pManager == NULL)
	{// NULLだったら

		// メモリの確保
		pManager = DEBUG_NEW CGameManager;

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

	return NULL;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CGameManager::Init(void)
{
	m_bControll = true;			// 操作できるか
	m_bEndNormalStage = false;	// 通常ステージが終了したか
	m_nNowStage = 0;			// 現在のステージ
	m_SceneType = SCENE_MAIN;	// シーンの種類

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CGameManager::Uninit(void)
{
	
}

//==========================================================================
// 更新処理
//==========================================================================
void CGameManager::Update(void)
{
	if (CGame::GetEnemyBase()->GetNumStage() - 1 <= m_nNowStage)
	{// 総ステージ数を超えたら
		m_nNowStage = CGame::GetEnemyBase()->GetNumStage() - 1;

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
	CPlayer* pPlayer = nullptr;

	// リストループ
	while (playerList.ListLoop(&pPlayer))
	{
		pPlayer->GetSkillPoint()->AddPoint(POINT_WAVECLEAR);
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

	// カメラリセット
	CCamera* pCamera = CManager::GetInstance()->GetCamera();
	if (pCamera != NULL)
	{
		pCamera->Reset(CScene::MODE_GAME);
	}

	// ステージ切り替え
	CGame::GetStage()->ChangeStage("data\\TEXT\\stage\\info.txt");

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
	CGame::ResetBeforeBoss();

	// シーンのリセット
	CManager::GetInstance()->GetScene()->ResetScene();


	// プレイヤー取得
	CListManager<CPlayer> playerList = CPlayer::GetListObj();
	CPlayer* pPlayer = nullptr;

	// リストループ
	int i = 0;
	while (playerList.ListLoop(&pPlayer))
	{
		// 位置設定
		pPlayer->SetPosition(D3DXVECTOR3(-500.0f + i * 50.0f, 5000.0f, 0.0f));
		i++;
	}

	// カメラの情報取得
	CCamera* pCamera = CManager::GetInstance()->GetCamera();
	pCamera->ResetBoss();

	// 黒フレーム侵入
	//CManager::GetInstance()->GetBlackFrame()->SetState(CBlackFrame::STATE_IN);

	// 敵の再配置
	CEnemyManager* pEnemyManager = CGame::GetEnemyManager();
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
	CGame::GetStage()->ChangeStage("data\\TEXT\\stage\\info.txt");

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
	CEnemyManager *pEnemyManager = CGame::GetEnemyManager();
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

	if (CGame::GetEnemyBase()->GetNumStage() <= m_nNowStage)
	{// 総ステージ数を超えたら
		m_nNowStage = CGame::GetEnemyBase()->GetNumStage();

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
