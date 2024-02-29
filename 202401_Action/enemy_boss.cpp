//==========================================================================
// 
//  ボス処理 [enemy_boss.cpp]
//  Author : 相馬靜雅
// 
//==========================================================================
#include "enemy_boss.h"
#include "manager.h"
#include "debugproc.h"
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
#include "game.h"

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

	m_pAtkPattern.push_back(DEBUG_NEW CBossSideSwipeCombo());	// 横なぎコンボ
	m_pAtkPattern.push_back(DEBUG_NEW CBossOverHead());			// 振り下ろし
	m_pAtkPattern.push_back(DEBUG_NEW CBossLaunchBallast());	// 瓦礫飛ばし
	m_pAtkPattern.push_back(DEBUG_NEW CBossHandSlap());			// 下B
	m_pAtkPattern.push_back(DEBUG_NEW CBossRolling());			// ローリング
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

	// ロックオンの距離
	m_fRockOnDistance = 700.0f;

	// 黒フレーム捌ける
	//CManager::GetInstance()->GetBlackFrame()->SetState(CBlackFrame::STATE_OUT);

	// 攻撃抽選
	//DrawingRandomAction();
	// 
	// 行動可能判定
	//m_bActionable = true;

	// ダウン復帰
	m_pReturnDown = DEBUG_NEW CReturnDown_Boss();


	// 視界・追い着きフラグリセット
	m_bCatchUp = false;
	m_bInSight = false;

	// 攻撃切り替え
	ChangeATKState(m_pAtkPattern[0]);
	//m_pATKState->ChangeMotionIdx(this);

	// スーパーアーマー
	m_bActiveSuperArmor = true;

	// リセット処理
	CGame::GetInstance()->ResetBeforeBoss();


	return S_OK;
}

//==========================================================================
//  終了処理
//==========================================================================
void CEnemyBoss::Uninit()
{
	// 終了処理
	CEnemy::Uninit();

	/*for (int i = 0; i < static_cast<int>(m_pAtkPattern.size()); i++)
	{
		delete m_pAtkPattern[i];
		m_pAtkPattern[i] = nullptr;
	}*/
}

