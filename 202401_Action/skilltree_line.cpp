//=============================================================================
// 
// スキルツリーライン処理 [skilltree_line.cpp]
// Author : 相馬靜雅
// 
//=============================================================================
#include "skilltree_line.h"
#include "renderer.h"
#include "texture.h"
#include "manager.h"
#include "sound.h"
#include "input.h"
#include "game.h"
#include "calculation.h"
#include "skilltree_icon.h"

//==========================================================================
// マクロ定義
//==========================================================================
namespace
{
	const char* TEXTURE = "data\\TEXTURE\\skilltree\\line_01.png";
	const float SIZE_HEIGHT = 15.0f;
}
CListManager<CSkillTree_Line> CSkillTree_Line::m_List = {};	// リスト

//==========================================================================
// コンストラクタ
//==========================================================================
CSkillTree_Line::CSkillTree_Line(int nPriority) : CObject2D(nPriority)
{
	// 値のクリア
	m_SkillLine = sSkillLine();	// スキルライン
}

//==========================================================================
// デストラクタ
//==========================================================================
CSkillTree_Line::~CSkillTree_Line()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CSkillTree_Line* CSkillTree_Line::Create(const MyLib::Vector3& myposition, const MyLib::Vector3& parentposition)
{
	// 生成用のオブジェクト
	CSkillTree_Line* pEffect = nullptr;

	// メモリの確保
	pEffect = DEBUG_NEW CSkillTree_Line;

	if (pEffect != nullptr)
	{
		// スキルラインの情報
		pEffect->m_SkillLine.mypos = myposition;
		pEffect->m_SkillLine.parentpos = parentposition;

		// 初期化処理
		pEffect->Init();
	}

	return pEffect;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CSkillTree_Line::Init()
{
	// 初期化処理
	HRESULT hr = CObject2D::Init();
	if (FAILED(hr))
	{// 失敗したとき
		return E_FAIL;
	}

	// 種類の設定
	SetType(TYPE_OBJECT2D);

	// テクスチャの割り当て
	int nIdx = CTexture::GetInstance()->Regist(TEXTURE);
	BindTexture(nIdx);

	float fSize = UtilFunc::Calculation::GetFabsPosLength2D(m_SkillLine.mypos, m_SkillLine.parentpos);
	fSize *= 0.5f;

	// サイズ設定
	SetSize(D3DXVECTOR2(fSize, SIZE_HEIGHT));
	SetSizeOrigin(GetSize());


	// 目標の角度を求める
	float rotZ = atan2f((m_SkillLine.mypos.x - m_SkillLine.parentpos.x), (m_SkillLine.mypos.y - m_SkillLine.parentpos.y));
	rotZ += D3DX_PI * 0.5f;
	UtilFunc::Transformation::RotNormalize(rotZ);

	// 向き設定
	SetRotation(MyLib::Vector3(0.0f, 0.0f, rotZ));


	MyLib::Vector3 distance = m_SkillLine.parentpos - m_SkillLine.mypos;
	distance *= 0.5f;
	SetPosition(m_SkillLine.mypos + distance);
	SetOriginPosition(GetPosition());


	// 色設定
	SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));

	// 追加
	m_List.Regist(this);
	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CSkillTree_Line::Uninit()
{
	// 削除
	m_List.Delete(this);

	// 終了処理
	CObject2D::Uninit();
}

//==========================================================================
// 更新処理
//==========================================================================
void CSkillTree_Line::Update()
{
	MyLib::Vector3 distance = m_SkillLine.parentpos - m_SkillLine.mypos;
	distance *= 0.5f;
	SetOriginPosition(m_SkillLine.mypos + distance);

	SetPosition(GetPosition() + GetOriginPosition());

	// 更新処理
	CObject2D::Update();
}

//==========================================================================
// 頂点更新
//==========================================================================
void CSkillTree_Line::SetVtx()
{
	MyLib::Vector3 distance = m_SkillLine.parentpos - m_SkillLine.mypos;
	distance *= 0.5f;
	SetOriginPosition(m_SkillLine.mypos + distance);

	SetPosition(GetPosition() + GetOriginPosition());

	CObject2D::SetVtx();
}

void CSkillTree_Line::SetSkillLineInfo(const MyLib::Vector3& myposition, const MyLib::Vector3& parentposition)
{
	// スキルラインの情報
	m_SkillLine.mypos = myposition;
	m_SkillLine.parentpos = parentposition;
}

//==========================================================================
// 描画処理
//==========================================================================
void CSkillTree_Line::Draw()
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

