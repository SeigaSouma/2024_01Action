//=============================================================================
// 
//  タイマー処理 [timer.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "timer.h"
#include "manager.h"
#include "texture.h"
#include "renderer.h"
#include "object2D.h"
#include "multinumber.h"
#include "fade.h"
#include "debugproc.h"
#include "calculation.h"
#include "game.h"
#include "gamemanager.h"
#include "object_circlegauge2D.h"

//==========================================================================
// 定数定義
//==========================================================================
namespace
{
	const char* TEXTURE = "data\\TEXTURE\\number\\degital_01.png";	// テクスチャのファイル
	const char* TEXTURE_WINDOW = "data\\TEXTURE\\timerwindow.png";		// ウィンドウテクスチャのファイル
	const MyLib::Vector3 DEST_POSITION = MyLib::Vector3(1100.0f, 100.0f, 0.0f);
	const float START_RATIO = 4.5;			// 初期割合
	const float DEST_RATIO_WINDOW = 0.1f;	// 目標のウィンドウ割合
	const float WIDTH = 25.0f;			// 横幅
	const float HEIGHT = 25.0f;			// 縦幅
	const float DIS_RATIO_Y = 0.3f;			// Yの間隔
	const int NUM_TIMER = 2;			// 桁数
	const int MAX_TIME = 60 * 99;		// タイマーの最大数
	const float TIME_APPEARANCE = 0.7f;	// 出現時間
	const float TIME_ADDLITTLE = 2.0f;	// 少し加算時間
	const float TIME_ADJUSTMENT = 0.5f;	// 調整時間
	const float SIZE_CIRCLE = 50.0f;	// 円のサイズ
}

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================
CTimer *CTimer::m_pTimer = nullptr;	// 自身のポインタ

//==========================================================================
// 関数リスト
//==========================================================================
CTimer::STATE_FUNC CTimer::m_StateFuncList[] =
{
	&CTimer::StateWait,			// 待機
	&CTimer::StatAppearance,	// 出現
	&CTimer::StatAddLittle,		// 少し加算
	&CTimer::StateAdjustment,	// 調整
};

//==========================================================================
// コンストラクタ
//==========================================================================
CTimer::CTimer(int nPriority)
{
	// 値のクリア
	m_state = STATE_WAIT;		// 状態
	m_fStateTime = 0.0f;		// 状態時間
	m_fTime = 0.0f;				// 時間
	m_pos = mylib_const::DEFAULT_VECTOR3;	// 位置
	m_posOrigin = mylib_const::DEFAULT_VECTOR3;	// 元の位置
	m_bAddTime = false;			// タイマー加算のフラグ
	m_nProgress = 0;
	ZeroMemory(&m_apCircle[0], sizeof(m_apCircle));
}

//==========================================================================
// デストラクタ
//==========================================================================
CTimer::~CTimer()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CTimer *CTimer::Create()
{
	// 生成用のオブジェクト
	CTimer *pScore = nullptr;

	if (pScore == nullptr)
	{// nullptrだったら

		// メモリの確保
		pScore = DEBUG_NEW CTimer;

		if (pScore != nullptr)
		{// メモリの確保が出来ていたら

			// 初期化処理
			pScore->Init(mylib_const::DEFAULT_VECTOR3);
		}

		return pScore;
	}

	return nullptr;
}

