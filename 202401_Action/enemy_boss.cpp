//==========================================================================
// 
//  ボス処理 [enemy_boss.cpp]
//  Author : 相馬靜雅
// 
//==========================================================================
#include "enemy_boss.h"
#include "manager.h"
#include "debugproc.h"
#include "calculation.h"
#include "blackframe.h"
#include "beam.h"
#include "particle.h"
#include "hp_gauge_boss.h"

//==========================================================================
// 定数定義
//==========================================================================
namespace
{
	// 行動抽選の構造体
	struct sProbability 
	{
		CEnemyBoss::ACTION action;	// 行動
		float fProbability;			// 確率
	};

	const float LENGTH_PUNCH = 300.0f;		// パンチの長さ
	const float LENGTH_KICK = 500.0f;		// キックの長さ
	const float LENGTH_CHASEWALK = 800.0f;	// 歩き追従の長さ
	const float VELOCITY_WALK = 1.0f;		// 歩き
	const float VELOCITY_DASH = 3.5f;		// ダッシュ
	const float VELOCITY_TACKLE = 6.0f;		// タックル
	const float TIME_WAIT = 2.0f;			// 待機
	const float TIME_PROXIMITYCHASE = 5.0f;	// 近接攻撃の追従時間最大値
	std::vector<sProbability> ACT_PROBABILITY =	// 行動の抽選確率
	{
		{ CEnemyBoss::ACTION_CHASE, 0.0f },			// 追従
		{ CEnemyBoss::ACTION_PROXIMITY, 0.5f },		// 近接攻撃
		{ CEnemyBoss::ACTION_REMOTE, 0.3f },		// 遠隔攻撃
		{ CEnemyBoss::ACTION_ASSAULT, 0.2f },		// 突撃攻撃
		{ CEnemyBoss::ACTION_WAIT, 0.0f },			// 待機
		{ CEnemyBoss::ACTION_GUARD, 0.0f },			// ガード
		{ CEnemyBoss::ACTION_SELFEXPLOSION, 0.0f },	// 自爆
	};
}

//==========================================================================
// 関数ポインタ
//==========================================================================
CEnemyBoss::ACT_FUNC CEnemyBoss::m_ActFuncList[] =
{
	&CEnemyBoss::ActChase,				// 追い掛け
	&CEnemyBoss::ActAttackProximity,	// 近接
	&CEnemyBoss::ActAttackRemote,		// 遠隔
	&CEnemyBoss::ActAttackAssault,		// 突撃
	&CEnemyBoss::ActWait,				// 待機
	&CEnemyBoss::ActGuard,				// ガード
	&CEnemyBoss::ActAttackExplosion,	// 自爆
};

//==========================================================================
//  コンストラクタ
//==========================================================================
CEnemyBoss::CEnemyBoss(int nPriority) : CEnemy(nPriority)
{
	m_Action = ACTION_CHASE;		// 行動
	m_ActionBranch = ACTBRANCH_CHASE_SLOW;		// 行動分岐
	m_MakeForActionBranch = ACTBRANCH_CHASE_SLOW;	// 行動する為の行動
	m_TargetPosition = mylib_const::DEFAULT_VECTOR3;	// 目標の位置
	m_fActTime = 0.0f;			// 行動カウンター
	m_fAssultLength = 0.0f;		// 突撃長さ
	m_fAssultLengthDest = 0.0f;	// 目標の突撃長さ
	m_bCatchUp = false;			// 追い着き判定
	m_pBossHPGauge = nullptr;	// ボスのHPゲージ
}

//==========================================================================
//  デストラクタ
//==========================================================================
CEnemyBoss::~CEnemyBoss()
{

}

//==========================================================================
//  初期化処理
//==========================================================================
HRESULT CEnemyBoss::Init(void)
{
	//初期化処理
	CEnemy::Init();

	// HPの設定
	m_pBossHPGauge = CHP_GaugeBoss::Create(MyLib::Vector3(640.0f, 50.0f, 0.0f), GetLifeOrigin());	// ボスのHPゲージ
	m_pBossHPGauge->SetLife(0);

	// 黒フレーム捌ける
	CManager::GetInstance()->GetBlackFrame()->SetState(CBlackFrame::STATE_OUT);

	// 行動
	m_Action = ACTION_WAIT;

	return S_OK;
}

//==========================================================================
//  終了処理
//==========================================================================
void CEnemyBoss::Uninit(void)
{
	// 終了処理
	CEnemy::Uninit();
}

