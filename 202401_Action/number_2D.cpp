//=============================================================================
// 
//  数字(2D)処理 [number_2D.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "number_2D.h"
#include "manager.h"
#include "renderer.h"
#include "object2D.h"

//==========================================================================
// マクロ定義
//==========================================================================
#define WIDTH			(640.0f)					// 横幅
#define HEIGHT			(360.0f)					// 縦幅
#define SCROLL_SPEED	(-0.005f)					// スクロール速度

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================

//==========================================================================
// コンストラクタ
//==========================================================================
CNumber2D::CNumber2D(int nPriority) : CNumber(nPriority)
{
	// 値のクリア
	m_aObject2D = NULL;			// オブジェクト2Dのオブジェクト
}

//==========================================================================
// デストラクタ
//==========================================================================
CNumber2D::~CNumber2D()
{

}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CNumber2D::Init(int nPriority)
{
	// 生成処理
	m_aObject2D = CObject2D::Create(nPriority);

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CNumber2D::Uninit()
{
	// 終了処理
	if (m_aObject2D != NULL)
	{// NULLじゃなかったら
		m_aObject2D = NULL;
	}
}

//==========================================================================
// 解放処理
//==========================================================================
void CNumber2D::Release()
{
	if (m_aObject2D != NULL)
	{// NULLじゃなかったら
		m_aObject2D->Uninit();
		m_aObject2D = NULL;
	}
}

//==========================================================================
// 更新処理
//==========================================================================
void CNumber2D::Update()
{
	// 更新処理
	if (m_aObject2D != NULL)
	{
		m_aObject2D->Update();
	}
}

//==========================================================================
// 描画処理
//==========================================================================
void CNumber2D::Draw()
{
	// 描画処理
	if (m_aObject2D != NULL)
	{
		m_aObject2D->Draw();
	}
}

//==========================================================================
// 頂点情報設定処理
//==========================================================================
void CNumber2D::SetVtx()
{
	m_aObject2D->SetVtx();
}

//==========================================================================
// テクスチャの割り当て
//==========================================================================
void CNumber2D::BindTexture(int nIdx)
{
	// 割り当てる
	m_aObject2D->BindTexture(nIdx);
}

//==========================================================================
// 種類設定
//==========================================================================
void CNumber2D::SetType(const CObject::TYPE type)
{
	m_aObject2D->SetType(type);
}



//==========================================================================
// 位置設定
//==========================================================================
void CNumber2D::SetPosition(const MyLib::Vector3 pos)
{
	m_aObject2D->SetPosition(pos);
}

//==========================================================================
// 位置取得
//==========================================================================
MyLib::Vector3 CNumber2D::GetPosition() const
{
	return m_aObject2D->GetPosition();
}

//==========================================================================
// 移動量設定
//==========================================================================
void CNumber2D::SetMove(const MyLib::Vector3 move)
{
	m_aObject2D->SetMove(move);
}

//==========================================================================
// 移動量取得
//==========================================================================
MyLib::Vector3 CNumber2D::GetMove() const
{
	return m_aObject2D->GetMove();
}

//==========================================================================
// 向き設定
//==========================================================================
void CNumber2D::SetRotation(const MyLib::Vector3 rot)
{
	m_aObject2D->SetRotation(rot);
}

//==========================================================================
// 向き取得
//==========================================================================
MyLib::Vector3 CNumber2D::GetRotation() const
{
	return m_aObject2D->GetRotation();
}

//==========================================================================
// 色設定
//==========================================================================
void CNumber2D::SetColor(const D3DXCOLOR col)
{
	m_aObject2D->SetColor(col);
}

//==========================================================================
// 色取得
//==========================================================================
D3DXCOLOR CNumber2D::GetColor() const
{
	return m_aObject2D->GetColor();
}

//==========================================================================
// サイズ設定
//==========================================================================
void CNumber2D::SetSize(const D3DXVECTOR2 size)
{
	m_aObject2D->SetSize(size);		// サイズ
}

//==========================================================================
// サイズ取得
//==========================================================================
D3DXVECTOR2 CNumber2D::GetSize() const
{
	return m_aObject2D->GetSize();
}

//==========================================================================
// 元のサイズの設定
//==========================================================================
void CNumber2D::SetSizeOrigin(const D3DXVECTOR2 size)
{
	m_aObject2D->SetSizeOrigin(size);
}

//==========================================================================
// 元のサイズの取得
//==========================================================================
D3DXVECTOR2 CNumber2D::GetSizeOrigin() const
{
	return m_aObject2D->GetSizeOrigin();
}

//==========================================================================
// テクスチャ座標設定
//==========================================================================
void CNumber2D::SetTex(D3DXVECTOR2 *tex)
{
	m_aObject2D->SetTex(tex);
}

//==========================================================================
// テクスチャ座標取得
//==========================================================================
D3DXVECTOR2 *CNumber2D::GetTex()
{
	return m_aObject2D->GetTex();
}

//==========================================================================
// オブジェクト2Dオブジェクトの取得
//==========================================================================
CObject2D *CNumber2D::GetObject2D()
{
	return m_aObject2D;
}
