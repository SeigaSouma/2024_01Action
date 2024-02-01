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
#include "camera.h"
#include "sound.h"
#include "player.h"

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

	const float LENGTH_PUNCH = 600.0f;		// パンチの長さ
	const float LENGTH_CHASEWALK = 800.0f;	// 歩き追従の長さ
	const float VELOCITY_WALK = 1.0f;		// 歩き
	const float VELOCITY_DASH = 3.5f;		// ダッシュ
	const float VELOCITY_TACKLE = 6.0f;		// タックル
	const float TIME_WAIT = 1.0f;			// 待機
	const float TIME_PROXIMITYCHASE = 5.0f;	// 近接攻撃の追従時間最大値
	const float TIME_DMG = static_cast<float>(10) / static_cast<float>(mylib_const::DEFAULT_FPS);	// ダメージ時間
	const float TIME_DOWN = static_cast<float>(150) / static_cast<float>(mylib_const::DEFAULT_FPS);	// ダウン時間
}

//==========================================================================
// 関数ポインタ
//==========================================================================

//==========================================================================
//  コンストラクタ
//==========================================================================
CEnemyBoss::CEnemyBoss(int nPriority) : CEnemy(nPriority)
{
	m_TargetPosition = 0.0f;	// 目標の位置
	m_fActTime = 0.0f;			// 行動カウンター
	m_bCatchUp = false;			// 追い着き判定
	m_pBossHPGauge = nullptr;	// ボスのHPゲージ


	m_pAtkPattern.push_back(DEBUG_NEW CBossSideSwipeCombo());	// 横なぎコンボ
	m_pAtkPattern.push_back(DEBUG_NEW CBossOverHead());			// 振り下ろし
	m_pAtkPattern.push_back(DEBUG_NEW CBossLaunchBallast());	// 瓦礫飛ばし
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
	//CManager::GetInstance()->GetBlackFrame()->SetState(CBlackFrame::STATE_OUT);

	DrawingRandomAction();
	return S_OK;
}

//==========================================================================
//  終了処理
//==========================================================================
void CEnemyBoss::Uninit(void)
{
	// 終了処理
	CEnemy::Uninit();

	for (int i = 0; i < static_cast<int>(m_pAtkPattern.size()); i++)
	{
		delete m_pAtkPattern[i];
		m_pAtkPattern[i] = nullptr;
	}
}

//==========================================================================
// 殺す
//==========================================================================
void CEnemyBoss::Kill(void)
{
	if (m_pHPGauge != nullptr)
	{
		m_pBossHPGauge->Kill();
		m_pBossHPGauge = nullptr;
	}

	for (int i = 0; i < static_cast<int>(m_pAtkPattern.size()); i++)
	{
		delete m_pAtkPattern[i];
		m_pAtkPattern[i] = nullptr;
	}
	// 死亡処理
	CEnemy::Kill();
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

	// 更新処理
	CEnemy::Update();

	if (IsDeath() == true)
	{// 死亡フラグが立っていたら
		return;
	}

	// 攻撃実行処理
	PerformAttack();

	// HP更新
	m_pBossHPGauge->SetLife(GetLife());

	// プレイヤー情報
	CPlayer* pPlayer = CPlayer::GetListObj().GetData(m_nTargetPlayerIndex);
	if (pPlayer == nullptr)
	{
		return;
	}
	m_TargetPosition = pPlayer->GetPosition();

	//// 黒フレーム捌ける
	//if (CManager::GetInstance()->GetBlackFrame()->GetState() == CBlackFrame::STATE_INCOMPLETION)
	//{
	//	CManager::GetInstance()->GetBlackFrame()->SetState(CBlackFrame::STATE_OUT);
	//}
}

//==========================================================================
// 通常ヒット時の反応
//==========================================================================
void CEnemyBoss::NormalHitResponse()
{
	// ダメージ状態にする
	m_state = STATE_DMG;

	// 遷移カウンター設定
	m_fStateTime = TIME_DMG;

	// ヒットストップ
	CManager::GetInstance()->SetEnableHitStop(5);

	// 振動
	CManager::GetInstance()->GetCamera()->SetShake(10, 15.0f, 0.0f);

	// ノックバックの位置更新
	m_posKnokBack = GetPosition();
}

