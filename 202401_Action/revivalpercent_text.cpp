//=============================================================================
// 
// 復活確率テキスト処理 [revivalpercent_text.cpp]
// Author : 相馬靜雅
// 
//=============================================================================
#include "revivalpercent_text.h"
#include "revivaltime_gauge.h"
#include "renderer.h"
#include "texture.h"
#include "manager.h"
#include "calculation.h"
#include "multinumber.h"

//==========================================================================
// 定数定義
//==========================================================================
namespace
{
	const char* TEXTURE = "data\\TEXTURE\\dead\\respawn_01.png";
	const char* NUMBER_TEXTURE = "data\\TEXTURE\\number\\number_blackclover_01.png";
}

//==========================================================================
// 関数ポインタ
//==========================================================================
CRevivalPercentText::STATE_FUNC CRevivalPercentText::m_StateFunc[] =
{
	&CRevivalPercentText::StateNone,	// なし
	&CRevivalPercentText::StateFadeIn,	// フェードイン
	&CRevivalPercentText::StateFadeOut,	// フェードアウト
};

//==========================================================================
// コンストラクタ
//==========================================================================
CRevivalPercentText::CRevivalPercentText(float fadetime, int nPriority) : m_fFadeTime(fadetime), CObject2D(nPriority)
{
	// 値のクリア
	m_state = STATE_NONE;	// 状態
	m_fStateTime = 0.0f;	// 状態カウンター
}

//==========================================================================
// デストラクタ
//==========================================================================
CRevivalPercentText::~CRevivalPercentText()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CRevivalPercentText* CRevivalPercentText::Create(int respawnpercent, const float fadetime)
{
	// 生成用のオブジェクト
	CRevivalPercentText* pScreen = NULL;

	// メモリの確保
	pScreen = DEBUG_NEW CRevivalPercentText(fadetime);

	if (pScreen != NULL)
	{
		// 初期化処理
		pScreen->Init();
		pScreen->SetNumberObj(respawnpercent);
	}

	return pScreen;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CRevivalPercentText::Init(void)
{
	// 初期化処理
	HRESULT hr = CObject2D::Init();
	if (FAILED(hr))
	{// 失敗したとき
		return E_FAIL;
	}

	// 種類の設定
	SetType(TYPE_OBJECT2D);

	// テクスチャの割り当て
	int nIdx = CTexture::GetInstance()->Regist(TEXTURE);
	BindTexture(nIdx);

	// サイズ設定
	D3DXVECTOR2 setsize = CTexture::GetInstance()->GetImageSize(nIdx);
	setsize = UtilFunc::Transformation::AdjustSizeByWidth(setsize, 320.0f);
	SetSize(setsize);
	SetSizeOrigin(GetSize());

	// 位置設定
	SetPosition(MyLib::Vector3(SCREEN_WIDTH * 0.5f, 500.0f, 0.0f));

	// 色設定
	SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));

	// 状態カウンター
	m_fStateTime = 0.0f;
	m_state = STATE_FADEIN;

	// ゲージ生成
	MyLib::Vector3 spawnpos = GetPosition();
	spawnpos.y += 100.0f;
	CRevivalTimeGauge::Create(m_fFadeTime, spawnpos);
	return S_OK;
}

//==========================================================================
// 数字オブジェクト設定
//==========================================================================
void CRevivalPercentText::SetNumberObj(int number)
{
	MyLib::Vector3 spawnpos = GetPosition();
	spawnpos.x += 100.0f;
	m_apNumber = CMultiNumber::Create(spawnpos, D3DXVECTOR2(35.0f, 35.0f), 3, CNumber::OBJECTTYPE_2D, NUMBER_TEXTURE, true);	// 数字のオブジェクト
	m_apNumber->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));
	m_apNumber->SetValue(number);
}

//==========================================================================
// 終了処理
//==========================================================================
void CRevivalPercentText::Uninit(void)
{
	// 終了処理
	CObject2D::Uninit();

	if (m_apNumber != nullptr)
	{
		m_apNumber->Uninit();
		m_apNumber = nullptr;
	}
}

//==========================================================================
// 削除処理
//==========================================================================
void CRevivalPercentText::Kill(void)
{
	// 終了処理
	CObject2D::Uninit();

	if (m_apNumber != nullptr)
	{
		m_apNumber->Release();
		m_apNumber = nullptr;
	}
}

//==========================================================================
// 更新処理
//==========================================================================
void CRevivalPercentText::Update(void)
{
	// 状態別更新処理
	(this->*(m_StateFunc[m_state]))();
	if (IsDeath())
	{
		return;
	}

	// 更新処理
	CObject2D::Update();

	// 数字の更新処理
	m_apNumber->Update();
}

//==========================================================================
// なにもなし
//==========================================================================
void CRevivalPercentText::StateNone(void)
{

}

//==========================================================================
// フェードイン
//==========================================================================
void CRevivalPercentText::StateFadeIn(void)
{
	// 状態遷移カウンター減算
	m_fStateTime += CManager::GetInstance()->GetDeltaTime();

	// 不透明度更新
	D3DXCOLOR col = GetColor();
	col.a = m_fStateTime / m_fFadeTime;
	SetColor(col);

	m_apNumber->SetColor(col);

	if (m_fStateTime >= m_fFadeTime)
	{
		m_fStateTime = m_fFadeTime;
		m_state = STATE_NONE;
		return;
	}
}

//==========================================================================
// フェードアウト
//==========================================================================
void CRevivalPercentText::StateFadeOut(void)
{
	// 状態遷移カウンター減算
	m_fStateTime -= CManager::GetInstance()->GetDeltaTime();

	// 不透明度更新
	D3DXCOLOR col = GetColor();
	col.a = m_fStateTime / m_fFadeTime;
	SetColor(col);

	m_apNumber->SetColor(col);

	if (m_fStateTime <= 0)
	{
		m_fStateTime = 0.0f;

		// 削除
		Kill();
		return;
	}
}

//==========================================================================
// 描画処理
//==========================================================================
void CRevivalPercentText::Draw(void)
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
