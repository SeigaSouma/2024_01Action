//=============================================================================
// 
// チュートリアルをするか確認処理 [tutorial_checkshould.cpp]
// Author : 相馬靜雅
// 
//=============================================================================
#include "tutorial_checkshould.h"
#include "manager.h"
#include "sound.h"
#include "input.h"
#include "calculation.h"
#include "fade.h"

//==========================================================================
// マクロ定義
//==========================================================================
namespace
{
	const char* TEXTURE = "data\\TEXTURE\\shouldtutorial\\window.png";
	const char* SELECT_TEXTURE[] =
	{
		"data\\TEXTURE\\shouldtutorial\\select_no.png",
		"data\\TEXTURE\\shouldtutorial\\select_yes.png",
		"data\\TEXTURE\\shouldtutorial\\select_no2.png",
		"data\\TEXTURE\\shouldtutorial\\select_yes2.png",
	};
	const float DISTANCE = 140.0f;
}
std::vector<int> CTutorialCheckShould::m_nTexIdx = {};		// テクスチャのインデックス番号

//==========================================================================
// コンストラクタ
//==========================================================================
CTutorialCheckShould::CTutorialCheckShould(int nPriority) : CObject2D(nPriority)
{
	// 値のクリア
	m_nSelect = 0;	// 選択肢
	m_fFlashTime = 0.0f;	// 点滅時間
	memset(m_pSelectObj, NULL, sizeof(m_pSelectObj));	// 選択肢のオブジェクト
}

//==========================================================================
// デストラクタ
//==========================================================================
CTutorialCheckShould::~CTutorialCheckShould()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CTutorialCheckShould* CTutorialCheckShould::Create()
{
	// メモリの確保
	CTutorialCheckShould* pReset = DEBUG_NEW CTutorialCheckShould;

	if (pReset != nullptr)
	{
		// 初期化処理
		if (FAILED(pReset->Init()))
		{
			return nullptr;
		}
	}

	return pReset;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CTutorialCheckShould::Init()
{
	// 初期化処理
	HRESULT hr = CObject2D::Init();
	if (FAILED(hr))
	{// 失敗したとき
		return E_FAIL;
	}

	// 種類の設定
	SetType(TYPE_OBJECT2D);

	if (m_nTexIdx.empty())
	{
		for (const auto& tex : SELECT_TEXTURE)
		{
			// テクスチャ読み込み
			int nIdx = CTexture::GetInstance()->Regist(tex);
			m_nTexIdx.push_back(nIdx);
		}
	}

	// テクスチャの割り当て
	int nIdx = CTexture::GetInstance()->Regist(TEXTURE);
	BindTexture(nIdx);

	// サイズ設定
	D3DXVECTOR2 size = CTexture::GetInstance()->GetImageSize(nIdx);
	size = UtilFunc::Transformation::AdjustSizeByHeight(size, 150.0f);
	SetSize(size);
	SetSizeOrigin(GetSize());

	// 位置設定
	SetPosition(MyLib::Vector3(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f, 0.0f));

	// 選択肢生成
	CreateSelect();
	CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL::LABEL_SE_CURSOR_END);

	return S_OK;
}

//==========================================================================
// 選択肢生成
//==========================================================================
void CTutorialCheckShould::CreateSelect()
{
	for (int i = 0; i < SELECT_MAX; i++)
	{
		m_pSelectObj[i] = CObject2D::Create(GetPriority());

		// 種類の設定
		m_pSelectObj[i]->SetType(TYPE_OBJECT2D);

		// テクスチャ割り当て
		m_pSelectObj[i]->BindTexture(m_nTexIdx[i]);

		// サイズ設定
		D3DXVECTOR2 size = CTexture::GetInstance()->GetImageSize(m_nTexIdx[i]);
		size = UtilFunc::Transformation::AdjustSizeByHeight(size, 32.0f);
		m_pSelectObj[i]->SetSize(size);
		m_pSelectObj[i]->SetSizeOrigin(GetSize());

		// 位置設定
		m_pSelectObj[i]->SetPosition(MyLib::Vector3((SCREEN_WIDTH * 0.5f) - DISTANCE + (DISTANCE * i) * 2.0f, 410.0f, 0.0f));
	}

	int selectIdx = static_cast<int>(SELECT_NO);
	m_pSelectObj[selectIdx]->BindTexture(m_nTexIdx[selectIdx] + 2);

}

