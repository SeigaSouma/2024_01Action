//=============================================================================
// 
//  ボスの体力ゲージ処理 [hp_gauge_boss.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "hp_gauge_boss.h"
#include "manager.h"
#include "calculation.h"

//==========================================================================
// 定数定義
//==========================================================================
namespace
{
	const char* TEXTURE[] =	// テクスチャのファイル
	{
		"data\\TEXTURE\\bossgauge\\bossHP_base.png",
		"data\\TEXTURE\\bossgauge\\bossHP_white.png",
		"data\\TEXTURE\\bossgauge\\bossHP_gauge.png",
		"data\\TEXTURE\\bossgauge\\bossHP_fram.png",
	};
	const float TIME_DECREMENT = 2.0f;	// 減少完了までの時間
	const float TIME_DAMAGE = 0.3f;		// ダメージ状態の時間
	const float LENGTH_DAMAGE = 9.0f;		// ダメージ状態の長さ
	const int NUM_SHAKE_DAMAGE = 4;		// ダメージの振動回数
}

//==========================================================================
// 関数リスト
//==========================================================================
CHP_GaugeBoss::STATE_FUNC CHP_GaugeBoss::m_StateFuncList[] =
{
	&CHP_GaugeBoss::StateNone,		// 通常
	&CHP_GaugeBoss::StateDamage,	// ダメージ
};

//==========================================================================
// コンストラクタ
//==========================================================================
CHP_GaugeBoss::CHP_GaugeBoss(int nPriority) : CObject(nPriority)
{
	// 値のクリア
	for (int nCntGauge = 0; nCntGauge < VTXTYPE_MAX; nCntGauge++)
	{
		m_HPGauge[nCntGauge].pObj2D = nullptr;
		m_HPGauge[nCntGauge].fMaxWidth = 0.0f;		// 幅の最大値
		m_HPGauge[nCntGauge].fMaxHeight = 0.0f;		// 高さの最大値
		m_HPGauge[nCntGauge].fWidthDest = 0.0f;		// 幅の差分
	}

	m_nLife = 0;					// 体力
	m_nMaxLife = 0;					// 最大体力
	m_fDecrementTimer = 0.0f;		// 減少タイマー
	m_state = STATE::STATE_NONE;	// 状態
	m_fStateTimer = 0.0f;			// 状態タイマー
}

