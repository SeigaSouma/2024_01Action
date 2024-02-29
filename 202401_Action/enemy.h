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
#include "calculation.h"

// 前方宣言
class CHP_Gauge;
class CShadow;
class CEffect3D;
class CEnemyFixedMoveManager;
class CRockOnMarker;

class CEnemyState;
class CEnemyAttack;
class CEnemyReturnDown;

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
		TYPE_BOSS = 0,		// ボス
		TYPE_STONEGOLEM,	// ストーンゴーレム
		TYPE_GOBELIN,		// ゴブリン
		TYPE_PRACTICE,		// 練習
		TYPE_PRACTICE_A,	// 練習(攻撃)
		TYPE_PRACTICE_SA,	// 練習(強攻撃)
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
		STATE_WAIT,			// 待機
		STATE_DOWN,			// ダウン
		STATE_STRONGATK,	// 強攻撃
		STATE_DOWNWAIT,		// ダウン待機
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
		MOTION_DEF = 0,			// ニュートラル
		MOTION_WALK,			// 移動
		MOTION_ATTACK_NORMAL,	// 通常攻撃
		MOTION_ATTACK_STRONG,	// 強攻撃
		MOTION_DMG,				// ダメージ
		MOTION_DOWN,			// ダウン
		MOTION_RETURNDOWN,		// ダウン
		MOTION_KNOCKBACK,		// やられ
		MOTION_FADEOUT,			// フェードアウト
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
	virtual bool Hit(const int nValue, const MyLib::Vector3& hitpos, CGameManager::AttackType atkType = CGameManager::AttackType::ATTACK_NORMAL);
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

	void ToggleCatchUp(bool catchUp) { m_bCatchUp = catchUp; }	// 追い着き判定
	bool IsCatchUp() { return m_bCatchUp; }						// 追い着き判定
	void ToggleInSight(bool inSight) { m_bInSight = inSight; }	// 視界内判定
	bool IsInSight() { return m_bInSight; }						// 視界内判定
	bool IsActiveSuperArmor() { return m_bActiveSuperArmor; }	// スーパーアーマーの有効フラグ取得

	// モーション
	void SetMotion(int motionIdx);	// モーションの設定

	HRESULT RoadText(const char *pFileName);
	void SetParent(CEnemy *pParent);		// 親のポインタ設定
	CEnemy *GetEnemy();
	TYPE GetType() { return m_type; }	// 種類取得
	static int GetNumSurvival() { return m_nNumSurvival; }	// 生存人数
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
	// 行動更新系
	virtual void ActDefault();		// 通常行動
	virtual void ActWait();			// 待機行動

	// 状態更新系
	virtual void StateNone();		// 何もない状態
	virtual void StateSpawnWait();	// スポーン待機
	virtual void StateSpawn();		// スポーン
	virtual void StateDamage();		// ダメージ
	virtual void StateDead();		// 死亡
	virtual void StateFadeOut();	// フェードアウト
	virtual void StateWait();		// 待機処理
	virtual void StateDown();		// ダウン状態
	virtual void StateStrongAtk();	// 強攻撃
	virtual void StateDownWait();	// ダウン待機状態

	// その他関数
	virtual void ProcessLanding();	// 着地時処理
	virtual void AttackAction(CMotion::AttackInfo ATKInfo, int nCntATK) override;		// 攻撃時処理
	virtual void AttackInDicision(CMotion::AttackInfo* pATKInfo, int nCntATK) override;	// 攻撃判定中処理

	//=============================
	// メンバ変数
	//=============================
	ACTION m_Action;			// 行動
	float m_fActTime;			// 行動カウンター
	float m_fStrongAttackTime;	// 強攻撃のタイマー

	STATE m_state;							// 状態
	STATE m_Oldstate;						// 前回の状態
	float m_fStateTime;						// 状態カウンター
	int m_nTargetPlayerIndex;				// 追い掛けるプレイヤーのインデックス番号
	bool m_bDecrementSurvival;				// 生存人数減少フラグ
	bool m_bActionable;						// 行動可能か
	bool m_bDamageReceived;					// ダメージ受け付け判定
	float m_fDamageReciveTime;				// ダメージ受付時間
	bool m_bActiveSuperArmor;				// スーパーアーマー
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
	static int m_nNumSurvival;				// 生存人数

	CEnemyState* m_pATKState;		// 今の行動ポインタ
	CEnemyState* m_pNextATKState;	// 次の行動ポインタ
	bool m_bStateChanging;			// 状態が切り替わった瞬間
	std::vector<CEnemyAttack*> m_pAtkPattern;	// 攻撃の種類
	CEnemyReturnDown* m_pReturnDown;			// ダウン復帰

private:
	
	//=============================
	// 関数リスト
	//=============================
	typedef void(CEnemy::* ACT_FUNC)();
	static ACT_FUNC m_ActFuncList[];	// 行動関数リスト

	typedef void(CEnemy::* STATE_FUNC)();
	static STATE_FUNC m_StateFunc[];	// 状態関数リスト

	void UpdateState();					// 状態更新処理
	void Collision();					// 当たり判定
	void UpdateDamageReciveTimer();		// ダメージ受付時間更新
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


