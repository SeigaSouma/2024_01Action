//=============================================================================
// 
//  復活時間ゲージ処理 [revivaltime_gauge.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "revivaltime_gauge.h"
#include "texture.h"
#include "manager.h"
#include "renderer.h"
#include "calculation.h"

//==========================================================================
// 定数定義
//==========================================================================
namespace
{
	const float DEFAULT_WIDTH = 120.0f;		// デフォルトの幅
	const float DEFAULT_HEIGHT = 15.0f;		// デフォルトの高さ
	const float DEFAULT_TIMER = 4.0f;		// デフォルトのタイマー
	const char* TEXTURE[] =		// テクスチャのファイル
	{
		"",
		"",
		"data\\TEXTURE\\hpgauge\\hypnosis_fram.png",
	};
}

//==========================================================================
// 関数ポインタ
//==========================================================================
CRevivalTimeGauge::STATE_FUNC CRevivalTimeGauge::m_StateFunc[] =
{
	&CRevivalTimeGauge::StateNone,		// なし
	&CRevivalTimeGauge::StateFadeIn,	// フェードイン
	&CRevivalTimeGauge::StateFadeOut,	// フェードアウト
	&CRevivalTimeGauge::StateTimeComplete,	// 時間がなくなった瞬間
};

//==========================================================================
// コンストラクタ
//==========================================================================
CRevivalTimeGauge::CRevivalTimeGauge(float fadetime, int nPriority) : m_fFadeTime(fadetime), CObject(nPriority)
{
	// 値のクリア
	for (int nCntGauge = 0; nCntGauge < VTXTYPE_MAX; nCntGauge++)
	{
		m_pObj2DGauge[nCntGauge] = nullptr;	// 2Dゲージのオブジェクト
	}
	m_state = STATE_NONE;	// 状態
	m_fStateTime = 0.0f;	// 状態カウンター
	m_fValue = 0.0f;		// タイマーの値
}

//==========================================================================
// デストラクタ
//==========================================================================
CRevivalTimeGauge::~CRevivalTimeGauge()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CRevivalTimeGauge* CRevivalTimeGauge::Create(const float fadetime, MyLib::Vector3 pos)
{
	// 生成用のオブジェクト
	CRevivalTimeGauge* pHPGauge = nullptr;

	// メモリの確保
	pHPGauge = DEBUG_NEW CRevivalTimeGauge(fadetime);

	if (pHPGauge != nullptr)
	{// メモリの確保が出来ていたら

		// 位置設定
		pHPGauge->SetPosition(pos);
		pHPGauge->SetOriginPosition(pos);

		// 初期化処理
		pHPGauge->Init();
	}

	return pHPGauge;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CRevivalTimeGauge::Init(void)
{
	// 各種変数の初期化処理
	m_fValue = DEFAULT_TIMER;	// タイマーの値
	m_fStateTime = 0.0f;
	m_state = STATE_FADEIN;

	// 種類の設定
	SetType(TYPE_HPGAUGE);
	
	D3DXCOLOR col[] =
	{
		D3DXCOLOR(0.3f, 0.3f, 0.3f, 1.0f),
		D3DXCOLOR(1.0f, 0.7f, 0.3f, 1.0f),
		D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f)
	};
	for (int nCntGauge = 0; nCntGauge < VTXTYPE_MAX; nCntGauge++)
	{
		// 生成処理
		m_pObj2DGauge[nCntGauge] = CObject2D_Gauge::Create(DEFAULT_WIDTH, DEFAULT_HEIGHT, DEFAULT_TIMER * 60, TEXTURE[nCntGauge], GetPriority());
		if (m_pObj2DGauge[nCntGauge] == nullptr)
		{
			return E_FAIL;
		}

		m_pObj2DGauge[nCntGauge]->SetPosition(GetPosition());
		m_pObj2DGauge[nCntGauge]->SetOriginPosition(GetPosition());

		// 頂点カラーの設定
		m_pObj2DGauge[nCntGauge]->SetColor(col[nCntGauge]);
		
		// 種類の設定
		m_pObj2DGauge[nCntGauge]->SetType(CObject::TYPE_OBJECT2D);
		m_pObj2DGauge[nCntGauge]->SetMoveFactor(0.25f);	// 移動の係数設定
	}

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CRevivalTimeGauge::Uninit(void)
{
	for (int nCntGauge = 0; nCntGauge < VTXTYPE_MAX; nCntGauge++)
	{
		m_pObj2DGauge[nCntGauge] = nullptr;
	}

	// 情報削除
	Release();
}

//==========================================================================
// 終了処理
//==========================================================================
void CRevivalTimeGauge::Kill(void)
{
	for (int nCntGauge = 0; nCntGauge < VTXTYPE_MAX; nCntGauge++)
	{
		if (m_pObj2DGauge[nCntGauge] != nullptr)
		{
			// 終了処理
			m_pObj2DGauge[nCntGauge]->Uninit();
			m_pObj2DGauge[nCntGauge] = nullptr;
		}
	}

	// 情報削除
	Release();
}

//==========================================================================
// 更新処理
//==========================================================================
void CRevivalTimeGauge::Update(void)
{
	// 位置取得
	MyLib::Vector3 pos = GetPosition();

	for (const auto& gauge : m_pObj2DGauge)
	{
		D3DXCOLOR col = gauge->GetColor();
		col.a = m_fStateTime / m_fFadeTime;
		gauge->SetColor(col);
	}

	// 状態更新
	(this->*(m_StateFunc[m_state]))();
}

//==========================================================================
// なにもなし
//==========================================================================
void CRevivalTimeGauge::StateNone(void)
{
	m_fValue -= CManager::GetInstance()->GetDeltaTime();
	m_pObj2DGauge[VTXTYPE_GAUGE]->SetValue(static_cast<int>(m_fValue * 60));
	if (m_fValue <= 0.0f)
	{
		m_fValue = 0.0f;
		m_state = STATE_TIMECOMPLETE;
	}
}

//==========================================================================
// フェードイン
//==========================================================================
void CRevivalTimeGauge::StateFadeIn(void)
{
	// 状態遷移カウンター減算
	m_fStateTime += CManager::GetInstance()->GetDeltaTime();

	if (m_fStateTime >= m_fFadeTime)
	{
		m_fStateTime = m_fFadeTime;
		m_state = STATE_NONE;
		return;
	}
}

//==========================================================================
// フェードアウト
//==========================================================================
void CRevivalTimeGauge::StateFadeOut(void)
{
	// 状態遷移カウンター減算
	m_fStateTime -= CManager::GetInstance()->GetDeltaTime();

	if (m_fStateTime <= 0)
	{
		m_fStateTime = 0.0f;

		// 削除
		Uninit();
		return;
	}
}

//==========================================================================
// 時間がなくなった瞬間
//==========================================================================
void CRevivalTimeGauge::StateTimeComplete(void)
{

}

//==========================================================================
// 描画処理
//==========================================================================
void CRevivalTimeGauge::Draw(void)
{

}