//==========================================================================
//  更新処理
//==========================================================================
void CEnemyBoss::Update(void)
{
	// 死亡の判定
	if (IsDeath() == true)
	{// 死亡フラグが立っていたら
		return;
	}
	return;

	// 更新処理
	CEnemy::Update();

	if (IsDeath() == true)
	{// 死亡フラグが立っていたら
		return;
	}

	// HP更新
	m_pBossHPGauge->SetLife(GetLife());

	// 黒フレーム捌ける
	if (CManager::GetInstance()->GetBlackFrame()->GetState() == CBlackFrame::STATE_INCOMPLETION)
	{
		CManager::GetInstance()->GetBlackFrame()->SetState(CBlackFrame::STATE_OUT);
	}
}

//==========================================================================
// 行動設定
//==========================================================================
void CEnemyBoss::ActionSet(void)
{

}

//==========================================================================
// 行動更新
//==========================================================================
void CEnemyBoss::UpdateAction(void)
{
	// 状態別処理
	(this->*(m_ActFuncList[m_Action]))();
}

//==========================================================================
// 行動抽選
//==========================================================================
void CEnemyBoss::DrawingAction(void)
{

	// 0～1のランダム値取得
	float fRandomValue = static_cast<float>(std::rand()) / RAND_MAX;

	// 確率加算用変数
	float fDrawingProbability = 0.0;

	// 行動抽選要素分繰り返し
	for (const auto& candidate : ACT_PROBABILITY)
	{
		// 今回の確率分を加算
		fDrawingProbability += candidate.fProbability;

		if (fRandomValue < fDrawingProbability)
		{// 今回のランダム値が確率を超えたら

			// 行動決定
			m_Action = candidate.action;

			// 行動カウンターリセット
			m_fActTime = 0.0f;
			break;
		}
	}

	// 次の行動別
	int nActRand;
	float fLength = 0.0f;
	switch (m_Action)
	{
	case CEnemyBoss::ACTION_CHASE:	// 追従
		if (UtilFunc::Collision::CircleRange3D(GetPosition(), m_TargetPosition, LENGTH_CHASEWALK, 0.0f))
		{// 歩きの範囲
			m_ActionBranch = ACTBRANCH_CHASE_SLOW;
		}
		else
		{
			m_ActionBranch = ACTBRANCH_CHASE_DASH;
		}
		break;

	case CEnemyBoss::ACTION_PROXIMITY:	// 近接攻撃
		nActRand = UtilFunc::Transformation::Random(ACTBRANCH_PROXIMITY_PUNCH, ACTBRANCH_PROXIMITY_KICK);
		m_ActionBranch = static_cast<eActionBranch>(nActRand);

		// 行動別移動処理
		fLength = 0.0f;
		switch (m_ActionBranch)
		{
		case CEnemyBoss::ACTBRANCH_PROXIMITY_PUNCH:
			fLength = LENGTH_PUNCH;
			break;

		case CEnemyBoss::ACTBRANCH_PROXIMITY_KICK:
			fLength = LENGTH_KICK;
			break;
		}

		// 行動する為の行動決定
		if (UtilFunc::Collision::CircleRange3D(GetPosition(), m_TargetPosition, fLength, 0.0f))
		{// 攻撃範囲内

			// 追い着いた
			m_bCatchUp = true;
		}
		else
		{
			// 追い着いてない
			m_bCatchUp = false;

			if (UtilFunc::Collision::CircleRange3D(GetPosition(), m_TargetPosition, LENGTH_CHASEWALK, 0.0f))
			{// 歩きの範囲
				m_MakeForActionBranch = ACTBRANCH_CHASE_SLOW;
			}
			else
			{
				m_MakeForActionBranch = ACTBRANCH_CHASE_DASH;
			}
		}
		break;

	case CEnemyBoss::ACTION_REMOTE:	// 遠隔攻撃
		m_ActionBranch = ACTBRANCH_REMOTE_BEAM;
		break;

	case CEnemyBoss::ACTION_ASSAULT:	// 突撃攻撃
		m_ActionBranch = ACTBRANCH_ASSAULT_CHARGE;
		m_fAssultLength = 0.0f;
		break;

	default:
		break;
	}
}

//==========================================================================
// 待機
//==========================================================================
void CEnemyBoss::ActWait(void)
{
	// モーション取得
	CMotion* pMotion = GetMotion();
	if (pMotion == NULL)
	{
		return;
	}

	// 待機モーション設定
	pMotion->Set(MOTION_DEF);

	// 行動カウンター加算
	m_fActTime += CManager::GetInstance()->GetDeltaTime();

	// ターゲットの方を向く
	RotationTarget();

	if (TIME_WAIT <= m_fActTime)
	{// 待機時間超えたら

		// 行動抽選
		DrawingAction();
		m_fActTime = 0.0f;
	}
}


