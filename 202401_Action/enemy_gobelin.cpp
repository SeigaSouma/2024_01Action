//==========================================================================
// 
//  ゴブリン敵処理 [enemy_gobelin.cpp]
//  Author : 相馬靜雅
// 
//==========================================================================
#include "enemy_gobelin.h"
#include "manager.h"
#include "calculation.h"
#include "particle.h"
#include "player.h"

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
CEnemyGobelin::CEnemyGobelin(int nPriority) : CEnemy(nPriority)
{

}

//==========================================================================
// デストラクタ
//==========================================================================
CEnemyGobelin::~CEnemyGobelin()
{

}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CEnemyGobelin::Init()
{
	//初期化処理
	CEnemy::Init();

	// 行動
	m_Action = ACTION_DEF;
	m_pAtkPattern.push_back(DEBUG_NEW CEnemyNormalAttack());	// 通常攻撃
	m_pAtkPattern.push_back(DEBUG_NEW CEnemyStrongAttack());	// 通常攻撃

	// 視界・追い着きフラグリセット
	m_bCatchUp = false;
	m_bInSight = false;

	// 攻撃切り替え
	ChangeATKState(m_pAtkPattern[0]);
	m_pATKState->ChangeMotionIdx(this);

	// 強攻撃タイマー
	m_fStrongAttackTime = TIME_STRONGATK;

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CEnemyGobelin::Uninit()
{
	// 終了処理
	CEnemy::Uninit();
}

//==========================================================================
// 殺す
//==========================================================================
void CEnemyGobelin::Kill()
{
	// 死亡処理
	CEnemy::Kill();
}

//==========================================================================
// 更新処理
//==========================================================================
void CEnemyGobelin::Update()
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
// ダウン状態
//==========================================================================
void CEnemyGobelin::StateDown()
{
	// 行動可能判定
	m_bActionable = false;

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
// 描画処理
//==========================================================================
void CEnemyGobelin::Draw()
{
	// 描画処理
	CEnemy::Draw();
}

//==========================================================================
// モーションセット
//==========================================================================
void CEnemyGobelin::MotionSet()
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
void CEnemyGobelin::AttackAction(CMotion::AttackInfo ATKInfo, int nCntATK)
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
		break;
	}
}

//==========================================================================
// 攻撃判定中処理
//==========================================================================
void CEnemyGobelin::AttackInDicision(CMotion::AttackInfo* pATKInfo, int nCntATK)
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
