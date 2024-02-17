//=============================================================================
// 
//  敵ヘッダー [enemy.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _ENEMY_H_
#define _ENEMY_H_	// 二重インクルード防止

#include "objectChara.h"
#include "motion.h"
#include "enemymanager.h"
#include "resultmanager.h"
#include "listmanager.h"
#include "myeffekseer.h"

// 前方宣言
class CHP_Gauge;
class CShadow;
class CEffect3D;
class CEnemyFixedMoveManager;
class CRockOnMarker;

class CEnemyState;
class CEnemyAttack;

//==========================================================================
// クラス定義
//==========================================================================
// 敵クラス定義
class CEnemy : public CObjectChara
{
public:
	// 敵種類
	enum TYPE
	{
		TYPE_BOSS = 0,	// ボス
		TYPE_COOKIE,	// クッキー
		TYPE_ORAFU,		// オラフ
		TYPE_MAX
	};

	// 状態列挙
	enum STATE
	{
		STATE_NONE = 0,		// なにもない
		STATE_SPAWNWAIT,	// スポーン待機
		STATE_SPAWN,		// スポーン
		STATE_DMG,			// ダメージ
		STATE_DEAD,			// 死
		STATE_FADEOUT,		// フェードアウト
		STATE_PLAYERCHASE,	// プレイヤー追い掛け
		STATE_PARENTCHASE,	// 親追い掛け
		STATE_ATTACK,		// 攻撃
		STATE_WAIT,			// 待機
		STATE_DOWN,			// ダウン
		STATE_MAX
	};

	// 行動列挙
	enum ACTION
	{
		ACTION_DEF = 0,	// 通常行動
		ACTION_WAIT,	// 待機行動
		ACTION_MAX
	};

	enum MOTION
	{
		MOTION_DEF = 0,		// ニュートラルモーション
		MOTION_WALK,		// 移動モーション
		MOTION_ATK,			// 攻撃
		MOTION_DMG,			// ダメージ
		MOTION_DOWN,		// ダウン
		MOTION_KNOCKBACK,	// やられモーション
		MOTION_FADEOUT,		// フェードアウト
		MOTION_MAX
	};


	CEnemy(int nPriority = mylib_const::ENEMY_PRIORITY);
	virtual ~CEnemy();


	// オーバーライドされた関数
	virtual HRESULT Init() override;
	virtual void Uninit() override;
	virtual void Update() override;
	virtual void Draw() override;

	virtual void Kill();	// 削除

	void SetState(STATE state);		// 状態設定
	STATE GetState() { return m_state; }
	void SetStateTime(float time) { m_fStateTime = time; }	// 状態時間設定
	void SetDownTime(float time) { m_fDownTime = time; }	// ダウン時間設定
	virtual bool Hit(const int nValue, CGameManager::AttackType atkType = CGameManager::ATTACK_NORMAL);
	virtual void NormalHitResponse();	// ヒット時の反応
	virtual void CounterHitResponse();	// ヒット時の反応

	// ロックオン関連
	void SetTargetPosition(MyLib::Vector3 pos) { m_TargetPosition = pos; }	// 目標の位置設定
	void SetEnableRockOn(bool bSet) { m_bRockOnAccepting = bSet; }
	bool IsRockOnAccept() { return m_bRockOnAccepting; }

	// 攻撃状態切り替え
	void ChangeATKState(CEnemyState* state);
	void ChangeNextATKState(CEnemyState* state) { m_pNextATKState = state; }
	CEnemyState* GetATKState() { return m_pATKState; }
	CEnemyState* GetNextATKState() { return m_pNextATKState; }

	void PerformAttack();		// 攻撃実行処理
	void DrawingRandomAction();	// 攻撃ランダム抽選
	void ChangeNextAction();	// 次の攻撃へ切り替え

	virtual void ActChase(float moveMultiply, float catchLen);	// 追い掛け
	virtual void RotationTarget(float range = 90.0f);	// ターゲットの方を向く

	bool IsCatchUp() { return m_bCatchUp; }	// 追い着き判定
	bool IsInSight() { return m_bInSight; }	// 視界内判定

	// モーション
	void SetMotion(int motionIdx);	// モーションの設定

	HRESULT RoadText(const char *pFileName);
	void SetParent(CEnemy *pParent);		// 親のポインタ設定
	CEnemy *GetEnemy();
	TYPE GetType() { return m_type; }	// 種類取得
	static CListManager<CEnemy> GetListObj() { return m_List; }	// リスト取得
	static CEnemy* Create(const char* pFileName, MyLib::Vector3 pos, TYPE type = TYPE_BOSS);

protected:

