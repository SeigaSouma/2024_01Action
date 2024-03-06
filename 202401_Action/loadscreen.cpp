//=============================================================================
// 
//  フェード処理 [loadscreen.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "loadscreen.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"
#include "object2D.h"

//==========================================================================
// 定数定義
//==========================================================================
namespace
{
	const char* TEXTURE = "data\\TEXTURE\\load\\loadscreen.jpg";
}


//==========================================================================
// 静的メンバ変数宣言
//==========================================================================

//==========================================================================
// コンストラクタ
//==========================================================================
CLoadScreen::CLoadScreen()
{
	// 値のクリア
	m_aObject2D = nullptr;					// オブジェクト2Dのオブジェクト
}

//==========================================================================
// デストラクタ
//==========================================================================
CLoadScreen::~CLoadScreen()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CLoadScreen *CLoadScreen::Create()
{
	// 生成用のオブジェクト
	CLoadScreen *pFade = nullptr;

	if (pFade == nullptr)
	{// nullptrだったら

		// メモリの確保
		pFade = DEBUG_NEW CLoadScreen;

		if (pFade != nullptr)
		{// メモリの確保が出来ていたら

			// 初期化処理
			if (FAILED(pFade->Init()))
			{// 失敗していたら
				return nullptr;
			}
		}

		return pFade;
	}

	return nullptr;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CLoadScreen::Init()
{
	// 生成処理
	m_aObject2D = CObject2D_Anim::Create(0.0f, 5, 9, 2, false);
	if (m_aObject2D == nullptr)
	{// 失敗していたら
		return E_FAIL;
	}
	m_aObject2D->SetType(CObject::TYPE::TYPE_NONE);

	m_aObject2D->SetSize(D3DXVECTOR2(640.0f, 360.0f));	// サイズ
	m_aObject2D->SetPosition(D3DXVECTOR3(640.0f, 360.0f, 0.0f));	// 位置
	m_aObject2D->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));	// 色設定

	// テクスチャの割り当て
	int nIdx = CTexture::GetInstance()->Regist(TEXTURE);

	// テクスチャの割り当て
	m_aObject2D->BindTexture(nIdx);

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CLoadScreen::Uninit()
{
	if (m_aObject2D != nullptr)
	{// nullptrじゃなかったら

		// 終了処理
		m_aObject2D->Uninit();
		m_aObject2D = nullptr;
	}
}

void CLoadScreen::Kill()
{
	if (m_aObject2D != nullptr)
	{
		// 終了処理
		m_aObject2D->Uninit();
		delete m_aObject2D;
		m_aObject2D = nullptr;
	}
}

//==========================================================================
// 更新処理
//==========================================================================
void CLoadScreen::Update()
{
	if (m_aObject2D == nullptr)
	{
		return;
	}

	// 色取得
	D3DXCOLOR col = m_aObject2D->GetColor();

	// 更新処理
	m_aObject2D->Update();

	m_aObject2D->SetVtx();

}

//==========================================================================
// 描画処理
//==========================================================================
void CLoadScreen::Draw()
{
	// 描画処理
	m_aObject2D->Draw();
}

//==========================================================================
// オブジェクト2Dオブジェクトの取得
//==========================================================================
CObject2D_Anim*CLoadScreen::GetMyObject()
{
	return m_aObject2D;
}