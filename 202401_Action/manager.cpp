//=============================================================================
// 
//  マネージャ処理 [manager.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "manager.h"
#include "renderer.h"
#include "input.h"
#include "debugproc.h"
#include "object2D.h"
#include "object3D.h"
#include "objectBillboard.h"
#include "objectX.h"
#include "texture.h"
#include "calculation.h"

#include "pause.h"
#include "fade.h"
#include "instantfade.h"
#include "blackframe.h"
#include "light.h"
#include "camera.h"
#include "edit.h"
#include "resultmanager.h"
#include "rankingmanager.h"
#include "MyEffekseer.h"
#include "loadmanager.h"
#include "Imguimanager.h"
#include "fog.h"

//==========================================================================
// 定数定義
//==========================================================================
namespace
{
	const float TIME_LOAD = 2.0f;	// ロード時間

#if _DEBUG
	const CScene::MODE STARTMODE = CScene::MODE_GAME;
#else
	const CScene::MODE STARTMODE = CScene::MODE_TITLE;
#endif
}

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================
CManager *CManager::m_pManager = nullptr;					// マネージャのオブジェクト

//==========================================================================
// コンストラクタ
//==========================================================================
CManager::CManager()
{
	m_pRenderer = nullptr;			// レンダラー
	m_pInputKeyboard = nullptr;		// キーボード
	m_pInputGamepad = nullptr;		// ゲームパッド
	m_pSound = nullptr;				// サウンド
	m_pInputMouse = nullptr;		// マウス
	m_pDebugProc = nullptr;			// デバッグ表示
	m_pLight = nullptr;				// ライト
	m_pCamera = nullptr;			// カメラ
	m_pTexture = nullptr;			// テクスチャ
	m_pXLoad = nullptr;				// Xファイル
	m_pEdit = nullptr;				// エディット
	m_pScene = nullptr;				// シーン
	m_pFade = nullptr;				// フェード
	m_pInstantFade = nullptr;		// 遷移なしフェード
	m_pBlackFrame = nullptr;		// 黒フレーム
	m_pPause = nullptr;				// ポーズ
	m_pResultManager = nullptr;		// リザルトマネージャ
	m_pRankingManager = nullptr;	// ランキングマネージャ
	m_pMyEffekseer = nullptr;		// エフェクシア
	m_bWireframe = false;			// ワイヤーフレーム
	m_bHitStop = false;				// ヒットストップの判定
	m_nCntHitStop = 0;				// ヒットストップのカウンター
	m_OldMode = CScene::MODE_NONE;	// 前回のモード
	m_CurrentTime = 0;				// 現在時間
	m_OldTime = 0;					// 過去の時間
	m_fDeltaTime = 0.0f;			// 経過時間
	m_nNumPlayer = 0;				// プレイヤーの数
	m_fLoadTimer = 0.0f;			// ロードのタイマー
	m_bLoadComplete = false;		// ロード完了のフラグ
	m_bFirstLoad = false;			// 初回ロード

	// ロードフラグリセット
	m_bLoadComplete = false;

	// ロードのフェード設定フラグ
	m_bLoadFadeSet = false;
	m_bNowLoading = false;				// ロード完了のフラグ
}

//==========================================================================
// デストラクタ
//==========================================================================
CManager::~CManager()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CManager *CManager::Create()
{
	if (m_pManager == nullptr)
	{// まだ生成していなかったら

		// マネージャのインスタンス生成
		m_pManager = DEBUG_NEW CManager;
	}
	else
	{
		// インスタンス取得
		m_pManager->GetInstance();
	}

	return m_pManager;
}