	//=============================
	// 構造体定義
	//=============================
	// モーションの判定
	struct SMotionFrag
	{
		bool bJump;			// ジャンプ中かどうか
		bool bATK;			// 攻撃中かどうか
		bool bKnockback;	// ノックバック中かどうか
		bool bMove;			// 移動中かどうか
		bool bCharge;		// チャージ中かどうか
		SMotionFrag() : bJump(false), bATK(false), bKnockback(false), bMove(false), bCharge(false) {}
	};

	//=============================
	// メンバ関数
	//=============================
	virtual void ProcessLanding();	// 着地時処理
	virtual void AttackAction(CMotion::AttackInfo ATKInfo, int nCntATK) override;		// 攻撃時処理
	virtual void AttackInDicision(CMotion::AttackInfo* pATKInfo, int nCntATK) override;	// 攻撃判定中処理

	// 行動更新系
	virtual void ActDefault();		// 通常行動
	virtual void ActWait();		// 待機行動

	// 状態更新系
	virtual void StateNone();			// 何もない状態
	virtual void SpawnWait();			// スポーン待機
	virtual void Spawn();				// スポーン
	virtual void Damage();				// ダメージ
	virtual void Dead();				// 死亡
	virtual void FadeOut();				// フェードアウト
	virtual void StateWait();			// 待機処理
	virtual void StateDown();			// ダウン状態


	//=============================
	// メンバ変数
	//=============================
	ACTION m_Action;		// 行動
	float m_fActTime;		// 行動カウンター

	STATE m_state;							// 状態
	STATE m_Oldstate;						// 前回の状態
	float m_fStateTime;						// 状態カウンター
	int m_nTargetPlayerIndex;				// 追い掛けるプレイヤーのインデックス番号
	bool m_bActionable;						// 行動可能か
	float m_fDownTime;						// ダウンカウンター
	float m_fRockOnDistance;				// ロックオンの距離
	bool m_bRockOnAccepting;				// ロックオン受付
	bool m_bCatchUp;						// 追い着き判定
	bool m_bInSight;						// 視界内判定
	MyLib::Vector3 m_posKnokBack;			// ノックバックの位置
	SMotionFrag m_sMotionFrag;				// モーションのフラグ
	CHP_Gauge *m_pHPGauge;					// HPゲージの情報
	CEnemy *m_pParent;						// 親のポインタ
	D3DXCOLOR m_mMatcol;					// マテリアルの色
	MyLib::Vector3 m_TargetPosition;		// 目標の位置
	Effekseer::Handle m_pWeaponHandle;		// エフェクトの武器ハンドル

	CEnemyState* m_pATKState;		// 今の行動ポインタ
	CEnemyState* m_pNextATKState;	// 次の行動ポインタ
	std::vector<CEnemyAttack*> m_pAtkPattern;	// 攻撃の種類

private:
	
	//=============================
	// 関数リスト
	//=============================
	typedef void(CEnemy::* ACT_FUNC)();
	static ACT_FUNC m_ActFuncList[];	// 行動関数リスト


	void UpdateState();					// 状態更新処理
	void Collision();					// 当たり判定
	virtual void MotionSet() = 0;		// モーションの設定
	void RegistrChild(CEnemy *pChild);
	void ResetChild(CEnemy *pChild);
	void LimitArea(); // 大人の壁判定

	//=============================
	// メンバ変数
	//=============================
	TYPE m_type;			// 種類
	int m_nNumChild;			// 子の数
	CEnemy *m_pChild[mylib_const::MAX_ENEMY];	// 子のポインタ
	CShadow *m_pShadow;			// 影の情報
	CRockOnMarker* m_pRockOnMarker;		// ロックオンマーカー
	static CListManager<CEnemy> m_List;	// リスト
};



//=============================
// エネミーステート
//=============================
class CEnemyState
{
public:
	CEnemyState() : m_nIdxMotion(0), m_bCreateFirstTime(false), m_bBeforeAttackAction(false) {}

	virtual void Action(CEnemy* boss) = 0;	// 行動
	virtual void Attack(CEnemy* boss) = 0;	// 攻撃処理
	virtual void ChangeMotionIdx(CEnemy* boss)
	{
		// 派生クラスでインデックス設定されてる前提
		boss->SetMotion(m_nIdxMotion);
		m_bBeforeAttackAction = false;	// 攻撃前行動フラグ
	}