class CEnemyFlinch;	// 怯みポインタ

//=============================
// エネミーステート
//=============================
class CEnemyState
{
public:
	CEnemyState() : 
		m_nIdxMotion(0), 
		m_bCreateFirstTime(false), 
		m_bBeforeAttackAction(false),
		m_bFinchAction(false),
		m_pFlinchAction(nullptr) {}

	~CEnemyState();

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
	bool IsFlinchAction() { return m_bFinchAction; }	// 怯み行動フラグ

	// 怯み行動設定
	void SetFlinchAction(CEnemyFlinch* pFlinch);

	// 怯みへ切り替え
	void ChangeFlinchAction(CEnemy* boss);

protected:
	int m_nIdxMotion;				// モーション番号
	bool m_bCreateFirstTime;		// 初回生成のフラグ
	bool m_bBeforeAttackAction;		// 攻撃前行動フラグ
	bool m_bFinchAction;			// 怯み行動フラグ
	CEnemyFlinch* m_pFlinchAction;	// 怯み行動のポインタ
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
// 怯み
//=============================
class CEnemyFlinch : public CEnemyState
{
public:
	CEnemyFlinch() {}

	virtual void Action(CEnemy* boss) override;		// 行動
	virtual void Attack(CEnemy* boss) override {}	// 攻撃処理

	// モーションインデックス切り替え
	virtual void ChangeMotionIdx(CEnemy* boss) override
	{
		CEnemyState::ChangeMotionIdx(boss);
	}
};


//=============================
// ダウン復帰
//=============================
class CEnemyReturnDown : public CEnemyState
{
public:
	CEnemyReturnDown() { m_bCreateFirstTime = true; }

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

	CEnemyAttack() : 
		m_bWillDirectlyTrans(true), 
		m_bSetAngleBeforeAttack(true), 
		m_bSetAngleNotAttacking(false), 
		m_bChainAttack(false), 
		m_pChainAttackPtr(nullptr)
	{
		m_bCreateFirstTime = true;
	}

	~CEnemyAttack()
	{
		if (m_pChainAttackPtr != nullptr)
		{
			delete m_pChainAttackPtr;
			m_pChainAttackPtr = nullptr;
		}
	}

	virtual void Action(CEnemy* boss) override = 0;	// 行動
	virtual void Attack(CEnemy* boss) override;		// 攻撃処理

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
	bool IsFlinchAction() { return m_bFinchAction; }		// 怯み行動フラグ

protected:

	// 連続攻撃設定
	void SetChainAttack(CEnemyAttack* pAttack)
	{
		if (m_pChainAttackPtr != nullptr)
		{
			delete m_pChainAttackPtr;
		}

		m_bChainAttack = true;
		m_pChainAttackPtr = pAttack;
	}

	// 連続攻撃の行動へ切替
	void ChangeChainAttack(CEnemy* boss)
	{
		if (m_pChainAttackPtr == nullptr)
		{
			return;
		}
		boss->ChangeATKState(m_pChainAttackPtr);
		m_pChainAttackPtr->ChangeMotionIdx(boss);

		// 視界内・追い着き判定
		boss->ToggleCatchUp(true);
		boss->ToggleInSight(true);
	}

	bool m_bWillDirectlyTrans;		// 直接遷移フラグ
	bool m_bSetAngleBeforeAttack;	// 攻撃前向き合わせフラグ
	bool m_bSetAngleNotAttacking;	// 攻撃判定外向き合わせフラグ
	bool m_bChainAttack;			// 連続攻撃フラグ
	CEnemyAttack* m_pChainAttackPtr;// 連続攻撃のポインタ
};

// 近接攻撃
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

// 遠距離攻撃
class CEnemyRemote : public CEnemyAttack
{
public:
	CEnemyRemote() {}

	virtual void Action(CEnemy* boss) override;	// 行動
	virtual void ChangeMotionIdx(CEnemy* boss) override = 0;	// モーションインデックス切り替え
};


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
		m_nIdxMotion = CEnemy::MOTION_ATTACK_NORMAL;
		CEnemyAttack::ChangeMotionIdx(boss);

		// 直接遷移する
		m_bWillDirectlyTrans = true;
	}
};

// 強攻撃
class CEnemyStrongAttack : public CEnemyProximity
{
public:
	CEnemyStrongAttack() { m_bWillDirectlyTrans = true; }
	CEnemyStrongAttack(float len) : CEnemyProximity(len) {}

	// モーションインデックス切り替え
	virtual void ChangeMotionIdx(CEnemy* boss) override
	{
		m_nIdxMotion = CEnemy::MOTION_ATTACK_STRONG;
		CEnemyAttack::ChangeMotionIdx(boss);

		// 強攻撃状態に設定
		boss->SetState(CEnemy::STATE_STRONGATK);
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