//==========================================================================
// インスタンス取得
//==========================================================================
CManager *CManager::GetInstance()
{
	return m_pManager;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CManager::Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{

	HRESULT hr;
	m_bHitStop = false;		// ヒットストップの判定
	m_nCntHitStop = 0;		// ヒットストップのカウンター
	m_nNumPlayer = 0;		// プレイヤーの数

	//**********************************
	// キーボード
	//**********************************
	if (m_pInputKeyboard != nullptr)
	{// 確保されていたら
		return E_FAIL;
	}

	// メモリ確保
	m_pInputKeyboard = DEBUG_NEW CInputKeyboard;

	if (m_pInputKeyboard != nullptr)
	{// メモリの確保が出来ていたら

		// 初期化処理
		hr = m_pInputKeyboard->Init(hInstance, hWnd);
		if (FAILED(hr))
		{// 初期化処理が失敗した場合
			return E_FAIL;
		}
	}
	
	//**********************************
	// ゲームパッド
	//**********************************
	if (m_pInputGamepad != nullptr)
	{// 確保されていたら
		return E_FAIL;
	}

	// メモリ確保
	m_pInputGamepad = DEBUG_NEW CInputGamepad;

	if (m_pInputGamepad != nullptr)
	{// メモリの確保が出来ていたら

		// 初期化処理
		hr = m_pInputGamepad->Init(hInstance, hWnd);
		if (FAILED(hr))
		{// 初期化処理が失敗した場合
			return E_FAIL;
		}
	}


	//**********************************
	// マウス
	//**********************************
	if (m_pInputMouse != nullptr)
	{// 確保されていたら
		return E_FAIL;
	}

	// メモリ確保
	m_pInputMouse = DEBUG_NEW CInputMouse;

	if (m_pInputMouse != nullptr)
	{// メモリの確保が出来ていたら

		// 初期化処理
		hr = m_pInputMouse->Init(hInstance, hWnd);
		if (FAILED(hr))
		{// 初期化処理が失敗した場合
			return E_FAIL;
		}
	}


	//**********************************
	// レンダラー
	//**********************************
	if (m_pRenderer != nullptr)
	{// 確保されていたら

		return E_FAIL;
	}

	// メモリ確保
	m_pRenderer = DEBUG_NEW CRenderer;

	if (m_pRenderer != nullptr)
	{// メモリの確保が出来ていたら

		// 初期化処理
		hr = m_pRenderer->Init(hWnd, TRUE);
		if (FAILED(hr))
		{// 初期化処理が失敗した場合
			return E_FAIL;
		}
	}


	// Imguiの初期化
	ImguiMgr::Init(hWnd, m_pRenderer->GetDevice());

	//**********************************
	// サウンド
	//**********************************
	if (m_pSound != nullptr)
	{// 確保されていたら
		return E_FAIL;
	}

	// メモリ確保
	m_pSound = DEBUG_NEW CSound;

	if (m_pSound != nullptr)
	{// メモリの確保が出来ていたら

		// 初期化処理
		hr = m_pSound->Init(hWnd);
		if (FAILED(hr))
		{// 初期化処理が失敗した場合
			return E_FAIL;
		}
	}


	//**********************************
	// デバッグ表示
	//**********************************
	if (m_pDebugProc != nullptr)
	{// 確保されていたら
		return E_FAIL;
	}

	// メモリ確保
	m_pDebugProc = DEBUG_NEW CDebugProc;

	if (m_pDebugProc != nullptr)
	{// メモリの確保が出来ていたら

		// 初期化処理
		hr = m_pDebugProc->Init(hInstance, hWnd);
		if (FAILED(hr))
		{// 初期化処理が失敗した場合
			return E_FAIL;
		}
	}


	//**********************************
	// ライト
	//**********************************
	if (m_pLight != nullptr)
	{// 確保されていたら
		return E_FAIL;
	}

	// メモリ確保
	m_pLight = DEBUG_NEW CLight;

	if (m_pLight != nullptr)
	{// メモリの確保が出来ていたら

		// 初期化処理
		hr = m_pLight->Init();
		if (FAILED(hr))
		{// 初期化処理が失敗した場合
			return E_FAIL;
		}
	}


	//**********************************
	// カメラ
	//**********************************
	if (m_pCamera != nullptr)
	{// 確保されていたら
		return E_FAIL;
	}

	// メモリ確保
	m_pCamera = DEBUG_NEW CCamera;

	if (m_pCamera != nullptr)
	{// メモリの確保が出来ていたら

		// 初期化処理
		hr = m_pCamera->Init();
		if (FAILED(hr))
		{// 初期化処理が失敗した場合
			return E_FAIL;
		}
	}

	//**********************************
	// エフェクシア
	//**********************************
	m_pMyEffekseer = CMyEffekseer::Create();
	if (m_pMyEffekseer == nullptr)
	{
		return E_FAIL;
	}

	//**********************************
	// テクスチャ
	//**********************************
	m_pTexture = CTexture::Create();
	if (m_pTexture == nullptr)
	{
		return E_FAIL;
	}

	//**********************************
	// Xファイル
	//**********************************
	m_pXLoad = CXLoad::Create();
	if (m_pXLoad == nullptr)
	{
		return E_FAIL;
	}

	//**********************************
	// フェード
	//**********************************
	m_pFade = CFade::Create();
	if (m_pFade == nullptr)
	{// 失敗していたら
		return E_FAIL;
	}


	// ロードフラグリセット
	m_bLoadComplete = false;
	m_bLoadFadeSet = false;	// ロードのフェード設定フラグ
	m_bNowLoading = true;

	// シーンのロードを開始
	GetLoadManager()->LoadScene(CScene::MODE_NONE);

	return S_OK;
}

//==========================================================================
// 読み込み
//==========================================================================
void CManager::Load()
{

	// 全てのテクスチャ読み込み
	m_pTexture->LoadAll();

	// 全てのモデル読み込み
	m_pXLoad->LoadAll();

	//**********************************
	// 遷移なしフェード
	//**********************************
	m_pInstantFade = CInstantFade::Create();
	if (m_pInstantFade == nullptr)
	{
		return;
	}

	//**********************************
	// 黒フレーム
	//**********************************
	m_pBlackFrame = CBlackFrame::Create();
	if (m_pBlackFrame == nullptr)
	{
		return;
	}

	//**********************************
	// ポーズ
	//**********************************
	m_pPause = CPause::Create(STARTMODE);
	if (m_pPause == nullptr)
	{
		return;
	}

	//**********************************
	// リザルトマネージャ
	//**********************************
	m_pResultManager = CResultManager::Create();
	if (m_pResultManager == nullptr)
	{
		return;
	}

	//**********************************
	// ランキングマネージャ
	//**********************************
	m_pRankingManager = CRankingManager::Create();
	if (m_pRankingManager == nullptr)
	{
		return;
	}

	// モード設定
	NoLoadSetMode(STARTMODE);
}

//==========================================================================
// 次のモード設定
//==========================================================================
void CManager::SetMode(CScene::MODE mode)
{
	GetLoadManager()->UnLoad();

	// 次のモード設定
	NoLoadSetMode(mode);

	// ロードのタイマーリセット
	m_fLoadTimer = 0.0f;

	// ロードフラグリセット
	m_bLoadComplete = false;
	m_bLoadFadeSet = false;				// ロードのフェード設定フラグ
	m_bNowLoading = true;

	// シーンのロードを開始
	GetLoadManager()->LoadScene(mode);
}

//==========================================================================
// 次のモード設定
//==========================================================================
void CManager::NoLoadSetMode(CScene::MODE mode)
{
	// リセット
	Reset(mode);

	// 生成処理
	m_pScene = CScene::Create(mode);

	// ロードのタイマーリセット
	m_fLoadTimer = 0.0f;
}

//==========================================================================
// リセット
//==========================================================================
void CManager::Reset(CScene::MODE mode)
{
	m_bHitStop = false;		// ヒットストップの判定
	m_nCntHitStop = 0;		// ヒットストップのカウンター

	// 前回のモード設定
	m_OldMode = GetMode();

	if (mode == CScene::MODE_GAME && m_pResultManager != nullptr && m_pRankingManager != nullptr)
	{// 次のモードがゲームだったら

		// スコア情報リセット
		m_pResultManager->Reset();
		m_pRankingManager->Reset();
	}

	// BGMストップ
	if (m_pSound != nullptr && mode != CScene::MODE_RANKING)
	{
		m_pSound->StopSound();
	}

	// メモリの確保がされていたら
	if (m_pScene != nullptr)
	{
		// 終了処理
		m_pScene->Uninit();
		delete m_pScene;
		m_pScene = nullptr;
	}

	// 全てのオブジェクト破棄
	CObject::ReleaseAll();

	// ポーズ状況入れ替え
	if (m_pPause != nullptr)
	{
		ChangePauseMode(mode);

		if (m_pPause->IsPause() == true)
		{// ポーズ中だったら
			m_pPause->SetPause();
		}
	}

	// 黒フレームリセット
	m_pBlackFrame->Reset();

	// カメラの情報リセット
	if (m_pCamera != nullptr)
	{
		m_pCamera->Reset(mode);
	}
}

//==========================================================================
// 現在のモード取得
//==========================================================================
CScene::MODE CManager::GetMode()
{
	// モード取得
	if (m_pScene != nullptr)
	{
		return m_pScene->GetMode();
	}
	else
	{
		return CScene::MODE_NONE;
	}
}

//==========================================================================
// 終了処理
//==========================================================================
void CManager::Uninit()
{
	// 全てのオブジェクト破棄
	CObject::ReleaseAll();

	// BGMストップ
	m_pSound->StopSound();

	// キーボードの破棄
	if (m_pInputKeyboard != nullptr)
	{// メモリの確保が出来ていたら

		// 終了処理
		m_pInputKeyboard->Uninit();

		// メモリの開放
		delete m_pInputKeyboard;
		m_pInputKeyboard = nullptr;
	}

	// ゲームパッドの破棄
	if (m_pInputGamepad != nullptr)
	{// メモリの確保が出来ていたら

		// 終了処理
		m_pInputGamepad->Uninit();

		// メモリの開放
		delete m_pInputGamepad;
		m_pInputGamepad = nullptr;
	}

	// マウスの破棄
	if (m_pInputMouse != nullptr)
	{// メモリの確保が出来ていたら

		// 終了処理
		m_pInputMouse->Uninit();

		// メモリの開放
		delete m_pInputMouse;
		m_pInputMouse = nullptr;
	}

	// レンダラーの破棄
	if (m_pRenderer != nullptr)
	{// メモリの確保が出来ていたら

		// 終了処理
		m_pRenderer->Uninit();

		// メモリの開放
		delete m_pRenderer;
		m_pRenderer = nullptr;
	}

	// Imguiの終了
	ImguiMgr::Uninit();

	// ライトの破棄
	if (m_pLight != nullptr)
	{// メモリの確保が出来ていたら

		// 終了処理
		m_pLight->Uninit();

		// メモリの開放
		delete m_pLight;
		m_pLight = nullptr;
	}

	// カメラの破棄
	if (m_pCamera != nullptr)
	{// メモリの確保が出来ていたら

		// 終了処理
		m_pCamera->Uninit();

		// メモリの開放
		delete m_pCamera;
		m_pCamera = nullptr;
	}

	if (m_pMyEffekseer != nullptr)
	{
		// 終了処理
		m_pMyEffekseer->Uninit();
		m_pMyEffekseer = nullptr;
	}

	// サウンドの破棄
	if (m_pSound != nullptr)
	{// メモリの確保が出来ていたら

		// 終了処理
		m_pSound->Uninit();

		// メモリの開放
		delete m_pSound;
		m_pSound = nullptr;
	}

	// デバッグ表示の破棄
	if (m_pDebugProc != nullptr)
	{// メモリの確保が出来ていたら

		// 終了処理
		m_pDebugProc->Uninit();

		// メモリの開放
		delete m_pDebugProc;
		m_pDebugProc = nullptr;
	}

	// エディットの破棄
	if (m_pEdit != nullptr)
	{// メモリの確保が出来ていたら

		m_pEdit = nullptr;
	}

	//**********************************
	// 全てのテクスチャ破棄
	//**********************************
	if (m_pTexture != nullptr)
	{// メモリの確保が出来ていたら

		// 終了処理
		m_pTexture->Unload();

		// メモリの開放
		delete m_pTexture;
		m_pTexture = nullptr;
	}

	if (m_pXLoad != nullptr)
	{
		// 終了処理
		m_pXLoad->Uninit();

		// メモリの開放
		m_pXLoad = nullptr;
	}

	if (m_pScene != nullptr)
	{// メモリの確保が出来ていたら

		// 終了処理
		m_pScene->Uninit();
		delete m_pScene;
		m_pScene = nullptr;
	}

	// フェードの破棄
	if (m_pFade != nullptr)
	{// メモリの確保がされていたら

		// 終了処理
		m_pFade->Uninit();
		delete m_pFade;
		m_pFade = nullptr;
	}

	// 遷移なしフェードの破棄
	if (m_pInstantFade != nullptr)
	{// メモリの確保がされていたら

		// 終了処理
		m_pInstantFade->Uninit();
		delete m_pInstantFade;
		m_pInstantFade = nullptr;
	}

	// 黒フレームの破棄
	if (m_pBlackFrame != nullptr)
	{// メモリの確保がされていたら

		// 終了処理
		m_pBlackFrame->Uninit();
		delete m_pBlackFrame;
		m_pBlackFrame = nullptr;
	}

	if (m_pPause != nullptr)
	{// メモリの確保がされていたら

		// 終了処理
		m_pPause->Uninit();
		delete m_pPause;
		m_pPause = nullptr;
	}

	if (m_pResultManager != nullptr)
	{// メモリの確保がされていたら

		// 終了処理
		m_pResultManager->Uninit();
		delete m_pResultManager;
		m_pResultManager = nullptr;
	}

	if (m_pRankingManager != nullptr)
	{// メモリの確保がされていたら

		// 終了処理
		m_pRankingManager->Uninit();
		delete m_pRankingManager;
		m_pRankingManager = nullptr;
	}

}

//==========================================================================
// 更新処理
//==========================================================================
void CManager::Update()
{
	// キーボード情報取得
	CInputKeyboard *pInputKeyboard = GetInputKeyboard();

	// 過去の時間保存
	m_OldTime = m_CurrentTime;

	// 経過時間
	m_CurrentTime = timeGetTime();
	m_fDeltaTime = (float)(m_CurrentTime - m_OldTime) / 1000;

	// Imguiの更新
	ImguiMgr::Update();

	// 初回ロード判定
	if (!m_bFirstLoad)
	{
		bool bComplete = GetLoadManager()->IsLoadComplete();

		if (bComplete &&
			!m_bLoadFadeSet)
		{
			CManager::GetInstance()->GetInstantFade()->SetFade();
			m_bLoadFadeSet = true;	// フェードのセットフラグ
		}

		// ロードマネージャの更新
		GetLoadManager()->Update();

		if (m_bLoadFadeSet)
		{// フェードが設定されてる状態

			// 遷移なしフェードの更新処理
			m_pInstantFade->Update();

			if (m_pInstantFade->GetState() == CInstantFade::STATE_FADECOMPLETION)
			{
				m_bLoadComplete = true;	// ロード完了
				m_bNowLoading = false;	// ロード中フラグオフ
				m_bFirstLoad = true;	// 初回ロード終了
				SetMode(STARTMODE);
			}
		}
		return;
	}

	if (m_bNowLoading)
	{
		// ロード中
		if (!m_bLoadComplete)
		{
			// ロード時間加算
			m_fLoadTimer += m_fDeltaTime;
		}

		bool bComplete = GetLoadManager()->IsLoadComplete();

		if (bComplete &&
			m_fLoadTimer >= TIME_LOAD &&
			!m_bLoadFadeSet)
		{// ロード完了の条件完了

			CManager::GetInstance()->GetInstantFade()->SetFade();
			m_bLoadFadeSet = true;	// フェードのセットフラグ
		}

		if (m_bLoadFadeSet)
		{// フェードが設定されてる状態
			if (m_pInstantFade->GetState() == CInstantFade::STATE_FADECOMPLETION)
			{
				m_bLoadComplete = true;	// ロード完了
				m_bNowLoading = false;	// ロード中フラグオフ
			}
		}

		// ロードマネージャの更新
		if (!m_bLoadComplete)
		{
			GetLoadManager()->Update();
		}
	}

	// フェードの更新処理
	m_pFade->Update();

	// 遷移なしフェードの更新処理
	m_pInstantFade->Update();

	if (!m_bLoadComplete)
	{
		return;
	}

	if (m_bLoadComplete)
	{
		// 黒フレーム
		if (m_pBlackFrame != nullptr)
		{
			m_pBlackFrame->Update();
		}

		// キーボードの更新処理
		m_pInputKeyboard->Update();

		// ゲームパッドの更新処理
		m_pInputGamepad->Update();

		// マウスの更新処理
		m_pInputMouse->Update();

		if ((pInputKeyboard->GetTrigger(DIK_P) == true || m_pInputGamepad->GetTrigger(CInputGamepad::BUTTON_START, 0) == true) &&
			m_pFade->GetState() == CFade::STATE_NONE &&
			(GetMode() == CScene::MODE_GAME || GetMode() == CScene::MODE::MODE_GAMETUTORIAL))
		{// フェード中じゃないとき

			// サウンド再生
			m_pSound->PlaySound(CSound::LABEL::LABEL_SE_CURSOR_END);
			m_pPause->SetPause();
		}

		// ポーズの更新処理
		if (m_pPause->IsPause() == true)
		{// ポーズ中だったら
			m_pPause->Update();

			if (!GetLoadManager()->IsLoadComplete())
			{
				return;
			}
//#if _DEBUG

			// カメラの更新処理
			m_pCamera->Update();
//#endif

			return;
		}

		if (m_bHitStop == true)
		{// ヒットストップ中は更新停止

			// ヒットストップカウンター減算
			m_nCntHitStop--;

			if (m_nCntHitStop <= 0)
			{// カウンターがなくなったら
				m_bHitStop = false;
			}
		}
		else
		{
			m_nCntHitStop = 20;
		}

#if _DEBUG
		if (pInputKeyboard->GetTrigger(DIK_F2) == true)
		{// F2でワイヤーフレーム切り替え
			m_bWireframe = m_bWireframe ? false : true;
		}

		if (pInputKeyboard->GetTrigger(DIK_F6))
		{
			MyFog::ToggleFogFrag();
		}

		if (pInputKeyboard->GetTrigger(DIK_F8) == true)
		{// F8でエディットモード切替え

			if (m_pEdit == nullptr)
			{// nullptrだったら

				// エディットの生成処理
				m_pEdit = CEdit::Create();
			}
			else
			{
				// 終了させる
				m_pEdit->Release();
				m_pEdit = nullptr;
			}
		}
#endif

		// レンダラーの更新処理
		if (m_pRenderer != nullptr)
		{
			m_pRenderer->Update();
		}

		// ライトの更新処理
		m_pLight->Update();

		// カメラの更新処理
		m_pCamera->Update();

		// デバッグ表示の更新処理
		m_pDebugProc->Update();

		if (m_pEdit == nullptr && m_pScene != nullptr)
		{// メモリの確保が出来ていたら
			m_pScene->Update();
		}
	}
}

//==========================================================================
// 描画処理
//==========================================================================
void CManager::Draw()
{
	/*if (!m_bLoadComplete)
	{
		GetLoadManager()->Draw();
		return;
	}*/

	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = m_pRenderer->GetDevice();

	if (m_bWireframe)
	{
		pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);	// 埋めるモード
	}

	// レンダラーの描画処理
	m_pRenderer->Draw();

	pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);	// 埋めるモード
}

