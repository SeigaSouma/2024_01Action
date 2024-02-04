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
#include "skilltree_line.h"
#include "skilltree_command.h"
#include "game.h"

//==========================================================================
// 定数定義
//==========================================================================
namespace
{
	const char* LOADTEXT = "data\\TEXT\\skilltree\\iconinfo.json";
	const char* AUTOSAVE_MASTERING = "data\\TEXT\\skilltree\\mastering.json";
	const float TIME_FADE = 0.4f;	// フェードアウト時間
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
	m_pCommand = nullptr;	// コマンドのオブジェクト
	m_bOnScreen = false;	// スクリーン上にいるかのフラグ
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
		m_pThisPtr->Init();
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

	// 後でjson読み込みに変更する
#if 0
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

	// Jsonへの書き込み
	SaveJson();
#else
	// Jsonからの読み込み
	LoadJson();
#endif

	// スキルアイコンの習得状況
	for (auto const& iconinfo : m_SkillInfo)
	{
		m_SkillIconMastering.push_back(CSkillTree_Icon::MASTERING_YET);
	}

	// 状態カウンター
	m_fStateTime = 0.0f;
	m_state = STATE_NONE;
	m_bOnScreen = false;	// スクリーン上にいるかのフラグ

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CSkillTree::Uninit(void)
{
	m_pThisPtr = nullptr;

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
	m_pSkillIcon.clear();	// スキルアイコン

	// スクリーンの終了
	if (m_pScreen != nullptr)
	{
		m_pScreen->Uninit();
		m_pScreen = nullptr;
	}

	// カーソル終了
	m_pCursor->Uninit();
	m_pCursor = nullptr;

	// コマンド終了
	if (m_pCommand != nullptr)
	{
		m_pCommand->Uninit();
		m_pCommand = nullptr;
	}

	m_pThisPtr = nullptr;
	// 情報削除
	Release();
}

//==========================================================================
// 更新処理
//==========================================================================
void CSkillTree::Update(void)
{
	if (IsDeath())
	{
		return;
	}

	if (!m_bOnScreen)
	{
		return;
	}

	// 状態別処理
	(this->*(m_StateFuncList[m_state]))();

	// 位置取得
	MyLib::Vector3 pos = GetPosition();

	if (!m_pSkillIcon.empty())
	{
		int i = 0;
		for (auto const& iconinfo : m_SkillInfo)
		{
			// アイコンの位置設定
			m_pSkillIcon[i]->SetPosition(pos + iconinfo.pos);
			i++;
		}
	}


	// ラインのリスト取得
	CListManager<CSkillTree_Line> lineList = CSkillTree_Line::GetListObj();
	CSkillTree_Line* pLine = nullptr;

	// リストループ
	while (lineList.ListLoop(&pLine))
	{
		pLine->SetPosition(pos);
		pLine->SetVtx();
	}

}

//==========================================================================
// 何もない状態
//==========================================================================
void CSkillTree::StateNone(void)
{
	if (!m_bOnScreen)
	{
		return;
	}

	// ゲームパッド情報取得
	CInputGamepad* pInputGamepad = CManager::GetInstance()->GetInputGamepad();
	if (pInputGamepad->GetTrigger(CInputGamepad::BUTTON_B, 0))
	{
		m_state = STATE_FADEOUT;
	}
}

//==========================================================================
// フェードイン状態
//==========================================================================
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

	// コマンドの透明度設定
	m_pCommand->SetAlpha(m_fAlpha);


	// ラインのリスト取得
	CListManager<CSkillTree_Line> lineList = CSkillTree_Line::GetListObj();
	CSkillTree_Line* pLine = nullptr;

	// リストループ
	while (lineList.ListLoop(&pLine))
	{
		// カーソルの透明度設定
		col = pLine->GetColor();
		col.a = m_fAlpha;
		pLine->SetColor(col);
	}


	if (m_fStateTime >= TIME_FADE)
	{
		m_fStateTime = TIME_FADE;
		m_state = STATE_NONE;
		return;
	}
}

