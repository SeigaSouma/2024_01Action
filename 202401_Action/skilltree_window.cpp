//=============================================================================
// 
// スキルツリーウィンドウ処理 [skilltree_window.cpp]
// Author : 相馬靜雅
// 
//=============================================================================
#include "skilltree_window.h"
#include "renderer.h"
#include "texture.h"
#include "manager.h"
#include "calculation.h"

//==========================================================================
// マクロ定義
//==========================================================================
namespace
{
	const char* TEXTURE = "data\\TEXTURE\\skilltree\\skilltree_window3.png";
}

//==========================================================================
// コンストラクタ
//==========================================================================
CSkillTree_Window::CSkillTree_Window(int nPriority) : CObject2D(nPriority)
{
	// 値のクリア
}

//==========================================================================
// デストラクタ
//==========================================================================
CSkillTree_Window::~CSkillTree_Window()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CSkillTree_Window* CSkillTree_Window::Create()
{
	// 生成用のオブジェクト
	CSkillTree_Window* pWindow = nullptr;

	// メモリの確保
	pWindow = DEBUG_NEW CSkillTree_Window;

	if (pWindow != nullptr)
	{
		// 初期化処理
		pWindow->Init();
	}

	return pWindow;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CSkillTree_Window::Init()
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
	D3DXVECTOR2 size = CTexture::GetInstance()->GetImageSize(nIdx);
	size = UtilFunc::Transformation::AdjustSizeByHeight(size, 300.0f);
	SetSize(size);
	SetSizeOrigin(GetSize());

	// 位置設定
	SetPosition(MyLib::Vector3(1050.0f, 450.0f, 0.0f));

	// 色設定
	SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));

	return S_OK;
}

