//=============================================================================
// 
//  総合戦果処理 [battleresult_Overall.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "battleresult_Overall.h"
#include "manager.h"
#include "calculation.h"
#include "game.h"
#include "input.h"
#include "2D_effect.h"
#include "player.h"

//==========================================================================
// 定数定義
//==========================================================================
namespace
{
	const char* TEXTURE_NUMBER = "data\\TEXTURE\\number\\number_oradano.png";		// 数字のテクスチャ
	const char* TEXTURE_RANK[] =	// ランクのテクスチャ
	{ 
		"data\\TEXTURE\\battleresult\\rank_B.png",	// B
		"data\\TEXTURE\\battleresult\\rank_A.png",	// A
		"data\\TEXTURE\\battleresult\\rank_S.png",	// S
	};
	const MyLib::Vector3 POSITION_RANK = MyLib::Vector3(535.0f, 560.0f, 0.0f);

	const MyLib::Vector3 POSITION_POINT[] =
	{
		MyLib::Vector3(652.0f, 600.0f, 0.0f),	// クリア時間の位置
		MyLib::Vector3(922.0f, 600.0f, 0.0f),	// 被ダメージの位置
	};

	const D3DXVECTOR2 SIZE_RANK = D3DXVECTOR2(70.0f, 70.0f);
	const D3DXVECTOR2 SIZE_NUMBER = D3DXVECTOR2(30.0f, 30.0f);

	const float TIME_FADEIN = 0.4f;		// フェードイン
	const float TIME_WAIT_POINT = 0.5f;	// ポイント入場待機
	const float TIME_FADEIN_POINT = 0.3f;		// フェードイン
	const float TIME_FADEOUT = 0.3f;	// フェードアウト
}

//==========================================================================
// 関数リスト
//==========================================================================
CBattleResult_Overall::STATE_FUNC CBattleResult_Overall::m_StateFuncList[] =
{
	&CBattleResult_Overall::StateNone,		// なにもなし
	&CBattleResult_Overall::StateFadeIn,	// フェードイン
	&CBattleResult_Overall::StateRankWait,	// ポイント入場待ち
	&CBattleResult_Overall::StateFadeIn_Point,	// フェードイン
	&CBattleResult_Overall::StateFadeOut,	// フェードアウト
};

//==========================================================================
// コンストラクタ
//==========================================================================
CBattleResult_Overall::CBattleResult_Overall(int nPriority) : CObject2D(nPriority)
{
	// 値のクリア
	m_state = STATE::STATE_FADEIN;	// 状態
	m_fStateTimer = 0.0f;			// 状態タイマー
	m_bCompleteStaging = false;		// 演出完了フラグ
}

//==========================================================================
// デストラクタ
//==========================================================================
CBattleResult_Overall::~CBattleResult_Overall()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CBattleResult_Overall* CBattleResult_Overall::Create(CGameRating::RATING rank, int prevPoint, int currentPoint)
{
	// メモリの確保
	CBattleResult_Overall* pMarker = DEBUG_NEW CBattleResult_Overall;

	if (pMarker != nullptr)
	{
		pMarker->m_nPoint[NUMBERTYPE::NUMBERTYPE_PREV] = prevPoint;			// 前回ポイント
		pMarker->m_nPoint[NUMBERTYPE::NUMBERTYPE_CURRENT] = currentPoint;	// 今回ポイント
		pMarker->m_OverallRating = rank;

		// 初期化処理
		pMarker->Init();
	}

	return pMarker;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CBattleResult_Overall::Init()
{

	// オブジェクト2Dの初期化
	CObject2D::Init();

	// テクスチャ設定
	int windowTex = CTexture::GetInstance()->Regist(TEXTURE_RANK[m_OverallRating]);
	BindTexture(windowTex);

	// 位置設定
	SetPosition(POSITION_RANK);

	// サイズ設定
	SetSize(SIZE_RANK);
	SetSizeOrigin(SIZE_RANK * 3.0f);

	// ポイント差分生成
	CreatePoint();

	// 状態設定
	m_state = STATE_FADEIN;
	m_fStateTimer = 0.0f;
	SetAlpha(0.0f);

	return S_OK;
}

//==========================================================================
// ポイント差分生成
//==========================================================================
void CBattleResult_Overall::CreatePoint()
{
	// 今回の評価情報取得
	CGameRating* pRating = CGame::GetInstance()->GetGameManager()->GetGameRating();
	CGameRating::sRating ratingInfo = pRating->GetRatingInfo();

	for (int i = 0; i < NUMBERTYPE::NUMBERTYPE_MAX; i++)
	{
		// 生成
		m_pDiffPoint[i] = CMultiNumber::Create(
			POSITION_POINT[i],
			SIZE_NUMBER,
			2,
			CNumber::EObjectType::OBJECTTYPE_2D,
			TEXTURE_NUMBER, false, GetPriority());
		if (m_pDiffPoint[i] == nullptr)
		{
			continue;
		}
		CMultiNumber* pNumber = m_pDiffPoint[i];

		// 右寄せに設定
		pNumber->SetAlignmentType(CMultiNumber::AlignmentType::ALIGNMENT_RIGHT);

		pNumber->SetValue(m_nPoint[i]);

		// 色設定
		pNumber->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));
	}
}

