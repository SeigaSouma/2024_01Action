//=============================================================================
// 
//  プレイヤーの体力ゲージ処理 [hp_gauge_player.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "hp_gauge_player.h"
#include "texture.h"
#include "manager.h"
#include "renderer.h"
#include "calculation.h"

//==========================================================================
// 定数定義
//==========================================================================
namespace
{
	const float DEFAULT_WIDTH = 120.0f;
	const float DEFAULT_HEIGHT = 15.0f;
	const char* TEXTURE[] =		// テクスチャのファイル
	{
		/*"data\\TEXTURE\\hpgauge\\gaugeMoto.png",
	"data\\TEXTURE\\hpgauge\\gauge.png",
	"data\\TEXTURE\\hpgauge\\Fram.png",*/
		"",
		"",
		"data\\TEXTURE\\hpgauge\\hypnosis_fram.png",
	};
}

//==========================================================================
// コンストラクタ
//==========================================================================
CHP_GaugePlayer::CHP_GaugePlayer(int nPriority) : CObject(nPriority)
{
	// 値のクリア
	for (int nCntGauge = 0; nCntGauge < VTXTYPE_MAX; nCntGauge++)
	{
		m_pObj2DGauge[nCntGauge] = nullptr;
	}

	m_nLifeValue = 0;			// 値
	m_nMaxLifeValue = 0;		// 最大値
	m_nOriginLifeValue = 0;	// 初期値
}

//==========================================================================
// デストラクタ
//==========================================================================
CHP_GaugePlayer::~CHP_GaugePlayer()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CHP_GaugePlayer *CHP_GaugePlayer::Create(MyLib::Vector3 pos, int nMaxLife)
{
	// 生成用のオブジェクト
	CHP_GaugePlayer *pHPGauge = nullptr;

	if (pHPGauge == nullptr)
	{// nullptrだったら

		// メモリの確保
		pHPGauge = DEBUG_NEW CHP_GaugePlayer;

		if (pHPGauge != nullptr)
		{// メモリの確保が出来ていたら

			pHPGauge->m_nMaxLifeValue = nMaxLife;	// スタミナの値
			pHPGauge->m_nOriginLifeValue = nMaxLife;	// スタミナの初期値

			// 位置設定
			pHPGauge->SetPosition(pos);

			// 初期化処理
			pHPGauge->Init();

			// 種類の設定
			pHPGauge->SetType(TYPE_HPGAUGE);
		}

		return pHPGauge;
	}

	return nullptr;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CHP_GaugePlayer::Init(void)
{
	// 最大体力
	m_nLifeValue = m_nOriginLifeValue;

	D3DXCOLOR col[] =
	{
		D3DXCOLOR(0.3f, 0.3f, 0.3f, 1.0f),
		D3DXCOLOR(1.0f, 0.0f, 1.0f, 1.0f),
		D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f)
	};
	for (int nCntGauge = 0; nCntGauge < VTXTYPE_MAX; nCntGauge++)
	{
		// 生成処理
		m_pObj2DGauge[nCntGauge] = CObject2D_Gauge::Create(DEFAULT_WIDTH, DEFAULT_HEIGHT, m_nMaxLifeValue, TEXTURE[nCntGauge]);
		if (m_pObj2DGauge[nCntGauge] == nullptr)
		{
			return E_FAIL;
		}

		m_pObj2DGauge[nCntGauge]->SetOriginPosition(GetPosition());

		// 頂点カラーの設定
		m_pObj2DGauge[nCntGauge]->SetColor(col[nCntGauge]);

		// 種類の設定
		m_pObj2DGauge[nCntGauge]->SetType(CObject::TYPE_OBJECT2D);
		m_pObj2DGauge[nCntGauge]->SetSize(D3DXVECTOR2(DEFAULT_WIDTH, DEFAULT_HEIGHT));

		if (nCntGauge == VTXTYPE_PINK)
		{
			m_pObj2DGauge[nCntGauge]->SetMoveFactor(0.15f);	// 移動の係数設定
		}
		else
		{
			m_pObj2DGauge[nCntGauge]->SetMoveFactor(1.0f);	// 移動の係数設定
		}
	}

	// 値設定
	SetLife(m_nMaxLifeValue);

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CHP_GaugePlayer::Uninit(void)
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
void CHP_GaugePlayer::Kill(void)
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
void CHP_GaugePlayer::Update(void)
{
	// 位置取得
	MyLib::Vector3 pos = GetPosition();

	for (const auto& gauge : m_pObj2DGauge)
	{
		gauge->SetPosition(pos);

		//// 色取得
		//D3DXCOLOR col = m_pObj2DGauge[nCntGauge]->GetColor();

		//col = UtilFunc::Transformation::HSVtoRGB(0.0f, 0.0f, 1.0f + sinf(D3DX_PI * ((float)m_nCntTkTk / 60.0f)) * 0.3f);

		// 色設定
		//m_pObj2DGauge[nCntGauge]->SetColor(col);
	}
}

//==========================================================================
// 位置更新
//==========================================================================
void CHP_GaugePlayer::SetLife(int nLife)
{
	// 現在の体力設定
	m_nLifeValue = nLife;
	m_pObj2DGauge[VTXTYPE_PINK]->SetValue(m_nLifeValue);
}

//==========================================================================
// 色更新
//==========================================================================
void CHP_GaugePlayer::ChangeColor(int nCntGauge)
{


	CObject2D_Gauge* pGauge = m_pObj2DGauge[nCntGauge];

	// サイズ取得
	D3DXVECTOR2 size = pGauge->GetSize();

	// 色取得
	D3DXCOLOR col = pGauge->GetColor();

	float maxwidth = pGauge->GetMaxWidth();


	if (
		size.x / maxwidth <= 0.95f &&
		size.x / maxwidth > 0.6f)
	{//HPゲージ8割

		col = D3DXCOLOR(0.2f, 0.6f, 0.2f, 1.0f);
	}
	else if (
		size.x / maxwidth <= 0.6f &&
		size.x / maxwidth > 0.4f)
	{//HPゲージ6割

		col = D3DXCOLOR(0.8f, 0.7f, 0.2f, 1.0f);
	}
	else if (
		size.x / maxwidth <= 0.4f &&
		size.x / maxwidth > 0.2f)
	{//HPゲージ4割

		col = D3DXCOLOR(0.8f, 0.5f, 0.2f, 1.0f);
	}
	else if (size.x / maxwidth <= 0.2f)
	{//危険エリア

		col = D3DXCOLOR(0.8f, 0.2f, 0.2f, 1.0f);
	}
	else
	{//満タン

		col = D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f);
	}

	// 色設定
	m_pObj2DGauge[nCntGauge]->SetColor(col);

	// サイズ取得
	m_pObj2DGauge[nCntGauge]->SetSize(size);

}


//==========================================================================
// 最大値のアップグレード
//==========================================================================
int CHP_GaugePlayer::UpgradeMaxValue(int addvalue)
{
	int oldMaxLife = m_nMaxLifeValue;

	// スタミナの最大値上昇
	m_nMaxLifeValue += addvalue;

	for (const auto& gauge : m_pObj2DGauge)
	{
		SetPosition(gauge->UpgradeMaxValue(addvalue));
	}

	// 現在の体力設定
	float ratio = static_cast<float>(m_nMaxLifeValue) / static_cast<float>(oldMaxLife);
	m_nLifeValue *= ratio;

	m_pObj2DGauge[VTXTYPE_PINK]->SetValue(m_nLifeValue);

	return m_nLifeValue;
}

//==========================================================================
// 描画処理
//==========================================================================
void CHP_GaugePlayer::Draw(void)
{

}
