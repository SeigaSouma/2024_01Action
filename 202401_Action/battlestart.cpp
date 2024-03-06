//=============================================================================
// 
//  戦闘開始処理 [battlestart.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "battlestart.h"
#include "battlestart_effect.h"
#include "texture.h"
#include "manager.h"
#include "renderer.h"
#include "calculation.h"
#include "particle.h"
#include "game.h"
#include "camera.h"

//==========================================================================
// 定数定義
//==========================================================================
namespace
{
	const char* SWORD_TEXTURE = "data\\TEXTURE\\battlestart\\sword.png";				// テクスチャのファイル
	const char* SWORD_TEXTURE_COMPLETE = "data\\TEXTURE\\battlestart\\sword_complete.png";				// テクスチャのファイル
	const char* TEXT_TEXTURE_COMPLETE = "data\\TEXTURE\\battlestart\\battlestart_complete.png";				// テクスチャのファイル
	const char* TEXT_TEXTURE = "data\\TEXTURE\\battlestart\\battlestart.png";	// テクスチャのファイル
	const float TIME_GETTOGETHER = 0.6f;	// 集まる時間
	const float TIME_CHARGE = 1.0f;			// 溜め時間
	const float TIME_STINGS = 0.05f;		// 刺さる時間
	const float TIME_SCALE = 0.7f;			// 拡縮時間
	const float TIME_FADEOUT = 0.3f;		// フェードアウト時間
	const MyLib::Vector3 DESTROTATION_GETTOGETHER = MyLib::Vector3(0.0f, 0.0f, D3DX_PI * 0.75f);
	const MyLib::Vector3 DESTPOSITION_GETTOGETHER = MyLib::Vector3(130.0f, 230.0f, 0.0f);
	const MyLib::Vector3 DESTPOSITION_STINGS = MyLib::Vector3(0.0f, 360.0f, 0.0f);
}

//==========================================================================
// 関数ポインタ
//==========================================================================
CBattleStart::STATE_FUNC CBattleStart::m_StateFuncList[] =
{
	&CBattleStart::StateGetTogether,	// 集まる
	&CBattleStart::StateCharge,			// 溜め
	&CBattleStart::StateStings,			// 刺さる
	&CBattleStart::StateScale,			// 拡縮
	&CBattleStart::StateFadeOut,		// フェードアウト
};

//==========================================================================
// コンストラクタ
//==========================================================================
CBattleStart::CBattleStart(int nPriority) : CObject2D(nPriority)
{
	// 値のクリア
	m_state = STATE::STATE_GETTOGETHER;		// 状態
	m_fStateTime = 0.0f;	// 状態タイマー
	memset(m_RockOnInfo, 0, sizeof(m_RockOnInfo));
}