//==========================================================================
// プレイヤーの数取得
//==========================================================================
int CManager::GetNumPlayer()
{
	return m_nNumPlayer;
}

//==========================================================================
// プレイヤーの数設定
//==========================================================================
void CManager::SetNumPlayer(int nNum)
{
	// 値の正規化処理
	UtilFunc::Transformation::ValueNormalize(nNum, mylib_const::MAX_PLAYER, 0);

	m_nNumPlayer = nNum;
}

//==========================================================================
// ポーズの種類切り替え
//==========================================================================
void CManager::ChangePauseMode(CScene::MODE mode)
{
	switch (mode)
	{
	case CScene::MODE::MODE_GAME:
	case CScene::MODE::MODE_GAMETUTORIAL:

		// 削除
		if (m_pPause!= nullptr)
		{
			m_pPause->Kill();
			delete m_pPause;
			m_pPause = nullptr;
		}

		// 再生成
		m_pPause = CPause::Create(mode);
		break;

	default:
		break;
	}
}

//==========================================================================
// 経過時間取得
//==========================================================================
float CManager::GetDeltaTime()
{
	return m_fDeltaTime;
}

//==========================================================================
// ヒットストップの設定
//==========================================================================
void CManager::SetEnableHitStop(int nCntHitStop)
{
	m_bHitStop = true;
	m_nCntHitStop = nCntHitStop;
}

