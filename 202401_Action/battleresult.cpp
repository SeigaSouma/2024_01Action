//=============================================================================
// 
//  戦闘準備処理 [battleresult.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "battleresult.h"
#include "manager.h"
#include "calculation.h"
#include "game.h"
#include "input.h"
#include "battlestart.h"

//==========================================================================
// 定数定義
//==========================================================================
namespace
{
	const char* TEXTURE_WINDOW = "data\\TEXTURE\\battleresult\\resultwindow.png";		// ウィンドウのテクスチャ
	const char* TEXTURE_NUMBER = "data\\TEXTURE\\number\\number_oradano.png";		// 数字のテクスチャ
	const char* TEXTURE_RANK[] =	// ランクのテクスチャ
	{ 
		"data\\TEXTURE\\battleresult\\rank_B.png",	// B
		"data\\TEXTURE\\battleresult\\rank_A.png",	// A
		"data\\TEXTURE\\battleresult\\rank_S.png",	// S
	};
	const MyLib::Vector3 POSITION_RANK[] =
	{
		MyLib::Vector3(990.0f, 270.0f, 0.0f),	// クリア時間の位置
		MyLib::Vector3(990.0f, 370.0f, 0.0f),	// 被ダメージの位置
		MyLib::Vector3(990.0f, 470.0f, 0.0f),	// 死亡回数の位置
	};

	const MyLib::Vector3 POSITION_SCORE[] =
	{
		MyLib::Vector3(880.0f, 237.0f, 0.0f),	// クリア時間の位置
		MyLib::Vector3(880.0f, 337.0f, 0.0f),	// 被ダメージの位置
		MyLib::Vector3(880.0f, 437.0f, 0.0f),	// 死亡回数の位置
	};

	const int DIGIT_NUMBER[] =
	{
		6,	// クリア時間
		4,	// 被ダメージ
		2,	// 死亡回数
	};
	const D3DXVECTOR2 SIZE_NUMBER = D3DXVECTOR2(22.0f, 22.0f);
	const float DSTANCE_TIMER = 75.0f;
}

//==========================================================================
// コンストラクタ
//==========================================================================
CBattleResult::CBattleResult(int nPriority) : CObject2D(nPriority)
{
	// 値のクリア
	memset(m_pRank, 0, sizeof(m_pRank));		// 種類ごとのランク
	memset(m_pNumber, 0, sizeof(m_pNumber));	// 種類ごとの数字
}

