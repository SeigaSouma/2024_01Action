//=============================================================================
// 
//  ボスヘッダー [enemy_boss.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _ENEMYBOSS_H_
#define _ENEMYBOSS_H_	// 二重インクルード防止

#include "enemy.h"

// 前方宣言
class CHP_GaugeBoss;

//==========================================================================
// クラス定義
//==========================================================================
// ボスクラス定義
class CEnemyBoss : public CEnemy
{
public:

	//=============================
	// 列挙型定義
	//=============================
	//モーション列挙
	enum MOTION
	{
		MOTION_DEF = 0,			// ニュートラルモーション
		MOTION_WALK,			// 移動モーション
		MOTION_DASH,			// ダッシュ移動モーション
		MOTION_GUARD,			// ガードモーション
		MOTION_PUNCH,			// パンチモーション
		MOTION_KICK,			// キックモーション
		MOTION_BEAM,			// ビームモーション
		MOTION_CHARGE_TACKLE,	// タックルチャージモーション
		MOTION_TACKLE,			// タックルモーション
		MOTION_KNOCKBACK,		// やられモーション
		MOTION_FADEOUT,			// 帰還モーション
	};

	// 行動分岐
	enum eActionBranch
	{
		ACTBRANCH_CHASE_SLOW = 0,	// 歩き追従
		ACTBRANCH_CHASE_DASH,		// ダッシュ追従
		ACTBRANCH_PROXIMITY_PUNCH,	// 近接攻撃(パンチ)
		ACTBRANCH_PROXIMITY_KICK,	// 近接攻撃(キック)
		ACTBRANCH_REMOTE_BEAM,		// 遠距離攻撃(ビーム)
		ACTBRANCH_ASSAULT_CHARGE,	// 突撃攻撃(溜め)
		ACTBRANCH_ASSAULT_TACKLE,	// 突撃攻撃(タックル)
	};

	// 行動列挙
	enum ACTION
	{
		ACTION_CHASE = 0,		// 追従
		ACTION_PROXIMITY,		// 近接攻撃
		ACTION_REMOTE,			// 遠隔攻撃
		ACTION_ASSAULT,			// 突撃攻撃
		ACTION_WAIT,			// 待機
		ACTION_GUARD,			// ガード
		ACTION_SELFEXPLOSION,	// 自爆
		ACTION_MAX
	};

	CEnemyBoss(int nPriority = mylib_const::ENEMY_PRIORITY);
	~CEnemyBoss();

	// オーバーライドされた関数
	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
	void Kill(void) override;

	void SetAction(ACTION action) { m_Action = action; }	// アクション設定
	void SetTargetPosition(MyLib::Vector3 pos);	// 目標の位置設定

private:


	//=============================
	// 関数リスト
	//=============================
	typedef void(CEnemyBoss::*ACT_FUNC)(void);
	static ACT_FUNC m_ActFuncList[];	// 行動関数リスト

	//=============================
	// メンバ関数
	//=============================
	// 行動関数
	void ActionSet(void) override;		// 行動の設定
	void DrawingAction(void);			// 行動抽選
	void UpdateAction(void) override;	// 行動更新
	void ActChase(void);				// 追い掛け
	void ActAttackProximity(void);		// 近接攻撃
	void ActAttackRemote(void);			// 遠隔攻撃
	void ActAttackAssault(void);		// 突撃攻撃
	void ActWait(void);					// 待機
	void ActGuard(void);				// ガード
	void ActAttackExplosion(void);		// 自爆攻撃

	// 行動内関数
	void ChaseSlow(void);		// 歩き追い掛け
	void ChaseDash(void);		// ダッシュ追い掛け
	void AttackPunch(void);		// パンチ攻撃
	void AttackKick(void);		// キック攻撃
	void AttackBeam(void);		// ビーム攻撃
	void ChargeTackle(void);	// タックルチャージ
	void AttackTackle(void);	// タックル攻撃

	// その他関数
	void MotionSet(void) override;	// モーションの設定
	void RotationTarget(void);		// ターゲットの方を向く
	void AttackAction(CMotion::AttackInfo ATKInfo, int nCntATK) override;		// 攻撃時処理
	void AttackInDicision(CMotion::AttackInfo ATKInfo, int nCntATK) override;	// 攻撃判定中処理

	//=============================
	// メンバ変数
	//=============================
	ACTION m_Action;		// 行動
	eActionBranch m_ActionBranch;	// 行動分岐
	eActionBranch m_MakeForActionBranch;	// 行動する為の行動
	MyLib::Vector3 m_TargetPosition;	// 目標の位置
	float m_fActTime;		// 行動カウンター
	float m_fAssultLength;		// 突撃長さ
	float m_fAssultLengthDest;	// 目標の突撃長さ
	bool m_bCatchUp;	// 追い着き判定
	CHP_GaugeBoss *m_pBossHPGauge;	// ボスのHPゲージ
};



#endif