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

//==========================================================================
// 定数定義
//==========================================================================
namespace
{
	const float DEFAULT_AUTOHEAL = 0.15f;	// デフォルトの自動回復
	const float DEFAULT_WIDTH = 120.0f;		// デフォルトの幅
	const float DEFAULT_HEIGHT = 15.0f;		// デフォルトの高さ
	const float TIME_STATESUB = static_cast<float>(20) / static_cast<float>(mylib_const::DEFAULT_FPS);		// 減算状態の時間
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
CStaminaGauge_Player::STATE_FUNC CStaminaGauge_Player::m_StateFunc[] =
{
	&CStaminaGauge_Player::StateNormal,	// 通常
	&CStaminaGauge_Player::StateSub,	// 減算
};

//==========================================================================
// コンストラクタ
//==========================================================================
CStaminaGauge_Player::CStaminaGauge_Player(int nPriority) : CObject(nPriority)
{
	// 値のクリア
	for (int nCntGauge = 0; nCntGauge < VTXTYPE_MAX; nCntGauge++)
	{
		m_pObj2DGauge[nCntGauge] = nullptr;	// 2Dゲージのオブジェクト
	}
	m_state = STATE_NORMAL;		// 状態
	m_fStateTime = 0.0f;		// 状態のカウンター
	m_fStaminaValue = 0.0f;		// スタミナの値
	m_fMaxStaminaValue = 0.0f;	// スタミナの最大値
	m_fOriginStaminaValue = 0.0f;	// スタミナの初期値
	m_fAutoHeal = 0.0f;			// 自動回復
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
HRESULT CStaminaGauge_Player::Init(void)
{
	// 各種変数の初期化処理
	m_fStaminaValue = m_fMaxStaminaValue;	// スタミナの値
	m_fAutoHeal = DEFAULT_AUTOHEAL;			// 自動回復

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
		m_pObj2DGauge[nCntGauge] = CObject2D_Gauge::Create(DEFAULT_WIDTH, DEFAULT_HEIGHT, static_cast<int>(m_fMaxStaminaValue), TEXTURE[nCntGauge]);
		if (m_pObj2DGauge[nCntGauge] == nullptr)
		{// nullptrだったら
			return E_FAIL;
		}

		m_pObj2DGauge[nCntGauge]->SetOriginPosition(GetPosition());

		// 頂点カラーの設定
		m_pObj2DGauge[nCntGauge]->SetColor(col[nCntGauge]);
		
		// 種類の設定
		m_pObj2DGauge[nCntGauge]->SetType(CObject::TYPE_OBJECT2D);
		m_pObj2DGauge[nCntGauge]->SetSize(D3DXVECTOR2(DEFAULT_WIDTH, DEFAULT_HEIGHT));
		m_pObj2DGauge[nCntGauge]->SetMoveFactor(0.25f);	// 移動の係数設定
	}

	// 値設定
	SetValue(m_fMaxStaminaValue);
	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CStaminaGauge_Player::Uninit(void)
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
void CStaminaGauge_Player::Kill(void)
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
void CStaminaGauge_Player::Update(void)
{
	// 位置取得
	MyLib::Vector3 pos = GetPosition();

	for (const auto& gauge : m_pObj2DGauge)
	{
		gauge->SetPosition(pos);
	}

	// 状態更新
	(this->*(m_StateFunc[m_state]))();
}

//==========================================================================
// 通常状態
//==========================================================================
void CStaminaGauge_Player::StateNormal(void)
{
	// カウンターリセット
	m_fStateTime = 0.0f;

	// 自動回復
	AddValue(m_fAutoHeal);
}

//==========================================================================
// 減算状態
//==========================================================================
void CStaminaGauge_Player::StateSub(void)
{
	// カウンター減算
	m_fStateTime -= CManager::GetInstance()->GetDeltaTime();
	if (m_fStateTime <= 0.0f)
	{
		m_state = STATE_NORMAL;
	}
}

//==========================================================================
// 色更新
//==========================================================================
void CStaminaGauge_Player::ChangeColor(void)
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
void CStaminaGauge_Player::Draw(void)
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
#if 0
	// 最大値
	float oldMaxvalue = m_fMaxStaminaValue;
	m_fMaxStaminaValue += addvalue;
	float nowlen = DEFAULT_WIDTH * (m_fMaxStaminaValue / m_fOriginStaminaValue);

	for (const auto& gauge : m_pObj2DGauge)
	{
		// 最大値
		gauge->SetMaxValue(static_cast<int>(m_fMaxStaminaValue));

		// 最大の長さ
		gauge->SetMaxWidth(nowlen);

		// ゲージリセット
		D3DXVECTOR2 size = gauge->GetSize();
		size.x = nowlen;
		gauge->SetSize(size);

		gauge->SetValue(static_cast<int>(m_fMaxStaminaValue));
	}

	// 元の長さとの差分
	float difflen = nowlen - DEFAULT_WIDTH;

	// 位置設定
	MyLib::Vector3 newpos = GetOriginPosition();
	newpos.x += difflen;
	SetPosition(newpos);

	for (const auto& gauge : m_pObj2DGauge)
	{
		gauge->SetPosition(newpos);
	}
#else

	// スタミナの最大値上昇
	m_fMaxStaminaValue = m_fOriginStaminaValue + addvalue;

	for (const auto& gauge : m_pObj2DGauge)
	{
		SetPosition(gauge->UpgradeMaxValue(addvalue));
	}
	
#endif
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
void CStaminaGauge_Player::CorrectionValue(void)
{
	UtilFunc::Transformation::ValueNormalize(m_fStaminaValue, m_fMaxStaminaValue, 0.0f);

	m_pObj2DGauge[VTXTYPE_GAUGE]->SetValue(static_cast<int>(m_fStaminaValue));
}

//==========================================================================
// 値取得
//==========================================================================
float CStaminaGauge_Player::GetValue(void)
{
	return m_fStaminaValue;
}