//==========================================================================
// 終了処理
//==========================================================================
void CTutorialCheckShould::Uninit()
{
	// インデックス番号リセット
	m_nTexIdx.clear();

	for (int i = 0; i < SELECT_MAX; i++)
	{
		if (m_pSelectObj[i] == nullptr)
		{
			continue;
		}
		m_pSelectObj[i] = nullptr;
		m_pSelectObj[i] = nullptr;
	}

	// 終了処理
	CObject2D::Uninit();
}

//==========================================================================
// 削除処理
//==========================================================================
void CTutorialCheckShould::Kill()
{
	for (int i = 0; i < SELECT_MAX; i++)
	{
		if (m_pSelectObj[i] == nullptr)
		{
			continue;
		}
		m_pSelectObj[i]->Uninit();
		m_pSelectObj[i] = nullptr;
	}

	// 終了処理
	CObject2D::Uninit();
}

//==========================================================================
// 更新処理
//==========================================================================
void CTutorialCheckShould::Update()
{

	m_fFlashTime += CManager::GetInstance()->GetDeltaTime();	// 点滅時間

	// 選択肢更新
	UpdateSelect();
	if (IsDeath())
	{
		return;
	}

	for (int i = 0; i < SELECT_MAX; i++)
	{
		D3DXCOLOR col = m_pSelectObj[i]->GetColor();
		if (m_nSelect == i)
		{
			col = UtilFunc::Transformation::HSVtoRGB(0.0f, 0.0f, 0.7f + fabsf(sinf(D3DX_PI * (m_fFlashTime / 1.0f)) * 0.3f));
			m_pSelectObj[i]->BindTexture(m_nTexIdx[i] + 2);
		}
		else
		{
			// 黒っぽくする
			col = D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.0f);
			m_pSelectObj[i]->BindTexture(m_nTexIdx[i]);
		}
		m_pSelectObj[i]->SetColor(col);
	}

	// 更新処理
	CObject2D::Update();
}

//==========================================================================
// 選択肢更新
//==========================================================================
void CTutorialCheckShould::UpdateSelect()
{
	// キーボード情報取得
	CInputKeyboard* pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();

	// ゲームパッド情報取得
	CInputGamepad* pInputGamepad = CManager::GetInstance()->GetInputGamepad();

	if (pInputKeyboard->GetTrigger(DIK_D) ||
		pInputGamepad->GetTrigger(CInputGamepad::BUTTON_RIGHT, 0) ||
		(pInputGamepad->GetLStickTrigger(CInputGamepad::STICK_X) && pInputGamepad->GetStickMoveL(0).x > 0))
	{// 右

		// パターンNo.を更新
		m_nSelect = (m_nSelect + (SELECT_MAX - 1)) % SELECT_MAX;

		// サウンド再生
		CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL::LABEL_SE_CURSOR_MOVE);
	}
	else if (pInputKeyboard->GetTrigger(DIK_A) ||
		pInputGamepad->GetTrigger(CInputGamepad::BUTTON_LEFT, 0) ||
		(pInputGamepad->GetLStickTrigger(CInputGamepad::STICK_X) && pInputGamepad->GetStickMoveL(0).x < 0))
	{// 左

		// パターンNo.を更新
		m_nSelect = (m_nSelect + 1) % SELECT_MAX;

		// サウンド再生
		CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL::LABEL_SE_CURSOR_MOVE);
	}

	// 決定
	if (pInputGamepad->GetTrigger(CInputGamepad::BUTTON_A, 0) ||
		pInputKeyboard->GetTrigger(DIK_RETURN))
	{
		switch (m_nSelect)
		{
		case SELECT_NO:
			
			CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL::LABEL_SE_CURSOR_END);

			// モード設定
			CManager::GetInstance()->GetFade()->SetFade(CScene::MODE_GAME);
			break;

		case SELECT_OK:

			// モード設定
			CManager::GetInstance()->GetFade()->SetFade(CScene::MODE_GAMETUTORIAL);
			break;
		}
	}

	// 戻る
	if (pInputGamepad->GetTrigger(CInputGamepad::BUTTON_B, 0) ||
		pInputKeyboard->GetTrigger(DIK_BACK))
	{
		CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL::LABEL_SE_CURSOR_END);
		return;
	}
}

//==========================================================================
// 描画処理
//==========================================================================
void CTutorialCheckShould::Draw()
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();

	// アルファテストを有効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);

	// 描画処理
	CObject2D::Draw();

	// アルファテストを有効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);
}
