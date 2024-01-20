//=============================================================================
// 
//  シーン処理 [scene.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "scene.h"
#include "renderer.h"
#include "calculation.h"
#include "Xload.h"
#include "map.h"
#include "fade.h"
#include "elevation.h"
#include "player.h"
#include "camera.h"

// 遷移先
#include "game.h"
#include "title.h"
#include "tutorial.h"
#include "result.h"
#include "ranking.h"

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================
CXLoad *CScene::m_pXLoad = NULL;				// Xファイルのオブジェクト
CElevation *CScene::m_pObject3DMesh = NULL;		// オブジェクト3Dメッシュのオブジェクト

//==========================================================================
// コンストラクタ
//==========================================================================
CScene::CScene()
{
	// 変数のクリア
	m_mode = MODE_TITLE;
}

//==========================================================================
// デストラクタ
//==========================================================================
CScene::~CScene()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CScene *CScene::Create(CScene::MODE mode)
{
	// 生成用のオブジェクト
	CScene *pScene = NULL;

	if (pScene == NULL)
	{// NULLだったら

		// 生成処理
		switch (mode)
		{
		case CScene::MODE_TITLE:
			pScene = DEBUG_NEW CTitle;
			break;

		case CScene::MODE_TUTORIAL:
			pScene = DEBUG_NEW CTutorial;
			break;

		case CScene::MODE_GAME:
			pScene = DEBUG_NEW CGame;
			break;

		case CScene::MODE_RESULT:
			pScene = DEBUG_NEW CResult;
			break;

		case CScene::MODE_RANKING:
			pScene = DEBUG_NEW CRanking;
			break;
		}

		if (pScene != NULL)
		{// メモリの確保が出来ていたら

			// モードの設定
			pScene->m_mode = mode;
		}

		return pScene;
	}

	return NULL;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CScene::Init(void)
{
	HRESULT hr;

	//**********************************
	// Xファイル
	//**********************************
	if (m_pXLoad != NULL)
	{// 確保されていたら
		return E_FAIL;
	}

	// メモリ確保
	m_pXLoad = CXLoad::Create();
	if (m_pXLoad == NULL)
	{// メモリの確保が出来ていなかったら
		return E_FAIL;
	}

	//**********************************
	// マップの生成
	//**********************************
	if (FAILED(MyMap::Create("data\\TEXT\\map\\info.txt")))
	{// 失敗した場合
		return E_FAIL;
	}

	//**********************************
	// 起伏の地面
	//**********************************
	m_pObject3DMesh = CElevation::Create("data\\TEXT\\elevation\\field_mountain.txt");

	// 初期化処理
	hr = m_pXLoad->Init();

	if (FAILED(hr))
	{// 初期化処理が失敗した場合
		return E_FAIL;
	}

	// キャラ生成
	for (int nCntPlayer = 0; nCntPlayer < CManager::GetInstance()->GetNumPlayer(); nCntPlayer++)
	{
		if (CPlayer::Create(nCntPlayer) == nullptr)
		{
			return E_FAIL;
		}
	}

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CScene::Uninit(void)
{
	// Xファイルの破棄
	if (m_pXLoad != NULL)
	{// メモリの確保が出来ていたら

		// 終了処理
		m_pXLoad->Uninit();

		// メモリの開放
		m_pXLoad = NULL;
	}

}

//==========================================================================
// 更新処理
//==========================================================================
void CScene::Update(void)
{
	
}

//==========================================================================
// 描画処理
//==========================================================================
void CScene::Draw(void)
{
	
}

//==========================================================================
// シーンのリセット
//==========================================================================
void CScene::ResetScene(void)
{
	//**********************************
	// 破棄フェーズ
	//**********************************
	// 起伏の地面
	if (m_pObject3DMesh != NULL)
	{
		m_pObject3DMesh->Uninit();
		m_pObject3DMesh = NULL;
	}

	// マップ
	MyMap::Release();

	//**********************************
	// 生成フェーズ
	//**********************************
	// マップ
	if (FAILED(MyMap::Create("data\\TEXT\\map\\info_boss.txt")))
	{// 失敗した場合
		return;
	}

	// ボスステージの起伏生成
	m_pObject3DMesh = CElevation::Create("data\\TEXT\\elevation\\field_city.txt");
}

//==========================================================================
// 現在のモード取得
//==========================================================================
CScene::MODE CScene::GetMode(void)
{
	return m_mode;
}

//==========================================================================
// Xファイルの取得
//==========================================================================
CXLoad *CScene::GetXLoad(void)
{
	return m_pXLoad;
}

//==========================================================================
// 3Dオブジェクトの取得
//==========================================================================
CElevation *CScene::GetElevation(void)
{
	return m_pObject3DMesh;
}
