//=============================================================================
// 
//  ロードマネージャ [loadmanager.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "loadmanager.h"
#include "manager.h"
#include "renderer.h"
#include "scene.h"
#include "loadscreen.h"
#include "fade.h"
#include "instantfade.h"

//==========================================================================
// コンストラクタ
//==========================================================================
CLoadManager::CLoadManager()
{
	m_LoadingThread;
	m_pLoadScreen = nullptr;
	isLoadComplete = false;
	m_ModeNext = CScene::MODE_NONE;
}

//==========================================================================
// デストラクタ
//==========================================================================
CLoadManager::~CLoadManager()
{
	// ロードスレッドがまだ生きているなら待機
	if (m_LoadingThread.joinable()) {
		m_LoadingThread.join();
	}
}

//==========================================================================
// 生成処理
//==========================================================================
CLoadManager* CLoadManager::Create(void)
{
	// 生成用のオブジェクト
	CLoadManager* pLoadManager = NULL;

	// メモリの確保
	pLoadManager = DEBUG_NEW CLoadManager;

	if (pLoadManager != nullptr)
	{// メモリの確保が出来ていたら

		// 初期化処理
		pLoadManager->Init();
	}

	return pLoadManager;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CLoadManager::Init(void)
{
	// ロードスレッドがまだ生きているなら待機
	if (m_LoadingThread.joinable()) {
		m_LoadingThread.join();
	}
	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CLoadManager::Uninit(void)
{
	
}

//==========================================================================
// 更新処理
//==========================================================================
void CLoadManager::Update(void)
{
	if (m_pLoadScreen != nullptr)
	{
		m_pLoadScreen->Update();
	}

}

void CLoadManager::UnLoad(void)
{
	if (m_LoadingThread.joinable())
	{
		m_LoadingThread.join();
	}
}

void CLoadManager::ResetLoad()
{
	// ResetInternalLoad 関数を呼び出してロードをリセット
	ResetInternalLoad();

}

void CLoadManager::ResetInternalLoad()
{
	// ロックして安全にリセット処理を行う
	//std::lock_guard<std::mutex> lock(isLoadedMutex);

	// スレッドが動作中なら終了を待つ
	if (m_LoadingThread.joinable())
	{
		m_LoadingThread.join();
	}

	// ロードが完了していないことを示すフラグをリセット
	isLoadComplete = false;
}

//==========================================================================
// シーンのロードを開始
//==========================================================================
void CLoadManager::LoadScene(CScene::MODE mode)
{
	m_ModeNext = mode;

	if (m_pLoadScreen == nullptr)
	{
		m_pLoadScreen = CLoadScreen::Create();
	}

	if (m_LoadingThread.joinable())
	{
		// デタッチする前にスレッドが完了するまで待機
		m_LoadingThread.join();
	}

	// ResetLoad 関数を呼び出して新しいシーンのロードを準備
	ResetLoad();

	// ロードが再び始まるのでフラグをリセット
	{
		//std::lock_guard<std::mutex> lock(isLoadedMutex);
		//isLoadComplete = false;
	}
	isLoadComplete = false;

    // ロード処理の開始
	m_LoadingThread = std::thread(&CLoadManager::LoadInBackground, this);

	// サブスレッドをデタッチ
	m_LoadingThread.detach();
}

//==========================================================================
// ロード画面を非同期で表示
//==========================================================================
void CLoadManager::LoadInBackground(void)
{
	// ロードが再び始まるのでフラグをリセット
	{
		//std::lock_guard<std::mutex> lock(isLoadedMutex);
		//isLoadComplete = false;
	}
	isLoadComplete = false;

	try
	{
		// 読み込み処理
		Load();
	}
	catch (const std::exception& e)
	{// 例外
		return;
	}

	if (m_LoadingThread.joinable())
	{
		m_LoadingThread.join();
	}

	// ロードが完了したらフラグをセット
	{
		//std::lock_guard<std::mutex> lock(isLoadedMutex);
		//isLoadComplete = true;
	}
	isLoadComplete = true;
}

//==========================================================================
// ロード処理
//==========================================================================
void CLoadManager::Load()
{
	
	// シーンの初期化処理
	CManager::GetInstance()->GetScene()->Init();
}

//==========================================================================
// 描画処理
//==========================================================================
void CLoadManager::Draw(void)
{
	if (m_pLoadScreen != nullptr)
	{
		m_pLoadScreen->Draw();
	}
}

//==========================================================================
// ロードが完了したかどうかを返す
//==========================================================================
bool CLoadManager::IsLoadComplete()
{
	//std::lock_guard<std::mutex>  lock(isLoadedMutex);
	return isLoadComplete;
}