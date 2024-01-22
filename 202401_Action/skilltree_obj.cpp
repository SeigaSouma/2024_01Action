//=============================================================================
// 
//  スキルツリーオブジェクト処理 [skilltree_obj.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "skilltree_obj.h"
#include "manager.h"
#include "texture.h"
#include "renderer.h"
#include "sound.h"
#include "calculation.h"
#include "game.h"
#include "player.h"
#include "input.h"
#include "skilltree.h"

//==========================================================================
// 定数定義
//==========================================================================
namespace
{
	const char* BAGMODEL = "data\\MODEL\\skilltree_obj.x";
	const MyLib::Vector3 POSITION = MyLib::Vector3(0.0f, 0.0f, 1000.0f);	// 半径
	const float RADIUS = 80.0f;	// 半径
	const float TIME_DMG = static_cast<float>(30) / static_cast<float>(mylib_const::DEFAULT_FPS);		// ダメージ時間 
	const float TIME_INVICIBLE = static_cast<float>(60) / static_cast<float>(mylib_const::DEFAULT_FPS);	// 無敵時間
}

//==========================================================================
// 関数ポインタ
//==========================================================================
CSkillTree_Obj::STATE_FUNC CSkillTree_Obj::m_StateFuncList[] =
{
	&CSkillTree_Obj::StateNone,		// なにもなし
	&CSkillTree_Obj::StateTransfer,	// 転移
};

//==========================================================================
// コンストラクタ
//==========================================================================
CSkillTree_Obj::CSkillTree_Obj(int nPriority) : CObjectX(nPriority)
{
	// 値のクリア
	m_fStateTime = 0.0f;	// 状態カウンター
	m_state = STATE_NONE;	// 状態
}

//==========================================================================
// デストラクタ
//==========================================================================
CSkillTree_Obj::~CSkillTree_Obj()
{
	
}

//==========================================================================
// 生成処理
//==========================================================================
CSkillTree_Obj *CSkillTree_Obj::Create(void)
{
	// 生成用のオブジェクト
	CSkillTree_Obj *pBag = NULL;

	// メモリの確保
	pBag = DEBUG_NEW CSkillTree_Obj;

	if (pBag != NULL)
	{// メモリの確保が出来ていたら

		// 初期化処理
		pBag->Init();
	}

	return pBag;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CSkillTree_Obj::Init(void)
{

	// 種類の設定
	CObject::SetType(TYPE_OBJECTX);

	// 初期化処理
	HRESULT hr = CObjectX::Init(BAGMODEL);
	if (FAILED(hr))
	{
		return E_FAIL;
	}

	// 位置設定
	SetPosition(POSITION);
	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CSkillTree_Obj::Uninit(void)
{

	// 終了処理
	CObjectX::Uninit();
}

//==========================================================================
// 更新処理
//==========================================================================
void CSkillTree_Obj::Update(void)
{
	// 状態カウンター減算
	m_fStateTime -= CManager::GetInstance()->GetDeltaTime();

	// プレイヤーとの当たり判定
	CollisionPlayer();

	// 状態別処理
	(this->*(m_StateFuncList[m_state]))();
}

//==========================================================================
// プレイヤーとの当たり判定処理
//==========================================================================
void CSkillTree_Obj::CollisionPlayer(void)
{
	if (!CGame::GetGameManager()->IsControll())
	{// 行動できないとき
		return;
	}

	// プレイヤー取得
	CListManager<CPlayer> playerList = CPlayer::GetListObj();
	CPlayer* pPlayer = nullptr;

	MyLib::Vector3 pos = GetPosition();

	bool bHit = false;

	// リストループ
	while (playerList.ListLoop(&pPlayer))
	{
		pPlayer->SetEnableTouchBeacon(false);
		if (UtilFunc::Collision::CircleRange3D(pos, pPlayer->GetPosition(), RADIUS, pPlayer->GetRadius()))
		{
			bHit = true;
			pPlayer->SetEnableTouchBeacon(true);
			break;
		}
	}

	if (bHit)
	{
		// ゲームパッド情報取得
		CInputGamepad* pInputGamepad = CManager::GetInstance()->GetInputGamepad();
		if (pInputGamepad->GetTrigger(CInputGamepad::BUTTON_A, 0))
		{
			// スキルツリーに変更
			CGame::GetGameManager()->SetType(CGameManager::SCENE_SKILLTREE);

			// スキルツリー生成
			CSkillTree::Create();
		}
	}
}

//==========================================================================
// なにもない状態
//==========================================================================
void CSkillTree_Obj::StateNone(void)
{
	m_fStateTime = 0.0f;
}

//==========================================================================
// 転移状態
//==========================================================================
void CSkillTree_Obj::StateTransfer(void)
{
	m_fStateTime = 0.0f;

	// 向き取得
	MyLib::Vector3 rot = GetRotation();
	rot.y += D3DX_PI * 0.05f;
	UtilFunc::Transformation::RotNormalize(rot.y);

	// 向き設定
	SetRotation(rot);
}


//==========================================================================
// 描画処理
//==========================================================================
void CSkillTree_Obj::Draw(void)
{
	// 描画
	CObjectX::Draw();
}

