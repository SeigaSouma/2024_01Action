//==========================================================================
// 
//  クッキー敵処理 [enemy_cookie.cpp]
//  Author : 相馬靜雅
// 
//==========================================================================
#include "enemy_cookie.h"
#include "manager.h"
#include "debugproc.h"
#include "calculation.h"
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
// 関数ポインタ
//==========================================================================
CEnemyCookie::ACT_FUNC CEnemyCookie::m_ActFuncList[] =
{
	&CEnemyCookie::ActChase,			// 追い掛け
	&CEnemyCookie::ActAttackProximity,	// 近接
	&CEnemyCookie::ActWait,				// 待機
};

//==========================================================================
// コンストラクタ
//==========================================================================
CEnemyCookie::CEnemyCookie(int nPriority) : CEnemy(nPriority)
{
	m_Action = ACTION_CHASE;	// 行動
	m_fActTime = 0.0f;			// 行動カウンター
	m_bCatchUp = false;			// 追い着き判定
}

//==========================================================================
// デストラクタ
//==========================================================================
CEnemyCookie::~CEnemyCookie()
{

}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CEnemyCookie::Init(void)
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
void CEnemyCookie::Uninit(void)
{
	// 終了処理
	CEnemy::Uninit();
}

//==========================================================================
// 殺す
//==========================================================================
void CEnemyCookie::Kill(void)
{
	// 死亡処理
	CEnemy::Kill();
}

//==========================================================================
// 更新処理
//==========================================================================
void CEnemyCookie::Update(void)
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
void CEnemyCookie::ActionSet(void)
{

}

//==========================================================================
// 行動更新
//==========================================================================
void CEnemyCookie::UpdateAction(void)
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
void CEnemyCookie::ActWait(void)
{
	// モーション取得
	CMotion* pMotion = GetMotion();
	if (pMotion == NULL)
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

		// 追い着き判定
		m_bCatchUp = UtilFunc::Collision::CircleRange3D(GetPosition(), m_TargetPosition, LENGTH_PUNCH, 0.0f);
	}
}


//==========================================================================
// 追い掛け
//==========================================================================
void CEnemyCookie::ActChase(void)
{
	// 移動フラグを立てる
	m_sMotionFrag.bMove = true;

	// ターゲットの方を向く
	RotationTarget();

	// 行動別移動処理
	ChaseNormal();
}

//==========================================================================
// 歩き追い掛け
//==========================================================================
void CEnemyCookie::ChaseNormal(void)
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
void CEnemyCookie::ActAttackProximity(void)
{
	if (m_bCatchUp == false)
	{// 追い着いてない時

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

		// ターゲットの方を向く
		RotationTarget();

		// 行動別移動処理
		AttackPunch();
	}
}

//==========================================================================
// パンチ攻撃
//==========================================================================
void CEnemyCookie::AttackPunch(void)
{
	// モーション取得
	CMotion* pMotion = GetMotion();
	if (pMotion == NULL)
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
void CEnemyCookie::Draw(void)
{
	// 描画処理
	CEnemy::Draw();
}

//==========================================================================
// モーションセット
//==========================================================================
void CEnemyCookie::MotionSet(void)
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
// ターゲットの方を向く
//==========================================================================
void CEnemyCookie::RotationTarget(void)
{
	// 位置取得
	MyLib::Vector3 pos = GetPosition();
	MyLib::Vector3 rot = GetRotation();

	// 目標の角度を求める
	float fRotDest = atan2f((pos.x - m_TargetPosition.x), (pos.z - m_TargetPosition.z));

	// 目標との差分
	float fRotDiff = fRotDest - rot.y;

	//角度の正規化
	UtilFunc::Transformation::RotNormalize(fRotDiff);

	//角度の補正をする
	rot.y += fRotDiff * 0.1f;
	UtilFunc::Transformation::RotNormalize(rot.y);

	// 向き設定
	SetRotation(rot);

	// 目標の向き設定
	SetRotDest(fRotDest);
}

//==========================================================================
// 攻撃時処理
//==========================================================================
void CEnemyCookie::AttackAction(CMotion::AttackInfo ATKInfo, int nCntATK)
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
void CEnemyCookie::AttackInDicision(CMotion::AttackInfo* pATKInfo, int nCntATK)
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

	// モーション別処理
	switch (nMotionType)
	{
	case MOTION_PUNCH:
		break;
	}
}