//==========================================================================
// ガード
//==========================================================================
void CEnemyBoss::ActGuard(void)
{
	// モーション取得
	CMotion* pMotion = GetMotion();
	if (pMotion == NULL)
	{
		return;
	}

	// ガードモーション設定
	pMotion->Set(MOTION_GUARD);

	// 行動カウンターリセット
	m_fActTime = 0.0f;

	// ターゲットの方を向く
	RotationTarget();
}

//==========================================================================
// 追い掛け
//==========================================================================
void CEnemyBoss::ActChase(void)
{
	// 移動フラグを立てる
	m_sMotionFrag.bMove = true;

	// ターゲットの方を向く
	RotationTarget();

	// 行動別移動処理
	switch (m_ActionBranch)
	{
	case CEnemyBoss::ACTBRANCH_CHASE_SLOW:
		ChaseSlow();
		break;

	case CEnemyBoss::ACTBRANCH_CHASE_DASH:
		ChaseDash();
		break;

	default:
		break;
	}
}

//==========================================================================
// 歩き追い掛け
//==========================================================================
void CEnemyBoss::ChaseSlow(void)
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
// ダッシュ追い掛け
//==========================================================================
void CEnemyBoss::ChaseDash(void)
{
	// 情報取得
	MyLib::Vector3 move = GetMove();
	MyLib::Vector3 rot = GetRotation();
	float fMove = GetVelocity();

	// 移動量設定
	move.x = sinf(D3DX_PI + rot.y) * fMove * VELOCITY_DASH;
	move.z = cosf(D3DX_PI + rot.y) * fMove * VELOCITY_DASH;

	// 移動量設定
	SetMove(move);
}

//==========================================================================
// 近接攻撃
//==========================================================================
void CEnemyBoss::ActAttackProximity(void)
{
	if (m_bCatchUp == false)
	{// 追い着いてない時

		// ターゲットの方を向く
		RotationTarget();

		// 移動フラグを立てる
		m_sMotionFrag.bMove = true;

		// 行動する為の行動別移動処理
		switch (m_MakeForActionBranch)
		{
		case CEnemyBoss::ACTBRANCH_CHASE_SLOW:
			ChaseSlow();
			break;

		case CEnemyBoss::ACTBRANCH_CHASE_DASH:
			ChaseDash();
			break;
		}

		// 行動別移動処理
		float fLength = 0.0f;
		switch (m_ActionBranch)
		{
		case CEnemyBoss::ACTBRANCH_PROXIMITY_PUNCH:
			fLength = LENGTH_PUNCH;
			break;

		case CEnemyBoss::ACTBRANCH_PROXIMITY_KICK:
			fLength = LENGTH_KICK;
			break;
		}

		// 追い着き判定
		m_bCatchUp = UtilFunc::Collision::CircleRange3D(GetPosition(), m_TargetPosition, fLength, 0.0f);

		// 行動カウンター加算
		m_fActTime += CManager::GetInstance()->GetDeltaTime();;
		if (TIME_PROXIMITYCHASE <= m_fActTime)
		{
			// 追い着き判定
			m_bCatchUp = true;
		}
	}
	else
	{// 攻撃の長さ内

		// 攻撃フラグを立てる
		m_sMotionFrag.bATK = true;

		// 行動別移動処理
		switch (m_ActionBranch)
		{
		case CEnemyBoss::ACTBRANCH_PROXIMITY_PUNCH:
			AttackPunch();
			break;

		case CEnemyBoss::ACTBRANCH_PROXIMITY_KICK:
			AttackKick();
			break;
		}
	}
}

//==========================================================================
// パンチ攻撃
//==========================================================================
void CEnemyBoss::AttackPunch(void)
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
// キック攻撃
//==========================================================================
void CEnemyBoss::AttackKick(void)
{
	// モーション取得
	CMotion* pMotion = GetMotion();
	if (pMotion == NULL)
	{
		return;
	}

	int nType = pMotion->GetType();
	if (nType == MOTION_KICK && pMotion->IsFinish() == true)
	{// キック攻撃が終わってたら

		// 待機行動
		m_Action = ACTION_WAIT;

		// 待機モーション設定
		pMotion->Set(MOTION_DEF);
		return;
	}

	if (nType != MOTION_KICK)
	{
		// キックモーション設定
		pMotion->Set(MOTION_KICK);
	}

	// 攻撃フラグを立てる
	m_sMotionFrag.bATK = true;
}

