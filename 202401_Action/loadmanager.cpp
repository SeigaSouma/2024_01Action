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

//==========================================================================
// コンストラクタ
//==========================================================================
CLoadManager::CLoadManager()
{
	m_LoadingThread;
	m_pLoadScreen = nullptr;
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
	// ロックして安全にリセット処理を行う
	//std::lock_guard<std::mutex> lock(isLoadedMutex);

	// ResetInternalLoad 関数を呼び出してロードをリセット
	ResetInternalLoad();

}

void CLoadManager::ResetInternalLoad()
{
	// ロックして安全にリセット処理を行う
	std::lock_guard<std::mutex> lock(isLoadedMutex);

	// ここで必要なリソースの解放やスレッドの終了処理などを行います
	// 以下は例として、スレッドの終了処理として join() を使っていますが、
	// 実際のプロジェクトではスレッドプールの管理やリソースの解放処理を追加してください。

	// スレッドが動作中なら終了を待つ
	if (m_LoadingThread.joinable())
	{
		m_LoadingThread.join();
	}

	// ここでリソースの解放などを行う
	// ...

	// ロードが完了していないことを示すフラグをリセット
	isLoadComplete = false;
}

//==========================================================================
// シーンのロードを開始
//==========================================================================
void CLoadManager::LoadScene(void)
{
	if (m_pLoadScreen == nullptr)
	{
		m_pLoadScreen = CLoadScreen::Create();
	}

	// ResetLoad 関数を呼び出して新しいシーンのロードを準備
	ResetLoad();

	// ロードが再び始まるのでフラグをリセット
	isLoadComplete = false;

	// ロード画面を表示
	//std::cout << "Loading..." << std::endl;

	if (m_LoadingThread.joinable())
	{
		m_LoadingThread.join();
	}

	auto count = std::thread::hardware_concurrency();
	if (count < 3)
	{

	}

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
		std::lock_guard<std::mutex> lock(isLoadedMutex);
		isLoadComplete = false;
	}

	try
	{
		Load();
	}
	catch (const std::exception& e)
	{
		// 例外が発生した場合はここでエラーログを出力などの適切な処理を行う
		std::cerr << "LoadInBackground: Exception caught: " << e.what() << std::endl;
	}

	// スレッドを強制終了する例（Windows用）
	TerminateThread(m_LoadingThread.native_handle(), 0);

	// ロードが完了したらフラグをセット
	{
		std::lock_guard<std::mutex> lock(isLoadedMutex);
		isLoadComplete = true;
	}
}

//==========================================================================
// ロード処理
//==========================================================================
void CLoadManager::Load()
{
	// シーンの初期化処理
	CManager::GetInstance()->GetScene()->Init();


	// ロードが完了したらフラグをセット
	isLoadComplete = true;
}

//==========================================================================
// 描画処理
//==========================================================================
void CLoadManager::Draw(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();

	// ロードが完了していない場合はデバイスアクセスをロック
	if (!isLoadComplete)
	{
		// ロックを使ってスレッドセーフにデバイスにアクセス
		std::lock_guard<std::mutex> lock(isLoadedMutex);

		// 画面クリア(バックバッファとZバッファのクリア)
		pDevice->Clear(0, NULL, (D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER), D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f), 1.0f, 0);

		// 描画開始
		if (SUCCEEDED(pDevice->BeginScene()))
		{
			m_pLoadScreen->Update();
			m_pLoadScreen->Draw();

			// 描画終了
			pDevice->EndScene();
		}

		// バックバッファとフロントバッファの入れ替え
		pDevice->Present(NULL, NULL, NULL, NULL);
	}

}

