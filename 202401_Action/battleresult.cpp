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
#include "2D_effect.h"
#include "battleresult_Overall.h"
#include "player.h"
#include "skillpoint.h"

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
	const D3DXVECTOR2 SIZE_RANK = D3DXVECTOR2(40.0f, 40.0f);
	const D3DXVECTOR2 SIZE_NUMBER = D3DXVECTOR2(22.0f, 22.0f);
	const float DSTANCE_TIMER = 75.0f;

	const float TIME_FADEIN = 0.2f;		// フェードイン
	const float TIME_RANKWAIT = 0.8f;	// ランク入場待機
	const float TIME_RANKIN = 0.4f;		// ランク入場
	const float TIME_FADEOUT = 0.3f;	// フェードアウト
}

//==========================================================================
// 関数リスト
//==========================================================================
CBattleResult::STATE_FUNC CBattleResult::m_StateFuncList[] =
{
	&CBattleResult::StateFadeIn,			// フェードイン
	&CBattleResult::StateRankWait,			// ランク入場待ち
	&CBattleResult::StateRankIn,			// ランク入場
	&CBattleResult::StateRankIn_OverAll,	// 総合ランク入場
	&CBattleResult::StateReturnWait,		// 押下待機
	&CBattleResult::StateFadeOut,			// フェードアウト
};

//==========================================================================
// コンストラクタ
//==========================================================================
CBattleResult::CBattleResult(int nPriority) : CObject2D(nPriority)
{
	// 値のクリア
	memset(m_RankInfo, 0, sizeof(m_RankInfo));		// 種類ごとのランク
	memset(m_pRank, 0, sizeof(m_pRank));		// 種類ごとのランク
	memset(m_pClearTime, 0, sizeof(m_pClearTime));		// 種類ごとの数字

	m_pOverall = nullptr;	// 総合


	m_pDamage = nullptr;			// 種類ごとの数字
	m_pDead = nullptr;				// 種類ごとの数字
	m_state = STATE::STATE_FADEIN;	// 状態
	m_fStateTimer = 0.0f;			// 状態タイマー
	m_fSurvivalTimer = 0.0f;		// 生存タイマー
	m_bCompleteStaging = false;		// 演出完了フラグ
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
	
	CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL::LABEL_SE_BATTLERESULT);

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
		m_RankInfo[i].pObj2D = CObject2D::Create(GetPriority());
		if (m_RankInfo[i].pObj2D == nullptr)
		{
			continue;
		}
		CObject2D* pRank = m_RankInfo[i].pObj2D;

		// ランク割り出し
		switch (i)
		{
		case CGameRating::RATINGTYPE::RATINGTYPE_TIME:
			m_RankInfo[i].rating = pRating->CalculateClearTimeRank(ratingInfo.clearTime);
			break;

		case CGameRating::RATINGTYPE::RATINGTYPE_DMG:
			m_RankInfo[i].rating = pRating->CalculateRecieveDamageRank(ratingInfo.receiveDamage);
			break;

		case CGameRating::RATINGTYPE::RATINGTYPE_DEAD:
			m_RankInfo[i].rating = pRating->CalculateNumDeadRank(ratingInfo.numDead);
			break;
		}

		// テクスチャ設定
		int rankTex = CTexture::GetInstance()->Regist(TEXTURE_RANK[m_RankInfo[i].rating]);
		pRank->BindTexture(rankTex);

		// 位置設定
		pRank->SetPosition(POSITION_RANK[i]);

		// サイズ設定
		pRank->SetSize(SIZE_RANK * 3.0f);
		pRank->SetSizeOrigin(pRank->GetSize());

		// 色設定
		pRank->SetAlpha(0.0f);

	}

}

//==========================================================================
// 総合ランク生成
//==========================================================================
void CBattleResult::CreateRankOverall(int prevPoint)
{
	// 総合戦果
	CGameRating* pRating = CGame::GetInstance()->GetGameManager()->GetGameRating();
	CGameRating::RATING rating = pRating->CalculateRank(
		m_RankInfo[CGameRating::RATINGTYPE::RATINGTYPE_TIME].rating,
		m_RankInfo[CGameRating::RATINGTYPE::RATINGTYPE_DMG].rating,
		m_RankInfo[CGameRating::RATINGTYPE::RATINGTYPE_DEAD].rating);

	// 生成
	m_pOverall = CBattleResult_Overall::Create(rating, prevPoint, prevPoint + pRating->CalculateOverrallRankPoint(rating));

}

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

		// 色設定
		pNumber->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));
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

	// 右寄せに設定
	m_pDamage->SetAlignmentType(CMultiNumber::AlignmentType::ALIGNMENT_RIGHT);

	// 値設定
	m_pDamage->SetValue(ratingInfo.receiveDamage);

	// 色設定
	m_pDamage->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));
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

	// 色設定
	m_pDead->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));
}

