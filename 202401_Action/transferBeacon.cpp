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
#include "3D_effect.h"
#include "MyEffekseer.h"
#include "pickupguide.h"

//==========================================================================
// 定数定義
//==========================================================================
namespace
{
	const char* BAGMODEL = "data\\MODEL\\transfer\\transferstone.x";
	const float RADIUS = 80.0f;	// 半径
	const float TIME_VERTICAL = 3.0f;		// 上昇時間
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
	m_nCntEffect = 0;
	m_fStateTime = 0.0f;	// 状態カウンター
	m_state = STATE_NONE;	// 状態
	m_TransType = TRANSTYPE_ENHANCE;	// 転移種類
	m_pPickupGuide = nullptr;	// ピックアップガイドのポインタ
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
CTransferBeacon *CTransferBeacon::Create(eTransType transtype, const MyLib::Vector3& pos)
{
	// メモリの確保
	CTransferBeacon* pBag = DEBUG_NEW CTransferBeacon;

	if (pBag != NULL)
	{// メモリの確保が出来ていたら

		// 転移種類
		pBag->m_TransType = transtype;

		// 位置割り当て
		pBag->SetPosition(pos);
		pBag->SetOriginPosition(pos);

		// 初期化処理
		pBag->Init();
	}

	return pBag;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CTransferBeacon::Init()
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

	// 生成位置
	MyLib::Vector3 spawnpos = GetOriginPosition();
	spawnpos.y -= 150.0f;

	// 転移
	CMyEffekseer::GetInstance()->SetEffect(
		CMyEffekseer::EFKLABEL_TRANSFER,
		spawnpos,
		0.0f, 0.0f, 100.0f, false);

	// ピックアップガイド生成
	if (!m_pPickupGuide)
	{
		m_pPickupGuide = CPickupGuide::Create(GetOriginPosition(), CPickupGuide::TYPE_TRANSFER);
	}

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CTransferBeacon::Uninit()
{
	// ピックアップガイド
	m_pPickupGuide = nullptr;

	// リストから削除
	m_List.Delete(this);

	// 終了処理
	CObjectX::Uninit();
}

//==========================================================================
// 削除
//==========================================================================
void CTransferBeacon::Kill()
{
	// ピックアップガイド
	if (m_pPickupGuide)
	{
		m_pPickupGuide->Uninit();
		m_pPickupGuide = nullptr;
	}

	// リストから削除
	m_List.Delete(this);

	// 終了処理
	CObjectX::Uninit();
}

//==========================================================================
// 更新処理
//==========================================================================
void CTransferBeacon::Update()
{
	// 状態カウンター加算
	m_fStateTime += CManager::GetInstance()->GetDeltaTime();

	MyLib::Vector3 pos = GetPosition();
	pos.y = 150.0f + sinf(D3DX_PI * (m_fStateTime / TIME_VERTICAL)) * 10.0f;
	SetPosition(pos);

	m_nCntEffect = (m_nCntEffect + 1) % 6;
	if (m_nCntEffect == 0)
	{
		float move = UtilFunc::Transformation::Random(-100, 100) * 0.01f;
		float moveY = UtilFunc::Transformation::Random(20, 30) * 0.1f;
		float radius = UtilFunc::Transformation::Random(50, 80);

		pos.y -= 20.0f;
		CEffect3D* pEffect = CEffect3D::Create(
			pos,
			MyLib::Vector3(move, moveY, move),
			D3DXCOLOR(0.2f, 1.0f, 0.4f, 1.0f),
			radius, 60, CEffect3D::MOVEEFFECT_SUB, CEffect3D::TYPE_SMOKE);
		//pEffect->SetDisableZSort();
	}

	if (m_fStateTime >= TIME_VERTICAL * 2.0f)
	{
		m_fStateTime = 0.0f;
	}


	// プレイヤーとの当たり判定
	CollisionPlayer();

	// 状態別処理
	(this->*(m_StateFuncList[m_state]))();

	// 遷移なしフェード取得
	if (CManager::GetInstance()->GetInstantFade()->GetState() == CInstantFade::STATE_FADECOMPLETION)
	{
		Kill();
	}
}

//==========================================================================
// プレイヤーとの当たり判定処理
//==========================================================================
void CTransferBeacon::CollisionPlayer()
{
	if (!CGame::GetInstance()->GetGameManager()->IsControll())
	{// 行動できないとき
		return;
	}

	if (CManager::GetInstance()->GetInstantFade()->GetState() != CInstantFade::STATE_NONE)
	{
		return;
	}

	// プレイヤー取得
	CListManager<CPlayer> playerList = CPlayer::GetListObj();
	CPlayer* pPlayer = nullptr;

	MyLib::Vector3 pos = GetPosition();

	bool bHit = false;
	CPickupGuide::STATE guideState = CPickupGuide::STATE_FADEOUT;

	// リストループ
	while (playerList.ListLoop(&pPlayer))
	{
		pPlayer->SetEnableTouchBeacon(false);
		if (UtilFunc::Collision::CircleRange3D(pos, pPlayer->GetPosition(), RADIUS, pPlayer->GetRadius()))
		{
			bHit = true;
			pPlayer->SetEnableTouchBeacon(true);
			guideState = CPickupGuide::STATE_FADEIN;
			break;
		}
	}

	if (bHit)
	{
		// ゲームパッド情報取得
		CInputGamepad* pInputGamepad = CManager::GetInstance()->GetInputGamepad();
		CInputKeyboard* pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();
		if (pInputGamepad->GetTriggerRT(0) ||
			pInputKeyboard->GetTrigger(DIK_RETURN))
		{
			// 遷移なしフェード追加
			CManager::GetInstance()->GetInstantFade()->SetFade(D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f), 40);

			switch (m_TransType)
			{
			case CTransferBeacon::TRANSTYPE_ENHANCE:
				// 強化状態に変更
				CGame::GetInstance()->GetGameManager()->SetType(CGameManager::SCENE_ENHANCE);
				break;

			case CTransferBeacon::TRANSTYPE_GAMEMAIN:
				// 遷移状態に変更
				CGame::GetInstance()->GetGameManager()->SetType(CGameManager::SCENE_TRANSITION);
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

	// ピックアップガイドの状態設定
	if (m_pPickupGuide)
	{
		m_pPickupGuide->SetState(guideState);
	}
}

//==========================================================================
// なにもない状態
//==========================================================================
void CTransferBeacon::StateNone()
{
	
}

//==========================================================================
// 転移状態
//==========================================================================
void CTransferBeacon::StateTransfer()
{

	// 向き取得
	MyLib::Vector3 rot = GetRotation();
	rot.y += D3DX_PI * 0.05f;
	UtilFunc::Transformation::RotNormalize(rot.y);

	// 向き設定
	SetRotation(rot);
}

//==========================================================================
// エフェクト設定
//==========================================================================
void CTransferBeacon::SetEffect()
{
	// 生成位置
	MyLib::Vector3 spawnpos = GetOriginPosition();
	spawnpos.y -= 150.0f;

	// 転移
	CMyEffekseer::GetInstance()->SetEffect(
		CMyEffekseer::EFKLABEL_TRANSFER,
		spawnpos,
		0.0f, 0.0f, 100.0f, false);
}

//==========================================================================
// 描画処理
//==========================================================================
void CTransferBeacon::Draw()
{
	// 描画
	CObjectX::Draw();
}

