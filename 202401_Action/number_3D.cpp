//=============================================================================
// 
//  数字(2D)処理 [number_2D.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "number_3D.h"
#include "manager.h"
#include "renderer.h"
#include "object3D.h"

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
CNumber3D::CNumber3D(int nPriority) : CNumber(nPriority)
{
	// 値のクリア
	m_aObject3D = NULL;			// オブジェクト2Dのオブジェクト
	m_bAddAlpha = true;
}

//==========================================================================
// デストラクタ
//==========================================================================
CNumber3D::~CNumber3D()
{

}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CNumber3D::Init(int nPriority)
{
	// 生成処理
	m_aObject3D = CObject3D::Create(nPriority);

	// 種類設定
	SetType(CObject::TYPE_NUMBER);

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CNumber3D::Uninit()
{
	// 終了処理
	if (m_aObject3D != NULL)
	{// NULLじゃなかったら
		m_aObject3D = NULL;
	}
}

//==========================================================================
// 解放処理
//==========================================================================
void CNumber3D::Release()
{
	if (m_aObject3D != NULL)
	{// NULLじゃなかったら
		m_aObject3D->Uninit();
		m_aObject3D = NULL;
	}
}

//==========================================================================
// 更新処理
//==========================================================================
void CNumber3D::Update()
{
	// 更新処理
	if (m_aObject3D != NULL)
	{
		m_aObject3D->Update();
	}
}

//==========================================================================
// 描画処理
//==========================================================================
void CNumber3D::Draw()
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();

	// 描画処理
	if (m_aObject3D != NULL)
	{
		// アルファテストを有効にする
		pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
		pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
		pDevice->SetRenderState(D3DRS_ALPHAREF, 0);

		// αブレンディングを加算合成に設定
		if (m_bAddAlpha == true)
		{
			pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
			pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
			pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
		}

		m_aObject3D->Draw();

		// αブレンディングを元に戻す
		pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

		// アルファテストを無効にする
		pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
		pDevice->SetRenderState(D3DRS_ALPHAREF, 0);
	}
}

//==========================================================================
// 頂点情報設定処理
//==========================================================================
void CNumber3D::SetVtx()
{
	m_aObject3D->SetVtx();
}

//==========================================================================
// テクスチャの割り当て
//==========================================================================
void CNumber3D::BindTexture(int nIdx)
{
	// 割り当てる
	m_aObject3D->BindTexture(nIdx);
}

//==========================================================================
// 種類設定
//==========================================================================
void CNumber3D::SetType(const CObject::TYPE type)
{
	m_aObject3D->SetType(type);
}



//==========================================================================
// 位置設定
//==========================================================================
void CNumber3D::SetPosition(const MyLib::Vector3 pos)
{
	m_aObject3D->SetPosition(pos);
}

//==========================================================================
// 位置取得
//==========================================================================
MyLib::Vector3 CNumber3D::GetPosition() const
{
	return m_aObject3D->GetPosition();
}

//==========================================================================
// 移動量設定
//==========================================================================
void CNumber3D::SetMove(const MyLib::Vector3 move)
{
	m_aObject3D->SetMove(move);
}

//==========================================================================
// 移動量取得
//==========================================================================
MyLib::Vector3 CNumber3D::GetMove() const
{
	return m_aObject3D->GetMove();
}

//==========================================================================
// 向き設定
//==========================================================================
void CNumber3D::SetRotation(const MyLib::Vector3 rot)
{
	m_aObject3D->SetRotation(rot);
}

//==========================================================================
// 向き取得
//==========================================================================
MyLib::Vector3 CNumber3D::GetRotation() const
{
	return m_aObject3D->GetRotation();
}

//==========================================================================
// 色設定
//==========================================================================
void CNumber3D::SetColor(const D3DXCOLOR col)
{
	m_aObject3D->SetColor(col);
}

//==========================================================================
// 色取得
//==========================================================================
D3DXCOLOR CNumber3D::GetColor() const
{
	return m_aObject3D->GetColor();
}

//==========================================================================
// サイズ設定
//==========================================================================
void CNumber3D::SetSize3D(const MyLib::Vector3 size)
{
	m_aObject3D->SetSize(size);		// サイズ
}

//==========================================================================
// サイズ取得
//==========================================================================
MyLib::Vector3 CNumber3D::GetSize3D() const
{
	return m_aObject3D->GetSize();
}

//==========================================================================
// テクスチャ座標設定
//==========================================================================
void CNumber3D::SetTex(D3DXVECTOR2 *tex)
{
	m_aObject3D->SetTex(tex);
}

//==========================================================================
// テクスチャ座標取得
//==========================================================================
D3DXVECTOR2 *CNumber3D::GetTex()
{
	return m_aObject3D->GetTex();
}

//==========================================================================
// オブジェクト3Dオブジェクトの取得
//==========================================================================
CObject3D *CNumber3D::GetObject3D()
{
	return m_aObject3D;
}
