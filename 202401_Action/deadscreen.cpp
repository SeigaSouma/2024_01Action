//=============================================================================
// 
// 死亡画面処理 [deadscreen.cpp]
// Author : 相馬靜雅
// 
//=============================================================================
#include "deadscreen.h"
#include "renderer.h"
#include "texture.h"
#include "manager.h"
#include "sound.h"
#include "calculation.h"

//==========================================================================
// 定数定義
//==========================================================================
namespace
{
	const char* TEXTURE = "data\\TEXTURE\\dead\\BG_01.jpg";
	const float FADE_ALPHA = 0.4f;	// フェードの透明度
	const float TIME_FADE = static_cast<float>(40) / static_cast<float>(mylib_const::DEFAULT_FPS);	// フェードにかかる時間
}

//==========================================================================
// 関数ポインタ
//==========================================================================
CDeadScreen::STATE_FUNC CDeadScreen::m_StateFunc[] =
{
	&CDeadScreen::StateNone,	// なし
	&CDeadScreen::StateFadeIn,	// フェードイン
	&CDeadScreen::StateFadeOut,	// フェードアウト
};

//==========================================================================
// コンストラクタ
//==========================================================================
CDeadScreen::CDeadScreen(int nPriority) : CObject2D(nPriority)
{
	// 値のクリア
	m_state = STATE_NONE;	// 状態
	m_fStateTime = 0.0f;	// 状態カウンター
}

//==========================================================================
// デストラクタ
//==========================================================================
CDeadScreen::~CDeadScreen()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CDeadScreen* CDeadScreen::Create(void)
{
	// 生成用のオブジェクト
	CDeadScreen* pScreen = NULL;

	// メモリの確保
	pScreen = DEBUG_NEW CDeadScreen;

	if (pScreen != NULL)
	{
		// 初期化処理
		pScreen->Init();
	}

	return pScreen;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CDeadScreen::Init(void)
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
	SetSize(D3DXVECTOR2(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f));
	SetSizeOrigin(GetSize());

	// 位置設定
	SetPosition(MyLib::Vector3(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f, 0.0f));

	// 色設定
	SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));

	// 状態カウンター
	m_fStateTime = 0.0f;
	m_state = STATE_FADEIN;
	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CDeadScreen::Uninit(void)
{
	// 終了処理
	CObject2D::Uninit();
}

//==========================================================================
// 更新処理
//==========================================================================
void CDeadScreen::Update(void)
{
	// 状態別更新処理
	(this->*(m_StateFunc[m_state]))();

	// 更新処理
	CObject2D::Update();
}

//==========================================================================
// なにもなし
//==========================================================================
void CDeadScreen::StateNone(void)
{

}

//==========================================================================
// フェードイン
//==========================================================================
void CDeadScreen::StateFadeIn(void)
{
	// 状態遷移カウンター減算
	m_fStateTime += CManager::GetInstance()->GetDeltaTime();

	// 不透明度更新
	D3DXCOLOR col = GetColor();
	col.a = FADE_ALPHA * (m_fStateTime / TIME_FADE);
	SetColor(col);

	if (m_fStateTime >= TIME_FADE)
	{
		m_fStateTime = TIME_FADE;
		m_state = STATE_NONE;
		return;
	}
}

//==========================================================================
// フェードアウト
//==========================================================================
void CDeadScreen::StateFadeOut(void)
{
	// 状態遷移カウンター減算
	m_fStateTime -= CManager::GetInstance()->GetDeltaTime();

	// 不透明度更新
	D3DXCOLOR col = GetColor();
	col.a = FADE_ALPHA * (m_fStateTime / TIME_FADE);
	SetColor(col);

	if (m_fStateTime <= 0)
	{
		m_fStateTime = 0.0f;

		// 削除
		Uninit();
		return;
	}
}

//==========================================================================
// 描画処理
//==========================================================================
void CDeadScreen::Draw(void)
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
