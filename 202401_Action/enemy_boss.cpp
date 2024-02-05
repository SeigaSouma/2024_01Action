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
#include "3D_effect.h"
#include "bullet_obstacle.h"
#include "ballast.h"
#include "model.h"

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
	m_bInSight = false;			// 視界内判定
	m_pBossHPGauge = nullptr;	// ボスのHPゲージ


	m_pATKState = nullptr;		// 今の行動ポインタ
	m_pNextATKState = nullptr;	// 次の行動ポインタ

	m_pAtkPattern.push_back(DEBUG_NEW CBossSideSwipeCombo());	// 横なぎコンボ
	m_pAtkPattern.push_back(DEBUG_NEW CBossOverHead());			// 振り下ろし
	m_pAtkPattern.push_back(DEBUG_NEW CBossLaunchBallast());	// 瓦礫飛ばし
	m_pAtkPattern.push_back(DEBUG_NEW CBossHandSlap());
	m_pAtkPattern.push_back(DEBUG_NEW CBossRolling());	// ローリング
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
HRESULT CEnemyBoss::Init()
{
	//初期化処理
	CEnemy::Init();

	// HPの設定
	m_pBossHPGauge = CHP_GaugeBoss::Create(MyLib::Vector3(640.0f, 50.0f, 0.0f), GetLifeOrigin());	// ボスのHPゲージ
	m_pBossHPGauge->SetLife(0);

	// 黒フレーム捌ける
	//CManager::GetInstance()->GetBlackFrame()->SetState(CBlackFrame::STATE_OUT);

	// 攻撃抽選
	//DrawingRandomAction();

	ChangeATKState(m_pAtkPattern[0]);
	return S_OK;
}

//==========================================================================
//  終了処理
//==========================================================================
void CEnemyBoss::Uninit()
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
void CEnemyBoss::Kill()
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
void CEnemyBoss::Update()
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

	if (GetMotion()->GetType() == CEnemyBoss::MOTION_ROLLING)
	{
		MyLib::Vector3 pos = UtilFunc::Transformation::WorldMtxChangeToPosition(GetModel()[2]->GetWorldMtx());

		CMyEffekseer::GetInstance()->SetPosition(m_pWeaponHandle, pos);
	}

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
	GetMotion()->Set(MOTION_DOWN);

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
void CEnemyBoss::ActionSet()
{

}

//==========================================================================
// 行動更新
//==========================================================================
void CEnemyBoss::UpdateAction()
{

}

//==========================================================================
// 攻撃状態切り替え
//==========================================================================
void CEnemyBoss::ChangeATKState(CBossState* state)
{
	if (m_pATKState != nullptr &&
		!m_pATKState->IsCreateFirstTime())
	{
		delete m_pATKState;
	}
	m_pATKState = state;
}

//==========================================================================
// 攻撃状態切り替え
//==========================================================================
void CEnemyBoss::PerformAttack()
{
	if (m_pATKState == nullptr)
	{
		return;
	}

	if (m_bActionable)
	{
		m_pATKState->Action(this);
	}
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

		if (!m_pAtkPattern[randomIndex]->IsDirectlyTrans())
		{// 何か挟んで行動する場合

			// 次の行動設定
			ChangeNextATKState(m_pAtkPattern[randomIndex]);

			// 遷移前処理
			m_pAtkPattern[randomIndex]->BeforeTransitionProcess(this);
		}
		else
		{
			ChangeATKState(m_pAtkPattern[randomIndex]);
			m_bCatchUp = false;
			m_bInSight = false;

			// モーションインデックス切り替え
			m_pATKState->ChangeMotionIdx(this);
		}
	}
}

//==========================================================================
// 次の攻撃へ切り替え
//==========================================================================
void CEnemyBoss::ChangeNextAction()
{
	// 保存していた次の行動を設定
	ChangeATKState(m_pNextATKState);
	m_bCatchUp = false;
	m_bInSight = false;

	// モーションインデックス切り替え
	m_pATKState->ChangeMotionIdx(this);
}


//==========================================================================
// 待機
//==========================================================================
void CEnemyBoss::ActWait()
{
	m_bActionable = false;

	// モーション取得
	CMotion* pMotion = GetMotion();
	if (pMotion == nullptr)
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
void CEnemyBoss::ActChase()
{
	// 移動フラグを立てる
	m_sMotionFrag.bMove = true;

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
// ターゲットの方を向く
//==========================================================================
void CEnemyBoss::RotationTarget(float range)
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


	// 視界判定
	m_bInSight = UtilFunc::Collision::CollisionViewRange3D(GetPosition(), m_TargetPosition, rot.y, range);
}

//==========================================================================
// 描画処理
//==========================================================================
void CEnemyBoss::Draw()
{
	// 描画処理
	CEnemy::Draw();
}

//==========================================================================
// モーションセット
//==========================================================================
void CEnemyBoss::MotionSet()
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
			pMotion->Set(MOTION_WALK);
		}
	}
}

