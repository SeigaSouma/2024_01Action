//=============================================================================
// 
// 死亡マネージャト処理 [deadmanager.cpp]
// Author : 相馬靜雅
// 
//=============================================================================
#include "deadmanager.h"
#include "renderer.h"
#include "texture.h"
#include "manager.h"
#include "sound.h"
#include "calculation.h"
#include "deadscreen.h"
#include "deadtext.h"
#include "revivalpercent_text.h"
#include "player.h"

//==========================================================================
// 定数定義
//==========================================================================
namespace
{
	const float TIME_SET_SCREEN = static_cast<float>(1) / static_cast<float>(mylib_const::DEFAULT_FPS);	// スクリーンの設定時間
	const float TIME_SET_TEXT = static_cast<float>(40) / static_cast<float>(mylib_const::DEFAULT_FPS);	// テキストの設定時間
	const float TIME_SET_REVIVALPERCENT_TEXT = 2.0f;	// 復活確率文字の設定時間
	const float TIME_FADE_TEXT = static_cast<float>(80) / static_cast<float>(mylib_const::DEFAULT_FPS);	// テキストのフェードにかかる時間
	const float TIME_FADE_REVIVALPERCENT_TEXT = static_cast<float>(40) / static_cast<float>(mylib_const::DEFAULT_FPS);	// 復活確率文字のフェードにかかる時間
}
CDeadManager* CDeadManager::m_pThisPtr = nullptr;	// 自身のポインタ

//==========================================================================
// コンストラクタ
//==========================================================================
CDeadManager::CDeadManager(int nPriority) : CObject(nPriority)
{
	// 値のクリア
	m_fTime = 0.0f;				// カウンター
	m_bSetScreen = false;		// スクリーンのセットフラグ
	m_bSetText = false;			// テキストのセットフラグ
	m_bSetRevivalText = false;	// 復活確率テキストのセットフラグ
	m_bRespawnPossible = false;	// 復活できるか
	m_pScreen = nullptr;		// スクリーンのオブジェ
	m_pText = nullptr;			// 数字のオブジェクト
	m_pRespawnPercent = nullptr;	// リスポーン確率
}

//==========================================================================
// デストラクタ
//==========================================================================
CDeadManager::~CDeadManager()
{

}

//==========================================================================
// インスタンス取得
//==========================================================================
CDeadManager* CDeadManager::GetInstance()
{
	if (m_pThisPtr == nullptr)
	{
		m_pThisPtr = Create();
	}
	return m_pThisPtr;
}

//==========================================================================
// 生成処理
//==========================================================================
CDeadManager* CDeadManager::Create()
{
	if (m_pThisPtr == nullptr)
	{// まだ生成していなかったら

		// インスタンス生成
		m_pThisPtr = DEBUG_NEW CDeadManager;
	}

	// 初期化処理
	m_pThisPtr->Init();
	return m_pThisPtr;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CDeadManager::Init()
{
	// 種類の設定
	SetType(TYPE_OBJECT2D);

	m_fTime = 0.0f;	// カウンターリセット

	// リスポーン抽選
	CPlayer* pPlayer = CPlayer::GetListObj().GetData(0);
	m_bRespawnPossible = DrawingRevival(pPlayer->GetRespawnPercent());

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CDeadManager::Uninit()
{
	// 開放処理
	Release();
	m_pThisPtr = nullptr;
	m_pScreen = nullptr;			// スクリーンのオブジェ
	m_pText = nullptr;				// 数字のオブジェクト
	m_pRespawnPercent = nullptr;	// リスポーン確率
}

//==========================================================================
// 更新処理
//==========================================================================
void CDeadManager::Update()
{
	// 時間加算
	m_fTime += CManager::GetInstance()->GetDeltaTime();

	// スクリーン設定
	if (TIME_SET_SCREEN <= m_fTime &&
		!m_bSetScreen)
	{
		m_pScreen = CDeadScreen::Create(TIME_FADE_REVIVALPERCENT_TEXT);

		m_bSetScreen = true;
	}

	// テキスト設定
	if (TIME_SET_TEXT <= m_fTime &&
		!m_bSetText)
	{
		m_pText = CDeadText::Create(TIME_FADE_TEXT, TIME_FADE_REVIVALPERCENT_TEXT);
		m_bSetText = true;
	}

	// 復活確率設定
	if (TIME_SET_REVIVALPERCENT_TEXT <= m_fTime &&
		!m_bSetRevivalText)
	{
		CPlayer* pPlayer = CPlayer::GetListObj().GetData(0);
		m_pRespawnPercent = CRevivalPercentText::Create(pPlayer->GetRespawnPercent(), TIME_FADE_REVIVALPERCENT_TEXT);
		m_bSetRevivalText = true;
	}
}

//==========================================================================
// フェードアウト設定
//==========================================================================
void CDeadManager::SetFadeOut()
{
	m_pScreen->SetState(CDeadScreen::STATE_FADEOUT);		// スクリーンのオブジェ
	m_pText->SetState(CDeadText::STATE_FADEOUT);			// 数字のオブジェクト
	m_pRespawnPercent->SetState(CRevivalPercentText::STATE_FADEOUT);	// リスポーン確率
}

//==========================================================================
// 復活抽選
//==========================================================================
bool CDeadManager::DrawingRevival(int winningPercent)
{
	return UtilFunc::Transformation::Random(0, 99) < winningPercent;
}

//==========================================================================
// 描画処理
//==========================================================================
void CDeadManager::Draw()
{

}
