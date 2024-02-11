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
// 関数ポインタ
//==========================================================================
CEnemyOrafu::ACT_FUNC CEnemyOrafu::m_ActFuncList[] =
{
	&CEnemyOrafu::ActChase,			// 追い掛け
	&CEnemyOrafu::ActAttackProximity,	// 近接
	&CEnemyOrafu::ActWait,				// 待機
};

//==========================================================================
// コンストラクタ
//==========================================================================
CEnemyOrafu::CEnemyOrafu(int nPriority) : CEnemy(nPriority)
{
	m_Action = ACTION_CHASE;	// 行動
	m_fActTime = 0.0f;			// 行動カウンター
	m_bCatchUp = false;			// 追い着き判定
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
	m_Action = ACTION_PROXIMITY;

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
// 行動設定
//==========================================================================
void CEnemyOrafu::ActionSet()
{

}

//==========================================================================
// 行動更新
//==========================================================================
void CEnemyOrafu::UpdateAction()
{
	if (!m_bActionable)
	{
		return;
	}

	// 状態別処理
	(this->*(m_ActFuncList[m_Action]))();
}

//==========================================================================
// 待機
//==========================================================================
void CEnemyOrafu::ActWait()
{
	// モーション取得
	CMotion* pMotion = GetMotion();
	if (pMotion == nullptr)
	{
		return;
	}

	// 待機モーション設定
	pMotion->Set(MOTION_DEF);

	// ターゲットの方を向く
	RotationTarget();

	// 行動カウンター加算
	m_fActTime += CManager::GetInstance()->GetDeltaTime();

	if (TIME_WAIT <= m_fActTime)
	{// 待機時間超えたら

		// 近接攻撃
		m_fActTime = 0.0f;
		m_Action = ACTION_PROXIMITY;

		// プレイヤー情報
		CPlayer* pPlayer = CPlayer::GetListObj().GetData(m_nTargetPlayerIndex);
		if (pPlayer != nullptr)
		{
			m_TargetPosition = pPlayer->GetPosition();
		}

		// 追い着き判定
		m_bCatchUp = UtilFunc::Collision::CircleRange3D(GetPosition(), m_TargetPosition, LENGTH_PUNCH, 0.0f);
	}
}


//==========================================================================
// 追い掛け
//==========================================================================
void CEnemyOrafu::ActChase()
{
	// 移動フラグを立てる
	m_sMotionFrag.bMove = true;

	// 位置取得
	MyLib::Vector3 pos = GetPosition();

	// プレイヤー情報
	CPlayer* pPlayer = CPlayer::GetListObj().GetData(m_nTargetPlayerIndex);
	if (pPlayer == nullptr)
	{
		return;
	}
	m_TargetPosition = pPlayer->GetPosition();

	// 円の判定
	if (UtilFunc::Collision::CircleRange3D(GetPosition(), pPlayer->GetPosition(), LENGTH_PLAYERCHASE, 0.0f))
	{

	}

	// ターゲットの方を向く
	RotationTarget();

	// 行動別移動処理
	ChaseNormal();
}

//==========================================================================
// 歩き追い掛け
//==========================================================================
void CEnemyOrafu::ChaseNormal()
{
	// 情報取得
	MyLib::Vector3 move = GetMove();
	MyLib::Vector3 rot = GetRotation();
	float fMove = GetVelocity();

	// 移動量設定
	move.x = sinf(D3DX_PI + rot.y) * fMove * VELOCITY_WALK;
	move.z = cosf(D3DX_PI + rot.y) * fMove * VELOCITY_WALK;

	// 移動量設定
	SetMove(move);
}

//==========================================================================
// 近接攻撃
//==========================================================================
void CEnemyOrafu::ActAttackProximity()
{
	if (m_bCatchUp == false)
	{// 追い着いてない時

		// 位置取得
		MyLib::Vector3 pos = GetPosition();

		// プレイヤー情報
		CPlayer* pPlayer = CPlayer::GetListObj().GetData(m_nTargetPlayerIndex);
		if (pPlayer == nullptr)
		{
			return;
		}
		m_TargetPosition = pPlayer->GetPosition();

		// ターゲットの方を向く
		RotationTarget();

		// 移動フラグを立てる
		m_sMotionFrag.bMove = true;

		// 行動する為の行動別移動処理
		ChaseNormal();

		// 行動別移動処理
		float fLength = 0.0f;
		fLength = LENGTH_PUNCH;

		// 追い着き判定
		m_bCatchUp = UtilFunc::Collision::CircleRange3D(GetPosition(), m_TargetPosition, fLength, 0.0f);
	}
	else
	{// 攻撃の長さ内

		// 攻撃フラグを立てる
		m_sMotionFrag.bATK = true;

		// 行動別移動処理
		AttackPunch();
	}
}

//==========================================================================
// パンチ攻撃
//==========================================================================
void CEnemyOrafu::AttackPunch()
{
	// モーション取得
	CMotion* pMotion = GetMotion();
	if (pMotion == nullptr)
	{
		return;
	}

	int nType = pMotion->GetType();
	if (nType == MOTION_PUNCH && pMotion->IsFinish() == true)
	{// パンチ攻撃が終わってたら

		// 待機行動
		m_Action = ACTION_WAIT;

		// 待機モーション設定
		pMotion->Set(MOTION_DEF);

		// 追い着いてない判定
		m_bCatchUp = false;
		return;
	}

	if (nType != MOTION_PUNCH)
	{
		// パンチモーション設定
		pMotion->Set(MOTION_PUNCH);
	}

	// 攻撃フラグを立てる
	m_sMotionFrag.bATK = true;
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
		//else
		//{
		//	// ニュートラルモーション
		//	pMotion->Set(MOTION_DEF);
		//}
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
