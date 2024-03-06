//=============================================================================
// 
//  プレイヤーのスタミナゲージ処理 [stamina_gauge_player.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "stamina_gauge_player.h"
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
	const float DEFAULT_AUTOHEAL = 0.15f;	// デフォルトの自動回復
	const float DEFAULT_WIDTH = 150.0f;		// デフォルトの幅
	const float DEFAULT_HEIGHT = 18.0;		// デフォルトの高さ
	const float TIME_QUICKHEAL = 2.6f;		// 急速回復の時間
	const float TIME_STATESUB = static_cast<float>(50) / static_cast<float>(mylib_const::DEFAULT_FPS);		// 減算状態の時間
	const char* TEXTURE[] =		// テクスチャのファイル
	{
		"data\\TEXTURE\\hpgauge\\black.png",
		"data\\TEXTURE\\hpgauge\\staminagauhge.png",
	};
	const float LENGTH_TEXTUREREPEAT = 18.0f;	// テクスチャがリピートする長さ
}

//==========================================================================
// 関数ポインタ
//==========================================================================
CStaminaGauge_Player::STATE_FUNC CStaminaGauge_Player::m_StateFunc[] =
{
	&CStaminaGauge_Player::StateNormal,	// 通常
	&CStaminaGauge_Player::StateSub,	// 減算
	&CStaminaGauge_Player::StateQuickHealing,	// 急速回復
};

//==========================================================================
// コンストラクタ
//==========================================================================
CStaminaGauge_Player::CStaminaGauge_Player(int nPriority) : CObject(nPriority)
{
	// 値のクリア
	memset(m_pObj2DGauge, 0, sizeof(m_pObj2DGauge));	// 2Dゲージのオブジェクト
	m_state = STATE_NORMAL;		// 状態
	m_fStateTime = 0.0f;		// 状態のカウンター
	m_fStaminaValue = 0.0f;		// スタミナの値
	m_fMaxStaminaValue = 0.0f;	// スタミナの最大値
	m_fOriginStaminaValue = 0.0f;	// スタミナの初期値
	m_fAutoHeal = 0.0f;			// 自動回復
	m_pTip = nullptr;	// ゲージの先端
}

