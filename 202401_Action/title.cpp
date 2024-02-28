//=============================================================================
// 
//  タイトル処理 [title.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "title.h"
#include "input.h"
#include "fade.h"
#include "renderer.h"
#include "calculation.h"
#include "debugproc.h"
#include "sound.h"
#include "particle.h"
#include "gallery.h"
#include "torch.h"
#include "MyEffekseer.h"
#include "titlelogo.h"
#include "title_pressenter.h"

//==========================================================================
// 定数定義
//==========================================================================
namespace
{
	const float TIME_FADELOGO = 0.6f;	// ロゴのフェードアウト時間
	const char* TEXTURE = "data\\TEXTURE\\title\\titlelogo3.png";
}

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================
CTitle* CTitle::m_pThisPtr = nullptr;	// 自身のポインタ

//==========================================================================
// 関数ポインタ
//==========================================================================
CTitle::SCENE_FUNC CTitle::m_SceneFunc[] =
{
	&CTitle::SceneNone,			// なにもなし
	&CTitle::SceneFadeInLogo,	// ロゴフェードイン
	&CTitle::SceneFadeOutLoGo,	// ロゴフェードアウト
};

//==========================================================================
// コンストラクタ
//==========================================================================
CTitle::CTitle()
{
	// 値のクリア
	m_SceneType = SCENETYPE::SCENETYPE_NONE;	// シーンの種類
	m_fSceneTime = 0.0f;						// シーンカウンター
	m_pLogo = nullptr;		// ロゴのポインタ
	m_pPressEnter = nullptr;	// プレスエンター
}

//==========================================================================
// デストラクタ
//==========================================================================
CTitle::~CTitle()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CTitle* CTitle::Create()
{
	if (m_pThisPtr == nullptr)
	{// まだ生成していなかったら

		// インスタンス生成
		m_pThisPtr = DEBUG_NEW CTitle;
	}

	return m_pThisPtr;
}

//==========================================================================
// インスタンス取得
//==========================================================================
CTitle* CTitle::GetInstance()
{
	return m_pThisPtr;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CTitle::Init()
{

	// BGM再生
	CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL_BGM_TITLE);

	// 初期化処理
	if (FAILED(CScene::Init()))
	{// 失敗した場合
		return E_FAIL;
	}


	// 観衆設定
	CGallery::SetGallery();

	// 観衆のリスト取得
	CListManager<CGallery> galleryList = CGallery::GetList();
	CGallery* pGallery = nullptr;
	while (galleryList.ListLoop(&pGallery))
	{
		pGallery->SetState(CGallery::STATE_CLEARHEAT);
	}

	// 松明設定
	CTorch::SetTorch();

	// タイトル画面
	//CTitleScreen::Create();

	// タイトルロゴ生成
	m_pLogo = CTitleLogo::Create(1.0f);

	// プレスエンター
	m_pPressEnter = CTitle_PressEnter::Create(1.0f);

	// 塵
	CMyEffekseer::GetInstance()->SetEffect(
		CMyEffekseer::EFKLABEL::EFKLABEL_TITLEBLUR,
		MyLib::Vector3(185.0f, 65.0f, -148.0f),
		0.0f, 0.0f, 10.0f, false);

	// シーンの種類
	m_SceneType = SCENETYPE::SCENETYPE_NONE;

	// 成功
	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CTitle::Uninit()
{
	m_pThisPtr = nullptr;

	// 終了処理
	CScene::Uninit();
}

//==========================================================================
// 更新処理
//==========================================================================
void CTitle::Update()
{
	CManager::GetInstance()->GetDebugProc()->Print(
		"現在のモード：【タイトル】\n"
		"切り替え：【 F 】\n\n");

	// 入力情報取得
	CInputKeyboard* pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();
	CInputGamepad* pInputGamepad = CManager::GetInstance()->GetInputGamepad();

	if (CManager::GetInstance()->GetFade()->GetState() != CFade::STATE_NONE)
	{// フェード中は抜ける
		return;
	}

	// 状態別更新処理
	(this->*(m_SceneFunc[m_SceneType]))();
}

//==========================================================================
// なにもなし
//==========================================================================
void CTitle::SceneNone()
{
	// シーンカウンター
	m_fSceneTime = TIME_FADELOGO;
}

//==========================================================================
// ロゴフェードイン
//==========================================================================
void CTitle::SceneFadeInLogo()
{

}

//==========================================================================
// ロゴフェードアウト
//==========================================================================
void CTitle::SceneFadeOutLoGo()
{
	// シーンカウンター減算
	m_fSceneTime -= CManager::GetInstance()->GetDeltaTime();

	// 不透明度更新
	float alpha = m_fSceneTime / TIME_FADELOGO;
	m_pLogo->SetAlpha(alpha);

	// エンターの色
	m_pPressEnter->SetAlpha(alpha);

	if (m_fSceneTime <= 0.0f)
	{
		m_fSceneTime = 0.0f;
		m_SceneType = SCENETYPE_NONE;

		// 不透明度更新
		m_pLogo->SetAlpha(1.0f);
		m_pLogo->Uninit();
		m_pLogo = nullptr;

		// エンターの色
		m_pPressEnter->SetAlpha(1.0f);
		m_pPressEnter->Uninit();
		m_pPressEnter = nullptr;
		return;
	}
}

//==========================================================================
// 描画処理
//==========================================================================
void CTitle::Draw()
{

}