//==========================================================================
// 終了処理
//==========================================================================
void CBattleResult::Uninit()
{
	for (int i = 0; i < CGameRating::RATINGTYPE::RATINGTYPE_MAX; i++)
	{
		if (m_RankInfo[i].pObj2D != nullptr)
		{
			m_RankInfo[i].pObj2D = nullptr;
		}

		if (m_pClearTime[i] != nullptr)
		{
			m_pClearTime[i]->Uninit();
			m_pClearTime[i] = nullptr;
		}
	}

	if (m_pDamage != nullptr)
	{
		m_pDamage->Uninit();
		m_pDamage = nullptr;
	}

	if (m_pDead != nullptr)
	{
		m_pDead->Uninit();
		m_pDead = nullptr;
	}

	/*if (m_pOverall != nullptr)
	{
		m_pOverall->Uninit();
		m_pOverall = nullptr;
	}*/

	CObject2D::Uninit();
}

//==========================================================================
// 削除
//==========================================================================
void CBattleResult::Kill()
{
	for (int i = 0; i < CGameRating::RATINGTYPE::RATINGTYPE_MAX; i++)
	{
		if (m_RankInfo[i].pObj2D != nullptr)
		{
			m_RankInfo[i].pObj2D->Uninit();
			m_RankInfo[i].pObj2D = nullptr;
		}

		if (m_pClearTime[i] != nullptr)
		{
			m_pClearTime[i]->Release();
			m_pClearTime[i] = nullptr;
		}
	}

	if (m_pDamage != nullptr)
	{
		m_pDamage->Release();
		m_pDamage = nullptr;
	}

	if (m_pDead != nullptr)
	{
		m_pDead->Release();
		m_pDead = nullptr;
	}

	/*if (m_pOverall != nullptr)
	{
		m_pOverall->Kill();
		m_pOverall = nullptr;
	}*/

	CObject2D::Uninit();
}

//==========================================================================
// 更新処理
//==========================================================================
void CBattleResult::Update()
{
	// タイマー加算
	m_fStateTimer += CManager::GetInstance()->GetDeltaTime();
	m_fSurvivalTimer += CManager::GetInstance()->GetDeltaTime();

	// 状態別処理
	(this->*(m_StateFuncList[m_state]))();
	if (IsDeath()){
		return;
	}

	if (m_fSurvivalTimer >= 0.3f && !m_bCompleteStaging)
	{
		// 入力情報取得
		CInputKeyboard* pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();
		CInputGamepad* pInputGamepad = CManager::GetInstance()->GetInputGamepad();

		if (pInputGamepad->GetTrigger(CInputGamepad::BUTTON::BUTTON_A, 0) ||
			pInputGamepad->GetTrigger(CInputGamepad::BUTTON::BUTTON_B, 0) ||
			pInputGamepad->GetTrigger(CInputGamepad::BUTTON::BUTTON_START, 0) ||
			pInputKeyboard->GetTrigger(DIK_RETURN) ||
			pInputKeyboard->GetTrigger(DIK_SPACE) ||
			pInputKeyboard->GetTrigger(DIK_BACKSPACE)
			)
		{
			AllSetting();
		}
	}

	// 今回の評価情報取得
	CGameRating* pRating = CGame::GetInstance()->GetGameManager()->GetGameRating();
	CGameRating::sRating ratingInfo = pRating->GetRatingInfo();

	// タイマーを分、秒、ミリ秒に変換
	int minutes = static_cast<int>(ratingInfo.clearTime / 60);
	int seconds = static_cast<int>(ratingInfo.clearTime) % 60;
	int milliseconds = static_cast<int>((ratingInfo.clearTime - static_cast<int>(ratingInfo.clearTime)) * 1000);
	milliseconds /= 10;

	for (int i = 0; i < 3; i++)
	{
		if (m_pClearTime[i] == nullptr)
		{
			continue;
		}

		int time = 0;

		switch (i)
		{
		case 0:
			m_pClearTime[i]->SetValue(milliseconds);
			break;

		case 1:
			m_pClearTime[i]->SetValue(seconds);
			break;

		case 2:
			m_pClearTime[i]->SetValue(minutes);
			break;
		}

		MyLib::Vector3 pos = POSITION_SCORE[CGameRating::RATINGTYPE::RATINGTYPE_TIME];

		pos.x -= DSTANCE_TIMER * i;
		m_pClearTime[i]->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, GetAlpha()));
		m_pClearTime[i]->Update();
		m_pClearTime[i]->SetPosition(pos);
	}

	m_pDamage->SetValue(ratingInfo.receiveDamage);
	m_pDamage->SetPosition(POSITION_SCORE[CGameRating::RATINGTYPE::RATINGTYPE_DMG]);
	m_pDamage->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, GetAlpha()));
	m_pDamage->Update();

	m_pDead->SetValue(ratingInfo.numDead);
	m_pDead->SetPosition(POSITION_SCORE[CGameRating::RATINGTYPE::RATINGTYPE_DEAD]);
	m_pDead->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, GetAlpha()));
	m_pDead->Update();

	// 更新処理
	CObject2D::Update();
}