//==========================================================================
// 遠隔攻撃
//==========================================================================
void CEnemyBoss::ActAttackRemote(void)
{
	// 攻撃フラグを立てる
	m_sMotionFrag.bATK = true;

	// ターゲットの方を向く
	RotationTarget();

	// 行動別移動処理
	switch (m_ActionBranch)
	{
	case CEnemyBoss::ACTBRANCH_REMOTE_BEAM:
		AttackBeam();
		break;

	default:
		break;
	}
}

//==========================================================================
// ビーム攻撃
//==========================================================================
void CEnemyBoss::AttackBeam(void)
{
	// モーション取得
	CMotion* pMotion = GetMotion();
	if (pMotion == NULL)
	{
		return;
	}

	int nType = pMotion->GetType();
	if (nType == MOTION_BEAM && pMotion->IsFinish() == true)
	{// ビーム攻撃が終わってたら

		// 待機行動
		m_Action = ACTION_WAIT;

		// 待機モーション設定
		pMotion->Set(MOTION_DEF);
		return;
	}

	if (nType != MOTION_BEAM)
	{
		// ビームモーション設定
		pMotion->Set(MOTION_BEAM);
	}

	// 攻撃フラグを立てる
	m_sMotionFrag.bATK = true;
}

//==========================================================================
// 突撃攻撃
//==========================================================================
void CEnemyBoss::ActAttackAssault(void)
{

	// 行動別移動処理
	switch (m_ActionBranch)
	{
	case CEnemyBoss::ACTBRANCH_ASSAULT_CHARGE:
		ChargeTackle();
		break;

	case CEnemyBoss::ACTBRANCH_ASSAULT_TACKLE:
		AttackTackle();
		break;

	default:
		break;
	}

}

//==========================================================================
// タックルチャージ
//==========================================================================
void CEnemyBoss::ChargeTackle(void)
{
	// モーション取得
	CMotion* pMotion = GetMotion();
	if (pMotion == NULL)
	{
		return;
	}

	int nType = pMotion->GetType();
	if (nType == MOTION_CHARGE_TACKLE && pMotion->IsFinish() == true)
	{// タックルチャージが終わってたら

		// タックル行動
		m_ActionBranch = ACTBRANCH_ASSAULT_TACKLE;

		// タックル設定
		pMotion->Set(MOTION_TACKLE);

		// 目標との距離算出
		m_fAssultLengthDest = UtilFunc::Calculation::GetFabsPosLength3D(GetPosition(), m_TargetPosition);
		
		// 目標の突撃長さ
		m_fAssultLengthDest *= 1.5f;
		return;
	}

	if (nType != MOTION_CHARGE_TACKLE)
	{
		// タックルチャージ設定
		pMotion->Set(MOTION_CHARGE_TACKLE);
	}

	// ターゲットの方を向く
	RotationTarget();

	// チャージフラグを立てる
	m_sMotionFrag.bCharge = true;
}

//==========================================================================
// タックル攻撃
//==========================================================================
void CEnemyBoss::AttackTackle(void)
{
	// モーション取得
	CMotion* pMotion = GetMotion();
	if (pMotion == NULL)
	{
		return;
	}

	if (m_fAssultLength >= m_fAssultLengthDest)
	{// タックル攻撃が目標の長さ分移動したら

		// 待機行動
		m_Action = ACTION_WAIT;

		// 待機モーション設定
		pMotion->Set(MOTION_DEF);
		return;
	}

	// 突撃の距離
	float fMove = GetVelocity();
	m_fAssultLength += fMove * VELOCITY_TACKLE;

	// 攻撃判定中に追加する予定
#if 1
	// 情報取得
	MyLib::Vector3 move = GetMove();
	MyLib::Vector3 rot = GetRotation();

	// タックル移動量設定
	move.x = sinf(D3DX_PI + rot.y) * fMove * VELOCITY_TACKLE;
	move.z = cosf(D3DX_PI + rot.y) * fMove * VELOCITY_TACKLE;

	// 移動量設定
	SetMove(move);
#endif

	if (pMotion->GetType() != MOTION_TACKLE)
	{
		// タックルモーション設定
		pMotion->Set(MOTION_TACKLE);
	}

	// 攻撃フラグを立てる
	m_sMotionFrag.bATK = true;
}

//==========================================================================
// 自爆攻撃
//==========================================================================
void CEnemyBoss::ActAttackExplosion(void)
{

}

