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
#include "hp_gauge_tip.h"

//==========================================================================
// 定数定義
//==========================================================================
namespace
{
	const float DEFAULT_WIDTH = 150.0f;
	const float DEFAULT_HEIGHT = 18.0f;
	const char* TEXTURE[] =		// テクスチャのファイル
	{
		"data\\TEXTURE\\hpgauge\\black.png",
		"data\\TEXTURE\\hpgauge\\hpgauge.png",
	};
	const float LENGTH_TEXTUREREPEAT = 18.0f;	// テクスチャがリピートする長さ
}

//==========================================================================
// コンストラクタ
//==========================================================================
CHP_GaugePlayer::CHP_GaugePlayer(int nPriority) : CObject(nPriority)
{
	// 値のクリア
	memset(m_pObj2DGauge, 0, sizeof(m_pObj2DGauge));	// 2Dゲージのオブジェクト
	m_nLifeValue = 0;			// 値
	m_nMaxLifeValue = 0;		// 最大値
	m_nOriginLifeValue = 0;	// 初期値
	m_pTip = nullptr;	// ゲージの先端
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
HRESULT CHP_GaugePlayer::Init()
{
	// 最大体力
	m_nLifeValue = m_nOriginLifeValue;

	for (int nCntGauge = 0; nCntGauge < VTXTYPE_MAX; nCntGauge++)
	{
		// 生成処理
		m_pObj2DGauge[nCntGauge] = CObject2D_Gauge::Create(DEFAULT_WIDTH, DEFAULT_HEIGHT, m_nMaxLifeValue, TEXTURE[nCntGauge], 11);
		if (m_pObj2DGauge[nCntGauge] == nullptr)
		{
			return E_FAIL;
		}

		m_pObj2DGauge[nCntGauge]->SetOriginPosition(GetPosition());

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


	// 先端生成
	m_pTip = CHPGaugeTip::Create(GetPosition() - MyLib::Vector3(DEFAULT_WIDTH, 0.0f, 0.0f), GetPosition() + MyLib::Vector3(DEFAULT_WIDTH, 0.0f, 0.0f));

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CHP_GaugePlayer::Uninit()
{
	for (int nCntGauge = 0; nCntGauge < VTXTYPE_MAX; nCntGauge++)
	{
		m_pObj2DGauge[nCntGauge] = nullptr;
	}

	if (m_pTip != nullptr)
	{
		m_pTip->Uninit();
		m_pTip = nullptr;
	}

	// 情報削除
	Release();
}

//==========================================================================
// 終了処理
//==========================================================================
void CHP_GaugePlayer::Kill()
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

	if (m_pTip != nullptr)
	{
		m_pTip->Kill();
		m_pTip = nullptr;
	}

	// 情報削除
	Release();
}

//==========================================================================
// 更新処理
//==========================================================================
void CHP_GaugePlayer::Update()
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


	for (int i = 0; i < VTXTYPE::VTXTYPE_MAX; i++)
	{
		// サイズ取得
		D3DXVECTOR2 size = m_pObj2DGauge[i]->GetSize();

		D3DXVECTOR2* pTex = m_pObj2DGauge[i]->GetTex();

		float ratio = size.x / LENGTH_TEXTUREREPEAT;

		pTex[1] = D3DXVECTOR2(ratio, 0.0f);
		pTex[3] = D3DXVECTOR2(ratio, 1.0f);

		SetVtx(i);
	}


	// 先端
	if (m_pTip != nullptr) {
		MyLib::Vector3 left, right;
		float maxlen = m_pObj2DGauge[0]->GetMaxWidth();

		left = pos - MyLib::Vector3(maxlen, 0.0f, 0.0f);
		right = pos + MyLib::Vector3(maxlen, 0.0f, 0.0f);
		m_pTip->SetLeftPosition(left);
		m_pTip->SetRightPosition(right);
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

	float ratio = ((float)m_nLifeValue / (float)m_nMaxLifeValue);
	if (ratio <= 0.3f &&
		m_pObj2DGauge[VTXTYPE_PINK] != nullptr)
	{
		m_pObj2DGauge[VTXTYPE_PINK]->SetColor(D3DXCOLOR(1.0f, 0.2f, 0.2f, 1.0f));
	}
	else
	{
		m_pObj2DGauge[VTXTYPE_PINK]->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	}
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

	// 体力の最大値上昇
	m_nMaxLifeValue = m_nOriginLifeValue + addvalue;

	for (const auto& gauge : m_pObj2DGauge)
	{
		SetPosition(gauge->UpgradeMaxValue(addvalue, false));
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
void CHP_GaugePlayer::Draw()
{

}

void CHP_GaugePlayer::SetVtx(int nIdx)
{
	// 頂点設定
	m_pObj2DGauge[nIdx]->SetVtx();

	// 位置取得
	MyLib::Vector3 pos = GetPosition();

	D3DXVECTOR2* pTex = m_pObj2DGauge[nIdx]->GetTex();

	// 頂点情報へのポインタ
	VERTEX_2D* pVtx;

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_pObj2DGauge[nIdx]->GetVtxBuff()->Lock(0, 0, (void**)&pVtx, 0);

	// サイズ取得
	D3DXVECTOR2 size = m_pObj2DGauge[nIdx]->GetSize();

	// 頂点座標の設定
	pVtx[0].tex = pTex[0];
	pVtx[1].tex = pTex[1];
	pVtx[2].tex = pTex[2];
	pVtx[3].tex = pTex[3];

	// 頂点バッファをアンロックロック
	m_pObj2DGauge[nIdx]->GetVtxBuff()->Unlock();
}