//==========================================================================
// 全て設定
//==========================================================================
void CBattleResult::AllSetting()
{


	// 不透明度設定
	SetAlpha(1.0f);

	for (int i = 0; i < CGameRating::RATINGTYPE::RATINGTYPE_MAX; i++)
	{
		if (m_RankInfo[i].pObj2D == nullptr)
		{
			continue;
		}
		CObject2D* pRank = m_RankInfo[i].pObj2D;
		pRank->SetSize(SIZE_RANK);
		pRank->SetAlpha(1.0f);
	}

	if (m_pOverall == nullptr)
	{
		// 前回ポイント取得
		CListManager<CPlayer> playerList = CPlayer::GetListObj();
		CPlayer* pPlayer = playerList.GetData(0);
		CSkillPoint* pSkillPoint = pPlayer->GetSkillPoint();

		// 総合戦果生成
		CreateRankOverall(pSkillPoint->GetPoint());
	}
	m_pOverall->AllSetting();

	// 演出完了フラグ
	m_bCompleteStaging = true;
	m_state = STATE::STATE_RETURNWAIT;
	m_fStateTimer = 0.0f;
}

//==========================================================================
// フェードイン
//==========================================================================
void CBattleResult::StateFadeIn()
{
	float alpha = UtilFunc::Correction::EasingLinear(0.0f, 1.0f, 0.0f, TIME_FADEIN, m_fStateTimer);
	SetAlpha(alpha);

	if (m_fStateTimer >= TIME_FADEIN)
	{
		m_state = STATE::STATE_RANKWAIT;
		m_fStateTimer = 0.0f;
		SetAlpha(1.0f);
		return;
	}
}

//==========================================================================
// ランク入場待ち
//==========================================================================
void CBattleResult::StateRankWait()
{
	if (m_fStateTimer >= TIME_RANKWAIT)
	{
		m_state = STATE::STATE_RANKIN;
		m_fStateTimer = 0.0f;
		return;
	}
}

//==========================================================================
// ランク入場
//==========================================================================
void CBattleResult::StateRankIn()
{
	for (int i = 0; i < CGameRating::RATINGTYPE::RATINGTYPE_MAX; i++)
	{
		if (m_RankInfo[i].pObj2D == nullptr)
		{
			continue;
		}
		CObject2D* pRank = m_RankInfo[i].pObj2D;

		// サイズ取得
		D3DXVECTOR2 size = pRank->GetSize();
		D3DXVECTOR2 sizeOrigin = pRank->GetSizeOrigin();
		D3DXVECTOR2 destSize = UtilFunc::Transformation::AdjustSizeByWidth(size, SIZE_RANK.x);

		// 徐々に減速
		float len = UtilFunc::Correction::EasingEaseOut(sizeOrigin.x, SIZE_RANK.x, 0.0f, TIME_RANKIN, m_fStateTimer);
		pRank->SetSize(D3DXVECTOR2(len, len));


		float alpha = UtilFunc::Correction::EasingLinear(0.0f, 1.0f, 0.0f, TIME_RANKIN, m_fStateTimer);
		pRank->SetAlpha(alpha);
	}

	if (m_fStateTimer >= TIME_RANKIN)
	{
		m_state = STATE::STATE_RANKIN_OVERALL;
		CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL::LABEL_SE_BATTLERESULT_RANK);

		for (int i = 0; i < CGameRating::RATINGTYPE::RATINGTYPE_MAX; i++)
		{
			if (m_RankInfo[i].pObj2D == nullptr)
			{
				continue;
			}
			CObject2D* pRank = m_RankInfo[i].pObj2D;
			pRank->SetSize(SIZE_RANK);
			pRank->SetAlpha(1.0f);
		}
		return;
	}
}