//==========================================================================
// デストラクタ
//==========================================================================
CBattleResult::~CBattleResult()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CBattleResult* CBattleResult::Create()
{
	// メモリの確保
	CBattleResult* pMarker = DEBUG_NEW CBattleResult;

	if (pMarker != nullptr)
	{
		// 初期化処理
		pMarker->Init();
	}

	return pMarker;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CBattleResult::Init()
{

	// オブジェクト2Dの初期化
	CObject2D::Init();

	// テクスチャ設定
	int windowTex = CTexture::GetInstance()->Regist(TEXTURE_WINDOW);
	BindTexture(windowTex);

	// 位置設定
	SetPosition(MyLib::Vector3(640.0f, 360.0f, 0.0f));

	// サイズ設定
	D3DXVECTOR2 size = CTexture::GetInstance()->GetImageSize(windowTex);
	size = UtilFunc::Transformation::AdjustSizeByWidth(size, 640.0f);
	SetSize(size);
	SetSizeOrigin(size);

	// クリアランク生成
	CreateRank();

	// 数字生成
	CreateClearTime();	// クリアタイム生成
	CreateDamage();		// 被ダメージ生成
	CreateDead();		// 死亡回数生成
	
	return S_OK;
}

//==========================================================================
// クリアランク生成
//==========================================================================
void CBattleResult::CreateRank()
{
	CGameRating* pRating = CGame::GetInstance()->GetGameManager()->GetGameRating();

	// 今回の評価情報取得
	CGameRating::sRating ratingInfo = pRating->GetRatingInfo();

	for (int i = 0; i < CGameRating::RATINGTYPE::RATINGTYPE_MAX; i++)
	{
		// 生成
		m_pRank[i] = CObject2D::Create(GetPriority());
		if (m_pRank[i] == nullptr)
		{
			continue;
		}
		CObject2D* pRank = m_pRank[i];

		// 今回のランク
		CGameRating::RATING rating;

		// ランク割り出し
		switch (i)
		{
		case CGameRating::RATINGTYPE::RATINGTYPE_TIME:
			rating = pRating->CalculateClearTimeRank(ratingInfo.clearTime);
			break;

		case CGameRating::RATINGTYPE::RATINGTYPE_DMG:
			rating = pRating->CalculateRecieveDamageRank(ratingInfo.receiveDamage);
			break;

		case CGameRating::RATINGTYPE::RATINGTYPE_DEAD:
			rating = pRating->CalculateNumDeadRank(ratingInfo.numDead);
			break;
		}

		// テクスチャ設定
		int rankTex = CTexture::GetInstance()->Regist(TEXTURE_RANK[rating]);
		pRank->BindTexture(rankTex);

		// 位置設定
		pRank->SetPosition(POSITION_RANK[i]);

		// サイズ設定
		pRank->SetSize(D3DXVECTOR2(40.0f, 40.0f));
		SetSizeOrigin(pRank->GetSize());
	}

}

#if 0
//==========================================================================
// 数字生成
//==========================================================================
void CBattleResult::CreateNumber()
{

	// 今回の評価情報取得
	CGameRating* pRating = CGame::GetInstance()->GetGameManager()->GetGameRating();
	CGameRating::sRating ratingInfo = pRating->GetRatingInfo();

	for (int i = 0; i < CGameRating::RATINGTYPE::RATINGTYPE_MAX; i++)
	{
		// 生成
		m_pNumber[i] = CMultiNumber::Create(
			POSITION_SCORE[i],
			D3DXVECTOR2(25.0f, 25.0f),
			DIGIT_NUMBER[i],
			CNumber::EObjectType::OBJECTTYPE_2D,
			TEXTURE_NUMBER, true, GetPriority());
		if (m_pNumber[i] == nullptr)
		{
			continue;
		}
		CMultiNumber* pNumber = m_pNumber[i];

		// 位置設定
		pNumber->SetPosition(POSITION_SCORE[i]);

		// 右寄せに設定
		pNumber->SetAlignmentType(CMultiNumber::AlignmentType::ALIGNMENT_RIGHT);

		switch (i)
		{
		case CGameRating::RATINGTYPE::RATINGTYPE_TIME:
			pNumber->SetValue(ratingInfo.clearTime);
			break;

		case CGameRating::RATINGTYPE::RATINGTYPE_DMG:
			pNumber->SetValue(ratingInfo.numDead);
			break;

		case CGameRating::RATINGTYPE::RATINGTYPE_DEAD:
			pNumber->SetValue(ratingInfo.numDead);
			break;
		}
	}
}
#endif

//==========================================================================
// クリアタイム生成
//==========================================================================
void CBattleResult::CreateClearTime()
{
	// 今回の評価情報取得
	CGameRating* pRating = CGame::GetInstance()->GetGameManager()->GetGameRating();
	CGameRating::sRating ratingInfo = pRating->GetRatingInfo();

	for (int i = 0; i < 3; i++)
	{
		// 生成
		m_pClearTime[i] = CMultiNumber::Create(
			POSITION_SCORE[CGameRating::RATINGTYPE::RATINGTYPE_TIME],
			SIZE_NUMBER,
			2,
			CNumber::EObjectType::OBJECTTYPE_2D,
			TEXTURE_NUMBER, false, GetPriority());
		if (m_pClearTime[i] == nullptr)
		{
			continue;
		}
		CMultiNumber* pNumber = m_pClearTime[i];

		MyLib::Vector3 pos = POSITION_SCORE[CGameRating::RATINGTYPE::RATINGTYPE_TIME];

		pos.x -= DSTANCE_TIMER * i;

		// 位置設定
		pNumber->SetPosition(pos);

		// 右寄せに設定
		pNumber->SetAlignmentType(CMultiNumber::AlignmentType::ALIGNMENT_RIGHT);

		// 分、秒、ミリ秒の計算
		int minutes = static_cast<int>(ratingInfo.clearTime) / (60);
		int seconds = static_cast<int>(ratingInfo.clearTime) % (60);
		int milliseconds = static_cast<int>(ratingInfo.clearTime) % 1000;

		switch (i)
		{
		case 0:
			pNumber->SetValue(milliseconds);
			break;

		case 1:
			pNumber->SetValue(seconds);
			break;

		case 2:
			pNumber->SetValue(minutes);
			break;
		}
	}
}

//==========================================================================
// 被ダメージ生成
//==========================================================================
void CBattleResult::CreateDamage()
{
	// 今回の評価情報取得
	CGameRating* pRating = CGame::GetInstance()->GetGameManager()->GetGameRating();
	CGameRating::sRating ratingInfo = pRating->GetRatingInfo();

	// 生成
	m_pDamage = CMultiNumber::Create(
		POSITION_SCORE[CGameRating::RATINGTYPE::RATINGTYPE_DMG],
		SIZE_NUMBER,
		DIGIT_NUMBER[CGameRating::RATINGTYPE::RATINGTYPE_DMG],
		CNumber::EObjectType::OBJECTTYPE_2D,
		TEXTURE_NUMBER, true, GetPriority());
	if (m_pDamage == nullptr)
	{
		return;
	}

	// 位置設定
	m_pDamage->SetPosition(POSITION_SCORE[CGameRating::RATINGTYPE::RATINGTYPE_DMG]);

	// 右寄せに設定
	m_pDamage->SetAlignmentType(CMultiNumber::AlignmentType::ALIGNMENT_RIGHT);

	// 値設定
	m_pDamage->SetValue(ratingInfo.receiveDamage);
}

//==========================================================================
// 死亡回数生成
//==========================================================================
void CBattleResult::CreateDead()
{
	// 今回の評価情報取得
	CGameRating* pRating = CGame::GetInstance()->GetGameManager()->GetGameRating();
	CGameRating::sRating ratingInfo = pRating->GetRatingInfo();

	// 生成
	m_pDead = CMultiNumber::Create(
		POSITION_SCORE[CGameRating::RATINGTYPE::RATINGTYPE_DEAD],
		SIZE_NUMBER,
		DIGIT_NUMBER[CGameRating::RATINGTYPE::RATINGTYPE_DEAD],
		CNumber::EObjectType::OBJECTTYPE_2D,
		TEXTURE_NUMBER, true, GetPriority());
	if (m_pDead == nullptr)
	{
		return;
	}

	// 右寄せに設定
	m_pDead->SetAlignmentType(CMultiNumber::AlignmentType::ALIGNMENT_RIGHT);

	// 値設定
	m_pDead->SetValue(ratingInfo.numDead);
}

//==========================================================================
// 終了処理
//==========================================================================
void CBattleResult::Uninit()
{
	for (int i = 0; i < CGameRating::RATINGTYPE::RATINGTYPE_MAX; i++)
	{
		if (m_pRank[i] != nullptr)
		{
			m_pRank[i] = nullptr;
		}

		if (m_pNumber[i] != nullptr)
		{
			m_pNumber[i]->Uninit();
			m_pNumber[i] = nullptr;
		}
	}

	CObject2D::Uninit();
}

//==========================================================================
// 削除
//==========================================================================
void CBattleResult::Kill()
{
	for (int i = 0; i < CGameRating::RATINGTYPE::RATINGTYPE_MAX; i++)
	{
		if (m_pRank[i] != nullptr)
		{
			m_pRank[i]->Uninit();
			m_pRank[i] = nullptr;
		}

		if (m_pNumber[i] != nullptr)
		{
			m_pNumber[i]->Release();
			m_pNumber[i] = nullptr;
		}
	}

	CObject2D::Uninit();
}

//==========================================================================
// 更新処理
//==========================================================================
void CBattleResult::Update()
{

	// 今回の評価情報取得
	CGameRating* pRating = CGame::GetInstance()->GetGameManager()->GetGameRating();
	CGameRating::sRating ratingInfo = pRating->GetRatingInfo();
#if 0
	for (int i = 0; i < CGameRating::RATINGTYPE::RATINGTYPE_MAX; i++)
	{
		if (m_pNumber[i] != nullptr)
		{
			switch (i)
			{
			case CGameRating::RATINGTYPE::RATINGTYPE_TIME:
			{
				// 分、秒、ミリ秒の計算
				int minutes = static_cast<int>(m_fTime) / (60);
				int seconds = (static_cast<int>(m_fTime) % (60));
				int milliseconds = static_cast<int>(m_fTime) % 1000;

				int time = minutes * 10000 + seconds * 100 + milliseconds;
				m_pNumber[i]->SetValue(time);
			}
				break;

			case CGameRating::RATINGTYPE::RATINGTYPE_DMG:
				m_pNumber[i]->SetValue(ratingInfo.receiveDamage);
				break;

			case CGameRating::RATINGTYPE::RATINGTYPE_DEAD:
				m_pNumber[i]->SetValue(ratingInfo.numDead);
				break;
			}
		}
		m_pNumber[i]->SetPosition(POSITION_SCORE[i]);
		m_pNumber[i]->Update();

	}
#endif

	for (int i = 0; i < 3; i++)
	{
		switch (i)
		{
		case 0:
			m_pClearTime[i]->SetValue(static_cast<int>(ratingInfo.clearTime) % 1000);
			break;

		case 1:
			m_pClearTime[i]->SetValue(static_cast<int>(ratingInfo.clearTime) % (60));
			break;

		case 2:
			m_pClearTime[i]->SetValue(static_cast<int>(ratingInfo.clearTime) / (60));
			break;
		}

		MyLib::Vector3 pos = POSITION_SCORE[CGameRating::RATINGTYPE::RATINGTYPE_TIME];

		pos.x -= DSTANCE_TIMER * i;
		m_pClearTime[i]->Update();
		m_pClearTime[i]->SetPosition(pos);
	}

	m_pDamage->SetValue(ratingInfo.receiveDamage);
	m_pDamage->SetPosition(POSITION_SCORE[CGameRating::RATINGTYPE::RATINGTYPE_DMG]);
	m_pDamage->Update();
	m_pDead->SetValue(ratingInfo.numDead);
	m_pDead->SetPosition(POSITION_SCORE[CGameRating::RATINGTYPE::RATINGTYPE_DEAD]);
	m_pDead->Update();

	// 更新処理
	CObject2D::Update();
}

//==========================================================================
// 描画処理
//==========================================================================
void CBattleResult::Draw()
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
