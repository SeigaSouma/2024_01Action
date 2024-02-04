//=============================================================================
// 
// スキルツリーアイコン処理 [skilltree_icon.cpp]
// Author : 相馬靜雅
// 
//=============================================================================
#include "skilltree_icon.h"
#include "renderer.h"
#include "texture.h"
#include "manager.h"
#include "sound.h"
#include "input.h"
#include "game.h"
#include "calculation.h"
#include "player.h"
#include "skilltree.h"
#include "skillpoint.h"
#include "skilltree_behavior.h"
#include "multinumber.h"

//==========================================================================
// マクロ定義
//==========================================================================
namespace
{
	const D3DXCOLOR COLOR[] =
	{
		D3DXCOLOR(0.4f, 0.4f, 0.4f, 1.0f),	// 未習得
		D3DXCOLOR(1.0f, 0.4f, 1.0f, 1.0f),	// 習得済み
		D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f),	// 習得可能
	};
	const char* LOADTEXT = "data\\TEXT\\skilltree\\icontexture.txt";
	const char* NUMBER_TEXTURE = "data\\TEXTURE\\number\\number_blackclover_01.png";
	const float SIZE_ICON = 50.0f;	// アイコンサイズ
	const float TIME_FADE = 0.5f;	// フェードアウト時間
	const MyLib::Vector3 DISTANCE = MyLib::Vector3(SIZE_ICON, SIZE_ICON, 0.0f);
}

std::vector<int> CSkillTree_Icon::m_nTexIdx = {};	// テクスチャインデックス番号
bool CSkillTree_Icon::m_bLoadComplete = false;		// ロード完了のフラグ
CListManager<CSkillTree_Icon> CSkillTree_Icon::m_List = {};	// リスト

//==========================================================================
// 関数ポインタ
//==========================================================================
CSkillTree_Icon::STATE_FUNC CSkillTree_Icon::m_StateFuncList[] =
{
	&CSkillTree_Icon::StateNone,		// なにもなし
};

//==========================================================================
// コンストラクタ
//==========================================================================
CSkillTree_Icon::CSkillTree_Icon(int nPriority) : CObject2D(nPriority)
{
	// 値のクリア
	m_fStateTime = 0;				// 状態遷移カウンター
	m_state = STATE_NONE;			// 状態
	m_Mastering = MASTERING_YET;	// 習得状態
	m_SkillIconInfo = sSkillIcon();	// スキルアイコン
	m_pAbillity = nullptr;			// 能力のオブジェクト
}

//==========================================================================
// デストラクタ
//==========================================================================
CSkillTree_Icon::~CSkillTree_Icon()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CSkillTree_Icon* CSkillTree_Icon::Create(sSkillIcon iconinfo)
{
	// 生成用のオブジェクト
	CSkillTree_Icon* pEffect = nullptr;

	// メモリの確保
	pEffect = DEBUG_NEW CSkillTree_Icon;

	if (pEffect != nullptr)
	{
		// アイコン情報設定
		pEffect->m_SkillIconInfo = iconinfo;

		// 初期化処理
		pEffect->Init();
	}

	return pEffect;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CSkillTree_Icon::Init(void)
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
	BindTexture(m_nTexIdx[m_SkillIconInfo.texID]);

	// サイズ設定
	SetSize(D3DXVECTOR2(SIZE_ICON, SIZE_ICON));
	SetSizeOrigin(GetSize());

	// 状態カウンター
	m_fStateTime = 0.0f;

	// 位置設定
	SetPosition(MyLib::Vector3(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f, 0.0f));

	// 色設定
	SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));

	// 能力割り当て
	m_pAbillity = CAbillityStrategy::CreateInstance(m_SkillIconInfo, CPlayer::GetListObj().GetData(0));

	// リストに追加
	m_List.Regist(this);


	// 生成処理
	m_apNumber = CMultiNumber::Create(GetPosition() + DISTANCE, GetSize() * 0.5f, 1, CNumber::OBJECTTYPE_2D, NUMBER_TEXTURE, true);
	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CSkillTree_Icon::Uninit(void)
{

	// リストから削除
	m_List.Delete(this);

	if (m_pAbillity != nullptr)
	{
		delete m_pAbillity;
		m_pAbillity = nullptr;
	}

	if (m_apNumber != nullptr)
	{
		m_apNumber->Uninit();
		m_apNumber = nullptr;
	}

	// 終了処理
	CObject2D::Uninit();
}