//==========================================================================
// 総合ランク入場
//==========================================================================
void CBattleResult::StateRankIn_OverAll()
{
	if (m_fStateTimer >= 0.5f)
	{
		// 前回ポイント取得
		CListManager<CPlayer> playerList = CPlayer::GetListObj();
		CPlayer* pPlayer = playerList.GetData(0);
		CSkillPoint* pSkillPoint = pPlayer->GetSkillPoint();

		// 総合戦果生成
		CreateRankOverall(pSkillPoint->GetPoint());
		m_state = STATE::STATE_RETURNWAIT;
		m_fStateTimer = 0.0f;
	}
}

//==========================================================================
// 押下待機
//==========================================================================
void CBattleResult::StateReturnWait()
{
	//m_fStateTimer = 0.0f;

	if (m_fStateTimer >= 0.4f)
	{
		for (int i = 0; i < CGameRating::RATINGTYPE::RATINGTYPE_MAX; i++)
		{
			if (m_RankInfo[i].pObj2D == nullptr ||
				m_RankInfo[i].rating != CGameRating::RATING::RATING_S)
			{
				continue;
			}
			CObject2D* pRank = m_RankInfo[i].pObj2D;

			MyLib::Vector3 pos = pRank->GetPosition();
			pos.x += UtilFunc::Transformation::Random(-20, 20);
			pos.y += UtilFunc::Transformation::Random(-20, 20);

			D3DXCOLOR col;
			if (UtilFunc::Transformation::Random(0, 1) == 0) {
				col = D3DXCOLOR(0.9f, 0.9f, 1.0f, 0.4f);
			}
			else {
				col = D3DXCOLOR(0.2f, 0.2f, 1.0f, 0.4f);
			}

			// エフェクトの設定
			CEffect2D* pEffect = CEffect2D::Create(pos, 0.0f, col, static_cast<float>(UtilFunc::Transformation::Random(10, 30)),
				20,
				CEffect2D::MOVEEFFECT::MOVEEFFECT_SUB, CEffect2D::TYPE::TYPE_JUJI3);
			pEffect->SetRotation(MyLib::Vector3(0.0f, 0.0f, UtilFunc::Transformation::GetRandomPi()));
		}

		m_fStateTimer = UtilFunc::Transformation::Random(0, 30) * 0.01f;
	}

	// 入力情報取得
	CInputKeyboard* pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();
	CInputGamepad* pInputGamepad = CManager::GetInstance()->GetInputGamepad();

	if (m_bCompleteStaging &&
		(pInputGamepad->GetTrigger(CInputGamepad::BUTTON::BUTTON_A, 0) ||
		pInputGamepad->GetTrigger(CInputGamepad::BUTTON::BUTTON_B, 0) ||
		pInputGamepad->GetTrigger(CInputGamepad::BUTTON::BUTTON_START, 0) ||
		pInputKeyboard->GetTrigger(DIK_RETURN) ||
		pInputKeyboard->GetTrigger(DIK_SPACE) ||
		pInputKeyboard->GetTrigger(DIK_BACKSPACE)
		))
	{
		m_state = STATE::STATE_FADEOUT;
		m_fStateTimer = 0.0f;

		m_pOverall->SetState(CBattleResult_Overall::STATE::STATE_FADEOUT);

		CGame::GetInstance()->GetGameManager()->SetType(CGameManager::SceneType::SCENE_MAINCLEAR);
		CGame::GetInstance()->GetGameManager()->GameClearSettings();
		CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL::LABEL_SE_BATTLERESULT_END);
	}

	if (m_pOverall->IsCompleteStaging())
	{
		m_bCompleteStaging = true;
	}
}

//==========================================================================
// フェードアウト
//==========================================================================
void CBattleResult::StateFadeOut()
{
	float alpha = UtilFunc::Correction::EasingLinear(1.0f, 0.0f, 0.0f, TIME_FADEOUT, m_fStateTimer);
	SetAlpha(alpha);

	for (int i = 0; i < CGameRating::RATINGTYPE::RATINGTYPE_MAX; i++)
	{
		if (m_RankInfo[i].pObj2D == nullptr)
		{
			continue;
		}
		CObject2D* pRank = m_RankInfo[i].pObj2D;
		pRank->SetAlpha(alpha);
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