//==========================================================================
//  描画処理
//==========================================================================
void CEnemyBoss::Draw(void)
{
	// 描画処理
	CEnemy::Draw();
}

//==========================================================================
//  殺す
//==========================================================================
void CEnemyBoss::Kill(void)
{
	if (m_pHPGauge != nullptr)
	{
		m_pBossHPGauge->Kill();
		m_pBossHPGauge = nullptr;
	}

	// 死亡処理
	CEnemy::Kill();
}

//==========================================================================
//  モーションセット
//==========================================================================
void CEnemyBoss::MotionSet(void)
{
	if (m_Action == ACTION_GUARD)
	{
		return;
	}

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
			switch (m_MakeForActionBranch)
			{
			case CEnemyBoss::ACTBRANCH_CHASE_SLOW:
				pMotion->Set(MOTION_WALK);
				break;

			case CEnemyBoss::ACTBRANCH_CHASE_DASH:
				pMotion->Set(MOTION_DASH);
				break;
			}
		}
		else if (m_sMotionFrag.bKnockback == true)
		{// やられ中だったら

			// やられモーション
			pMotion->Set(MOTION_KNOCKBACK);
		}
		else if (m_sMotionFrag.bCharge == true)
		{// チャージ中だったら
			
			m_sMotionFrag.bCharge = false;
			// チャージモーション
			pMotion->Set(MOTION_CHARGE_TACKLE);
		}
		else if (m_sMotionFrag.bATK == true)
		{// 攻撃していたら

			// 攻撃判定OFF
			m_sMotionFrag.bATK = false;

			// 行動別設定処理
			switch (m_ActionBranch)
			{
			case ACTBRANCH_PROXIMITY_PUNCH:
				pMotion->Set(MOTION_PUNCH);
				break;

			case ACTBRANCH_PROXIMITY_KICK:
				pMotion->Set(MOTION_KICK);
				break;

			case ACTBRANCH_REMOTE_BEAM:
				pMotion->Set(MOTION_BEAM);
				break;

			case ACTBRANCH_ASSAULT_TACKLE:
				pMotion->Set(MOTION_TACKLE);
				break;
			}
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
void CEnemyBoss::RotationTarget(void)
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
void CEnemyBoss::AttackAction(CMotion::AttackInfo ATKInfo, int nCntATK)
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

	case MOTION_KICK:
		break;

	case MOTION_BEAM:
	{
		D3DXCOLOR col = D3DXCOLOR(
			0.5f + UtilFunc::Transformation::Random(-100, 100) * 0.001f,
			0.1f + UtilFunc::Transformation::Random(-100, 100) * 0.001f,
			0.5f + UtilFunc::Transformation::Random(-100, 100) * 0.001f,	// 色
			1.0f);

		CBeam::Create(
			weponpos,	// 位置
			MyLib::Vector3(
				sinf(D3DX_PI + rot.y) * 100.0f,
				cosf(D3DX_PI * 0.55f) * 100.0f,
				cosf(D3DX_PI + rot.y) * 100.0f),	// 移動量
			col,	// 色
			60.0f,	// 半径
			400.0f,	// 長さ
			30,		// 寿命
			32,		// 密度
			1,		// ダメージ
			CCollisionObject::TAG_ENEMY	// タグ
		);
	}
	break;

	case MOTION_CHARGE_TACKLE:
		break;

	case MOTION_TACKLE:
		break;
	}
}

//==========================================================================
// 攻撃判定中処理
//==========================================================================
void CEnemyBoss::AttackInDicision(CMotion::AttackInfo ATKInfo, int nCntATK)
{
	// 基底の攻撃判定中処理
	CEnemy::AttackInDicision(ATKInfo, nCntATK);

	// モーション取得
	CMotion* pMotion = GetMotion();
	if (pMotion == NULL)
	{
		return;
	}

	// モーション情報取得
	int nMotionType = pMotion->GetType();
	MyLib::Vector3 weponpos = pMotion->GetAttackPosition(GetModel(), ATKInfo);

	// モーション別処理
	switch (nMotionType)
	{
	case MOTION_PUNCH:
		break;

	case MOTION_KICK:
		break;

	case MOTION_BEAM:
		my_particle::Create(weponpos, my_particle::TYPE_ATTACK_BODY);
		break;

	case MOTION_CHARGE_TACKLE:
		break;

	case MOTION_TACKLE:
		break;
	}
}

//==========================================================================
// 目標の位置設定
//==========================================================================
void CEnemyBoss::SetTargetPosition(MyLib::Vector3 pos)
{
	m_TargetPosition = pos;
}
