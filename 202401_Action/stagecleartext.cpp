//=============================================================================
// 
// ステージクリアテキスト処理 [stagecleartext.cpp]
// Author : 相馬靜雅
// 
//=============================================================================
#include "stagecleartext.h"
#include "renderer.h"
#include "texture.h"
#include "manager.h"
#include "sound.h"
#include "calculation.h"

//==========================================================================
// マクロ定義
//==========================================================================
namespace
{
	const char* TEXTURE = "data\\TEXTURE\\stageclear_01.png";
	const int TIME_SCALEUP = 25;				// 拡大時間
	const int TIME_SCALEDOWN = 25;				// 縮小時間
	const int TIME_SCALENONE = 25;				// 変更なし時間
	const int TIME_FADEOUT = 120;				// フェードアウト時間
	const float SIZE_SCALEORIGIN = 0.3f;		// 元の大きさ
	const float SIZE_SCALEUP = 0.6f;			// 拡大の大きさ
	const float SIZE_SCALEDOWN = 0.25f;			// 縮小の大きさ
}

//==========================================================================
// 関数ポインタ
//==========================================================================
CStageClearText::STATE_FUNC CStageClearText::m_StateFuncList[] =
{
	&CStageClearText::StateNone,
	&CStageClearText::StateScaleUP,
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
	m_nTexIdx = 0;			// テクスチャのインデックス番号
	m_nCntState = 0;		// 状態遷移カウンター
	m_state = STATE_NONE;	// 状態
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
CStageClearText *CStageClearText::Create(const MyLib::Vector3 pos)
{
	// 生成用のオブジェクト
	CStageClearText *pEffect = NULL;

	if (pEffect == NULL)
	{// NULLだったら

		// メモリの確保
		pEffect = DEBUG_NEW CStageClearText;

		if (pEffect != NULL)
		{// メモリの確保が出来ていたら

			// 初期化処理
			pEffect->Init();
			pEffect->SetPosition(pos);
		}

		return pEffect;
	}

	return NULL;
}

//==================================================================================
// 初期化処理
//==================================================================================
HRESULT CStageClearText::Init(void)
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
	m_nTexIdx = CTexture::GetInstance()->Regist(TEXTURE);

	// テクスチャの割り当て
	BindTexture(m_nTexIdx);

	// サイズ取得
	D3DXVECTOR2 size = CTexture::GetInstance()->GetImageSize(m_nTexIdx) * SIZE_SCALEORIGIN;

	// サイズ設定
	SetSize(D3DXVECTOR2(0.0f, 0.0f));
	SetSizeOrigin(size);

	m_nCntState = TIME_SCALEUP;
	m_state = STATE_SCALEUP;

	return S_OK;
}

//==================================================================================
// 終了処理
//==================================================================================
void CStageClearText::Uninit(void)
{
	// 終了処理
	CObject2D::Uninit();
}

//==================================================================================
// 更新処理
//==================================================================================
void CStageClearText::Update(void)
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

//==================================================================================
// 何もない状態
//==================================================================================
void CStageClearText::StateNone(void)
{
	// 状態遷移カウンター減算
	m_nCntState--;

	if (m_nCntState <= 0)
	{
		m_nCntState = 0;
	}
}

//==================================================================================
// 拡大状態
//==================================================================================
void CStageClearText::StateScaleUP(void)
{
	// サイズ取得
	D3DXVECTOR2 size = GetSize();
	D3DXVECTOR2 DestImageSize = CTexture::GetInstance()->GetImageSize(m_nTexIdx) * SIZE_SCALEUP;

	// 状態遷移カウンター減算
	m_nCntState--;

	// 徐々に加速
	size.x = UtilFunc::Correction::EasingEaseIn(0.0f, DestImageSize.x, 1.0f - (float)m_nCntState / (float)TIME_SCALEUP);
	size.y = UtilFunc::Correction::EasingEaseIn(0.0f, DestImageSize.y, 1.0f - (float)m_nCntState / (float)TIME_SCALEUP);

	if (m_nCntState <= 0)
	{
		m_nCntState = TIME_SCALEDOWN;
		m_state = STATE_SCALEDOWN;
	}

	// サイズ設定
	SetSize(size);
}

//==================================================================================
// 縮小状態
//==================================================================================
void CStageClearText::StateScaleDOWN(void)
{
	// サイズ取得
	D3DXVECTOR2 size = GetSize();
	D3DXVECTOR2 ImageSize = CTexture::GetInstance()->GetImageSize(m_nTexIdx);

	// 状態遷移カウンター減算
	m_nCntState--;

	// 徐々に減速
	size.x = UtilFunc::Correction::EasingEaseOut(ImageSize.x * SIZE_SCALEUP, ImageSize.x * SIZE_SCALEDOWN, 1.0f - (float)m_nCntState / (float)TIME_SCALEDOWN);
	size.y = UtilFunc::Correction::EasingEaseOut(ImageSize.y * SIZE_SCALEUP, ImageSize.y * SIZE_SCALEDOWN, 1.0f - (float)m_nCntState / (float)TIME_SCALEDOWN);

	if (m_nCntState <= 0)
	{
		m_nCntState = TIME_SCALENONE;
		m_state = STATE_SCALENONE;
	}

	// サイズ設定
	SetSize(size);
}

//==================================================================================
// 整い状態
//==================================================================================
void CStageClearText::StateScaleNone(void)
{
	// サイズ取得
	D3DXVECTOR2 size = GetSize();
	D3DXVECTOR2 sizeOrigin = GetSizeOrigin();
	D3DXVECTOR2 ImageSize = CTexture::GetInstance()->GetImageSize(m_nTexIdx);

	// 状態遷移カウンター減算
	m_nCntState--;

	// 徐々に減速
	size.x = UtilFunc::Correction::EasingEaseOut(ImageSize.x * SIZE_SCALEDOWN, sizeOrigin.x, 1.0f - (float)m_nCntState / (float)TIME_SCALENONE);
	size.y = UtilFunc::Correction::EasingEaseOut(ImageSize.y * SIZE_SCALEDOWN, sizeOrigin.y, 1.0f - (float)m_nCntState / (float)TIME_SCALENONE);

	if (m_nCntState <= 0)
	{
		m_nCntState = TIME_FADEOUT;
		m_state = STATE_FADEOUT;
	}

	// サイズ設定
	SetSize(size);
}

//==================================================================================
// フェードアウト状態
//==================================================================================
void CStageClearText::StateFadeOut(void)
{
	// 色取得
	D3DXCOLOR col = GetColor();

	// 状態遷移カウンター減算
	m_nCntState--;

	// 不透明度更新
	col.a = (float)m_nCntState / (float)TIME_FADEOUT;

	// 色設定
	SetColor(col);

	if (m_nCntState <= 0)
	{
		m_nCntState = 0;
		Uninit();
		return;
	}
}

//==================================================================================
// 描画処理
//==================================================================================
void CStageClearText::Draw(void)
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
void CStageClearText::SetVtx(void)
{
	// 頂点設定
	CObject2D::SetVtx();
}

