//=============================================================================
// 
// スキルツリーウィンドウ処理 [skilltree_description.cpp]
// Author : 相馬靜雅
// 
//=============================================================================
#include "skilltree_description.h"
#include "renderer.h"
#include "texture.h"
#include "manager.h"
#include "calculation.h"

//==========================================================================
// マクロ定義
//==========================================================================
namespace
{
	const char* LOADTEXT = "data\\TEXT\\skilltree\\descriptiontexture.txt";
}

//==========================================================================
// 静的メンバ変数定義
//==========================================================================
std::vector<int> CSkillTree_Description::m_nTexIdx = {};	// テクスチャインデックス番号
bool CSkillTree_Description::m_bLoadComplete = false;		// ロード完了のフラグ

//==========================================================================
// コンストラクタ
//==========================================================================
CSkillTree_Description::CSkillTree_Description(int nPriority) : CObject2D(nPriority)
{
	// 値のクリア
	m_nCurrentIdx = 0;	// テクスチャのインデックス番号
}

//==========================================================================
// デストラクタ
//==========================================================================
CSkillTree_Description::~CSkillTree_Description()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CSkillTree_Description* CSkillTree_Description::Create()
{
	// 生成用のオブジェクト
	CSkillTree_Description* pWindow = nullptr;

	// メモリの確保
	pWindow = DEBUG_NEW CSkillTree_Description;

	if (pWindow != nullptr)
	{
		// 初期化処理
		pWindow->Init();
	}

	return pWindow;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CSkillTree_Description::Init()
{
	// 初期化処理
	HRESULT hr = CObject2D::Init();
	if (FAILED(hr))
	{// 失敗したとき
		return E_FAIL;
	}

	// 種類の設定
	SetType(TYPE_OBJECT2D);

	// テクスチャ読み込み
	if (!m_bLoadComplete)
	{
		ReadTexture();
	}

	// テクスチャの割り当て
	BindTexture(m_nTexIdx[0]);

	// サイズ設定
	D3DXVECTOR2 size = CTexture::GetInstance()->GetImageSize(m_nTexIdx[0]);
	size = UtilFunc::Transformation::AdjustSizeByWidth(size, 160.0f);
	SetSize(size);
	SetSizeOrigin(GetSize());

	// 位置設定
	SetPosition(MyLib::Vector3(1050.0f, 430.0f, 0.0f));

	// 色設定
	SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));

	return S_OK;
}

//==========================================================================
// 更新処理
//==========================================================================
void CSkillTree_Description::Update()
{
	CObject2D::Update();

	// テクスチャの割り当て
	BindTexture(m_nTexIdx[m_nCurrentIdx]);
}

//==========================================================================
// テクスチャのインデックス番号
//==========================================================================
void CSkillTree_Description::SetIdxTex(int nIdx)
{ 
	int idx = nIdx;
	if (idx >= static_cast<int>(m_nTexIdx.size()))
	{
		idx = static_cast<int>(m_nTexIdx.size()) - 1;
	}
	m_nCurrentIdx = idx;
}

//==========================================================================
// テクスチャ読み込み処理
//==========================================================================
HRESULT CSkillTree_Description::ReadTexture()
{
	char aComment[MAX_COMMENT] = {};	// コメント用
	int nTexNum = 0;					// ファイルの数
	int nCntTexture = 0;				// テクスチャ読み込みカウント

	// ファイルポインタ
	FILE* pFile = NULL;

	//ファイルを開く
	pFile = fopen(LOADTEXT, "r");

	if (pFile == NULL)
	{//ファイルが開けた場合
		return E_FAIL;
	}

	while (1)
	{// END_SCRIPTが来るまで繰り返す

		// 文字列の読み込み
		fscanf(pFile, "%s", &aComment[0]);

		// テクスチャ数の設定
		if (strcmp(&aComment[0], "NUM_TEXTURE") == 0)
		{// NUM_MODELがきたら

			fscanf(pFile, "%s", &aComment[0]);	// =の分
			fscanf(pFile, "%d", &nTexNum);	// テクスチャ数
		}

		while (nCntTexture != nTexNum)
		{// テクスチャの数分読み込むまで繰り返し

			fscanf(pFile, "%s", &aComment[0]);	// =の分

			if (strcmp(&aComment[0], "TEXTURE_FILENAME") == 0)
			{// TEXTURE_FILENAMEがきたら

				fscanf(pFile, "%s", &aComment[0]);	// =の分
				fscanf(pFile, "%s", &aComment[0]);	// ファイル名

				// テクスチャの割り当て
				int nIdx = CTexture::GetInstance()->Regist(&aComment[0]);
				m_nTexIdx.push_back(nIdx);

				nCntTexture++;	// テクスチャ数加算
			}
		}

		if (strcmp(&aComment[0], "END_SCRIPT") == 0)
		{// 終了文字でループを抜ける

			break;
		}
	}

	// ファイルを閉じる
	fclose(pFile);

	// ロード完了のフラグを立てる
	m_bLoadComplete = true;
	return S_OK;
}