//=============================================================================
// 
//  ゲーム処理 [tutorial.cpp]
//  Author : 相馬靜雅
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
// 静的メンバ変数宣言
//==========================================================================
CTutorial* CTutorial::m_pThisPtr = nullptr;	// 自身のポインタ

//==========================================================================
// コンストラクタ
//==========================================================================
CTutorial::CTutorial()
{
	// 値のクリア
	m_pLimitArea = nullptr;			// エリア制限のオブジェクト
	//m_pGameManager = nullptr;		// ゲームマネージャのオブジェクト
	m_pEnemyBase = nullptr;			// 敵の拠点
	m_pEnemyManager = nullptr;		// 敵マネージャのオブジェクト
	m_bEdit = false;				// エディットの判定
	m_clear = false;				// クリア判定
	m_fMaxRokOnDistance = 0.0f;		// ロックオンの最大距離
}

//==========================================================================
// デストラクタ
//==========================================================================
CTutorial::~CTutorial()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CTutorial* CTutorial::Create()
{
	if (m_pThisPtr == nullptr)
	{// まだ生成していなかったら

		// インスタンス生成
		m_pThisPtr = DEBUG_NEW CTutorial;
	}

	return m_pThisPtr;
}

//==========================================================================
// インスタンス取得
//==========================================================================
CTutorial* CTutorial::GetInstance()
{
	return m_pThisPtr;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CTutorial::Init()
{
	// エディット判定OFF
	m_bEdit = false;

	// ロックオンの最大距離
	m_fMaxRokOnDistance = mylib_const::MAX_ROCKONDISTANCE_GAME;

	// プレイヤーの数設定
	CManager::GetInstance()->SetNumPlayer(1);

	// 初期化処理
	if (FAILED(CScene::Init()))
	{// 失敗した場合
		return E_FAIL;
	}

	//**********************************
	// ゲームマネージャ
	//**********************************
	//m_pGameManager = CTutorialManager::Create();

	//**********************************
	// 敵の拠点
	//**********************************
	m_pEnemyBase = CEnemyBase::Create("data\\TEXT\\enemydata\\base.txt");
	if (m_pEnemyBase == nullptr)
	{// nullptrだったら
		return E_FAIL;
	}

	//**********************************
	// 敵マネージャ
	//**********************************
	m_pEnemyManager = CEnemyManager::Create("data\\TEXT\\enemydata\\manager.txt");
	if (m_pEnemyManager == nullptr)
	{// nullptrだったら
		return E_FAIL;
	}

	//**********************************
	// プレイヤー
	//**********************************
	// キャラ生成
	for (int nCntPlayer = 0; nCntPlayer < 1; nCntPlayer++)
	{
		if (CPlayer::Create(nCntPlayer) == nullptr)
		{
			return E_FAIL;
		}
	}
	// プレイヤー取得
	CListManager<CPlayer> playerList = CPlayer::GetListObj();
	CPlayer* pPlayer = nullptr;

	// リストループ
	while (playerList.ListLoop(&pPlayer))
	{
		pPlayer->SetPosition(MyLib::Vector3(0.0f, 10.0f, 0.0f));
		pPlayer->SetRotation(MyLib::Vector3(0.0f, 0.0f, 0.0f));
	}

	// スコアの生成処理
	//m_pScore = CScore::Create(MyLib::Vector3(1000.0f, 50.0f, 0.0f));

	CManager::GetInstance()->GetCamera()->Reset(CScene::MODE_GAME);

	// BGM再生
	//CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL_BGM_GAME);

	/*CLimitArea::sLimitEreaInfo info;
	info.fMaxX = 13000.0f;
	info.fMaxZ = 1200.0f;
	info.fMinX = -1200.0f;
	info.fMinZ = -1200.0f;
	m_pLimitArea = CLimitArea::Create(info);
	m_pLimitArea->SetEnableDisp(false);*/

	// スキルツリー生成
	CSkillTree::Create();
	//m_pGameManager->SetPrevEnhance();

	// クリアの判定
	SetEnableClear(true);


	// 成功
	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CTutorial::Uninit()
{
	m_pThisPtr = nullptr;

	

	//if (m_pGameManager != nullptr)
	//{
	//	// 終了処理
	//	m_pGameManager->Uninit();
	//	delete m_pGameManager;
	//	m_pGameManager = nullptr;
	//}

	// 敵マネージャ
	if (m_pEnemyManager != nullptr)
	{
		m_pEnemyManager->Uninit();
		delete m_pEnemyManager;
		m_pEnemyManager = nullptr;
	}

	// 敵の拠点
	if (m_pEnemyBase != nullptr)
	{
		m_pEnemyBase->Uninit();
		delete m_pEnemyBase;
		m_pEnemyBase = nullptr;
	}

	// 終了処理
	CScene::Uninit();
}

//==========================================================================
// 更新処理
//==========================================================================
void CTutorial::Update()
{

	//// ゲームマネージャ
	//if (m_pGameManager != nullptr)
	//{
	//	// 更新処理
	//	m_pGameManager->Update();
	//}

	CManager::GetInstance()->GetDebugProc()->Print(
		"現在のモード：【ゲーム】\n"
		"切り替え：【 F 】\n\n");

	// キーボード情報取得
	CInputKeyboard *pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();

	// ゲームパッド情報取得
	CInputGamepad *pInputGamepad = CManager::GetInstance()->GetInputGamepad();


	if (GetEnemyManager() != nullptr)
	{// 敵マネージャの更新処理
		GetEnemyManager()->Update();
	}

	// 敵の拠点
	if (m_pEnemyBase != nullptr)
	{
		m_pEnemyBase->Update();
	}

#if _DEBUG

	if (pInputKeyboard->GetTrigger(DIK_F))
	{
		// モード設定
		CManager::GetInstance()->GetFade()->SetFade(CScene::MODE_RESULT);
	}

#endif

	// シーンの更新
	CScene::Update();

}

//==========================================================================
// 描画処理
//==========================================================================
void CTutorial::Draw()
{

}
//
////==========================================================================
//// ゲームマネージャの取得
////==========================================================================
//CTutorialManager *CTutorial::GetGameManager()
//{
//	return m_pGameManager;
//}

//==========================================================================
// 敵マネージャの取得
//==========================================================================
CEnemyManager *CTutorial::GetEnemyManager()
{
	return m_pEnemyManager;
}

//==========================================================================
// 敵の拠点
//==========================================================================
CEnemyBase *CTutorial::GetEnemyBase()
{
	return m_pEnemyBase;
}

//==========================================================================
// リセット処理
//==========================================================================
void CTutorial::ResetBeforeBoss()
{
#if 0
	// ステージの破棄
	if (m_pStage != nullptr)
	{// メモリの確保が出来ていたら

		// 終了処理
		m_pStage->Release();
		delete m_pStage;
		m_pStage = nullptr;
	}

	// 敵の拠点
	if (m_pEnemyBase != nullptr)
	{
		m_pEnemyBase->Uninit();
		delete m_pEnemyBase;
		m_pEnemyBase = nullptr;
	}

	// エリア制限
	if (m_pLimitArea != nullptr)
	{
		m_pLimitArea->Uninit();
		m_pLimitArea = nullptr;
	}

	// ステージ
	m_pStage = CStage::Create("data\\TEXT\\stage\\boss_info.txt");

	//**********************************
	// 敵の拠点
	//**********************************
	m_pEnemyBase = CEnemyBase::Create("data\\TEXT\\enemydata\\base_boss.txt");
	if (m_pEnemyBase == nullptr)
	{// nullptrだったら
		return;
	}
#endif
	// ロックオンの最大距離
	m_fMaxRokOnDistance = mylib_const::MAX_ROCKONDISTANCE_BOSS;
}

//==========================================================================
// エディターリセット処理
//==========================================================================
void CTutorial::EditReset()
{
	
	

}
