//==========================================================================
// 
//  練習敵処理 [enemy_practice.cpp]
//  Author : 相馬靜雅
// 
//==========================================================================
#include "enemy_practice.h"
#include "manager.h"
#include "particle.h"
#include "player.h"
#include "game.h"
#include "camera.h"

//==========================================================================
// 定数定義
//==========================================================================
namespace
{
	const float LENGTH_PUNCH = 110.0f;		// パンチの長さ
	const float VELOCITY_WALK = 1.0f;		// 歩き
	const float TIME_WAIT = 2.0f;			// 待機
	const float TIME_STRONGATK = 1.0f;		// 強攻撃発生までのタイマー
}

//==========================================================================
// コンストラクタ
//==========================================================================
CEnemyPractice::CEnemyPractice(int nPriority) : CEnemy(nPriority)
{

}

//==========================================================================
// デストラクタ
//==========================================================================
CEnemyPractice::~CEnemyPractice()
{

}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CEnemyPractice::Init()
{
	//初期化処理
	CEnemy::Init();

	// 行動
	m_Action = ACTION_DEF;
	
	// ダウン復帰
	m_pReturnDown = DEBUG_NEW CReturnDown_Practice();

	// 視界・追い着きフラグリセット
	m_bCatchUp = false;
	m_bInSight = false;

	// 強攻撃タイマー
	m_fStrongAttackTime = TIME_STRONGATK;

	// スーパーアーマー
	m_bActiveSuperArmor = true;

	float angle = GetPosition().AngleXZ(0.0f);
	SetRotation(MyLib::Vector3(0.0f, angle, 0.0f));
	SetRotDest(angle);

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CEnemyPractice::Uninit()
{
	// 終了処理
	CEnemy::Uninit();
}

//==========================================================================
// 殺す
//==========================================================================
void CEnemyPractice::Kill()
{
	// 死亡処理
	CEnemy::Kill();
}

//==========================================================================
// 更新処理
//==========================================================================
void CEnemyPractice::Update()
{
	// 死亡の判定
	if (IsDeath() == true)
	{// 死亡フラグが立っていたら
		return;
	}

	// 更新処理
	CEnemy::Update();

	// 死亡の判定
	if (IsDeath() == true)
	{// 死亡フラグが立っていたら
		return;
	}

	// 生成位置に固定
	SetPosition(CObject::GetOriginPosition());
	SetRotation(MyLib::Vector3(0.0f, GetPosition().AngleXZ(0.0f), 0.0f));

	if (GetMotion()->GetType() == MOTION::MOTION_ATTACK_STRONG)
	{
		CMyEffekseer::GetInstance()->SetPosition(m_pWeaponHandle, GetCenterPosition());
	}
}


//==========================================================================
// ヒット処理
//==========================================================================
bool CEnemyPractice::Hit(const int nValue, const MyLib::Vector3& hitpos, CGameManager::AttackType atkType)
{
	bool bHit = false;

	// 向き取得
	MyLib::Vector3 rot = GetRotation();
	MyLib::Vector3 pos = GetPosition();

	if (m_bDamageReceived)
	{// ダメージ受付中のみ

		// 当たった
		bHit = true;

		// 行動可能判定
		if (!m_bActiveSuperArmor)
		{
			m_bActionable = false;
		}

		// ダメージを受け付けない
		m_bDamageReceived = false;

		// ダメージ時間設定
		m_fDamageReciveTime = static_cast<float>(10) / static_cast<float>(mylib_const::DEFAULT_FPS);

		// 過去の状態保存
		m_Oldstate = m_state;

		switch (atkType)
		{
		case CGameManager::ATTACK_NORMAL:
			NormalHitResponse();
			break;

		case CGameManager::ATTACK_COUNTER:
			CounterHitResponse();
			break;
		}
	}

	return bHit;
}

//==========================================================================
// 通常ヒット時の反応
//==========================================================================
void CEnemyPractice::NormalHitResponse()
{
	
	// ダメージ状態にする
	if (m_state != STATE_DOWN &&
		m_state != STATE_STRONGATK)
	{
		m_state = STATE_DMG;
	}

	// 遷移カウンター設定
	m_fStateTime = static_cast<float>(10) / static_cast<float>(mylib_const::DEFAULT_FPS);

	// 振動
	CManager::GetInstance()->GetCamera()->SetShake(10, 15.0f, 0.0f);
}

//==========================================================================
// カウンターヒット時の反応
//==========================================================================
void CEnemyPractice::CounterHitResponse()
{
	
	// ダウン状態にする
	m_state = STATE_DOWN;

	// 遷移カウンター設定
	m_fStateTime = static_cast<float>(10) / static_cast<float>(mylib_const::DEFAULT_FPS);

	// ダウンカウンター設定
	m_fDownTime = static_cast<float>(150) / static_cast<float>(mylib_const::DEFAULT_FPS);

	// やられモーション
	GetMotion()->Set(MOTION_DOWN);

	// ヒットストップ
	CManager::GetInstance()->SetEnableHitStop(5);

	// 振動
	CManager::GetInstance()->GetCamera()->SetShake(10, 15.0f, 0.0f);
}

//==========================================================================
// ダウン状態
//==========================================================================
void CEnemyPractice::StateDown()
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
// 描画処理
//==========================================================================
void CEnemyPractice::Draw()
{
	// 描画処理
	CEnemy::Draw();
}

//==========================================================================
// モーションセット
//==========================================================================
void CEnemyPractice::MotionSet()
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
	}
}

//==========================================================================
// 攻撃時処理
//==========================================================================
void CEnemyPractice::AttackAction(CMotion::AttackInfo ATKInfo, int nCntATK)
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
	case MOTION_ATTACK_NORMAL:
		CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL::LABEL_SE_ENEMY_SWING_SLOW);
		break;

	case MOTION_ATTACK_STRONG:
		if (nCntATK == 0) {
			m_pWeaponHandle = CMyEffekseer::GetInstance()->SetEffect(
				CMyEffekseer::EFKLABEL_STRONGATK_SIGN,
				GetCenterPosition(), 0.0f, 0.0f, 20.0f);

			CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL::LABEL_SE_STRONGATK, true);
		}
		else {
			CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL::LABEL_SE_ENEMY_SWING_SLOW);
		}
		break;
	}
}

//==========================================================================
// 攻撃判定中処理
//==========================================================================
void CEnemyPractice::AttackInDicision(CMotion::AttackInfo* pATKInfo, int nCntATK)
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
	case MOTION_ATTACK_NORMAL:
		break;
	}
}


//==========================================================================
// 近接攻撃の行動
//==========================================================================
void CAttack_PracticeNormalAttack::Action(CEnemy* boss)
{
	Attack(boss);
}

//==========================================================================
// 強攻撃の行動
//==========================================================================
void CAttack_PracticeStrongAttack::Action(CEnemy* boss)
{
	Attack(boss);
}