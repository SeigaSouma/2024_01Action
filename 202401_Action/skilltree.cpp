//=============================================================================
// 
//  スキルツリー処理 [skilltree.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "skilltree.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"
#include "object2D.h"
#include "calculation.h"
#include "input.h"
#include "skilltree_screen.h"
#include "skilltree_cursor.h"
#include "game.h"

//==========================================================================
// 定数定義
//==========================================================================
namespace
{
	const float TIME_FADE = 0.5f;	// フェードアウト時間
	const int MAX_ICON = 4;		// アイコン
}

//==========================================================================
// 静的メンバ変数定義
//==========================================================================
CSkillTree* CSkillTree::m_pThisPtr = nullptr;	// 自身のポインタ

//==========================================================================
// 関数ポインタ
//==========================================================================
CSkillTree::STATE_FUNC CSkillTree::m_StateFuncList[] =
{
	&CSkillTree::StateNone,		// なにもなし
	&CSkillTree::StateFadeIn,	// フェードイン
	&CSkillTree::StateFadeOut,	// フェードアウト
};

//==========================================================================
// コンストラクタ
//==========================================================================
CSkillTree::CSkillTree(int nPriority) : CObject(nPriority)
{
	// 値のクリア
	m_fAlpha = 0.0f;		// 不透明度
	m_fStateTime = 0.0f;	// 状態遷移カウンター
	m_state = STATE_NONE;	// 状態
	m_SkillInfo.clear();	// スキルアイコン
	m_pScreen = nullptr;	// スクリーンのオブジェクト
	m_pCursor = nullptr;	// カーソルのオブジェクト
}

//==========================================================================
// デストラクタ
//==========================================================================
CSkillTree::~CSkillTree()
{

}

//==========================================================================
// 生成処理
//==========================================================================
CSkillTree *CSkillTree::Create(void)
{
	if (m_pThisPtr == NULL)
	{// まだ生成していなかったら

		// インスタンス生成
		m_pThisPtr = DEBUG_NEW CSkillTree;

		// 初期化処理
		m_pThisPtr->Init();
	}
	else
	{
		// インスタンス取得
		m_pThisPtr->GetInstance();
	}

	return m_pThisPtr;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CSkillTree::Init(void)
{
	// 種類の設定
	SetType(CObject::TYPE_OBJECT2D);

	// 画面生成
	m_pScreen = CSkillTree_Screen::Create();

	// 後でjson読み込みに変更する
#if 1
	CSkillTree_Icon::sSkillIcon info = CSkillTree_Icon::sSkillIcon();

	for (int i = 0; i < MAX_ICON; i++)
	{
		info.ID = i;
		info.parentID = i - 1;
		info.needpoint = 5;
		info.texID = 0;
		info.pos = MyLib::Vector3(200.0f+i * 200.0f, 360.0f, 0.0f);

		m_SkillInfo.push_back(info);
	}
#endif

	for (auto const& iconinfo : m_SkillInfo)
	{
		int nIdx = static_cast<int>(m_pSkillIcon.size());
		m_pSkillIcon.push_back(nullptr);

		// スキルアイコン設定
		m_pSkillIcon[nIdx] = CSkillTree_Icon::Create();

		// 愛好情報設定
		m_pSkillIcon[nIdx]->SetIconInfo(iconinfo);
	}


	// 状態カウンター
	m_fStateTime = 0.0f;
	m_state = STATE_FADEIN;

	// カーソル生成
	m_pCursor = CSkillTree_Cursor::Create(0);

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CSkillTree::Uninit(void)
{
	
	// 情報削除
	Release();
}

//==========================================================================
// 削除
//==========================================================================
void CSkillTree::Kill(void)
{
	// アイコンの終了
	for (auto const& iconinfo : m_pSkillIcon)
	{
		iconinfo->Uninit();
	}

	// スクリーンの終了
	m_pScreen->Uninit();

	// カーソル終了
	m_pCursor->Uninit();

	// 情報削除
	Release();
}

//==========================================================================
// 更新処理
//==========================================================================
void CSkillTree::Update(void)
{
	// 状態別処理
	(this->*(m_StateFuncList[m_state]))();

	// 位置取得
	MyLib::Vector3 pos = GetPosition();

	int i = 0;
	for (auto const& iconinfo : m_SkillInfo)
	{
		// アイコンの位置設定
		m_pSkillIcon[i]->SetPosition(pos + iconinfo.pos);
		i++;
	}
}


//==================================================================================
// 何もない状態
//==================================================================================
void CSkillTree::StateNone(void)
{

	// ゲームパッド情報取得
	CInputGamepad* pInputGamepad = CManager::GetInstance()->GetInputGamepad();
	if (pInputGamepad->GetTrigger(CInputGamepad::BUTTON_B, 0))
	{
		m_state = STATE_FADEOUT;
	}
}

//==================================================================================
// フェードイン状態
//==================================================================================
void CSkillTree::StateFadeIn(void)
{
	// 状態遷移カウンター減算
	m_fStateTime += CManager::GetInstance()->GetDeltaTime();

	// 不透明度更新
	m_fAlpha = m_fStateTime / TIME_FADE;

	for (auto const& icon : m_pSkillIcon)
	{
		// 色取得
		D3DXCOLOR col = icon->GetColor();

		// 不透明度設定
		col.a = m_fAlpha;

		// 色設定
		icon->SetColor(col);
	}

	// スクリーンの透明度設定
	D3DXCOLOR col = m_pScreen->GetColor();
	col.a = m_fAlpha;
	m_pScreen->SetColor(col);

	// カーソルの透明度設定
	col = m_pCursor->GetColor();
	col.a = m_fAlpha;
	m_pCursor->SetColor(col);

	if (m_fStateTime >= TIME_FADE)
	{
		m_fStateTime = TIME_FADE;
		m_state = STATE_NONE;
		return;
	}
}

//==================================================================================
// フェードアウト状態
//==================================================================================
void CSkillTree::StateFadeOut(void)
{
	// 状態遷移カウンター減算
	m_fStateTime -= CManager::GetInstance()->GetDeltaTime();

	// 不透明度更新
	m_fAlpha = m_fStateTime / TIME_FADE;

	for (auto const& icon : m_pSkillIcon)
	{
		// 色取得
		D3DXCOLOR col = icon->GetColor();

		// 不透明度設定
		col.a = m_fAlpha;

		// 色設定
		icon->SetColor(col);
	}

	// スクリーンの透明度設定
	D3DXCOLOR col = m_pScreen->GetColor();
	col.a = m_fAlpha;
	m_pScreen->SetColor(col);

	// カーソルの透明度設定
	col = m_pCursor->GetColor();
	col.a = m_fAlpha;
	m_pCursor->SetColor(col);

	if (m_fStateTime <= 0)
	{
		m_fStateTime = 0.0f;

		// スキルツリーに変更
		CGame::GetGameManager()->SetType(CGameManager::SCENE_ENHANCE);

		// 削除
		Kill();
		return;
	}
}

//==========================================================================
// 描画処理
//==========================================================================
void CSkillTree::Draw(void)
{
	
}

//==========================================================================
// アイコン取得
//==========================================================================
std::vector<CSkillTree_Icon*> CSkillTree::GetIcon(void) const
{
	return m_pSkillIcon;
}