//==========================================================================
// 攻撃時処理
//==========================================================================
void CEnemyBoss::AttackAction(CMotion::AttackInfo ATKInfo, int nCntATK)
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
	case MOTION_SIDESWIPE:
		break;

	case MOTION_LAUNCHBALLAST:

		if (weponpos.y <= 0.0f)
		{
			weponpos.y = 0.0f;
		}

		CBulletObstacle::Create(weponpos, rot, D3DXVECTOR2(40.0f, 15.0f), 150.0f);
		CBallast::Create(weponpos, MyLib::Vector3(5.0f, 12.0f, 5.0f), 20, 3.0f);

		// 振動
		CManager::GetInstance()->GetCamera()->SetShake(8, 25.0f, 0.0f);
		break;

	case MOTION_HANDSLAP:
		CBallast::Create(weponpos, MyLib::Vector3(5.0f, 12.0f, 5.0f), 20, 3.0f);

		// 振動
		CManager::GetInstance()->GetCamera()->SetShake(8, 25.0f, 0.0f);
		break;

	case MOTION_ROLLING:
		if (nCntATK == 0)
		{
			MyLib::Vector3 pos = GetPosition();
			pos.y += 150.0f;

			m_pWeaponHandle = CMyEffekseer::GetInstance()->SetEffect(
				CMyEffekseer::EFKLABEL_STRONGATK_SIGN,
				pos, 0.0f, 0.0f, 50.0f);
		}
		else
		{
			CMyEffekseer::GetInstance()->SetEffect(
				CMyEffekseer::EFKLABEL_BOSS_ROLLING,
				weponpos,
				MyLib::Vector3(0.0f, D3DX_PI + rot.y, 0.0f), 0.0f, 80.0f, true);
		}
		break;
	}
}

//==========================================================================
// 攻撃判定中処理
//==========================================================================
void CEnemyBoss::AttackInDicision(CMotion::AttackInfo* pATKInfo, int nCntATK)
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
	case MOTION_SIDESWIPE:
		break;

	case MOTION_LAUNCHBALLAST:
		break;
	}
}

//==========================================================================
// ダウン状態
//==========================================================================
void CEnemyBoss::StateDown()
{
	// 行動可能判定
	m_bActionable = false;

	// 色設定
	m_mMatcol = D3DXCOLOR(1.0f, 0.5f, 0.1f, 1.0f);

	if (m_fDownTime <= 0.0f)
	{
		m_state = STATE_NONE;
		// 行動可能判定
		m_bActionable = true;

		// 行動抽選
		DrawingRandomAction();
		return;
	}

	// モーション取得
	CMotion* pMotion = GetMotion();
	if (pMotion == nullptr)
	{
		return;
	}

	int nType = pMotion->GetType();
	if (nType != MOTION_DOWN)
	{
		// ダウンモーション設定
		pMotion->Set(MOTION_DOWN);
	}
}



//==========================================================================
// ステップの行動
//==========================================================================
void CBossStep::Action(CEnemyBoss* boss)
{
	// モーション取得
	CMotion* pMotion = boss->GetMotion();
	if (pMotion == nullptr)
	{
		return;
	}

	int nType = pMotion->GetType();
	if (nType == CEnemyBoss::MOTION_BACKSTEP && pMotion->IsFinish() == true)
	{// ステップ終了

		// 次の行動設定
		boss->ChangeNextAction();

		// 待機モーション設定
		pMotion->Set(CEnemyBoss::MOTION_DEF);
		return;
	}

	if (nType != CEnemyBoss::MOTION_BACKSTEP)
	{
		// モーション設定
		pMotion->Set(CEnemyBoss::MOTION_BACKSTEP);
	}
}

//==========================================================================
// 攻撃処理
//==========================================================================
void CBossAttack::Attack(CEnemyBoss* boss)
{
	// 攻撃が始まるまで向き合わせ
	if (boss->GetMotion()->IsBeforeInAttack())
	{
		// ターゲットの方を向く
		boss->RotationTarget();
	}

	// モーション取得
	CMotion* pMotion = boss->GetMotion();
	if (pMotion == nullptr)
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

}


//==========================================================================
// 近接攻撃の行動
//==========================================================================
void CBossProximity::Action(CEnemyBoss* boss)
{
	bool bAct = true;	// 行動できるかのフラグ

	// 視界内判定
	if (!boss->IsCatchUp() ||
		!boss->IsInSight())
	{
		// ターゲットの方を向く
		boss->RotationTarget();
		bAct = false;
	}

	// 範囲外時追従
	if (!boss->IsCatchUp())
	{
		// 追い掛け
		boss->ActChase();
		bAct = false;
	}

	if (!bAct)
	{
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
	bool bAct = true;	// 行動できるかのフラグ

	// 視界内判定
	if (!boss->IsInSight())
	{
		// ターゲットの方を向く
		boss->RotationTarget(45.0f);
		bAct = false;
	}

	if (!bAct)
	{
		return;
	}
	// 攻撃処理
	Attack(boss);
}