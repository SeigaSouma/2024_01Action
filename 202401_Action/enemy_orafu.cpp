//==========================================================================
// 
//  オラフ敵処理 [enemy_orafu.cpp]
//  Author : 相馬靜雅
// 
//==========================================================================
#include "enemy_orafu.h"
#include "manager.h"
#include "debugproc.h"
#include "calculation.h"
#include "particle.h"
#include "santabag.h"
#include "player.h"

//==========================================================================
// 定数定義
//==========================================================================
namespace
{
	const float LENGTH_PUNCH = 110.0f;		// パンチの長さ
	const float LENGTH_PLAYERCHASE = 600.0f;	// プレイヤー追いかける距離
	const float VELOCITY_WALK = 1.0f;		// 歩き
	const float TIME_WAIT = 2.0f;			// 待機
}

//==========================================================================
// コンストラクタ
//==========================================================================
CEnemyOrafu::CEnemyOrafu(int nPriority) : CEnemy(nPriority)
{

}

//==========================================================================
// デストラクタ
//==========================================================================
CEnemyOrafu::~CEnemyOrafu()
{

}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CEnemyOrafu::Init()
{
	//初期化処理
	CEnemy::Init();

	// 行動
	m_Action = ACTION_DEF;
	m_pAtkPattern.push_back(DEBUG_NEW CEnemyNormalAttack());	// 通常攻撃
	m_pAtkPattern.push_back(DEBUG_NEW CEnemyStrongAttack());	// 通常攻撃

	ChangeATKState(m_pAtkPattern[0]);

	m_bCatchUp = false;
	m_bInSight = false;

	// モーションインデックス切り替え
	m_pATKState->ChangeMotionIdx(this);
	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CEnemyOrafu::Uninit()
{
	// 終了処理
	CEnemy::Uninit();
}

//==========================================================================
// 殺す
//==========================================================================
void CEnemyOrafu::Kill()
{
	// 死亡処理
	CEnemy::Kill();
}

//==========================================================================
// 更新処理
//==========================================================================
void CEnemyOrafu::Update()
{
	// 死亡の判定
	if (IsDeath() == true)
	{// 死亡フラグが立っていたら
		return;
	}

	// 更新処理
	CEnemy::Update();
}

//==========================================================================
// 描画処理
//==========================================================================
void CEnemyOrafu::Draw()
{
	// 描画処理
	CEnemy::Draw();
}

//==========================================================================
// モーションセット
//==========================================================================
void CEnemyOrafu::MotionSet()
{
	// モーション取得
	CMotion* pMotion = GetMotion();
	if (pMotion == nullptr)
	{
		return;
	}

	if (pMotion->IsFinish() == true)
	{// 終了していたら

		// 現在の種類取得
		int nType = pMotion->GetType();

		if (m_sMotionFrag.bMove == true && m_sMotionFrag.bKnockback == false && m_sMotionFrag.bATK == false)
		{// 移動していたら

			// 攻撃していない
			m_sMotionFrag.bATK = false;

			// 行動別設定処理
			pMotion->Set(MOTION_WALK);
		}
		else if (m_sMotionFrag.bKnockback == true)
		{// やられ中だったら

			// やられモーション
			pMotion->Set(MOTION_KNOCKBACK);
		}
		else if (m_sMotionFrag.bATK == true)
		{// 攻撃していたら

			// 攻撃判定OFF
			m_sMotionFrag.bATK = false;

			// 行動別設定処理
			pMotion->Set(MOTION_PUNCH);
		}
	}
}

//==========================================================================
// 攻撃時処理
//==========================================================================
void CEnemyOrafu::AttackAction(CMotion::AttackInfo ATKInfo, int nCntATK)
{
	// モーション取得
	CMotion* pMotion = GetMotion();
	if (pMotion == nullptr)
	{
		return;
	}

	// モーション情報取得
	int nMotionType = pMotion->GetType();
	MyLib::Vector3 weponpos = pMotion->GetAttackPosition(GetModel(), ATKInfo);

	// 情報取得
	MyLib::Vector3 pos = GetPosition();
	MyLib::Vector3 rot = GetRotation();

	// モーション別処理
	switch (nMotionType)
	{
	case MOTION_PUNCH:
		break;
	}
}

//==========================================================================
// 攻撃判定中処理
//==========================================================================
void CEnemyOrafu::AttackInDicision(CMotion::AttackInfo* pATKInfo, int nCntATK)
{
	// 基底の攻撃判定中処理
	CEnemy::AttackInDicision(pATKInfo, nCntATK);

	// モーション取得
	CMotion* pMotion = GetMotion();
	if (pMotion == nullptr)
	{
		return;
	}

	// モーション情報取得
	int nMotionType = pMotion->GetType();
	MyLib::Vector3 weponpos = pMotion->GetAttackPosition(GetModel(), *pATKInfo);

	// モーション別処理
	switch (nMotionType)
	{
	case MOTION_PUNCH:
		break;
	}
}
