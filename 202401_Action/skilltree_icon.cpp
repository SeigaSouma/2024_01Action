//=============================================================================
// 
// スキルツリーアイコン処理 [skilltree_icon.cpp]
// Author : 相馬靜雅
// 
//=============================================================================
#include "skilltree_icon.h"
#include "renderer.h"
#include "texture.h"
#include "manager.h"
#include "sound.h"
#include "input.h"
#include "game.h"
#include "calculation.h"

//==========================================================================
// マクロ定義
//==========================================================================
namespace
{
	const char* TEXTURE[] =
	{
		"data\\TEXTURE\\skilltree\\icon_life.png",
		"data\\TEXTURE\\skilltree\\icon_power.png",
		"data\\TEXTURE\\skilltree\\icon_stamina.png",
	};
	const float SIZE_ICON = 50.0f;		// アイコンサイズ
	const float TIME_FADE = 0.5f;	// フェードアウト時間
}

//==========================================================================
// 関数ポインタ
//==========================================================================
CSkillTree_Icon::STATE_FUNC CSkillTree_Icon::m_StateFuncList[] =
{
	&CSkillTree_Icon::StateNone,		// なにもなし
};

//==========================================================================
// コンストラクタ
//==========================================================================
CSkillTree_Icon::CSkillTree_Icon(int nPriority) : CObject2D(nPriority)
{
	// 値のクリア
	m_fStateTime = 0;				// 状態遷移カウンター
	m_state = STATE_NONE;			// 状態
	m_Mastering = MASTERING_YET;	// 習得状態
	m_SkillIconInfo = sSkillIcon();	// スキルアイコン
}

//==========================================================================
// デストラクタ
//==========================================================================
CSkillTree_Icon::~CSkillTree_Icon()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CSkillTree_Icon* CSkillTree_Icon::Create(void)
{
	// 生成用のオブジェクト
	CSkillTree_Icon* pEffect = nullptr;

	// メモリの確保
	pEffect = DEBUG_NEW CSkillTree_Icon;

	if (pEffect != nullptr)
	{
		// 初期化処理
		pEffect->Init();
	}

	return pEffect;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CSkillTree_Icon::Init(void)
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
	int nIdx = CTexture::GetInstance()->Regist(TEXTURE[0]);
	BindTexture(nIdx);

	// サイズ設定
	SetSize(D3DXVECTOR2(SIZE_ICON, SIZE_ICON));
	SetSizeOrigin(GetSize());

	// 状態カウンター
	m_fStateTime = 0.0f;

	// 位置設定
	SetPosition(MyLib::Vector3(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f, 0.0f));

	// 色設定
	SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CSkillTree_Icon::Uninit(void)
{
	// 終了処理
	CObject2D::Uninit();
}

//==========================================================================
// 更新処理
//==========================================================================
void CSkillTree_Icon::Update(void)
{
	// 状態別処理
	(this->*(m_StateFuncList[m_state]))();

	if (IsDeath())
	{
		return;
	}

	// 頂点座標の設定
	SetVtx();
}

//==========================================================================
// 何もない状態
//==========================================================================
void CSkillTree_Icon::StateNone(void)
{

}

//==========================================================================
// 描画処理
//==========================================================================
void CSkillTree_Icon::Draw(void)
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
// アイコン情報設定
//==========================================================================
void CSkillTree_Icon::SetIconInfo(sSkillIcon iconinfo)
{
	m_SkillIconInfo = iconinfo;
}

//==========================================================================
// アイコン情報取得
//==========================================================================
CSkillTree_Icon::sSkillIcon CSkillTree_Icon::GetIconInfo(void)
{
	return m_SkillIconInfo;
}

//==========================================================================
// 頂点情報設定処理
//==========================================================================
void CSkillTree_Icon::SetVtx(void)
{
	// 頂点設定
	CObject2D::SetVtx();
}