//==========================================================================
// デストラクタ
//==========================================================================
CHP_GaugeBoss::~CHP_GaugeBoss()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CHP_GaugeBoss* CHP_GaugeBoss::Create(MyLib::Vector3 pos, int nMaxLife)
{

	// メモリの確保
	CHP_GaugeBoss* pHPGauge = DEBUG_NEW CHP_GaugeBoss;

	if (pHPGauge != nullptr)
	{// メモリの確保が出来ていたら

		// 最大体力
		pHPGauge->m_nMaxLife = nMaxLife;

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
HRESULT CHP_GaugeBoss::Init()
{
	// 種類設定
	SetType(CObject::TYPE_OBJECT2D);

	// 最大体力
	m_nLife = m_nMaxLife;

	for (int nCntGauge = 0; nCntGauge < VTXTYPE_MAX; nCntGauge++)
	{
		// 生成処理
		m_HPGauge[nCntGauge].pObj2D = CObject2D::Create(8);
		if (m_HPGauge[nCntGauge].pObj2D == nullptr)
		{
			return E_FAIL;
		}
		m_HPGauge[nCntGauge].pObj2D->SetType(CObject::TYPE::TYPE_NONE);

		// テクスチャの割り当て
		int nTexIdx = CTexture::GetInstance()->Regist(TEXTURE[nCntGauge]);

		// テクスチャの割り当て
		m_HPGauge[nCntGauge].pObj2D->BindTexture(nTexIdx);
	}


	int nTexIdx = CTexture::GetInstance()->Regist(TEXTURE[0]);

	// サイズ
	D3DXVECTOR2 size = CTexture::GetInstance()->GetImageSize(nTexIdx);
	size = UtilFunc::Transformation::AdjustSizeByWidth(size, 350.0f);
	m_HPGauge[VTXTYPE::VTXTYPE_GAUGE].pObj2D->SetSize(size);

	// サイズ
	D3DXVECTOR2 maxsize = m_HPGauge[VTXTYPE::VTXTYPE_GAUGE].pObj2D->GetSize();

	for (auto& info : m_HPGauge)
	{
		info.pObj2D->SetSize(size);

		// 各種変数の初期化
		info.fMaxWidth = maxsize.x;		// 幅の最大値
		info.fMaxHeight = maxsize.y;	// 高さの最大値
	}

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CHP_GaugeBoss::Uninit()
{
	for (int nCntGauge = 0; nCntGauge < VTXTYPE_MAX; nCntGauge++)
	{
		if (m_HPGauge[nCntGauge].pObj2D != nullptr)
		{
			// 終了処理
			m_HPGauge[nCntGauge].pObj2D->Uninit();
			m_HPGauge[nCntGauge].pObj2D = nullptr;
		}
	}

	// 情報削除
	Release();
}

//==========================================================================
// 終了処理
//==========================================================================
void CHP_GaugeBoss::Kill()
{
	for (int nCntGauge = 0; nCntGauge < VTXTYPE_MAX; nCntGauge++)
	{
		if (m_HPGauge[nCntGauge].pObj2D != nullptr)
		{
			// 終了処理
			m_HPGauge[nCntGauge].pObj2D->Uninit();
			m_HPGauge[nCntGauge].pObj2D = nullptr;
		}
	}


	// 情報削除
	Release();
}

//==========================================================================
// 更新処理
//==========================================================================
void CHP_GaugeBoss::Update()
{
	// 状態別処理
	(this->*(m_StateFuncList[m_state]))();

	// 減少処理
	GaugeDecrement(VTXTYPE_GAUGE);
	AlwaysDecrement();

	// 位置取得
	MyLib::Vector3 pos = GetPosition();
	for (int nCntGauge = 0; nCntGauge < VTXTYPE_MAX; nCntGauge++)
	{
		if (m_HPGauge[nCntGauge].pObj2D == nullptr)
		{
			continue;
		}

		// 位置設定
		m_HPGauge[nCntGauge].pObj2D->SetPosition(pos);

		// 更新処理
		m_HPGauge[nCntGauge].pObj2D->Update();

		// 頂点座標設定
		SetVtx(nCntGauge);
	}

}

//==========================================================================
// 位置更新
//==========================================================================
void CHP_GaugeBoss::SetLife(int nLife)
{
	// 現在の体力設定
	m_nLife = nLife;

	float ratio = ((float)m_nLife / (float)m_nMaxLife);

	if (ratio <= 0.3f &&
		m_HPGauge[VTXTYPE::VTXTYPE_GAUGE].pObj2D != nullptr)
	{
		m_HPGauge[VTXTYPE::VTXTYPE_GAUGE].pObj2D->SetColor(D3DXCOLOR(1.0f, 0.1f, 0.1f, 1.0f));
	}

	for (int nCntGauge = 0; nCntGauge < VTXTYPE_MAX; nCntGauge++)
	{
		if (m_HPGauge[nCntGauge].pObj2D != nullptr)
		{
			//目標の幅設定
			m_HPGauge[nCntGauge].fWidthDest = m_HPGauge[nCntGauge].fMaxWidth * ratio;
		}
	}
}

//==========================================================================
// 減少処理
//==========================================================================
void CHP_GaugeBoss::GaugeDecrement(int nCntGauge)
{

	// サイズ取得
	D3DXVECTOR2 size = m_HPGauge[nCntGauge].pObj2D->GetSize();

	// 差分で徐々に減らしていく
	size.x += (m_HPGauge[nCntGauge].fWidthDest - size.x) * 1.0f;

	// サイズ設定
	m_HPGauge[nCntGauge].pObj2D->SetSize(size);

	// 頂点座標設定
	SetVtx(nCntGauge);
}

//==========================================================================
// 常に減少の処理
//==========================================================================
void CHP_GaugeBoss::AlwaysDecrement()
{
	SHP_Gauge* gauge = &m_HPGauge[VTXTYPE::VTXTYPE_DIFFGAUGE];

	// 減少時間減算
	m_fDecrementTimer -= CManager::GetInstance()->GetDeltaTime();

	// サイズ取得
	D3DXVECTOR2 size = gauge->pObj2D->GetSize();
	size.x = UtilFunc::Correction::EasingLinear(m_fDecrementStart, m_fDecrementEnd, 0.0f, TIME_DECREMENT, TIME_DECREMENT - m_fDecrementTimer);

	if (size.x <= m_HPGauge[VTXTYPE::VTXTYPE_GAUGE].fWidthDest)
	{
		size.x = m_HPGauge[VTXTYPE::VTXTYPE_GAUGE].fWidthDest;
	}

	// サイズ設定
	gauge->pObj2D->SetSize(size);

	// 頂点座標設定
	SetVtx(VTXTYPE::VTXTYPE_DIFFGAUGE);
}

//==========================================================================
// ダメージ設定
//==========================================================================
void CHP_GaugeBoss::SetDamage(int nLife)
{
	m_nLife = nLife;

	// タイマーリセット
	m_fDecrementTimer = TIME_DECREMENT;

	// 白の長さ設定
	float ratio = ((float)m_nLife / (float)m_nMaxLife);
	m_fDecrementStart = m_HPGauge[VTXTYPE::VTXTYPE_DIFFGAUGE].pObj2D->GetSize().x;
	m_fDecrementEnd = m_HPGauge[VTXTYPE::VTXTYPE_GAUGE].fMaxWidth * ratio;

	// ダメージ状態設定
	m_state = STATE::STATE_DAMAGE;
	m_fStateTimer = TIME_DAMAGE;
}

//==========================================================================
// ダメージ
//==========================================================================
void CHP_GaugeBoss::StateDamage()
{
	// 状態時間減算
	m_fStateTimer -= CManager::GetInstance()->GetDeltaTime();

	if (m_fStateTimer <= 0.0f)
	{
		m_state = STATE_NONE;
		m_fStateTimer = 0.0f;

		// 位置設定
		SetPosition(GetOriginPosition());
		return;
	}

	float ratio = m_fStateTimer / TIME_DAMAGE;
	float len = LENGTH_DAMAGE * ratio;
	float phai = D3DX_PI * (ratio * NUM_SHAKE_DAMAGE);

	MyLib::Vector3 pos = GetOriginPosition();
	pos.x += sinf(D3DX_PI * 0.75f) * (sinf(phai) * len);
	pos.y += cosf(D3DX_PI * 0.75f) * (sinf(phai) * len);
	SetPosition(pos);

}

//==========================================================================
// 描画処理
//==========================================================================
void CHP_GaugeBoss::Draw()
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();

	for (int nCntGauge = 0; nCntGauge < VTXTYPE_MAX; nCntGauge++)
	{
		// 描画処理
		m_HPGauge[nCntGauge].pObj2D->Draw();
	}
}

//==========================================================================
// 頂点情報設定処理
//==========================================================================
void CHP_GaugeBoss::SetVtx(int nCntGauge)
{
	// 頂点設定
	m_HPGauge[nCntGauge].pObj2D->SetVtx();

	// 位置取得
	MyLib::Vector3 pos = GetPosition();

	D3DXVECTOR2 *pTex = m_HPGauge[nCntGauge].pObj2D->GetTex();

	// 頂点情報へのポインタ
	VERTEX_2D *pVtx;

	// 頂点バッファをロックし、頂点情報へのポインタを取得
	m_HPGauge[nCntGauge].pObj2D->GetVtxBuff()->Lock(0, 0, (void**)&pVtx, 0);

	// サイズ取得
	D3DXVECTOR2 size = m_HPGauge[nCntGauge].pObj2D->GetSize();

	// 頂点座標の設定
	pVtx[0].pos = MyLib::Vector3(pos.x + -size.x - (m_HPGauge[nCntGauge].fMaxWidth - size.x),	pos.y + -size.y, 0.0f);
	pVtx[1].pos = MyLib::Vector3(pos.x + size.x - (m_HPGauge[nCntGauge].fMaxWidth - size.x),	pos.y + -size.y, 0.0f);
	pVtx[2].pos = MyLib::Vector3(pos.x + -size.x - (m_HPGauge[nCntGauge].fMaxWidth - size.x),	pos.y + size.y, 0.0f);
	pVtx[3].pos = MyLib::Vector3(pos.x + size.x - (m_HPGauge[nCntGauge].fMaxWidth - size.x),	pos.y + size.y, 0.0f);

	pTex[0] = D3DXVECTOR2(0.0f, 0.0f);
	pTex[1] = D3DXVECTOR2(size.x / m_HPGauge[nCntGauge].fMaxWidth, 0.0f);
	pTex[2] = D3DXVECTOR2(0.0f, 1.0f);
	pTex[3] = D3DXVECTOR2(size.x / m_HPGauge[nCntGauge].fMaxWidth, 1.0f);

	// 頂点バッファをアンロックロック
	m_HPGauge[nCntGauge].pObj2D->GetVtxBuff()->Unlock();

}