//==========================================================================
// フェードアウト状態
//==========================================================================
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

	// コマンドの透明度設定
	m_pCommand->SetAlpha(m_fAlpha);


	// ラインのリスト取得
	CListManager<CSkillTree_Line> lineList = CSkillTree_Line::GetListObj();
	CSkillTree_Line* pLine = nullptr;

	// リストループ
	while (lineList.ListLoop(&pLine))
	{
		// カーソルの透明度設定
		col = pLine->GetColor();
		col.a = m_fAlpha;
		pLine->SetColor(col);
	}

	if (m_fStateTime <= 0)
	{
		m_fStateTime = 0.0f;

		// スキルツリーに変更
		CGame::GetInstance()->GetGameManager()->SetType(CGameManager::SCENE_ENHANCE);

		// スクリーンから捌ける
		OutScreen();

		// 削除
		//Kill();
		return;
	}
}


//==========================================================================
// Jsonからのロード
//==========================================================================
void CSkillTree::LoadJson(void)
{
	// ファイルからJSONを読み込む
	std::ifstream file(LOADTEXT);
	if (!file.is_open())
	{
		return;
	}

	nlohmann::json jsonData;
	file >> jsonData;	// jsonデータを与える

	// jsonデータから読み込む
	from_json(jsonData);
}

//==========================================================================
// Jsonへのセーブ
//==========================================================================
void CSkillTree::SaveJson(void)
{
	// ファイルにキャラクターのデータを書き込む
	std::ofstream outFile(LOADTEXT);
	if (!outFile.is_open())
	{
		return;
	}

	// 出力データ
	nlohmann::json outputData;
	to_json(outputData);

	// ファイルにデータを書き込む
	outFile << std::setw(4) << outputData << std::endl;
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

//==========================================================================
// 習得状況設定
//==========================================================================
void CSkillTree::SetMastering(int nIdx, CSkillTree_Icon::eMastering mastering)
{
	if (m_SkillIconMastering.empty() ||
		m_SkillIconMastering.size() <= nIdx)
	{
		return;
	}

	// 習得状況設定
	m_SkillIconMastering[nIdx] = mastering;

}

//==========================================================================
// スクリーン上に設定
//==========================================================================
void CSkillTree::SetScreen(void)
{
	if (m_bOnScreen)
	{
		return;
	}

	// 画面生成
	m_pScreen = CSkillTree_Screen::Create();

	for (auto const& iconinfo : m_SkillInfo)
	{
		int nIdx = static_cast<int>(m_pSkillIcon.size());
		m_pSkillIcon.push_back(nullptr);

		// スキルアイコン設定
		m_pSkillIcon[nIdx] = CSkillTree_Icon::Create(iconinfo);

		// 習得済みのものは能力付与
		m_pSkillIcon[nIdx]->SetMastering(m_SkillIconMastering[nIdx]);

		if (iconinfo.parentID != -1)
		{
			CSkillTree_Line::Create(iconinfo.pos, m_SkillInfo[iconinfo.parentID].pos);
		}
	}

	// カーソル生成
	m_pCursor = CSkillTree_Cursor::Create(0);

	// コマンド生成
	m_pCommand = CSkillTree_Command::Create();

	// 状態カウンター
	m_fStateTime = 0.0f;
	m_state = STATE_FADEIN;

	// スクリーン上にいるかのフラグ
	m_bOnScreen = true;
}

//==========================================================================
// スクリーンから捌ける
//==========================================================================
void CSkillTree::OutScreen(void)
{
	if (!m_bOnScreen)
	{
		return;
	}

	// アイコンの終了
	for (auto const& iconinfo : m_pSkillIcon)
	{
		iconinfo->Uninit();
	}
	m_pSkillIcon.clear();

	// スクリーンの終了
	m_pScreen->Uninit();
	m_pScreen = nullptr;

	// カーソル終了
	m_pCursor->Uninit();
	m_pCursor = nullptr;

	// コマンド終了
	m_pCommand->Uninit();
	m_pCommand = nullptr;

	// スクリーン上にいるかのフラグ
	m_bOnScreen = false;
}
