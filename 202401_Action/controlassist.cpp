//=============================================================================
// 
// 操作補助処理 [controlassist.cpp]
// Author : 相馬靜雅
// 
//=============================================================================
#include "controlassist.h"
#include "texture.h"
#include "calculation.h"

//==========================================================================
// 定数定義
//==========================================================================
namespace
{
	const char* TEXTURE[] =
	{ 
		"data\\TEXTURE\\control\\rockon.png",	// ロックオン
		"data\\TEXTURE\\control\\counter.png",	// 刹舞
	};
	const float WIDTH = 280.0f;	// 幅
	const MyLib::Vector3 CREATE_POSITION = MyLib::Vector3(WIDTH - 5.0f, 200.0f, 0.0f);	// 生成位置
}
CControlAssist* CControlAssist::m_pThisPtr= nullptr;		// 自身のポインタ

//==========================================================================
// コンストラクタ
//==========================================================================
CControlAssist::CControlAssist(int nPriority) : CObject(nPriority)
{
	// 値のクリア
	m_nNumDisp = 0;	// 描画数
	memset(m_pText, 0, sizeof(m_pText));
}

//==========================================================================
// デストラクタ
//==========================================================================
CControlAssist::~CControlAssist()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CControlAssist* CControlAssist::Create()
{
	if (m_pThisPtr != nullptr)
	{
		return m_pThisPtr;
	}

	// メモリの確保
	m_pThisPtr = DEBUG_NEW CControlAssist;

	if (m_pThisPtr != nullptr)
	{// メモリの確保が出来ていたら

		// 初期化処理
		m_pThisPtr->Init();
	}

	return m_pThisPtr;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CControlAssist::Init()
{
	HRESULT hr;

	// 種類の設定
	SetType(TYPE_OBJECT2D);

	for (int i = 0; i < CONTROLTYPE_MAX; i++)
	{
		m_pText[i] = CObject2D::Create(GetPriority());
		if (m_pText[i] == nullptr)
		{
			continue;
		}
		m_pText[i]->SetType(CObject::TYPE_OBJECT2D);

		// テクスチャの割り当て
		int nIdxTex = CTexture::GetInstance()->Regist(TEXTURE[i]);
		m_pText[i]->BindTexture(nIdxTex);

		// 初期設定
		m_pText[i]->SetPosition(CREATE_POSITION);

		// テクスチャサイズ取得
		D3DXVECTOR2 size = CTexture::GetInstance()->GetImageSize(nIdxTex);
		m_pText[i]->SetSize(UtilFunc::Transformation::AdjustSizeByWidth(size, WIDTH));

		// 描画オフ
		m_pText[i]->SetEnableDisp(false);
	}

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CControlAssist::Uninit()
{

	m_pThisPtr = nullptr;

	for (int i = 0; i < CONTROLTYPE_MAX; i++)
	{
		if (m_pText[i] == nullptr)
		{
			continue;
		}
		m_pText[i]->Uninit();
		m_pText[i] = nullptr;
	}

	// 終了処理
	Release();
}

//==========================================================================
// テキスト設定
//==========================================================================
void CControlAssist::SetText(CONTROLTYPE text)
{
	int type = static_cast<int>(text);
	if (m_pText[type] == nullptr)
	{
		return;
	}

	// 描画ON
	m_pText[type]->SetEnableDisp(true);
	m_pText[type]->SetPosition(CREATE_POSITION);

	// 位置設定
	float height = m_pText[type]->GetSize().y * 2.0f;
	m_pText[type]->AddPosition(MyLib::Vector3(0.0f, m_nNumDisp * height, 0.0f));

	// 描画数加算
	m_nNumDisp++;
}

//==========================================================================
// テキストリセット
//==========================================================================
void CControlAssist::ResetText()
{
	// 描画数リセット
	m_nNumDisp = 0;

	for (const auto& text : m_pText)
	{
		text->SetEnableDisp(false);
	}
}
