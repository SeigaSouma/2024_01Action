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
	const char* TEXTURE = "data\\TEXTURE\\skillpoint\\cyberwall_03.png";
	const char* TEXTURE2 = "data\\TEXTURE\\leaf002.png";
	const char* NUMBER_TEXTURE = "data\\TEXTURE\\number\\number_blackclover_01.png";
}

//==========================================================================
// コンストラクタ
//==========================================================================
CSkillPoint::CSkillPoint(int nPriority) : CObject2D(nPriority)
{
	// 値のクリア
	m_nPoint = 0;	// ポイント
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
CSkillPoint *CSkillPoint::Create(void)
{
	// 生成用のオブジェクト
	CSkillPoint *pTitleScreen = NULL;

	if (pTitleScreen == NULL)
	{// NULLだったら

		// メモリの確保
		pTitleScreen = DEBUG_NEW CSkillPoint;

		if (pTitleScreen != NULL)
		{// メモリの確保が出来ていたら

			// 初期化処理
			pTitleScreen->Init();
		}

		return pTitleScreen;
	}

	return NULL;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CSkillPoint::Init(void)
{
	// 種類の設定
	SetType(CObject::TYPE_OBJECT2D);

	// 初期化
	CObject2D::Init();
	SetType(CObject::TYPE_OBJECT2D);

	// テクスチャの割り当て
	int nIdx = CTexture::GetInstance()->Regist(TEXTURE);

	// テクスチャの割り当て
	BindTexture(nIdx);

	D3DXVECTOR2 size;
	// サイズ取得
	SetSize(CTexture::GetInstance()->GetImageSize(nIdx) * 0.1f);	// サイズ
	SetSizeOrigin(GetSize());	// サイズ
	SetPosition(MyLib::Vector3(1000.0f, 80.0f, 0.0f));	// 位置


	//// テクスチャの割り当て
	//int nMultiIdx = CTexture::GetInstance()->Regist(TEXTURE2);
	//multi = CTexture::GetInstance()->GetAdress(nMultiIdx);

	// 生成処理
	m_apNumber = CMultiNumber::Create({1150.0f, 80.0f, 0.0f}, D3DXVECTOR2(GetSize().x, GetSize().x), 2, CNumber::OBJECTTYPE_2D, NUMBER_TEXTURE, true);

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CSkillPoint::Uninit(void)
{
	// 数字のオブジェクトの終了処理
	if (m_apNumber != NULL)
	{
		m_apNumber->Uninit();
		m_apNumber = NULL;
	}

	// 終了処理
	CObject2D::Uninit();
}

//==========================================================================
// 削除処理
//==========================================================================
void CSkillPoint::Kill(void)
{
	// 数字のオブジェクト削除
	if (m_apNumber != NULL)
	{
		m_apNumber->Release();
		m_apNumber = NULL;
	}

	// 終了処理
	CObject2D::Uninit();
}

//==========================================================================
// 更新処理
//==========================================================================
void CSkillPoint::Update(void)
{
	// キーボード情報取得
	CInputKeyboard* pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();



	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();


	// ゲージの位置やサイズの更新（ここでゲージの位置やサイズを変更する）
	D3DXVECTOR2 size = GetSize();
	D3DXVECTOR2* pTex = GetTex();

	if (pInputKeyboard->GetPress(DIK_UP) == true)
	{//←キーが押された,左移動]
		pTex[1].x += 0.01f;
		pTex[3].x += 0.01f;
		/*size.x += 1.0f;*/
	}
	if (pInputKeyboard->GetPress(DIK_DOWN) == true)
	{//←キーが押された,左移動
		//size.x -= 1.0f;
		pTex[1].x -= 0.01f;
		pTex[3].x -= 0.01f;
	}
	SetSize(size);


	// クリッピング領域をゲージの進捗に合わせて変更
	float clipWidth = 2.0f * (GetSize().x / GetSizeOrigin().x) - 1.0f;



	// 更新処理
	CObject2D::Update();

	// 値の設定処理
	m_apNumber->Update();
	m_apNumber->SetValue(m_nPoint);


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
void CSkillPoint::AddPoint(void)
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
void CSkillPoint::SubPoint(void)
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
// 描画処理
//==========================================================================
void CSkillPoint::Draw(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();

	// 描画処理
	CObject2D::Draw();

}

