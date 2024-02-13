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
#include "camera.h"
#include "game.h"
#include "player.h"
#include "input.h"
#include "skilltree.h"
#include "instantfade.h"
#include "3D_effect.h"
#include "MyEffekseer.h"
#include "transferBeacon.h"

//==========================================================================
// 定数定義
//==========================================================================
namespace
{
	const char* BAGMODEL = "data\\MODEL\\enhance\\stoneplane.x";
	const MyLib::Vector3 POSITION = MyLib::Vector3(0.0f, 0.0f, 1100.0f);	// 半径
	const float RADIUS = 80.0f;	// 半径
	const float TIME_STARTUP = 1.0f;		// 起動時間
}
CSkillTree_Obj* CSkillTree_Obj::m_pThisPtr = nullptr;		// 自身のポインタ

//==========================================================================
// 関数ポインタ
//==========================================================================
CSkillTree_Obj::STATE_FUNC CSkillTree_Obj::m_StateFuncList[] =
{
	&CSkillTree_Obj::StateNone,		// なにもなし
	&CSkillTree_Obj::StateStartUp,	// 起動
};

//==========================================================================
// コンストラクタ
//==========================================================================
CSkillTree_Obj::CSkillTree_Obj(int nPriority) : CObjectX(nPriority)
{
	// 値のクリア
	m_fStateTime = 0.0f;	// 状態カウンター
	m_state = STATE_NONE;	// 状態
	m_pWeaponHandle = nullptr;		// エフェクトハンドルのポインタ
}

//==========================================================================
// デストラクタ
//==========================================================================
CSkillTree_Obj::~CSkillTree_Obj()
{
	
}

//==========================================================================
// インスタンス取得
//==========================================================================
CSkillTree_Obj* CSkillTree_Obj::GetInstance()
{
	if (m_pThisPtr == nullptr)
	{
		m_pThisPtr = Create();
	}
	return m_pThisPtr;
}

//==========================================================================
// 生成処理
//==========================================================================
CSkillTree_Obj *CSkillTree_Obj::Create()
{
	if (m_pThisPtr == nullptr)
	{// まだ生成していなかったら

		// インスタンス生成
		m_pThisPtr = DEBUG_NEW CSkillTree_Obj;
		m_pThisPtr->Init();
	}

	return m_pThisPtr;
}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CSkillTree_Obj::Init()
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

	MyLib::Vector3 pos = POSITION;
	pos.y += 250.0f;
	pos.z += 50.0f;


	//CMyEffekseer::GetInstance()->SetEffect(
		//	&m_pWeaponHandle,
		//	"data/Effekseer/debugline_green.efkefc",
		//	weponpos, rot, 0.0f, 40.0f, true);

	// ループエフェクト再生
	CMyEffekseer::GetInstance()->SetEffect(
		&m_pWeaponHandle,
		CMyEffekseer::EFKLABEL_STONEBASE_LIGHT,
		pos,
		0.0f, 0.0f, 100.0f, false);
	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CSkillTree_Obj::Uninit()
{
	m_pThisPtr = nullptr;

	// 終了処理
	CObjectX::Uninit();
}

