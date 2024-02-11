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
#include "instantfade.h"
#include "3D_effect.h"
#include "MyEffekseer.h"

//==========================================================================
// 定数定義
//==========================================================================
namespace
{
	const char* BAGMODEL = "data\\MODEL\\enhance\\stoneplane.x";
	const MyLib::Vector3 POSITION = MyLib::Vector3(0.0f, 0.0f, 1100.0f);	// 半径
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
	m_pWeaponHandle = nullptr;		// エフェクトハンドルのポインタ
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
void CSkillTree_Obj::CollisionPlayer(void)
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
		if (pInputGamepad->GetTrigger(CInputGamepad::BUTTON_A, 0) ||
			pInputKeyboard->GetTrigger(DIK_RETURN))
		{
			// スキルツリーに変更
			CGame::GetInstance()->GetGameManager()->SetType(CGameManager::SCENE_SKILLTREE);

			// スキルツリー生成
			CSkillTree::GetInstance()->SetScreen();


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

