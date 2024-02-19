//==========================================================================
// 
//  ストーンゴーレム敵処理 [enemy_stonegolem.cpp]
//  Author : 相馬靜雅
// 
//==========================================================================
#include "enemy_stonegolem.h"
#include "manager.h"
#include "debugproc.h"
#include "particle.h"

//==========================================================================
// 定数定義
//==========================================================================
namespace
{
	const float LENGTH_PUNCH = 110.0f;		// パンチの長さ
	const float VELOCITY_WALK = 1.0f;		// 歩き
	const float TIME_WAIT = 0.1f;			// 待機
}

//==========================================================================
// コンストラクタ
//==========================================================================
CEnemyGolem::CEnemyGolem(int nPriority) : CEnemy(nPriority)
{
	
}

//==========================================================================
// デストラクタ
//==========================================================================
CEnemyGolem::~CEnemyGolem()
{

}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CEnemyGolem::Init()
{
	//初期化処理
	CEnemy::Init();

	// 行動
	m_Action = ACTION_DEF;
	m_pAtkPattern.push_back(DEBUG_NEW CEnemyGolemUPSwipe());	// 1段目
	m_pAtkPattern.push_back(DEBUG_NEW CEnemyGolemSideSwipe());	// 2段目

	// ロックオンの距離
	m_fRockOnDistance = 400.0f;

	// 視界・追い着きフラグリセット
	m_bCatchUp = false;
	m_bInSight = false;

	// 攻撃切り替え
	ChangeATKState(m_pAtkPattern[0]);
	m_pATKState->ChangeMotionIdx(this);

	// スーパーアーマー
	m_bActiveSuperArmor = true;
	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CEnemyGolem::Uninit()
{
	for (int i = 0; i < static_cast<int>(m_pAtkPattern.size()); i++)
	{
		delete m_pAtkPattern[i];
		m_pAtkPattern[i] = nullptr;
	}

	// 終了処理
	CEnemy::Uninit();
}

//==========================================================================
// 殺す
//==========================================================================
void CEnemyGolem::Kill()
{
	for (int i = 0; i < static_cast<int>(m_pAtkPattern.size()); i++)
	{
		delete m_pAtkPattern[i];
		m_pAtkPattern[i] = nullptr;
	}

	// 死亡処理
	CEnemy::Kill();
}

//==========================================================================
// 更新処理
//==========================================================================
void CEnemyGolem::Update()
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
void CEnemyGolem::Draw()
{
	// 描画処理
	CEnemy::Draw();
}

//==========================================================================
// モーションセット
//==========================================================================
void CEnemyGolem::MotionSet()
{
	// モーション取得
	CMotion* pMotion = GetMotion();
	if (pMotion == NULL)
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
	}
}

//==========================================================================
// 攻撃時処理
//==========================================================================
void CEnemyGolem::AttackAction(CMotion::AttackInfo ATKInfo, int nCntATK)
{
	// モーション取得
	CMotion* pMotion = GetMotion();
	if (pMotion == NULL)
	{
		return;
	}

	// モーション情報取得
	int nMotionType = pMotion->GetType();
	MyLib::Vector3 weponpos = pMotion->GetAttackPosition(GetModel(), ATKInfo);

	// 情報取得
	MyLib::Vector3 pos = GetPosition();
	MyLib::Vector3 rot = GetRotation();

	switch (nMotionType)
	{
	case MOTION_ATTACK_STRONG:
		break;

	default:
		break;
	}

}

//==========================================================================
// 攻撃判定中処理
//==========================================================================
void CEnemyGolem::AttackInDicision(CMotion::AttackInfo* pATKInfo, int nCntATK)
{
	// 基底の攻撃判定中処理
	CEnemy::AttackInDicision(pATKInfo, nCntATK);

	// モーション取得
	CMotion* pMotion = GetMotion();
	if (pMotion == NULL)
	{
		return;
	}

	// モーション情報取得
	int nMotionType = pMotion->GetType();
	MyLib::Vector3 weponpos = pMotion->GetAttackPosition(GetModel(), *pATKInfo);

}
