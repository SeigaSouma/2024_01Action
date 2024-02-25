//=============================================================================
// 
// ステージクリアテキスト処理 [stagecleartext.cpp]
// Author : 相馬靜雅
// 
//=============================================================================
#include "stagecleartext.h"
#include "manager.h"
#include "sound.h"
#include "calculation.h"
#include "camera.h"
#include "battleresult.h"
#include "game.h"

//==========================================================================
// マクロ定義
//==========================================================================
namespace
{
	const char* TEXTURE = "data\\TEXTURE\\battlewin\\text_win.png";
	const float TIME_SCALNONE = 0.4f;			// なにもない
	const float TIME_SCALEDOWN = 0.16f;			// 縮小時間
	const float TIME_SCALENONE = 2.0f;			// 整わせる時間
	const float TIME_FADEOUT = 0.4f;			// フェードアウト時間
}

//==========================================================================
// 関数ポインタ
//==========================================================================
CStageClearText::STATE_FUNC CStageClearText::m_StateFuncList[] =
{
	&CStageClearText::StateNone,
	&CStageClearText::StateScaleDOWN,
	&CStageClearText::StateScaleNone,
	&CStageClearText::StateFadeOut,
};

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================

//==========================================================================
// コンストラクタ
//==========================================================================
CStageClearText::CStageClearText(int nPriority) : CObject2D(nPriority)
{
	// 値のクリア
	m_state = STATE_NONE;			// 状態
	m_fStateTimer = 0.0f;			// 状態タイマー
	m_bCreateResultWindow = false;	// リザルト画面の呼び出しフラグ
}

