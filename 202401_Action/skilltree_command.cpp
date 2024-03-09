//=============================================================================
// 
// スキルツリーコマンド処理 [skilltree_command.cpp]
// Author : 相馬靜雅
// 
//=============================================================================
#include "skilltree_command.h"
#include "texture.h"
#include "calculation.h"

//==========================================================================
// 定数定義
//==========================================================================
namespace
{
	const char* TEXTURE[] =
	{ 
		"data\\TEXTURE\\skilltree\\skilltree_back.png",
		"data\\TEXTURE\\skilltree\\skilltree_get.png",
		"data\\TEXTURE\\skilltree\\skilltree_reset.png",
	};
	const MyLib::Vector3 CREATE_POSITION = MyLib::Vector3(100.0f, 650.0f, 0.0f);	// 生成位置
	const float DISTANCE = 180.0f;	// 間隔
}

//==========================================================================
// コンストラクタ
//==========================================================================
CSkillTree_Command::CSkillTree_Command(int nPriority, const LAYER layer) : CObject(nPriority, layer)
{
	// 値のクリア
	memset(m_pIcon, 0, sizeof(m_pIcon));
}

//==========================================================================
// デストラクタ
//==========================================================================
CSkillTree_Command::~CSkillTree_Command()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CSkillTree_Command* CSkillTree_Command::Create()
{
	// メモリの確保
	CSkillTree_Command* pEffect = DEBUG_NEW CSkillTree_Command;

	if (pEffect != nullptr)
	{// メモリの確保が出来ていたら

		// 初期化処理
		pEffect->Init();
	}

	return pEffect;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CSkillTree_Command::Init()
{
	HRESULT hr;

	// 種類の設定
	SetType(TYPE_OBJECT2D);

	for (int i = 0; i < ICONTYPE_MAX; i++)
	{
		m_pIcon[i] = CObject2D::Create(GetPriority());
		if (m_pIcon[i] == nullptr)
		{
			continue;
		}
		m_pIcon[i]->SetType(CObject::TYPE_OBJECT2D);

		// テクスチャの割り当て
		int nIdxTex = CTexture::GetInstance()->Regist(TEXTURE[i]);
		m_pIcon[i]->BindTexture(nIdxTex);

		// 初期設定
		MyLib::Vector3 pos = CREATE_POSITION;
		pos.x += DISTANCE * i;
		m_pIcon[i]->SetPosition(pos);

		// テクスチャサイズ取得
		D3DXVECTOR2 size = CTexture::GetInstance()->GetImageSize(nIdxTex);
		m_pIcon[i]->SetSize(UtilFunc::Transformation::AdjustSizeByHeight(size, 50.0f));
	}

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CSkillTree_Command::Uninit()
{
	for (int i = 0; i < ICONTYPE_MAX; i++)
	{
		if (m_pIcon[i] == nullptr)
		{
			continue;
		}
		m_pIcon[i]->Uninit();
		m_pIcon[i] = nullptr;
	}

	// 終了処理
	Release();
}

//==========================================================================
// 不透明度設定
//==========================================================================
void CSkillTree_Command::SetAlpha(float alpha)
{
	for (int i = 0; i < ICONTYPE_MAX; i++)
	{
		if (m_pIcon[i] == nullptr)
		{
			continue;
		}
		D3DXCOLOR col = m_pIcon[i]->GetColor();
		col.a = alpha;
		m_pIcon[i]->SetColor(col);
	}
}
