//=============================================================================
// 
//  転移ビーコン処理 [transferBeacon.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "transferBeacon.h"
#include "manager.h"
#include "texture.h"
#include "renderer.h"
#include "sound.h"
#include "calculation.h"
#include "game.h"
#include "fade.h"
#include "instantfade.h"
#include "player.h"
#include "input.h"

//==========================================================================
// 定数定義
//==========================================================================
namespace
{
	const char* BAGMODEL = "data\\MODEL\\compactcore_01.x";
	const float RADIUS = 80.0f;	// 半径
	const float TIME_DMG = static_cast<float>(30) / static_cast<float>(mylib_const::DEFAULT_FPS);		// ダメージ時間 
	const float TIME_INVICIBLE = static_cast<float>(60) / static_cast<float>(mylib_const::DEFAULT_FPS);	// 無敵時間
}

//==========================================================================
// 関数ポインタ
//==========================================================================
CTransferBeacon::STATE_FUNC CTransferBeacon::m_StateFuncList[] =
{
	&CTransferBeacon::StateNone,		// なにもなし
	&CTransferBeacon::StateTransfer,	// 転移
};

//==========================================================================
// 静的メンバ変数
//==========================================================================
CListManager<CTransferBeacon> CTransferBeacon::m_List = {};	// リスト

//==========================================================================
// コンストラクタ
//==========================================================================
CTransferBeacon::CTransferBeacon(int nPriority) : CObjectX(nPriority)
{
	// 値のクリア
	m_fStateTime = 0.0f;	// 状態カウンター
	m_state = STATE_NONE;	// 状態
	m_TransType = TRANSTYPE_ENHANCE;	// 転移種類
}

//==========================================================================
// デストラクタ
//==========================================================================
CTransferBeacon::~CTransferBeacon()
{
	
}

//==========================================================================
// 生成処理
//==========================================================================
CTransferBeacon *CTransferBeacon::Create(eTransType transtype, const MyLib::Vector3 pos)
{
	// 生成用のオブジェクト
	CTransferBeacon *pBag = NULL;

	// メモリの確保
	pBag = DEBUG_NEW CTransferBeacon;

	if (pBag != NULL)
	{// メモリの確保が出来ていたら

		// 転移種類
		pBag->m_TransType = transtype;

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
HRESULT CTransferBeacon::Init(void)
{
	// リストに追加
	m_List.Regist(this);

	// 種類の設定
	CObject::SetType(TYPE_OBJECTX);

	// 初期化処理
	HRESULT hr = CObjectX::Init(BAGMODEL);
	if (FAILED(hr))
	{
		return E_FAIL;
	}
	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CTransferBeacon::Uninit(void)
{
	// リストから削除
	m_List.Delete(this);

	// 終了処理
	CObjectX::Uninit();
}

//==========================================================================
// 更新処理
//==========================================================================
void CTransferBeacon::Update(void)
{
	// 状態カウンター減算
	m_fStateTime -= CManager::GetInstance()->GetDeltaTime();

	// プレイヤーとの当たり判定
	CollisionPlayer();

	// 状態別処理
	(this->*(m_StateFuncList[m_state]))();

	// 遷移なしフェード取得
	if (CManager::GetInstance()->GetInstantFade()->GetState() == CInstantFade::STATE_FADECOMPLETION)
	{
		Uninit();
	}
}

//==========================================================================
// プレイヤーとの当たり判定処理
//==========================================================================
void CTransferBeacon::CollisionPlayer(void)
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
			// 遷移なしフェード追加
			CManager::GetInstance()->GetInstantFade()->SetFade(D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f), 40);

			switch (m_TransType)
			{
			case CTransferBeacon::TRANSTYPE_ENHANCE:
				// 強化状態に変更
				CGame::GetGameManager()->SetType(CGameManager::SCENE_ENHANCE);
				break;

			case CTransferBeacon::TRANSTYPE_GAMEMAIN:
				// 遷移状態に変更
				CGame::GetGameManager()->SetType(CGameManager::SCENE_TRANSITION);
				break;
			}

			// リストループ
			pPlayer = nullptr;
			while (playerList.ListLoop(&pPlayer))
			{
				pPlayer->SetEnableTouchBeacon(false);
			}
		}
	}
}

//==========================================================================
// なにもない状態
//==========================================================================
void CTransferBeacon::StateNone(void)
{
	m_fStateTime = 0.0f;
}

//==========================================================================
// 転移状態
//==========================================================================
void CTransferBeacon::StateTransfer(void)
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
void CTransferBeacon::Draw(void)
{
	// 描画
	CObjectX::Draw();
}