//==========================================================================
// 終了処理
//==========================================================================
void CBattleResult_Overall::Uninit()
{
	
	CObject2D::Uninit();
}

//==========================================================================
// 削除
//==========================================================================
void CBattleResult_Overall::Kill()
{
	for (int i = 0; i < NUMBERTYPE::NUMBERTYPE_MAX; i++)
	{
		if (m_pDiffPoint[i] != nullptr)
		{
			m_pDiffPoint[i]->Release();
			m_pDiffPoint[i] = nullptr;
		}
	}

	CObject2D::Uninit();
}

//==========================================================================
// 更新処理
//==========================================================================
void CBattleResult_Overall::Update()
{
	// 状態タイマー加算
	m_fStateTimer += CManager::GetInstance()->GetDeltaTime();

	// 状態別処理
	(this->*(m_StateFuncList[m_state]))();
	if (IsDeath()){
		return;
	}

	// 今回の評価情報取得
	CGameRating* pRating = CGame::GetInstance()->GetGameManager()->GetGameRating();
	CGameRating::sRating ratingInfo = pRating->GetRatingInfo();

	for (int i = 0; i < NUMBERTYPE::NUMBERTYPE_MAX; i++)
	{
		if (m_pDiffPoint[i] == nullptr)
		{
			continue;
		}

		m_pDiffPoint[i]->Update();
	}

	// 更新処理
	CObject2D::Update();
}

//==========================================================================
// 全て設定
//==========================================================================
void CBattleResult_Overall::AllSetting()
{
	SetAlpha(1.0f);
	SetSize(SIZE_RANK);

	for (int i = 0; i < NUMBERTYPE::NUMBERTYPE_MAX; i++)
	{
		if (m_pDiffPoint[i] == nullptr)
		{
			continue;
		}
		// 色設定
		m_pDiffPoint[i]->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	}

	// 演出完了フラグ
	m_bCompleteStaging = true;
	m_state = STATE::STATE_NONE;
	m_fStateTimer = 0.0f;
}

//==========================================================================
// 何もなし
//==========================================================================
void CBattleResult_Overall::StateNone()
{
	if (m_fStateTimer >= 0.4f)
	{
		if (m_OverallRating == CGameRating::RATING::RATING_S)
		{
			MyLib::Vector3 pos = GetPosition();
			pos.x += UtilFunc::Transformation::Random(-40, 40);
			pos.y += UtilFunc::Transformation::Random(-40, 40);

			D3DXCOLOR col;
			if (UtilFunc::Transformation::Random(0, 1) == 0) {
				col = D3DXCOLOR(0.9f, 0.9f, 1.0f, 0.5f);
			}
			else {
				col = D3DXCOLOR(0.1f, 0.1f, 1.0f, 0.5f);
			}

			// エフェクトの設定
			CEffect2D* pEffect = CEffect2D::Create(pos, 0.0f, col, static_cast<float>(UtilFunc::Transformation::Random(40, 60)),
				20,
				CEffect2D::MOVEEFFECT::MOVEEFFECT_SUB, CEffect2D::TYPE::TYPE_JUJI3);
			pEffect->SetRotation(MyLib::Vector3(0.0f, 0.0f, UtilFunc::Transformation::GetRandomPi()));
		}

		m_fStateTimer = UtilFunc::Transformation::Random(0, 30) * 0.01f;
	}
}

