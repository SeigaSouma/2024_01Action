//=============================================================================
// 
//  スキルポイント処理 [skillpoint.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "skillpoint.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"
#include "object2D.h"
#include "calculation.h"
#include "multinumber.h"
#include "input.h"

//==========================================================================
// 定数定義
//==========================================================================
namespace
{
	const char* TEXTURE = "data\\TEXTURE\\skillpoint\\skillpoint_01.png";
	const char* TEXTURE2 = "data\\TEXTURE\\leaf002.png";
	const char* NUMBER_TEXTURE = "data\\TEXTURE\\number\\number_blackclover_01.png";
	const float WIDTH_FRAME = 280.0f;	// フレームの幅
	const float TIME_SLIDE = 0.5f;		// スライドにかかる時間
	const float TIME_DECIDE = 1.0f;		// 到着
	const float TIME_ARRIVE = 2.0f;		// 到着
	const MyLib::Vector3 DISTANCE_NUMBER = MyLib::Vector3(150.0f, 0.0f, 0.0f);
	const MyLib::Vector3 DESTPOSITION_SLIDEIN = MyLib::Vector3(SCREEN_WIDTH - WIDTH_FRAME, 80.0f, 0.0f);	// スライドインの目標位置
	const MyLib::Vector3 DESTPOSITION_SLIDEOUT = MyLib::Vector3(SCREEN_WIDTH + WIDTH_FRAME, 80.0f, 0.0f);	// スライドアウトの目標位置

}

//==========================================================================
// 関数リスト
//==========================================================================
CSkillPoint::STATE_FUNC CSkillPoint::m_StateFunc[] =
{
	&CSkillPoint::StateArrive,			// 到着
	&CSkillPoint::StateSlideIn,			// スライドイン
	&CSkillPoint::StateSlideCompletion,	// スライド完了
	&CSkillPoint::StateSlideOut,		// スライドアウト
	&CSkillPoint::StateWait,			// 待機
	&CSkillPoint::StateEnhance,			// 強化
};

//==========================================================================
// コンストラクタ
//==========================================================================
CSkillPoint::CSkillPoint(int nPriority) : CObject2D(nPriority)
{
	// 値のクリア
	m_nPoint = 0;			// ポイント
	m_nVisualValue = 0;		// 見た目上の数値
	m_fStateTime = 0.0f;	// 状態タイマー
	m_fSlideTime = 0.0f;	// スライドタイマー
	m_State = STATE_ARRIVE;	// 状態
	m_apNumber = nullptr;	// 数字のオブジェクト
}

