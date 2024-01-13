//=============================================================================
// 
//  チュートリアル処理 [tutorial.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "tutorial.h"
#include "input.h"
#include "fade.h"
#include "renderer.h"
#include "calculation.h"
#include "debugproc.h"
#include "sound.h"
#include "camera.h"
#include "tutorial_screen.h"

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================

//==========================================================================
// コンストラクタ
//==========================================================================
CTutorial::CTutorial()
{
	// 値のクリア
}

//==========================================================================
// デストラクタ
//==========================================================================
CTutorial::~CTutorial()
{

}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CTutorial::Init(void)
{
	// 初期化処理
	if (FAILED(CScene::Init()))
	{// 失敗した場合
		return E_FAIL;
	}

	// BGM再生
	CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL_BGM_GAME);

	CTutorialScreen::Create();

	// 成功
	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CTutorial::Uninit(void)
{
	// 終了処理
	CScene::Uninit();
}

//==========================================================================
// 更新処理
//==========================================================================
void CTutorial::Update(void)
{
	// キーボード情報取得
	CInputKeyboard *pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();

	// ゲームパッド情報取得
	CInputGamepad *pInputGamepad = CManager::GetInstance()->GetInputGamepad();

	if (CManager::GetInstance()->GetFade()->GetState() != CFade::STATE_NONE)
	{// フェード中は抜ける
		return;
	}

	if (pInputKeyboard->GetTrigger(DIK_RETURN) || pInputGamepad->GetTrigger(CInputGamepad::BUTTON_A, 0) == true)
	{
		// モード設定
		CManager::GetInstance()->GetFade()->SetFade(CScene::MODE_GAME);
	}

	
	// シーンの更新処理
	CScene::Update();
}

//==========================================================================
// 描画処理
//==========================================================================
void CTutorial::Draw(void)
{
	
}