//==========================================================================
// デストラクタ
//==========================================================================
CStaminaGauge_Player::~CStaminaGauge_Player()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CStaminaGauge_Player *CStaminaGauge_Player::Create(MyLib::Vector3 pos, int maxvalue)
{
	// 生成用のオブジェクト
	CStaminaGauge_Player *pHPGauge = nullptr;

	if (pHPGauge == nullptr)
	{// nullptrだったら

		// メモリの確保
		pHPGauge = DEBUG_NEW CStaminaGauge_Player;

		if (pHPGauge != nullptr)
		{// メモリの確保が出来ていたら

			pHPGauge->m_fMaxStaminaValue = static_cast<float>(maxvalue);	// スタミナの値
			pHPGauge->m_fOriginStaminaValue = static_cast<float>(maxvalue);	// スタミナの初期値

			// 位置設定
			pHPGauge->SetPosition(pos);
			pHPGauge->SetOriginPosition(pos);

			// 初期化処理
			pHPGauge->Init();
		}

		return pHPGauge;
	}

	return nullptr;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CStaminaGauge_Player::Init()
{
	// 各種変数の初期化処理
	m_fStaminaValue = m_fMaxStaminaValue;	// スタミナの値
	m_fAutoHeal = DEFAULT_AUTOHEAL;			// 自動回復

	// 種類の設定
	SetType(TYPE_HPGAUGE);
	
	for (int nCntGauge = 0; nCntGauge < VTXTYPE_MAX; nCntGauge++)
	{
		// 生成処理
		m_pObj2DGauge[nCntGauge] = CObject2D_Gauge::Create(DEFAULT_WIDTH, DEFAULT_HEIGHT, static_cast<int>(m_fMaxStaminaValue), TEXTURE[nCntGauge], GetPriority());
		if (m_pObj2DGauge[nCntGauge] == nullptr)
		{
			return E_FAIL;
		}

		m_pObj2DGauge[nCntGauge]->SetOriginPosition(GetPosition());
		
		// 種類の設定
		m_pObj2DGauge[nCntGauge]->SetType(CObject::TYPE_OBJECT2D);
		m_pObj2DGauge[nCntGauge]->SetSize(D3DXVECTOR2(DEFAULT_WIDTH, DEFAULT_HEIGHT));
		m_pObj2DGauge[nCntGauge]->SetMoveFactor(0.25f);	// 移動の係数設定
	}

	// 値設定
	SetValue(m_fMaxStaminaValue);

	// 先端生成
	m_pTip = CHPGaugeTip::Create(GetPosition() - MyLib::Vector3(DEFAULT_WIDTH, 0.0f, 0.0f), GetPosition() + MyLib::Vector3(DEFAULT_WIDTH, 0.0f, 0.0f));
	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CStaminaGauge_Player::Uninit()
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
void CStaminaGauge_Player::Kill()
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
void CStaminaGauge_Player::Update()
{
	// 位置取得
	MyLib::Vector3 pos = GetPosition();

	for (const auto& gauge : m_pObj2DGauge)
	{
		gauge->SetPosition(pos);
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

	// 先端生成
	if (m_pTip != nullptr) {
		MyLib::Vector3 left, right;
		float maxlen = m_pObj2DGauge[0]->GetMaxWidth();

		left = pos - MyLib::Vector3(maxlen, 0.0f, 0.0f);
		right = pos + MyLib::Vector3(maxlen, 0.0f, 0.0f);
		m_pTip->SetLeftPosition(left);
		m_pTip->SetRightPosition(right);
	}

	// 状態更新
	(this->*(m_StateFunc[m_state]))();
}

//==========================================================================
// 通常状態
//==========================================================================
void CStaminaGauge_Player::StateNormal()
{
	// カウンターリセット
	m_fStateTime = 0.0f;

	// 自動回復
	AddValue(m_fAutoHeal);
}

//==========================================================================
// 減算状態
//==========================================================================
void CStaminaGauge_Player::StateSub()
{
	// カウンター減算
	m_fStateTime -= CManager::GetInstance()->GetDeltaTime();
	if (m_fStateTime <= 0.0f)
	{
		m_state = STATE_NORMAL;
	}
}

//==========================================================================
// 急速回復状態
//==========================================================================
void CStaminaGauge_Player::StateQuickHealing()
{
	// カウンターリセット
	m_fStateTime += CManager::GetInstance()->GetDeltaTime();
	UtilFunc::Transformation::ValueNormalize(m_fStateTime, TIME_QUICKHEAL, 0.0f);

	SetValue((m_fStateTime / TIME_QUICKHEAL) * m_fMaxStaminaValue);

	if (m_fMaxStaminaValue == m_fStaminaValue)
	{
		m_state = STATE_NORMAL;
	}
}

//==========================================================================
// 色更新
//==========================================================================
void CStaminaGauge_Player::ChangeColor()
{
	CObject2D_Gauge* pGauge = m_pObj2DGauge[VTXTYPE_GAUGE];

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
	pGauge->SetColor(col);

}

//==========================================================================
// 描画処理
//==========================================================================
void CStaminaGauge_Player::Draw()
{

}

//==========================================================================
// 値加算
//==========================================================================
void CStaminaGauge_Player::AddValue(float value)
{
	m_fStaminaValue += value;

	// 値の補正
	CorrectionValue();
}

//==========================================================================
// 値減算
//==========================================================================
void CStaminaGauge_Player::SubValue(float value)
{
	// スタミナ減算
	m_fStaminaValue -= value;

	// 減算状態に設定
	m_state = STATE_SUB;
	m_fStateTime = TIME_STATESUB;

	// 値の補正
	CorrectionValue();
}

//==========================================================================
// 値設定
//==========================================================================
void CStaminaGauge_Player::SetValue(float value)
{

	m_fStaminaValue = value;
	
	// 値の補正
	CorrectionValue();
}

//==========================================================================
// 最大値のアップグレード
//==========================================================================
void CStaminaGauge_Player::UpgradeMaxValue(int addvalue)
{
	// スタミナの最大値上昇
	m_fMaxStaminaValue = m_fOriginStaminaValue + addvalue;

	for (const auto& gauge : m_pObj2DGauge)
	{
		SetPosition(gauge->UpgradeMaxValue(addvalue, false));
	}
}

//==========================================================================
// 自動回復のアップグレード
//==========================================================================
void CStaminaGauge_Player::UpgradeAutoHeal(float multiply)
{
	// 自動回復強化
	m_fAutoHeal = DEFAULT_AUTOHEAL * multiply;
}

//==========================================================================
// 値の補正
//==========================================================================
void CStaminaGauge_Player::CorrectionValue()
{
	UtilFunc::Transformation::ValueNormalize(m_fStaminaValue, m_fMaxStaminaValue, 0.0f);

	m_pObj2DGauge[VTXTYPE_GAUGE]->SetValue(static_cast<int>(m_fStaminaValue));
}

//==========================================================================
// 値取得
//==========================================================================
float CStaminaGauge_Player::GetValue()
{
	return m_fStaminaValue;
}

//==========================================================================
// 状態設定
//==========================================================================
void CStaminaGauge_Player::SetState(STATE state)
{ 
	m_fStateTime = 0.0f;
	m_state = state;
}

//==========================================================================
// 状態設定
//==========================================================================
void CStaminaGauge_Player::SetVtx(int nIdx)
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
