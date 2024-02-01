//=============================================================================
// 
//  サンタの袋処理 [santabag.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "santabag.h"
#include "manager.h"
#include "texture.h"
#include "renderer.h"
#include "sound.h"
#include "calculation.h"
#include "game.h"
#include "fade.h"

//==========================================================================
// 定数定義
//==========================================================================
namespace
{
	const char* BAGMODEL = "data\\MODEL\\santabag_01.x";
	const int MAX_LIFE = 10;	// 体力の最大値
	const float TIME_DMG = static_cast<float>(30) / static_cast<float>(60);			// ダメージ時間 
	const float TIME_INVICIBLE = static_cast<float>(60) / static_cast<float>(60);	// 無敵時間
	const float NUM_TKTK = 10.0f;		// チカチカする回数
}

//==========================================================================
// 関数ポインタ
//==========================================================================
CSantaBag::STATE_FUNC CSantaBag::m_StateFuncList[] =
{
	&CSantaBag::StateNone,
	&CSantaBag::StateDamage,
	&CSantaBag::StateInvicible,
};

//==========================================================================
// 静的メンバ変数
//==========================================================================
CListManager<CSantaBag> CSantaBag::m_List = {};	// リスト


//==========================================================================
// コンストラクタ
//==========================================================================
CSantaBag::CSantaBag(int nPriority) : CObjectX(nPriority)
{
	// 値のクリア
	m_fStateTime = 0.0f;	// 状態カウンター
	m_state = STATE_NONE;	// 状態
	m_MatCol = mylib_const::DEFAULT_COLOR;	// マテリアルカラー
	m_nLife = 0;			// 寿命
	m_nLifeMax = 0;			// 寿命の最大値
}

//==========================================================================
// デストラクタ
//==========================================================================
CSantaBag::~CSantaBag()
{
	
}

//==========================================================================
// 生成処理
//==========================================================================
CSantaBag *CSantaBag::Create(const MyLib::Vector3 pos)
{
	// 生成用のオブジェクト
	CSantaBag *pBag = NULL;

	// メモリの確保
	pBag = DEBUG_NEW CSantaBag;

	if (pBag != NULL)
	{// メモリの確保が出来ていたら

		// 位置割り当て
		pBag->SetPosition(pos);

		// 初期化処理
		pBag->Init();
	}

	return pBag;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CSantaBag::Init(void)
{
	// 各種変数の初期化
	m_nLifeMax = MAX_LIFE;
	m_nLife = m_nLifeMax;	// 寿命

	// リストに追加
	m_List.Regist(this);

	// 種類の設定
	CObject::SetType(TYPE_OBJECTX);

	// 初期化処理
	HRESULT hr = CObjectX::Init(BAGMODEL);
	if (FAILED(hr))
	{// 失敗したとき
		return E_FAIL;
	}
	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CSantaBag::Uninit(void)
{

	// リストから削除
	m_List.Delete(this);

	// 終了処理
	CObjectX::Uninit();
}

//==========================================================================
// 更新処理
//==========================================================================
void CSantaBag::Update(void)
{
	// 状態カウンター減算
	m_fStateTime -= CManager::GetInstance()->GetDeltaTime();

	// 状態別処理
	(this->*(m_StateFuncList[m_state]))();

	if (m_nLife < 0)
	{
		// クリア失敗
		CGame::GetInstance()->SetEnableClear(false);

		// 終了処理
		Uninit();

		// リザルトに遷移
		CManager::GetInstance()->GetFade()->SetFade(CScene::MODE_RESULT);
	}
}

//==========================================================================
// なにもない状態
//==========================================================================
void CSantaBag::StateNone(void)
{
	// 通常色に変更
	m_MatCol = mylib_const::DEFAULT_COLOR;
	if (m_fStateTime <= 0.0f)
	{
		m_fStateTime = 0.0f;
	}
}

//==========================================================================
// ダメージ状態
//==========================================================================
void CSantaBag::StateDamage(void)
{
	// 赤色に変更
	m_MatCol = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);

	if (m_fStateTime <= 0.0f)
	{
		m_state = STATE_INVINCIBLE;
		m_fStateTime = TIME_INVICIBLE;
	}
}

//==========================================================================
// 無敵状態
//==========================================================================
void CSantaBag::StateInvicible(void)
{
	// 赤色に変更
	m_MatCol = D3DXCOLOR(1.0f, 1.0f, 1.0f, sinf(D3DX_PI * (TIME_INVICIBLE - m_fStateTime) * NUM_TKTK));

	if (m_fStateTime <= 0.0f)
	{
		m_state = STATE_NONE;
		m_fStateTime = 0.0f;
	}
}

//==========================================================================
// ヒット処理
//==========================================================================
void CSantaBag::Hit(void)
{
	if (m_state != STATE_NONE)
	{
		return;
	}

	// 体力減らす
	m_nLife--;

	// ダメージ状態に設定
	m_state = STATE_DMG;
	m_fStateTime = TIME_DMG;

	// 袋ダメージ音
	CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL_SE_SANTABAG_DMG);
}

//==========================================================================
// 描画処理
//==========================================================================
void CSantaBag::Draw(void)
{
	// 描画
	if (m_state == STATE_NONE)
	{
		CObjectX::Draw();
	}
	else
	{
		CObjectX::Draw(m_MatCol);
	}
}