//==========================================================================
// デストラクタ
//==========================================================================
CBattleStart::~CBattleStart()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CBattleStart* CBattleStart::Create(const MyLib::Vector3& pos)
{
	// メモリの確保
	CBattleStart* pMarker = DEBUG_NEW CBattleStart;

	if (pMarker != nullptr)
	{// メモリの確保が出来ていたら

		// 位置設定
		pMarker->SetPosition(pos);
		pMarker->SetOriginPosition(pos);

		// 初期化処理
		pMarker->Init();
	}

	return pMarker;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CBattleStart::Init()
{

	CTexture::GetInstance()->Regist(TEXT_TEXTURE);
	CTexture::GetInstance()->Regist(TEXT_TEXTURE_COMPLETE);
	CTexture::GetInstance()->Regist(SWORD_TEXTURE);
	CTexture::GetInstance()->Regist(SWORD_TEXTURE_COMPLETE);

	// オブジェクト2Dの初期化
	CObject2D::Init();
	int pointTex = CTexture::GetInstance()->Regist(TEXT_TEXTURE);
	BindTexture(pointTex);
	D3DXVECTOR2 size = CTexture::GetInstance()->GetImageSize(pointTex);
	size = UtilFunc::Transformation::AdjustSizeByWidth(size, 240.0f);
	SetSize(D3DXVECTOR2(size.x, 0.0f));
	SetSizeOrigin(size);

	// 種類の設定
	SetType(CObject::TYPE::TYPE_OBJECT2D);

	int nTexIdx = CTexture::GetInstance()->Regist(SWORD_TEXTURE);

	size = CTexture::GetInstance()->GetImageSize(nTexIdx);
	size = UtilFunc::Transformation::AdjustSizeByWidth(size, 120.0f);

	MyLib::Vector3 pos = GetPosition();

	for (int nCntGauge = 0; nCntGauge < VTXTYPE_MAX; nCntGauge++)
	{
		// 生成処理
		m_RockOnInfo[nCntGauge].p2D = CObject2D::Create();
		if (m_RockOnInfo[nCntGauge].p2D == nullptr)
		{
			return E_FAIL;
		}

		CObject2D* pObj = m_RockOnInfo[nCntGauge].p2D;

		// テクスチャ割り当て
		pObj->BindTexture(nTexIdx);

		// 種類の設定
		pObj->SetType(CObject::TYPE_NONE);

		// 情報設定
		pObj->SetSize(size);

		m_RockOnInfo[nCntGauge].Angle = 1;
		float angle = 0.0f;

		if (nCntGauge % 2 == 0)
		{
			angle += D3DX_PI;
			m_RockOnInfo[nCntGauge].Angle = -1;
		}

		
		pObj->SetRotation(MyLib::Vector3(0.0f, 0.0f, angle));
		pObj->SetOriginRotation(pObj->GetRotation());

		pObj->SetPosition(MyLib::Vector3(640.0f + 640.0f * m_RockOnInfo[nCntGauge].Angle, pos.y, 0.0f));
		pObj->SetOriginPosition(pObj->GetPosition());
	}
	CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL::LABEL_SE_BATTLESTART_START);


	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CBattleStart::Uninit()
{
	for (int nCntGauge = 0; nCntGauge < VTXTYPE_MAX; nCntGauge++)
	{
		m_RockOnInfo[nCntGauge].p2D->Uninit();
		m_RockOnInfo[nCntGauge].p2D = nullptr;
	}

	CObject2D::Uninit();
}

//==========================================================================
// 終了処理
//==========================================================================
void CBattleStart::Kill()
{
	for (int nCntGauge = 0; nCntGauge < VTXTYPE_MAX; nCntGauge++)
	{
		if (m_RockOnInfo[nCntGauge].p2D != nullptr)
		{
			// 終了処理
			m_RockOnInfo[nCntGauge].p2D->Uninit();
			m_RockOnInfo[nCntGauge].p2D = nullptr;
		}
	}

	CObject2D::Uninit();
}

//==========================================================================
// 更新処理
//==========================================================================
void CBattleStart::Update()
{
	// 状態タイマー加算
	m_fStateTime += CManager::GetInstance()->GetDeltaTime();

	// 状態別処理
	(this->*(m_StateFuncList[m_state]))();

	if (IsDeath())
	{
		return;
	}

	for (auto& info : m_RockOnInfo)
	{
		info.p2D->Update();
	}

	CObject2D::Update();
}

//==========================================================================
// 集まる
//==========================================================================
void CBattleStart::StateGetTogether()
{
	if (m_fStateTime >= TIME_GETTOGETHER)
	{
		m_fStateTime = 0.0f;
		m_state = STATE_CHARGE;
		CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL::LABEL_SE_BATTLESTART_CHARGE);

		SetSize(GetSizeOrigin());
		for (const auto& info : m_RockOnInfo)
		{
			info.p2D->SetOriginPosition(info.p2D->GetPosition());
		}
		return;
	}

	float ratio = m_fStateTime / TIME_GETTOGETHER;

	// サイズ設定
	D3DXVECTOR2 size = GetSize();
	size.y = UtilFunc::Correction::EasingEaseIn(0.0f, GetSizeOrigin().y, ratio);
	SetSize(size);

	float destAngle = DESTROTATION_GETTOGETHER.z;
	for (const auto& info : m_RockOnInfo)
	{
		MyLib::Vector3 setpos = info.p2D->GetPosition();
		MyLib::Vector3 posOrigin = info.p2D->GetOriginPosition();
		MyLib::Vector3 rot = info.p2D->GetRotation();
		MyLib::Vector3 rotOrigin = info.p2D->GetOriginRotation();

		UtilFunc::Transformation::RotNormalize(destAngle);
		rot.z = UtilFunc::Correction::EasingEaseIn(rotOrigin.z, destAngle, ratio);
		UtilFunc::Transformation::RotNormalize(rot.z);

		setpos.x = UtilFunc::Correction::EasingEaseIn(posOrigin.x, 640.0f + (DESTPOSITION_GETTOGETHER.x * info.Angle), ratio);
		setpos.y = UtilFunc::Correction::EasingEaseIn(posOrigin.y, DESTPOSITION_GETTOGETHER.y, ratio);

		info.p2D->SetRotation(rot);
		info.p2D->SetPosition(setpos);

		destAngle += D3DX_PI + D3DX_PI * 0.5f;
	}
}