//==========================================================================
// 終了処理
//==========================================================================
void CSkillTree_Icon::Kill(void)
{
	// リストから削除
	m_List.Delete(this);

	if (m_pAbillity != nullptr)
	{
		delete m_pAbillity;
		m_pAbillity = nullptr;
	}

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
void CSkillTree_Icon::Update(void)
{
	// 状態別処理
	(this->*(m_StateFuncList[m_state]))();
	if (IsDeath())
	{
		return;
	}

	if (m_Mastering != MASTERING_DONE &&
		(m_SkillIconInfo.parentID == -1 ||
		m_List.GetData(m_SkillIconInfo.parentID)->GetMastering() == MASTERING_DONE))
	{
		// 習得可能！
		m_Mastering = MASTERING_POSSIBLE;
	}

	D3DXCOLOR col = COLOR[m_Mastering];
	col.a = GetColor().a;
	SetColor(col);

	if (m_apNumber != nullptr)
	{
		m_apNumber->SetValue(m_SkillIconInfo.needpoint);
		m_apNumber->SetPosition(GetPosition() + DISTANCE);
		m_apNumber->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, GetColor().a));
	}


	// 状態保存
	CSkillTree::GetInstance()->SetMastering(m_SkillIconInfo.ID, m_Mastering);

	// 頂点座標の設定
	SetVtx();
}

//==========================================================================
// 何もない状態
//==========================================================================
void CSkillTree_Icon::StateNone(void)
{

}

//==========================================================================
// 描画処理
//==========================================================================
void CSkillTree_Icon::Draw(void)
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
// アイコン情報設定
//==========================================================================
void CSkillTree_Icon::SetIconInfo(sSkillIcon iconinfo)
{
	m_SkillIconInfo = iconinfo;
}

//==========================================================================
// アイコン情報取得
//==========================================================================
CSkillTree_Icon::sSkillIcon CSkillTree_Icon::GetIconInfo(void)
{
	return m_SkillIconInfo;
}

//==========================================================================
// 初期能力割り当て
//==========================================================================
void CSkillTree_Icon::BindStartAvillity(void)
{
	// 能力割り当て
	m_pAbillity->BindAbillity();

	// 習得済みにする
	m_Mastering = MASTERING_DONE;
}

//==========================================================================
// 能力割り当て
//==========================================================================
bool CSkillTree_Icon::BindAvillity(void)
{
	if (m_Mastering != MASTERING_POSSIBLE)
	{// 習得可能以外はリターン
		return false;
	}

	CPlayer* pPlayer = CPlayer::GetListObj().GetData(0);
	if (m_Mastering == MASTERING_POSSIBLE &&
		pPlayer->GetSkillPoint()->GetPoint() >= m_SkillIconInfo.needpoint)
	{// 習得可能 && ポイント足りてる

		// 能力割り当て
		m_pAbillity->BindAbillity();

		// スキルポイント減算
		pPlayer->GetSkillPoint()->SubPoint(m_SkillIconInfo.needpoint);

		// 習得済みにする
		m_Mastering = MASTERING_DONE;
		return true;
	}
	return false;
}

//==========================================================================
// 頂点情報設定処理
//==========================================================================
void CSkillTree_Icon::SetVtx(void)
{
	// 頂点設定
	CObject2D::SetVtx();
}


//==========================================================================
// テクスチャ読み込み処理
//==========================================================================
HRESULT CSkillTree_Icon::ReadTexture(void)
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