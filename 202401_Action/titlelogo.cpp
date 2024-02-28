//=============================================================================
// 
// タイトルロゴ処理 [titlelogo.cpp]
// Author : 相馬靜雅
// 
//=============================================================================
#include "titlelogo.h"
#include "renderer.h"
#include "texture.h"
#include "manager.h"
#include "sound.h"
#include "calculation.h"
#include "deadmanager.h"
#include "fade.h"

//==========================================================================
// 定数定義
//==========================================================================
namespace
{
	const char* TEXTURE = "data\\TEXTURE\\title\\titlelogo3.png";
}

//==========================================================================
// 関数ポインタ
//==========================================================================
CTitleLogo::STATE_FUNC CTitleLogo::m_StateFunc[] =
{
	&CTitleLogo::StateNone,	// なし
	&CTitleLogo::StateFadeIn,	// フェードイン
};

//==========================================================================
// コンストラクタ
//==========================================================================
CTitleLogo::CTitleLogo(float fadetime, int nPriority) : m_fFadeOutTime(fadetime), CObject2D(nPriority)
{
	// 値のクリア
	m_state = STATE_NONE;	// 状態
	m_fStateTime = 0.0f;	// 状態カウンター
}

//==========================================================================
// デストラクタ
//==========================================================================
CTitleLogo::~CTitleLogo()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CTitleLogo* CTitleLogo::Create(float fadetime)
{
	// 生成用のオブジェクト
	CTitleLogo* pScreen = nullptr;

	// メモリの確保
	pScreen = DEBUG_NEW CTitleLogo(fadetime);

	if (pScreen != nullptr)
	{
		// 初期化処理
		pScreen->Init();
	}

	return pScreen;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CTitleLogo::Init()
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

	D3DXVECTOR2 size = CTexture::GetInstance()->GetImageSize(nIdx);
	size = UtilFunc::Transformation::AdjustSizeByWidth(size, 400.0f);

	// サイズ設定
	SetSize(size);
	SetSizeOrigin(GetSize());

	// 位置設定
	SetPosition(MyLib::Vector3(SCREEN_WIDTH * 0.5f, 240.0f, 0.0f));

	// 色設定
	SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));

	// 状態カウンター
	m_fStateTime = m_fFadeOutTime;
	m_state = STATE_FADEIN;

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CTitleLogo::Uninit()
{
	// 終了処理
	CObject2D::Uninit();
}

//==========================================================================
// 更新処理
//==========================================================================
void CTitleLogo::Update()
{
	if (CManager::GetInstance()->GetFade()->GetState() != CFade::STATE_NONE)
	{// フェード中は抜ける
		return;
	}

	// 状態別更新処理
	(this->*(m_StateFunc[m_state]))();
	if (IsDeath())
	{
		return;
	}

	// 更新処理
	CObject2D::Update();
}

//==========================================================================
// なにもなし
//==========================================================================
void CTitleLogo::StateNone()
{

}

//==========================================================================
// フェードイン
//==========================================================================
void CTitleLogo::StateFadeIn()
{
	// 状態遷移カウンター減算
	m_fStateTime -= CManager::GetInstance()->GetDeltaTime();

	// 不透明度更新
	float alpha = 1.0f - (m_fStateTime / m_fFadeOutTime);
	SetAlpha(alpha);

	if (m_fStateTime <= 0.0f)
	{
		m_fStateTime = 0.0f;
		m_state = STATE_NONE;

		// 不透明度更新
		SetAlpha(1.0f);
		return;
	}
}

//==========================================================================
// 描画処理
//==========================================================================
void CTitleLogo::Draw()
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
