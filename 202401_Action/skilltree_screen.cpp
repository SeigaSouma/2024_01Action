//=============================================================================
// 
// スキルツリー画面ト処理 [skilltree_screen.cpp]
// Author : 相馬靜雅
// 
//=============================================================================
#include "skilltree_screen.h"
#include "renderer.h"
#include "texture.h"
#include "manager.h"
#include "sound.h"
#include "calculation.h"

//==========================================================================
// マクロ定義
//==========================================================================
namespace
{
	const char* TEXTURE = "data\\TEXTURE\\stageclear_01.png";
	const float TIME_FADE = 1.0f;		// フェードアウト時間
}

//==========================================================================
// 関数ポインタ
//==========================================================================
CSkillTree_Screen::STATE_FUNC CSkillTree_Screen::m_StateFuncList[] =
{
	&CSkillTree_Screen::StateNone,		// なにもなし
	&CSkillTree_Screen::StateFadeIn,	// フェードイン
	&CSkillTree_Screen::StateFadeOut,	// フェードアウト
};

//==========================================================================
// コンストラクタ
//==========================================================================
CSkillTree_Screen::CSkillTree_Screen(int nPriority) : CObject2D(nPriority)
{
	// 値のクリア
	m_fStateTime = 0;		// 状態遷移カウンター
	m_state = STATE_NONE;	// 状態
}

//==========================================================================
// デストラクタ
//==========================================================================
CSkillTree_Screen::~CSkillTree_Screen()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CSkillTree_Screen* CSkillTree_Screen::Create(void)
{
	// 生成用のオブジェクト
	CSkillTree_Screen* pEffect = NULL;

	// メモリの確保
	pEffect = DEBUG_NEW CSkillTree_Screen;

	if (pEffect != NULL)
	{
		// 初期化処理
		pEffect->Init();
	}

	return pEffect;
}

//==================================================================================
// 初期化処理
//==================================================================================
HRESULT CSkillTree_Screen::Init(void)
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

	// 状態カウンター
	m_fStateTime = TIME_FADE;
	m_state = STATE_FADEIN;

	// 位置設定
	SetPosition(MyLib::Vector3(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f, 0.0f));

	return S_OK;
}

//==================================================================================
// 終了処理
//==================================================================================
void CSkillTree_Screen::Uninit(void)
{
	// 終了処理
	CObject2D::Uninit();
}

//==================================================================================
// 更新処理
//==================================================================================
void CSkillTree_Screen::Update(void)
{
	if (IsDeath())
	{
		return;
	}

	// 状態別処理
	(this->*(m_StateFuncList[m_state]))();

	if (IsDeath())
	{
		return;
	}

	// 頂点座標の設定
	SetVtx();
}

//==================================================================================
// 何もない状態
//==================================================================================
void CSkillTree_Screen::StateNone(void)
{
	m_fStateTime = 0;
}

//==================================================================================
// フェードイン状態
//==================================================================================
void CSkillTree_Screen::StateFadeIn(void)
{
	// 色取得
	D3DXCOLOR col = GetColor();

	// 状態遷移カウンター減算
	m_fStateTime -= CManager::GetInstance()->GetDeltaTime();

	// 不透明度更新
	col.a = 1.0f - (m_fStateTime / TIME_FADE);

	// 色設定
	SetColor(col);

	if (m_fStateTime <= 0)
	{
		m_fStateTime = 0;
		return;
	}
}

//==================================================================================
// フェードアウト状態
//==================================================================================
void CSkillTree_Screen::StateFadeOut(void)
{
	// 色取得
	D3DXCOLOR col = GetColor();

	// 状態遷移カウンター減算
	m_fStateTime -= CManager::GetInstance()->GetDeltaTime();

	// 不透明度更新
	col.a = m_fStateTime / TIME_FADE;

	// 色設定
	SetColor(col);

	if (m_fStateTime <= 0)
	{
		m_fStateTime = 0;
		Uninit();
		return;
	}
}

//==================================================================================
// 描画処理
//==================================================================================
void CSkillTree_Screen::Draw(void)
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

//==========================================================================
// 頂点情報設定処理
//==========================================================================
void CSkillTree_Screen::SetVtx(void)
{
	// 頂点設定
	CObject2D::SetVtx();
}

