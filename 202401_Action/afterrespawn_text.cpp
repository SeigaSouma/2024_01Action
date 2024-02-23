//=============================================================================
// 
// リスポーン後テキスト処理 [afterrespawn_text.cpp]
// Author : 相馬靜雅
// 
//=============================================================================
#include "afterrespawn_text.h"
#include "manager.h"
#include "calculation.h"

//==========================================================================
// 定数定義
//==========================================================================
namespace
{
	const char* TEXTURE[] =
	{ 
		"data\\TEXTURE\\afterrespawn\\reset.png",				// 強化リセット
		"data\\TEXTURE\\afterrespawn\\addpoint.png",			// 情けポイント
	};
	const float TIME_APPEARANCE = 0.6f;	// 登場時間
	const float TIME_FADEOUT = 4.0f;	// フェードアウト時間
	const float TIME_START_FADEOUT = 3.5f;	// フェードアウトされるまでの時間
	const float WIDTH = 300.0f;	// 幅
	const MyLib::Vector3 CREATE_POSITION = MyLib::Vector3(SCREEN_WIDTH - WIDTH + 35.0f, 500.0f, 0.0f);	// 生成位置
}
CAfterRespawn_Text* CAfterRespawn_Text::m_pThisPtr= nullptr;		// 自身のポインタ

//==========================================================================
// 関数リスト
//==========================================================================
CAfterRespawn_Text::STATE_FUNC CAfterRespawn_Text::m_StateFuncList[] =
{
	&CAfterRespawn_Text::StateNone,			// 通常
	&CAfterRespawn_Text::StateAppearance,	// 登場
	&CAfterRespawn_Text::StateFadeOut,		// フェードアウト
};

//==========================================================================
// コンストラクタ
//==========================================================================
CAfterRespawn_Text::CAfterRespawn_Text(int nPriority) : CObject(nPriority)
{
	// 値のクリア
	m_state = STATE::STATE_NONE;	// 状態
	m_fStateTimer = 0.0f;			// 状態タイマー
	m_nNumDisp = 0;					// 描画数
	memset(m_pText, 0, sizeof(m_pText));
}

//==========================================================================
// デストラクタ
//==========================================================================
CAfterRespawn_Text::~CAfterRespawn_Text()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CAfterRespawn_Text* CAfterRespawn_Text::Create()
{
	if (m_pThisPtr != nullptr)
	{
		return m_pThisPtr;
	}

	// メモリの確保
	m_pThisPtr = DEBUG_NEW CAfterRespawn_Text;

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
HRESULT CAfterRespawn_Text::Init()
{
	HRESULT hr;

	// 種類の設定
	SetType(TYPE_OBJECT2D);

	for (int i = 0; i < TEXTTYPE_MAX; i++)
	{
		m_pText[i] = CObject2D::Create(10);
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
void CAfterRespawn_Text::Uninit()
{

	m_pThisPtr = nullptr;

	for (int i = 0; i < TEXTTYPE_MAX; i++)
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
// 更新処理
//==========================================================================
void CAfterRespawn_Text::Update()
{
	// 状態別処理
	(this->*(m_StateFuncList[m_state]))();

	/*for (int i = 0; i < TEXTTYPE_MAX; i++)
	{
		if (m_pText[i] == nullptr ||
			!m_pText[i]->IsDisp())
		{
			continue;
		}
	}*/
}

//==========================================================================
// 登場
//==========================================================================
void CAfterRespawn_Text::StateAppearance()
{

	// タイマー減算
	m_fStateTimer -= CManager::GetInstance()->GetDeltaTime();

	if (m_fStateTimer <= 0.0f)
	{
		// タイマーリセット
		m_fStateTimer = TIME_FADEOUT;
		m_state = STATE::STATE_FADEOUT;

		for (int i = 0; i < TEXTTYPE_MAX; i++)
		{
			if (m_pText[i] == nullptr ||
				!m_pText[i]->IsDisp())
			{
				continue;
			}
			// 位置
			MyLib::Vector3 pos = m_pText[i]->GetPosition();
			pos.x = CREATE_POSITION.x;
			m_pText[i]->SetPosition(pos);
		}
		return;
	}

	for (int i = 0; i < TEXTTYPE_MAX; i++)
	{
		if (m_pText[i] == nullptr ||
			!m_pText[i]->IsDisp())
		{
			continue;
		}

		MyLib::Vector3 pos = m_pText[i]->GetPosition();

		pos.x = UtilFunc::Correction::EasingEaseOut(1600.0f, CREATE_POSITION.x, TIME_FADEOUT, 0.0f, m_fStateTimer);

		// 位置移動
		m_pText[i]->SetPosition(pos);
	}
}

//==========================================================================
// フェードアウト
//==========================================================================
void CAfterRespawn_Text::StateFadeOut()
{
	m_fStateTimer -= CManager::GetInstance()->GetDeltaTime();
	if (m_fStateTimer <= TIME_FADEOUT - TIME_START_FADEOUT)
	{
		// 不透明度更新
		float alpha = UtilFunc::Correction::EasingLinear(1.0f, 0.0f, TIME_FADEOUT - TIME_START_FADEOUT, 0.0f, m_fStateTimer);

		for (int i = 0; i < TEXTTYPE_MAX; i++)
		{
			if (m_pText[i] == nullptr ||
				!m_pText[i]->IsDisp())
			{
				continue;
			}
			m_pText[i]->SetAlpha(alpha);

			// 位置移動
			m_pText[i]->AddPosition(MyLib::Vector3(0.0f, -1.0f, 0.0f));
		}
	}

	if (m_fStateTimer <= 0.0f)
	{
		Uninit();
	}
}

//==========================================================================
// テキスト設定
//==========================================================================
void CAfterRespawn_Text::SetText(TEXTTYPE text)
{
	int type = static_cast<int>(text);
	if (m_pText[type] == nullptr)
	{
		return;
	}

	// 描画ON
	m_pText[type]->SetEnableDisp(true);

	MyLib::Vector3 setpos = CREATE_POSITION;
	setpos.x = 1600.0f;

	for (int i = 0; i < TEXTTYPE_MAX; i++)
	{
		if (m_pText[i] == nullptr ||
			!m_pText[i]->IsDisp())
		{
			continue;
		}
		float height = m_pText[i]->GetSize().y * 2.0f;
		setpos.y += height;

		m_pText[i]->SetPosition(setpos);
		m_pText[i]->Update();
	}

	// 描画数加算
	m_nNumDisp++;

	// タイマーリセット
	m_fStateTimer = TIME_APPEARANCE;
	m_state = STATE::STATE_APPEARANCE;
}

//==========================================================================
// デフォルトテキスト設定
//==========================================================================
void CAfterRespawn_Text::SetDefaultText()
{
	SetText(TEXTTYPE::TEXTTYPE_RESET);
	SetText(TEXTTYPE::TEXTTYPE_ADDPOINT);
}

//==========================================================================
// テキストリセット
//==========================================================================
void CAfterRespawn_Text::ResetText()
{
	// 描画数リセット
	m_nNumDisp = 0;

	for (const auto& text : m_pText)
	{
		text->SetEnableDisp(false);
	}
}