//==========================================================================
// 更新処理
//==========================================================================
void CSkillTree_Obj::Update()
{
	float distance = static_cast<float>(UtilFunc::Transformation::Random(5, 180)) * 10.0f;
	float diffRadius = static_cast<float>(UtilFunc::Transformation::Random(-10, 10));
	for (int i = 0; i < 1; i++)
	{
		CEffect3D* pEffect = CEffect3D::Create(
			UtilFunc::Transformation::GetRandomPositionSphere(MyLib::Vector3(0.0f, 500.0f, 0.0f), distance),
			UtilFunc::Transformation::Random(-100, 100) * 0.01f,
			D3DXCOLOR(0.2f, 1.0f, 0.4f, 1.0f),
			30.0f + diffRadius, UtilFunc::Transformation::Random(160, 200), CEffect3D::MOVEEFFECT_SUB, CEffect3D::TYPE_POINT);
	}

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
void CSkillTree_Obj::CollisionPlayer()
{
	if (!CGame::GetInstance()->GetGameManager()->IsControll())
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
		CInputKeyboard* pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();
		if (pInputGamepad->GetTriggerRT(0) ||
			pInputKeyboard->GetTrigger(DIK_RETURN))
		{
			// 祈りモーション設定
			pPlayer->SetState(CPlayer::STATE_PRAYER);
			pPlayer->SetMotion(CPlayer::MOTION_PRAYER);

			// 祈り準備状態に設定
			CCamera* pCamera = CManager::GetInstance()->GetCamera();
			pCamera->SetControlState(DEBUG_NEW CCameraControlState_BeforePrayer(CManager::GetInstance()->GetCamera()));
		}
	}
}

//==========================================================================
// 起動処理
//==========================================================================
void CSkillTree_Obj::StartUp()
{
	
	// 起動
	m_state = STATE_STARTUP;

	// スキルツリーに変更
	CGame::GetInstance()->GetGameManager()->SetType(CGameManager::SCENE_SKILLTREE);

	// 目標の長さ設定
	CCamera* pCamera = CManager::GetInstance()->GetCamera();
	pCamera->SetLenDest(pCamera->GetOriginDistance() + 900.0f, 120, 1000.0f, 0.05f);

	// ループエフェクト停止
	if (m_pWeaponHandle != nullptr)
	{
		CMyEffekseer::GetInstance()->Stop(*m_pWeaponHandle);
		m_pWeaponHandle = nullptr;
	}

	// 生成位置
	MyLib::Vector3 pos = POSITION;
	pos.y += 250.0f;
	pos.z -= 20.0f;

	// 起動エフェクト
	CMyEffekseer::GetInstance()->SetEffect(
		CMyEffekseer::EFKLABEL_STONEBASE_BEGIN,
		pos,
		0.0f, 0.0f, 100.0f, true);
}

//==========================================================================
// 再起動処理
//==========================================================================
void CSkillTree_Obj::ReStartUp()
{

	// エフェクト全て停止
	CMyEffekseer::GetInstance()->StopAll();
	CMyEffekseer::GetInstance()->StopAll();

	// ループエフェクト再生
	MyLib::Vector3 pos = POSITION;
	pos.y += 250.0f;
	pos.z += 50.0f;

	CMyEffekseer::GetInstance()->SetEffect(
		&m_pWeaponHandle,
		CMyEffekseer::EFKLABEL_STONEBASE_LIGHT,
		pos,
		0.0f, 0.0f, 100.0f, false);
}

//==========================================================================
// なにもない状態
//==========================================================================
void CSkillTree_Obj::StateNone()
{
	m_fStateTime = 0.0f;
}

//==========================================================================
// 起動状態
//==========================================================================
void CSkillTree_Obj::StateStartUp()
{
	// 状態カウンター加算
	m_fStateTime += CManager::GetInstance()->GetDeltaTime();

	if (TIME_STARTUP <= m_fStateTime)
	{
		m_state = STATE_NONE;

		// スキルツリー生成
		CSkillTree::GetInstance()->SetScreen();

		// 生成位置
		MyLib::Vector3 pos = POSITION;
		pos.z -= 50.0f;

		// 祈り状態に設定
		CCamera* pCamera = CManager::GetInstance()->GetCamera();
		pCamera->SetTargetPosition(pos);
		pCamera->SetStateCamraR(DEBUG_NEW CStateCameraR_Prayer());
		pCamera->SetControlState(DEBUG_NEW CCameraControlState_Prayer(CManager::GetInstance()->GetCamera()));
	}
}

//==========================================================================
// 描画処理
//==========================================================================
void CSkillTree_Obj::Draw()
{
	// 描画
	CObjectX::Draw();
}