//==========================================================================
// フェードイン
//==========================================================================
void CBattleResult_Overall::StateFadeIn()
{
	// サイズ取得
	D3DXVECTOR2 size = GetSize();
	D3DXVECTOR2 sizeOrigin = GetSizeOrigin();
	D3DXVECTOR2 destSize = UtilFunc::Transformation::AdjustSizeByWidth(size, SIZE_RANK.x);

	// 徐々に減速
	float len = UtilFunc::Correction::EasingEaseOut(sizeOrigin.x, SIZE_RANK.x, 0.0f, TIME_FADEIN, m_fStateTimer);
	SetSize(D3DXVECTOR2(len, len));

	// 不透明度設定
	float alpha = UtilFunc::Correction::EasingLinear(0.0f, 1.0f, 0.0f, TIME_FADEIN, m_fStateTimer);
	SetAlpha(alpha);

	if (m_fStateTimer >= TIME_FADEIN)
	{
		m_state = STATE::STATE_WAIT_POINT;
		m_fStateTimer = 0.0f;
		SetAlpha(1.0f);
		SetSize(SIZE_RANK);
		CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL::LABEL_SE_BATTLERESULT_RANK);
		return;
	}
}

//==========================================================================
// ランク入場待ち
//==========================================================================
void CBattleResult_Overall::StateRankWait()
{
	if (m_fStateTimer >= TIME_WAIT_POINT)
	{
		m_state = STATE::STATE_FADEIN_POINT;
		m_fStateTimer = 0.0f;
	}
}

//==========================================================================
// ポイントフェードイン
//==========================================================================
void CBattleResult_Overall::StateFadeIn_Point()
{
	// 不透明度設定
	float alpha = UtilFunc::Correction::EasingLinear(0.0f, 1.0f, 0.0f, TIME_FADEIN_POINT, m_fStateTimer);

	for (int i = 0; i < NUMBERTYPE::NUMBERTYPE_MAX; i++)
	{
		if (m_pDiffPoint[i] == nullptr)
		{
			continue;
		}
		CMultiNumber* pNumber = m_pDiffPoint[i];

		// 色設定
		pNumber->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, alpha));
	}

	if (m_fStateTimer >= TIME_FADEIN_POINT)
	{
		m_state = STATE::STATE_NONE;
		m_fStateTimer = 0.0f;

		for (int i = 0; i < NUMBERTYPE::NUMBERTYPE_MAX; i++)
		{
			if (m_pDiffPoint[i] == nullptr)
			{
				continue;
			}
			// 色設定
			m_pDiffPoint[i]->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
		}

		// 演出完了フラグ
		m_bCompleteStaging = true;
		return;
	}
}

//==========================================================================
// フェードアウト
//==========================================================================
void CBattleResult_Overall::StateFadeOut()
{
	float alpha = UtilFunc::Correction::EasingLinear(1.0f, 0.0f, 0.0f, TIME_FADEOUT, m_fStateTimer);
	SetAlpha(alpha);

	for (int i = 0; i < NUMBERTYPE::NUMBERTYPE_MAX; i++)
	{
		if (m_pDiffPoint[i] == nullptr)
		{
			continue;
		}
		// 色設定
		m_pDiffPoint[i]->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, alpha));
	}

	if (m_fStateTimer >= TIME_FADEOUT)
	{
		Kill();
		return;
	}
}

//==========================================================================
// 描画処理
//==========================================================================
void CBattleResult_Overall::Draw()
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();

	// アルファテストを有効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);

	// オブジェクト2Dの描画
	CObject2D::Draw();

	// アルファテストを無効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);
}

//==========================================================================
// 描画処理
//==========================================================================
void CBattleResult_Overall::SetState(STATE state)
{
	m_state = state;
	m_fStateTimer = 0.0f;
}