//==========================================================================
// デストラクタ
//==========================================================================
CSkillPoint::~CSkillPoint()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CSkillPoint* CSkillPoint::Create()
{
	// メモリの確保
	CSkillPoint* pTitleScreen = DEBUG_NEW CSkillPoint;

	if (pTitleScreen != nullptr)
	{
		// 初期化処理
		pTitleScreen->Init();
	}

	return pTitleScreen;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CSkillPoint::Init()
{
	// 各種変数初期化
	m_State = STATE_WAIT;


	// 初期化
	CObject2D::Init();
	SetType(CObject::TYPE_OBJECT2D);

	// テクスチャの割り当て
	int nIdx = CTexture::GetInstance()->Regist(TEXTURE);
	BindTexture(nIdx);

	// サイズ設定
	D3DXVECTOR2 size = CTexture::GetInstance()->GetImageSize(nIdx);
	size = UtilFunc::Transformation::AdjustSizeByWidth(size, WIDTH_FRAME);
	SetSize(size);
	SetSizeOrigin(GetSize());

	// 位置設定
	SetPosition(DESTPOSITION_SLIDEOUT);

	// 生成処理
	m_apNumber = CMultiNumber::Create(GetPosition() + DISTANCE_NUMBER, D3DXVECTOR2(50.0f, 50.0f), 2, CNumber::OBJECTTYPE_2D, NUMBER_TEXTURE, true, GetPriority());

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CSkillPoint::Uninit()
{
	// 数字のオブジェクトの終了処理
	if (m_apNumber != nullptr)
	{
		m_apNumber->Uninit();
		m_apNumber = nullptr;
	}

	// 終了処理
	CObject2D::Uninit();
}

//==========================================================================
// 削除処理
//==========================================================================
void CSkillPoint::Kill()
{
	// 数字のオブジェクト削除
	if (m_apNumber != nullptr)
	{
		m_apNumber->Release();
		m_apNumber = nullptr;
	}

	// 終了処理
	CObject2D::Uninit();
}

//==========================================================================
// 更新処理
//==========================================================================
void CSkillPoint::Update()
{
	
	// 状態更新
	(this->*(m_StateFunc[m_State]))();

	// 更新処理
	CObject2D::Update();

	// 値の設定処理
	m_apNumber->SetPosition(GetPosition() + DISTANCE_NUMBER);
	m_apNumber->Update();
	m_apNumber->SetValue(m_nVisualValue);
}

//==========================================================================
// 到着
//==========================================================================
void CSkillPoint::StateArrive()
{
	// タイマー更新
	m_fStateTime += CManager::GetInstance()->GetDeltaTime();
	m_fSlideTime = TIME_SLIDE;

	if (TIME_ARRIVE <= m_fStateTime)
	{
		m_fStateTime = 0.0f;
		m_State = STATE_SLIDEOUT;
	}
}

//==========================================================================
// スライドイン
//==========================================================================
void CSkillPoint::StateSlideIn()
{
	// タイマー加算
	m_fStateTime += CManager::GetInstance()->GetDeltaTime();
	m_fSlideTime += CManager::GetInstance()->GetDeltaTime();

	// 位置更新
	MyLib::Vector3 pos = GetPosition();
	pos.x = UtilFunc::Correction::EasingEaseOut(DESTPOSITION_SLIDEOUT.x, DESTPOSITION_SLIDEIN.x, m_fSlideTime / TIME_SLIDE);
	SetPosition(pos);

	if (m_fSlideTime >= TIME_SLIDE)
	{
		m_fSlideTime = TIME_SLIDE;
		m_fStateTime = 0.0f;
		m_State = STATE_SLIDECOMPLETION;
	}
}

//==========================================================================
// スライド完了
//==========================================================================
void CSkillPoint::StateSlideCompletion()
{
	// タイマー加算
	m_fStateTime += CManager::GetInstance()->GetDeltaTime();

	if (TIME_DECIDE <= m_fStateTime)
	{
		// 見た目上の数値を更新
		m_nVisualValue = m_nPoint;
		m_fStateTime = 0.0f;
		m_State = STATE_ARRIVE;
	}
}

//==========================================================================
// スライドアウト
//==========================================================================
void CSkillPoint::StateSlideOut()
{
	// タイマー更新
	m_fStateTime += CManager::GetInstance()->GetDeltaTime();
	m_fSlideTime -= CManager::GetInstance()->GetDeltaTime();

	// 位置更新
	MyLib::Vector3 pos = GetPosition();
	pos.x = UtilFunc::Correction::EasingEaseOut(DESTPOSITION_SLIDEIN.x, DESTPOSITION_SLIDEOUT.x, 1.0f - (m_fSlideTime / TIME_SLIDE));
	SetPosition(pos);

	if (m_fSlideTime <= 0.0f)
	{
		m_fSlideTime = 0.0f;
		m_fStateTime = 0.0f;
		m_State = STATE_WAIT;
	}
}

//==========================================================================
// 待機
//==========================================================================
void CSkillPoint::StateWait()
{
	// 見た目上の数値を常に更新
	m_nVisualValue = m_nPoint;

	// 位置設定
	SetPosition(DESTPOSITION_SLIDEOUT);
}

//==========================================================================
// 強化
//==========================================================================
void CSkillPoint::StateEnhance()
{
	// 見た目上の数値を常に更新
	m_nVisualValue = m_nPoint;

	// 位置設定
	SetPosition(DESTPOSITION_SLIDEIN);
}

//==========================================================================
// ポイント設定
//==========================================================================
void CSkillPoint::SetPoint(int nValue)
{
	m_nPoint = nValue;
	if (m_nPoint <= 0)
	{
		m_nPoint = 0;
	}
}

//==========================================================================
// ポイント追加
//==========================================================================
void CSkillPoint::AddPoint()
{
	m_nPoint++;
}

//==========================================================================
// ポイント追加
//==========================================================================
void CSkillPoint::AddPoint(int nValue)
{
	m_nPoint += nValue;
}

//==========================================================================
// ポイント削除
//==========================================================================
void CSkillPoint::SubPoint()
{
	m_nPoint--;

	if (m_nPoint <= 0)
	{
		m_nPoint = 0;
	}
}

//==========================================================================
// ポイント削除
//==========================================================================
void CSkillPoint::SubPoint(int nValue)
{
	m_nPoint -= nValue;
	if (m_nPoint <= 0)
	{
		m_nPoint = 0;
	}
}

//==========================================================================
// スライドイン設定
//==========================================================================
void CSkillPoint::SetSlideIn()
{
	m_State = STATE_SLIDEIN;
	m_fStateTime = 0.0f;
	m_fSlideTime = 0.0f;
}

//==========================================================================
// 状態設定
//==========================================================================
void CSkillPoint::SetState(STATE state)
{
	m_State = state;
	m_fStateTime = 0.0f;
}

//==========================================================================
// 描画処理
//==========================================================================
void CSkillPoint::Draw()
{
	// 描画処理
	CObject2D::Draw();
}