//==========================================================================
// 殺す
//==========================================================================
void CEnemyBoss::Kill()
{
	if (m_pBossHPGauge != nullptr)
	{
		m_pBossHPGauge->Kill();
		m_pBossHPGauge = nullptr;
	}

	/*for (int i = 0; i < static_cast<int>(m_pAtkPattern.size()); i++)
	{
		delete m_pAtkPattern[i];
		m_pAtkPattern[i] = nullptr;
	}*/
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

	// HP更新
	m_pBossHPGauge->SetLife(GetLife());

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
	// ダメージ設定
	m_pBossHPGauge->SetDamage(GetLife());

	// ダメージ状態にする
	m_state = STATE_DMG;

	// 遷移カウンター設定
	m_fStateTime = TIME_DMG;

	// ヒットストップ
	//CManager::GetInstance()->SetEnableHitStop(5);

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
	// ダメージ設定
	m_pBossHPGauge->SetDamage(GetLife());

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
// 小ステップ
//==========================================================================
bool CEnemyBoss::SmallStep()
{
	bool bEnd = false;

	// モーション取得
	CMotion* pMotion = GetMotion();
	if (pMotion == nullptr)
	{
		return bEnd;
	}

	int nType = pMotion->GetType();
	if (nType == CEnemyBoss::MOTION_BACKSTEP_SMALL && pMotion->IsFinish() == true)
	{// ステップ終了

		// 待機モーション設定
		pMotion->Set(CEnemyBoss::MOTION_DEF);

		// 終了フラグ
		bEnd = true;
		return bEnd;
	}

	if (nType != CEnemyBoss::MOTION_BACKSTEP_SMALL)
	{
		// モーション設定
		pMotion->Set(CEnemyBoss::MOTION_BACKSTEP_SMALL);
	}
	return bEnd;
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
	case MOTION::MOTION_OVERHEADATK:
		if (nCntATK == 0) {
			// 振動
			CManager::GetInstance()->GetCamera()->SetShake(8, 25.0f, 0.0f);
			CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL::LABEL_SE_ENEMY_FIELD_STRONG);
			CBallast::Create(weponpos, MyLib::Vector3(5.0f, 12.0f, 5.0f), 20, 3.0f);

			CMyEffekseer::GetInstance()->SetEffect(
				CMyEffekseer::EFKLABEL::EFKLABEL_ENEMYATK_SAND,
				weponpos, 0.0f, 0.0f, 20.0f);
		}
		else{
			CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL::LABEL_SE_ENEMY_SWING_FAST);
		}
		break;

	case MOTION_SIDESWIPE:
		CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL::LABEL_SE_ENEMY_SWING_SLOW);
		break;

	case MOTION_LAUNCHBALLAST:
	{
		// プレイヤー取得
		CListManager<CPlayer> playerList = CPlayer::GetListObj();
		CPlayer* pPlayer = playerList.GetData(0);
		if (pPlayer != nullptr)
		{
			if (weponpos.y <= 0.0f)
			{
				weponpos.y = 0.0f;
			}

			MyLib::Vector3 playerpos = pPlayer->GetPosition();

			float angle = weponpos.AngleXZ(playerpos);
			CBulletObstacle::Create(weponpos, 
				MyLib::Vector3(0.0f, angle, 0.0f), 
				D3DXVECTOR2(60.0f, 15.0f), 150.0f);
			CBallast::Create(weponpos, MyLib::Vector3(5.0f, 12.0f, 5.0f), 20, 3.0f);

			// 振動
			CManager::GetInstance()->GetCamera()->SetShake(8, 25.0f, 0.0f);
			CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL::LABEL_SE_ENEMY_FIELD_SHORT);
		}
	}
		break;

	case MOTION_HANDSLAP:
		if (nCntATK == 0 || nCntATK == 1) {
			CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL::LABEL_SE_ENEMY_SWING_SLOW);
		}
		else {
			CBallast::Create(weponpos, MyLib::Vector3(5.0f, 12.0f, 5.0f), 20, 3.0f);

			// 振動
			CManager::GetInstance()->GetCamera()->SetShake(8, 25.0f, 0.0f);
			CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL::LABEL_SE_ENEMY_FIELD_SHORT);

			CMyEffekseer::GetInstance()->SetEffect(
				CMyEffekseer::EFKLABEL::EFKLABEL_ENEMYATK_SAND,
				weponpos, 0.0f, 0.0f, 20.0f);
		}
		break;

	case MOTION_ROLLING:
		if (nCntATK == 0)
		{
			MyLib::Vector3 pos = GetPosition();
			pos.y += 150.0f;

			m_pWeaponHandle = CMyEffekseer::GetInstance()->SetEffect(
				CMyEffekseer::EFKLABEL_STRONGATK_SIGN,
				pos, 0.0f, 0.0f, 50.0f);


			CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL::LABEL_SE_STRONGATK);
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

	if (m_fDownTime <= 0.0f)
	{
		m_state = STATE_NONE;
		// 行動可能判定
		m_bActionable = true;

		// ダウン復帰
		if (m_pReturnDown != nullptr)
		{
			ChangeATKState(m_pReturnDown);
			m_pATKState->ChangeMotionIdx(this);
		}
		else
		{
			// 行動抽選
			DrawingRandomAction();
		}
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
// 死亡
//==========================================================================
void CEnemyBoss::StateDead()
{
	// 行動可能判定
	m_bActionable = false;

	// ダメージを受け付けない
	m_bDamageReceived = false;

	SetMove(0.0f);

	// モーション取得
	CMotion* pMotion = GetMotion();
	if (pMotion == nullptr)
	{
		return;
	}

	int nType = pMotion->GetType();
	if (nType != MOTION::MOTION_KNOCKBACK)
	{
		pMotion->Set(MOTION::MOTION_KNOCKBACK);
	}

	if (nType == MOTION::MOTION_KNOCKBACK && pMotion->IsFinish() == true)
	{
		// 次の行動抽選
		m_state = STATE::STATE_FADEOUT;
		m_fStateTime = 2.0f;
		return;
	}
}

//==========================================================================
// フェードアウト
//==========================================================================
void CEnemyBoss::StateFadeOut()
{
	// 行動可能判定
	m_bActionable = false;

	// ダメージを受け付けない
	m_bDamageReceived = false;

	// モーション取得
	CMotion* pMotion = GetMotion();
	if (pMotion == nullptr)
	{
		return;
	}

	m_sMotionFrag.bMove = false;	// 移動判定OFF
	m_sMotionFrag.bATK = false;		// 攻撃判定OFF

	// 状態遷移カウンター減算
	m_fStateTime -= CManager::GetInstance()->GetDeltaTime();

	// 色設定
	m_mMatcol.a = m_fStateTime / 2.0f;

	if (m_fStateTime <= 0.0f)
	{
		// 敵の終了処理
		Kill();
		Uninit();
		return;
	}
}

//==========================================================================
// ダウン待機状態
//==========================================================================
void CEnemyBoss::StateDownWait()
{
	// ダウン待機状態
	CEnemy::StateDownWait();

	// モーション取得
	CMotion* pMotion = GetMotion();
	if (pMotion == nullptr)
	{
		return;
	}

	int nType = pMotion->GetType();
	if (nType != MOTION_DOWNWAIT)
	{
		// ダウン待機モーション設定
		pMotion->Set(MOTION_DOWNWAIT);
	}

}

//==========================================================================
// 攻撃前処理
//==========================================================================
void CBossSideSwipeCombo::BeforeAttack(CEnemy* enemy)
{
	CEnemyBoss* bossEnemy = dynamic_cast<CEnemyBoss*>(enemy);
	if (bossEnemy) 
	{
		m_bBeforeAttackAction = bossEnemy->SmallStep();
	}
	else 
	{
		// 攻撃前行動フラグ終了
		m_bBeforeAttackAction = true;
	}
}