//==========================================================================
// デストラクタ
//==========================================================================
CStageClearText::~CStageClearText()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CStageClearText* CStageClearText::Create(const MyLib::Vector3 pos)
{
	// メモリの確保
	CStageClearText* pEffect = DEBUG_NEW CStageClearText;

	if (pEffect != nullptr)
	{// メモリの確保が出来ていたら

		// 初期化処理
		pEffect->Init();
		pEffect->SetPosition(pos);
	}

	return pEffect;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CStageClearText::Init()
{
	HRESULT hr;

	// 初期化処理
	hr = CObject2D::Init();
	if (FAILED(hr))
	{// 失敗したとき
		return E_FAIL;
	}

	// 種類の設定
	SetType(TYPE_OBJECT2D);

	// テクスチャの割り当て
	int nTexIdx = CTexture::GetInstance()->Regist(TEXTURE);
	BindTexture(nTexIdx);

	// サイズ設定
	D3DXVECTOR2 size = CTexture::GetInstance()->GetImageSize(nTexIdx);
	size = UtilFunc::Transformation::AdjustSizeByWidth(size, 1600.0f);
	SetSize(size);
	SetSizeOrigin(size);

	// 縮小から開始
	m_fStateTimer = TIME_SCALEDOWN;
	m_state = eState::STATE_SCALEDOWN;

	// 向き設定
	SetRotation(MyLib::Vector3(0.0f, 0.0f, -D3DX_PI * 0.35f));
	SetOriginRotation(GetRotation());

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CStageClearText::Uninit()
{
	// 終了処理
	CObject2D::Uninit();
}

//==========================================================================
// 更新処理
//==========================================================================
void CStageClearText::Update()
{
	if (IsDeath())
	{
		return;
	}

	// 状態別処理
	(this->*(m_StateFuncList[m_state]))();

	if (IsDeath())
	{
		return;
	}

	// 頂点座標の設定
	SetVtx();
}

//==========================================================================
// 何もない状態
//==========================================================================
void CStageClearText::StateNone()
{
	// 状態遷移カウンター減算
	m_fStateTimer -= CManager::GetInstance()->GetDeltaTime();

	if (m_fStateTimer <= 0.0f)
	{
		m_fStateTimer = TIME_FADEOUT;
		m_state = eState::STATE_FADEOUT;
	}
}

//==========================================================================
// 縮小状態
//==========================================================================
void CStageClearText::StateScaleDOWN()
{
	// サイズ取得
	D3DXVECTOR2 size = GetSize();
	D3DXVECTOR2 sizeOrigin = GetSizeOrigin();
	D3DXVECTOR2 destSize = UtilFunc::Transformation::AdjustSizeByWidth(size, 240.0f);

	// 状態遷移カウンター減算
	m_fStateTimer -= CManager::GetInstance()->GetDeltaTime();

	if (m_fStateTimer <= 0.0f)
	{
		m_fStateTimer = TIME_SCALENONE;
		m_state = STATE_SCALENONE;

		// サイズ設定
		SetSize(destSize);
		SetSizeOrigin(destSize);

		// 振動
		CManager::GetInstance()->GetCamera()->SetShake(8, 25.0f, 0.0f);
		return;
	}

	// 徐々に減速
	size.x = UtilFunc::Correction::EasingEaseOut(sizeOrigin.x, destSize.x, 1.0f - (m_fStateTimer / TIME_SCALEDOWN));
	size.y = UtilFunc::Correction::EasingEaseOut(sizeOrigin.y, destSize.y, 1.0f - (m_fStateTimer / TIME_SCALEDOWN));

	// 向き設定
	float angle = UtilFunc::Correction::EasingEaseOut(GetOriginRotation().z, 0.0f, 1.0f - (m_fStateTimer / TIME_SCALEDOWN));
	SetRotation(MyLib::Vector3(0.0f, 0.0f, angle));

	// サイズ設定
	SetSize(size);
}

//==========================================================================
// 整い状態
//==========================================================================
void CStageClearText::StateScaleNone()
{
	// サイズ取得
	D3DXVECTOR2 size = GetSize();
	D3DXVECTOR2 sizeOrigin = GetSizeOrigin();
	D3DXVECTOR2 destSize = UtilFunc::Transformation::AdjustSizeByWidth(size, 260.0f);

	// 状態遷移カウンター減算
	m_fStateTimer -= CManager::GetInstance()->GetDeltaTime();

	if (m_fStateTimer <= 0.0f)
	{
		m_fStateTimer = TIME_SCALNONE;
		m_state = eState::STATE_NONE;

		// サイズ設定
		SetSize(destSize);
		SetSizeOrigin(destSize);
		return;
	}

	// 徐々に減速
	size.x = UtilFunc::Correction::EasingEaseOut(sizeOrigin.x, destSize.x, 1.0f - (m_fStateTimer / TIME_SCALENONE));
	size.y = UtilFunc::Correction::EasingEaseOut(sizeOrigin.y, destSize.y, 1.0f - (m_fStateTimer / TIME_SCALENONE));

	// サイズ設定
	SetSize(size);
}

//==========================================================================
// フェードアウト状態
//==========================================================================
void CStageClearText::StateFadeOut()
{
	// 状態遷移カウンター減算
	m_fStateTimer -= CManager::GetInstance()->GetDeltaTime();

	// 不透明度更新
	float alpha = m_fStateTimer / TIME_FADEOUT;
	SetAlpha(alpha);

	if (TIME_FADEOUT * 0.5f <= m_fStateTimer &&
		!m_bCreateResultWindow)
	{
		m_bCreateResultWindow = true;

		// 戦果生成
		CBattleResult::Create();
		CGame::GetInstance()->GetGameManager()->SetType(CGameManager::SceneType::SCENE_DURING_MAINRESULT);
	}

	if (m_fStateTimer <= 0.0f)
	{
		m_fStateTimer = 0.0f;
		Uninit();
		return;
	}
}

//==========================================================================
// 描画処理
//==========================================================================
void CStageClearText::Draw()
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();

	// アルファテストを有効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);

	// 描画処理
	CObject2D::Draw();

	// アルファテストを有効にする
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);
}

//==========================================================================
// 頂点情報設定処理
//==========================================================================
void CStageClearText::SetVtx()
{
	// 頂点設定
	CObject2D::SetVtx();
}