//==========================================================================
// レンダラーの取得
//==========================================================================
CRenderer *CManager::GetRenderer()
{
	return m_pRenderer;
}

//==========================================================================
// キーボードの取得
//==========================================================================
CInputKeyboard *CManager::GetInputKeyboard()
{
	return m_pInputKeyboard;
}

//==========================================================================
// ゲームパッドの取得
//==========================================================================
CInputGamepad *CManager::GetInputGamepad()
{
	return m_pInputGamepad;
}

//==========================================================================
// マウスの取得
//==========================================================================
CInputMouse *CManager::GetInputMouse()
{
	return m_pInputMouse;
}

//==========================================================================
// サウンドの取得
//==========================================================================
CSound *CManager::GetSound()
{
	return m_pSound;
}

//==========================================================================
// デバッグ表示の取得
//==========================================================================
CDebugProc *CManager::GetDebugProc()
{
	return m_pDebugProc;
}

//==========================================================================
// ライトの取得
//==========================================================================
CLight *CManager::GetLight()
{
	return m_pLight;
}

//==========================================================================
// カメラの取得
//==========================================================================
CCamera *CManager::GetCamera()
{
	return m_pCamera;
}

//==========================================================================
// エディットの取得
//==========================================================================
CEdit *CManager::GetEdit()
{
	return m_pEdit;
}

//==========================================================================
// フェードの取得
//==========================================================================
CFade *CManager::GetFade()
{
	return m_pFade;
}

//==========================================================================
// 遷移なしフェードの取得
//==========================================================================
CInstantFade *CManager::GetInstantFade()
{
	return m_pInstantFade;
}

//==========================================================================
// 黒フレームの取得
//==========================================================================
CBlackFrame *CManager::GetBlackFrame()
{
	return m_pBlackFrame;
}

//==========================================================================
// ポーズの取得
//==========================================================================
CPause *CManager::GetPause()
{
	return m_pPause;
}

//==========================================================================
// リザルトマネージャ取得
//==========================================================================
CResultManager *CManager::GetResultManager()
{
	return m_pResultManager;
}

//==========================================================================
// ランキングマネージャのオブジェクト
//==========================================================================
CRankingManager *CManager::GetRankingManager()
{
	return m_pRankingManager;
}

//==========================================================================
// シーン取得
//==========================================================================
CScene *CManager::GetScene()
{
	return m_pScene;
}