//==========================================================================
// カウンターヒット時の反応
//==========================================================================
void CEnemyBoss::CounterHitResponse()
{
	// ダウン状態にする
	m_state = STATE_DOWN;

	// 遷移カウンター設定
	m_fStateTime = TIME_DMG;

	// ダウンカウンター設定
	m_fDownTime = TIME_DOWN;

	// やられモーション
	//GetMotion()->Set(MOTION_DOWN);

	// ヒットストップ
	CManager::GetInstance()->SetEnableHitStop(5);

	// 振動
	CManager::GetInstance()->GetCamera()->SetShake(10, 15.0f, 0.0f);

	// ノックバックの位置更新
	m_posKnokBack = GetPosition();
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

}

//==========================================================================
// 攻撃状態切り替え
//==========================================================================
void CEnemyBoss::ChangeATKState(CBossState* state)
{
	m_pATKState = state;
}

//==========================================================================
// 攻撃状態切り替え
//==========================================================================
void CEnemyBoss::PerformAttack()
{
	m_pATKState->Action(this);
}

//==========================================================================
// 攻撃状態切り替え
//==========================================================================
void CEnemyBoss::DrawingRandomAction()
{
	// ランダムにアクションパターンを選択して実行
	if (!m_pAtkPattern.empty())
	{
		int randomIndex = rand() % m_pAtkPattern.size();
		ChangeATKState(m_pAtkPattern[randomIndex]);
		m_bCatchUp = false;

		// モーションインデックス切り替え
		m_pATKState->ChangeMotionIdx(this);
	}
}


//==========================================================================
// 攻撃処理
//==========================================================================
void CBossAttack::Attack(CEnemyBoss* boss)
{
	// モーション取得
	CMotion* pMotion = boss->GetMotion();
	if (pMotion == NULL)
	{
		return;
	}

	int nType = pMotion->GetType();
	if (nType == m_nIdxMotion && pMotion->IsFinish() == true)
	{// 攻撃が終わってたら

		// 次の行動抽選
		boss->DrawingRandomAction();

		// 待機モーション設定
		pMotion->Set(CEnemyBoss::MOTION_DEF);
		return;
	}

	if (nType != m_nIdxMotion)
	{
		// モーション設定
		pMotion->Set(m_nIdxMotion);
	}

	// 攻撃フラグを立てる
	//m_sMotionFrag.bATK = true;

}


//==========================================================================
// 近接攻撃の行動
//==========================================================================
void CBossProximity::Action(CEnemyBoss* boss)
{
	// 範囲外時追従
	if (!boss->IsCatchUp())
	{
		// 追い掛ける
		boss->ActChase();
		return;
	}

	// 攻撃処理
	Attack(boss);
}

//==========================================================================
// 遠距離攻撃の行動
//==========================================================================
void CBossRemote::Action(CEnemyBoss* boss)
{
	// 攻撃処理
	Attack(boss);
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
		DrawingRandomAction();
		m_fActTime = 0.0f;
	}
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

	// 情報取得
	MyLib::Vector3 move = GetMove();
	MyLib::Vector3 rot = GetRotation();
	float fMove = GetVelocity();

	// 移動量設定
	move.x += sinf(D3DX_PI + rot.y) * fMove * VELOCITY_WALK;
	move.z += cosf(D3DX_PI + rot.y) * fMove * VELOCITY_WALK;

	// 移動量設定
	SetMove(move);

	// 追い着き判定
	m_bCatchUp = UtilFunc::Collision::CircleRange3D(GetPosition(), m_TargetPosition, LENGTH_PUNCH, 0.0f);
}


//==========================================================================
// 描画処理
//==========================================================================
void CEnemyBoss::Draw(void)
{
	// 描画処理
	CEnemy::Draw();
}

//==========================================================================
// モーションセット
//==========================================================================
void CEnemyBoss::MotionSet(void)
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
	case MOTION_SIDESWIPE:
		break;

	case MOTION_LAUNCHBALLAST:
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
	case MOTION_SIDESWIPE:
		break;

	case MOTION_LAUNCHBALLAST:
		break;
	}
}