//==========================================================================
// 溜め
//==========================================================================
void CBattleStart::StateCharge()
{

	// 振動
	CManager::GetInstance()->GetCamera()->SetShake(3, 1.0f, 0.0f);

	if (m_fStateTime >= TIME_CHARGE)
	{
		m_fStateTime = 0.0f;
		m_state = STATE_STINGS;
		CManager::GetInstance()->GetSound()->StopSound(CSound::LABEL::LABEL_SE_BATTLESTART_CHARGE);
		CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL::LABEL_SE_BATTLESTART);

		for (const auto& info : m_RockOnInfo)
		{
			info.p2D->SetOriginPosition(info.p2D->GetPosition());
		}
		return;
	}

	for (const auto& info : m_RockOnInfo)
	{
		MyLib::Vector3 pos = info.p2D->GetPosition();
		MyLib::Vector3 rot = info.p2D->GetRotation();

		pos.x += sinf(D3DX_PI * 0.5f + rot.z) * 0.5f;
		pos.y += cosf(D3DX_PI * 0.5f + rot.z) * 0.5f;

		info.p2D->SetPosition(pos);
	}
}

//==========================================================================
// 刺さる
//==========================================================================
void CBattleStart::StateStings()
{
	if (m_fStateTime >= TIME_STINGS)
	{
		m_fStateTime = 0.0f;
		m_state = STATE_SCALE;

		// 振動
		CManager::GetInstance()->GetCamera()->SetShake(15, 15.0f, 0.0f);

		// 完了後のテクスチャに切替
		int nTexIdx = CTexture::GetInstance()->Regist(TEXT_TEXTURE_COMPLETE);
		BindTexture(nTexIdx);

		nTexIdx = CTexture::GetInstance()->Regist(SWORD_TEXTURE_COMPLETE);

		for (const auto& info : m_RockOnInfo)
		{
			MyLib::Vector3 setpos = info.p2D->GetPosition();
			MyLib::Vector3 posOrigin = info.p2D->GetOriginPosition();

			setpos.x = UtilFunc::Correction::EasingEaseIn(posOrigin.x, 640.0f + (DESTPOSITION_STINGS.x * info.Angle), 1.0f);
			setpos.y = UtilFunc::Correction::EasingEaseIn(posOrigin.y, DESTPOSITION_STINGS.y, 1.0f);

			info.p2D->SetPosition(setpos);
			info.p2D->BindTexture(nTexIdx);
		}

		// 刺さりパーティクル生成
		my_particle::Create(GetPosition(), my_particle::TYPE::TYPE_BATTLESTART);

		// 文字エフェクト生成
		CBattleStart_Effect::Create(GetPosition());
		return;
	}

	float ratio = m_fStateTime / TIME_STINGS;
	for (const auto& info : m_RockOnInfo)
	{
		MyLib::Vector3 setpos = info.p2D->GetPosition();
		MyLib::Vector3 posOrigin = info.p2D->GetOriginPosition();

		setpos.x = UtilFunc::Correction::EasingEaseIn(posOrigin.x, 640.0f + (DESTPOSITION_STINGS.x * info.Angle), ratio);
		setpos.y = UtilFunc::Correction::EasingEaseIn(posOrigin.y, DESTPOSITION_STINGS.y, ratio);

		info.p2D->SetPosition(setpos);
	}
}

//==========================================================================
// 拡縮
//==========================================================================
void CBattleStart::StateScale()
{
	if (m_fStateTime >= TIME_SCALE)
	{
		m_fStateTime = 0.0f;
		m_state = STATE_FADEOUT;
	}
}

//==========================================================================
// フェードアウト
//==========================================================================
void CBattleStart::StateFadeOut()
{
	if (m_fStateTime >= TIME_FADEOUT)
	{
		m_fStateTime = 0.0f;
		Uninit();

		// 戦闘開始に遷移
		CGame::GetInstance()->GetGameManager()->SetType(CGameManager::SceneType::SCENE_MAIN);
		return;
	}
	float ratio = 1.0f - m_fStateTime / TIME_FADEOUT;

	// 不透明度設定
	SetAlpha(ratio);
	for (const auto& info : m_RockOnInfo)
	{
		info.p2D->SetAlpha(ratio);
	}
}

//==========================================================================
// 描画処理
//==========================================================================
void CBattleStart::Draw()
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();

	// アルファテストを有効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);

	// 描画処理
	for (const auto& info : m_RockOnInfo)
	{
		info.p2D->Draw();
	}

	// オブジェクト2Dの描画
	CObject2D::Draw();

	// アルファテストを無効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);
}