	virtual void BeforeAttack(CEnemy* boss) { m_bBeforeAttackAction = true; }	// 攻撃前処理

	bool IsCreateFirstTime() { return m_bCreateFirstTime; }	// 初回生成のフラグ

protected:
	int m_nIdxMotion;			// モーション番号
	bool m_bCreateFirstTime;	// 初回生成のフラグ
	bool m_bBeforeAttackAction;	// 攻撃前行動フラグ
};

//=============================
// 行動前アクション
//=============================
class CEnemyBeforeAction : public CEnemyState
{
public:

	CEnemyBeforeAction() {}

	virtual void Action(CEnemy* boss) override;		// 行動
	virtual void Attack(CEnemy* boss) override {}	// 攻撃処理

	// モーションインデックス切り替え
	virtual void ChangeMotionIdx(CEnemy* boss) override
	{
		CEnemyState::ChangeMotionIdx(boss);
	}
};


//=============================
// 攻撃クラス
//=============================
class CEnemyAttack : public CEnemyState
{
public:

	CEnemyAttack() : m_bWillDirectlyTrans(true)
	{
		m_bCreateFirstTime = true;
	}

	virtual void Action(CEnemy* boss) override = 0;	// 行動
	virtual void Attack(CEnemy* boss) override;	// 攻撃処理


	// モーションインデックス切り替え
	virtual void ChangeMotionIdx(CEnemy* boss) override
	{
		CEnemyState::ChangeMotionIdx(boss);
	}

	// 遷移前処理
	virtual void BeforeTransitionProcess(CEnemy* boss)
	{
		boss->RotationTarget();
		boss->ActChase(1.0f, 600.0f);
	}	// 遷移前処理

	bool IsDirectlyTrans() { return m_bWillDirectlyTrans; }	// 直接遷移フラグ取得


protected:
	bool m_bWillDirectlyTrans;	// 直接遷移フラグ
};

//=============================
// 近接攻撃
//=============================
class CEnemyProximity : public CEnemyAttack
{
public:

	CEnemyProximity() : m_fAttackLength(200.0f) {}
	CEnemyProximity(float len) : m_fAttackLength(len) {}

	virtual void Action(CEnemy* boss) override;	// 行動
	virtual void ChangeMotionIdx(CEnemy* boss) override = 0;	// モーションインデックス切り替え

protected:
	float m_fAttackLength;	// 攻撃可能な間合い

};

//=============================
// 遠距離攻撃
//=============================
class CEnemyRemote : public CEnemyAttack
{
public:
	CEnemyRemote() {}

	virtual void Action(CEnemy* boss) override;	// 行動
	virtual void ChangeMotionIdx(CEnemy* boss) override = 0;	// モーションインデックス切り替え
};



namespace UtilFunc
{
	namespace Transformation
	{
		int Random(int nMinNum, int nMaxNum);
	}
}

//=============================
// 近接群
//=============================
// 通常攻撃
class CEnemyNormalAttack : public CEnemyProximity
{
public:
	CEnemyNormalAttack() { m_bWillDirectlyTrans = true; }

	// モーションインデックス切り替え
	virtual void ChangeMotionIdx(CEnemy* boss) override
	{
		m_nIdxMotion = CEnemy::MOTION_ATK;
		CEnemyAttack::ChangeMotionIdx(boss);

		// 直接遷移する
		m_bWillDirectlyTrans = true;
	}
};

// 強攻撃
class CEnemyStrongAttack : public CEnemyProximity
{
public:
	CEnemyStrongAttack() { m_bWillDirectlyTrans = false; }

	// 行動
	virtual void Action(CEnemy* boss) override
	{
		Attack(boss);
	}

	// モーションインデックス切り替え
	virtual void ChangeMotionIdx(CEnemy* boss) override
	{
		m_nIdxMotion = CEnemy::MOTION_ATK;
		CEnemyAttack::ChangeMotionIdx(boss);

		// 直接遷移しない
		m_bWillDirectlyTrans = false;
	}

	// 遷移前処理
	virtual void BeforeTransitionProcess(CEnemy* boss) override
	{
		// 挟む行動を設定
		boss->ChangeATKState(DEBUG_NEW CEnemyBeforeAction());
		boss->GetATKState()->ChangeMotionIdx(boss);
	}

};

//=============================
// 遠距離群
//=============================



#endif