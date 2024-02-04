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
#include "input.h"
#include "game.h"
#include "calculation.h"

//==========================================================================
// マクロ定義
//==========================================================================
namespace
{
	const char* TEXTURE = "data\\TEXTURE\\skilltree\\skilltree_bg4.png";
	const float SCROLL = 0.0001f;
}

//==========================================================================
// コンストラクタ
//==========================================================================
CSkillTree_Screen::CSkillTree_Screen(int nPriority) : CObject2D(nPriority)
{
	// 値のクリア
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

//==========================================================================
// 初期化処理
//==========================================================================
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

	// 位置設定
	SetPosition(MyLib::Vector3(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f, 0.0f));

	// 色設定
	SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CSkillTree_Screen::Uninit(void)
{
	// 終了処理
	CObject2D::Uninit();
}

//==========================================================================
// 更新処理
//==========================================================================
void CSkillTree_Screen::Update(void)
{
	D3DXVECTOR2* tex = GetTex();
	tex[0].x += SCROLL;
	tex[1].x += SCROLL;
	tex[2].x += SCROLL;
	tex[3].x += SCROLL;

	// 頂点座標の設定
	SetVtx();
}

//==========================================================================
// 描画処理
//==========================================================================
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

