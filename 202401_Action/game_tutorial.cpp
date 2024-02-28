//=============================================================================
// 
//  ゲーム処理 [game_tutorial.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "manager.h"
#include "game_tutorial.h"
#include "tutorialmanager.h"
#include "renderer.h"
#include "calculation.h"
#include "debugproc.h"
#include "fade.h"
#include "camera.h"
#include "input.h"

#include "enemymanager.h"
#include "player.h"
#include "skillpoint.h"
#include "enemybase.h"

//==========================================================================
// コンストラクタ
//==========================================================================
CGameTutorial::CGameTutorial()
{
	// 値のクリア

}

//==========================================================================
// デストラクタ
//==========================================================================
CGameTutorial::~CGameTutorial()
{

}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CGameTutorial::Init()
{
	
	// 初期化処理
	CGame::Init();
	
	// プレイヤー取得
	CListManager<CPlayer> playerList = CPlayer::GetListObj();
	CPlayer* pPlayer = nullptr;

	// リストループ
	while (playerList.ListLoop(&pPlayer))
	{
		pPlayer->GetSkillPoint()->SetPoint(99);
	}

	// 成功
	return S_OK;
}

//==========================================================================
// モード別初期化
//==========================================================================
void CGameTutorial::InitByMode()
{
	//**********************************
	// 敵の拠点
	//**********************************
	m_pEnemyBase = CEnemyBase::Create("data\\TEXT\\enemydata\\base_tutorial.txt");
	if (m_pEnemyBase == nullptr)
	{
		return;
	}

	//**********************************
	// 敵マネージャ
	//**********************************
	m_pEnemyManager = CEnemyManager::Create("data\\TEXT\\enemydata\\manager.txt");
	if (m_pEnemyManager == nullptr)
	{
		return;
	}
}

//==========================================================================
// 終了処理
//==========================================================================
void CGameTutorial::Uninit()
{
	// 終了処理
	CGame::Uninit();
}

//==========================================================================
// 更新処理
//==========================================================================
void CGameTutorial::Update()
{
	// 更新
	CGame::Update();
}

//==========================================================================
// 描画処理
//==========================================================================
void CGameTutorial::Draw()
{
	// 描画
	CGame::Draw();
}
