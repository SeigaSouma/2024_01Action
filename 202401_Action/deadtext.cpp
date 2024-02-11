//=============================================================================
// 
// 死亡テキスト処理 [deadtext.cpp]
// Author : 相馬靜雅
// 
//=============================================================================
#include "deadtext.h"
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
	const char* TEXTURE = "data\\TEXTURE\\dead\\deadtext_01.png";
}

//==========================================================================
// 関数ポインタ
//==========================================================================
CDeadText::STATE_FUNC CDeadText::m_StateFunc[] =
{
	&CDeadText::StateNone,		// なし
	&CDeadText::StateFadeIn,	// フェードイン
	&CDeadText::StateFadeOut,	// フェードアウト
};

//==========================================================================
// コンストラクタ
//==========================================================================
CDeadText::CDeadText(float fadein, float fadeout, int nPriority) : m_fFadeInTime(fadein), m_fFadeOutTime(fadeout), CObject2D(nPriority)
{
	// 値のクリア
	m_state = STATE_NONE;	// 状態
	m_fStateTime = 0.0f;	// 状態カウンター
}

//==========================================================================
// デストラクタ
//==========================================================================
CDeadText::~CDeadText()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CDeadText* CDeadText::Create(const float fadein, const float fadeout)
{
	// 生成用のオブジェクト
	CDeadText* pScreen = NULL;

	// メモリの確保
	pScreen = DEBUG_NEW CDeadText(fadein, fadeout);

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
HRESULT CDeadText::Init()
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
void CDeadText::Uninit()
{
	// 終了処理
	CObject2D::Uninit();
}

//==========================================================================
// 更新処理
//==========================================================================
void CDeadText::Update()
{
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
void CDeadText::StateNone()
{

}

//==========================================================================
// フェードイン
//==========================================================================
void CDeadText::StateFadeIn()
{
	// 状態遷移カウンター減算
	m_fStateTime += CManager::GetInstance()->GetDeltaTime();

	// 不透明度更新
	D3DXCOLOR col = GetColor();
	col.a = m_fStateTime / m_fFadeInTime;
	SetColor(col);

	if (m_fStateTime >= m_fFadeInTime)
	{
		m_fStateTime = m_fFadeOutTime;
		m_state = STATE_NONE;
		return;
	}
}

//==========================================================================
// フェードアウト
//==========================================================================
void CDeadText::StateFadeOut()
{
	// 状態遷移カウンター減算
	m_fStateTime -= CManager::GetInstance()->GetDeltaTime();

	// 不透明度更新
	D3DXCOLOR col = GetColor();
	col.a = m_fStateTime / m_fFadeOutTime;
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
void CDeadText::Draw()
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