//==========================================================================
// 生成処理
//==========================================================================
CTimer *CTimer::Create(MyLib::Vector3 pos)
{
	if (m_pTimer == nullptr)
	{// nullptrだったら

		// メモリの確保
		m_pTimer = DEBUG_NEW CTimer;

		if (m_pTimer != nullptr)
		{// メモリの確保が出来ていたら

			// 初期化処理
			m_pTimer->Init(pos);
		}

		return m_pTimer;
	}

	return nullptr;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CTimer::Init(MyLib::Vector3 pos)
{
	// 各種変数初期化
	m_pos = pos;
	m_posOrigin = m_pos;	// 元の位置
	m_fTime = START_TIME;	// 時間
	m_state = STATE_WAIT;			// 状態
	m_bAddTime = true;			// タイマー加算のフラグ
	D3DXCOLOR setcolor = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);

	for (int i = 0; i < NUM_CIRCLE; i++)
	{
		if (m_apCircle[i] == nullptr)
		{
			m_apCircle[i] = CObjectCircleGauge2D::Create(16, SIZE_CIRCLE);

			if (m_apCircle[i] != nullptr)
			{
				m_apCircle[i]->SetType(CObject::TYPE::TYPE_OBJECT2D);

				MyLib::Vector3 pos =
				{
					170.0f + SIZE_CIRCLE * i * 2,
					SIZE_CIRCLE,
					0.0f
				};

				m_apCircle[i]->SetRate(1.0f);
				m_apCircle[i]->SetRateDest(1.0f);
				m_apCircle[i]->SetPosition(pos);
				m_apCircle[i]->SetVtx();
			}
		}
	}

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CTimer::Uninit()
{
	m_pTimer = nullptr;
}

//==========================================================================
// 更新処理
//==========================================================================
void CTimer::Update()
{
	// 状態別処理
	(this->*(m_StateFuncList[m_state]))();

	if (!m_bAddTime)
	{
		return;
	}

	// 時間減算
	float fDeltaTime = CManager::GetInstance()->GetDeltaTime();

	if (fDeltaTime <= 10)
	{
		m_fTime -= fDeltaTime;
		UtilFunc::Transformation::ValueNormalize(m_fTime, static_cast<float>(MAX_TIME), 0.0f);

		// 分、秒、ミリ秒の計算
		int minutes = static_cast<int>(m_fTime) / (60);
		int seconds = (static_cast<int>(m_fTime) % (60));
		int milliseconds = static_cast<int>(m_fTime) % 1000;

		// デバッグ表示
		CManager::GetInstance()->GetDebugProc()->Print(
			"------------------[時間：%d分、%d秒、%d]------------------\n", minutes, seconds, milliseconds);

		if (m_nProgress <= NUM_CIRCLE - 1)
		{
			if (m_apCircle[m_nProgress] != nullptr)
			{
				float fSize = m_apCircle[m_nProgress]->GetSize();

				fSize -= fDeltaTime / (START_TIME / NUM_CIRCLE) * SIZE_CIRCLE;

				m_apCircle[m_nProgress]->SetSize(fSize);
				m_apCircle[m_nProgress]->SetVtx();

				if (fSize <= 0.0f)
				{
					m_nProgress++;
				}
			}
		}
	}

	if (m_fTime <= 0.0f)
	{
		// リザルトに遷移
		CManager::GetInstance()->GetFade()->SetFade(CScene::MODE_RESULT);
	}

}

//==========================================================================
// 待機状態
//==========================================================================
void CTimer::StateWait()
{
	// 時間リセット
	m_fStateTime = 0.0f;
}

//==========================================================================
// 出現状態
//==========================================================================
void CTimer::StatAppearance()
{
	// 時間加算
	m_fStateTime += CManager::GetInstance()->GetDeltaTime();

	if (m_fStateTime >= TIME_APPEARANCE)
	{
		// 状態遷移
		m_state = STATE_ADD_LITTLE;
		m_fStateTime = 0.0f;

		// タイマーを進める
		m_bAddTime = true;
	}
}

//==========================================================================
// 少し加算状態
//==========================================================================
void CTimer::StatAddLittle()
{
	// 時間加算
	m_fStateTime += CManager::GetInstance()->GetDeltaTime();

	if (m_fStateTime >= TIME_ADDLITTLE)
	{
		// 状態遷移
		m_state = STATE_ADJ;
		m_fStateTime = 0.0f;
		CGame::GetInstance()->GetGameManager()->SetType(CGameManager::SCENE_MAIN);
	}
}

//==========================================================================
// 調整状態
//==========================================================================
void CTimer::StateAdjustment()
{
	// 時間加算
	m_fStateTime += CManager::GetInstance()->GetDeltaTime();

	m_pos.x = UtilFunc::Correction::EasingLinear(m_posOrigin.x, DEST_POSITION.x, m_fStateTime / TIME_ADJUSTMENT);
	m_pos.y = UtilFunc::Correction::EasingLinear(m_posOrigin.y, DEST_POSITION.y, m_fStateTime / TIME_ADJUSTMENT);

	MyLib::Vector3 setpos = m_pos;

	if (m_fStateTime >= TIME_ADJUSTMENT)
	{
		// 状態遷移
		m_state = STATE_WAIT;
		m_fStateTime = 0.0f;
	}
}

//==========================================================================
// 描画処理
//==========================================================================
void CTimer::Draw()
{

}
