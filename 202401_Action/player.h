//=============================================================================
// 
//  プレイヤーヘッダー [player.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _PLAYER_H_
#define _PLAYER_H_	// 二重インクルード防止

#include "gamemanager.h"
#include "objectChara.h"
#include "listmanager.h"

//==========================================================================
// 前方宣言
//==========================================================================
class CShadow;
class CHP_GaugePlayer;
class CStaminaGauge_Player;
class CSkillPoint;
class CEnemy;
class CEndCounterSetting;

class CPlayerControlAttack;	// 攻撃
class CPlayerControlDefence;	// 防御
class CPlayerControlAvoid;	// 回避
class CPlayerGuard;	// ガード

//==========================================================================
// クラス定義
//==========================================================================
// プレイヤークラス定義
class CPlayer : public CObjectChara
{
public:
	// 列挙型定義
	enum MOTION
	{
		MOTION_DEF = 0,		// ニュートラルモーション
		MOTION_WALK,		// 移動
		MOTION_DASH,		// ダッシュ
		MOTION_AVOID,		// 回避
		MOTION_ATK,			// 攻撃
		MOTION_ATK2,		// 攻撃(派生1)
		MOTION_ATK3,		// 攻撃(派生2)
		MOTION_JUMP,		// ジャンプ
		MOTION_FALL,		// 落下中
		MOTION_KNOCKBACK,	// ノックバック
		MOTION_DEAD,		// 死亡
		MOTION_RESPAWN,		// 復活
		MOTION_GUARD,		// ガード
		MOTION_GUARD_DMG,	// ガードダメージ
		MOTION_COUNTER_ACCEPT,		// 反撃受け付け
		MOTION_COUNTER_TURN,		// 反撃受け流し
		MOTION_COUNTER_ATTACK,		// 反撃
		MOTION_DASHATK,			// ダッシュ攻撃
		MOTION_MAX
	};

	// 状態定義
	enum STATE
	{
		STATE_NONE = 0,		// なにもない
		STATE_INVINCIBLE,	// 無敵
		STATE_DMG,			// ダメージ
		STATE_KNOCKBACK,	// ノックバック
		STATE_DEAD,			// 死
		STATE_DEADWAIT,		// 死亡待機
		STATE_FADEOUT,		// フェードアウト
		STATE_RESPAWN,		// 復活
		STATE_COUNTER,		// 反撃
		STATE_AVOID,		// 回避
		STATE_MAX
	};

	// モーションの判定
	struct SMotionFrag
	{
		bool bJump;			// ジャンプ中
		bool bATK;			// 攻撃中
		bool bGuard;		// ガード
		bool bCounter;		// カウンター中
		bool bKnockBack;	// ノックバック中
		bool bDead;			// 死亡中
		bool bMove;			// 移動中
	};

	CPlayer(int nPriority = 2);
	~CPlayer();

	// オーバーライドされた関数
	virtual HRESULT Init(void) override;
	virtual void Uninit(void) override;
	virtual void Update(void) override;
	virtual void Draw(void) override;

	MyLib::HitResult_Character Hit(const int nValue, CGameManager::AttackType atkType = CGameManager::ATTACK_NORMAL);	// ヒット処理
	MyLib::HitResult_Character Hit(const int nValue, CEnemy* pEnemy, CGameManager::AttackType atkType = CGameManager::ATTACK_NORMAL);	// ヒット処理
	MyLib::HitResult_Character ProcessHit(const int nValue);

	STATE GetState(void);		// 状態取得
	void SetState(STATE state, int nCntState = 0);	// 状態設定
	virtual void Kill(void);			// 死亡処理
	void SwitchRockOnTarget(void);		// ロック対象切り替え

	// モーション
	void SetMotion(int motionIdx);	// モーションの設定
	bool IsJump() { return m_bJump; }	// ジャンプ判定
	float GetDashTime() { return m_fDashTime; }		// ダッシュ時間
	void SetComboStage(int stage) { m_nComboStage = stage;; }	// コンボの段階設定
	int GetComboStage() { return m_nComboStage; }	// コンボの段階取得
	bool IsReadyDashAtk() { return m_bReadyDashAtk; }
	void SetMotionFrag(SMotionFrag frag) { m_sMotionFrag = frag; }
	SMotionFrag GetMotionFrag() { return m_sMotionFrag; }

	// 転移ビーコン
	void SetEnableTouchBeacon(bool bTouch) { m_bTouchBeacon = bTouch; }	// ビーコンに触れてる判定設定
	bool IsTouchBeacon(void) { return m_bTouchBeacon; }	// ビーコンに触れてる判定取得

	// スキルポイント
	CSkillPoint* GetSkillPoint(void) { return m_pSkillPoint; }

	// スタミナ
	CStaminaGauge_Player* GetStaminaGauge() { return m_pStaminaGauge; }

	// スキルツリー用関数
	void UpgradeLife(int addvalue);	// 体力アップグレード
	void DowngradeLife(int addvalue);	// 体力ダウングレード
	void UpgradeMaxStamina(int addvalue);		// スタミナ最大値アップグレード
	void DowngradeMaxStamina(int addvalue);		// スタミナ最大値ダウングレード
	void UpgradeAutoHealStamina(float ratio);	// スタミナ自動回復アップグレード
	void DowngradeAutoHealStamina(float ratio);	// スタミナ自動回復ダウングレード
	void ChangeAtkControl(CPlayerControlAttack* control);	// 攻撃の操作変更
	void ChangeDefenceControl(CPlayerControlDefence* control);	// 防御の操作変更
	void ChangeAvoidControl(CPlayerControlAvoid* control);	// 回避の操作変更
	void ChangeGuardGrade(CPlayerGuard* guard);	// ガード性能変更

	// リスポーン
	void RespawnSetting(void);	// リスポーン時の設定
	int GetRespawnPercent(void) { return m_nRespawnPercent; }	// リスポーン確率取得
	void SetRespawnPercent(int value);	// リスポーン確率設定

	// 反撃
	bool IsLockOnAtStart() { return m_bLockOnAtStart; }	// 反撃開始時にロックオンしていたか
	void SetCounterSubValue(float value) { m_fCounterStaminaSubValue = value; }	// カウンター時の消費スタミナ設定
	float GetCounterSubValue() { return m_fCounterStaminaSubValue; }	// カウンター時の消費スタミナ取得
	void SetCounterHealValue(float value) { m_fCounterStaminaHealValue = value; }	// カウンター時のスタミナ回復設定
	float GetCounterHealValue() { return m_fCounterStaminaHealValue; }	// カウンター時のスタミナ回復設取得

	// ガード
	void SetGuardSubValue(float value) { m_fGuardStaminaSubValue = value; }	// ガード時の消費スタミナ設定
	float GetGuardSubValue() { return m_fGuardStaminaSubValue; }	// ガード時の消費スタミナ取得

	// ヒット系
	void DeadSetting(MyLib::HitResult_Character* result);

	// その他
	int GetMyPlayerIdx() { return m_nMyPlayerIdx; }

	static CPlayer* Create(int nIdx);	// 生成
	static CListManager<CPlayer> GetListObj(void) { return m_List; }	// リスト取得

protected:

	bool Collision(MyLib::Vector3 &pos, MyLib::Vector3 &move);	// 当たり判定
	void MotionSet(void);	// モーションの設定

	bool m_bJump;				// ジャンプ中かどうか
	bool m_bLandOld;			// 過去の着地情報
	bool m_bHitStage;			// ステージの当たり判定
	bool m_bLandField;			// フィールドの着地判定
	bool m_bHitWall;			// 壁の当たり判定
	int m_nMyPlayerIdx;			// プレイヤーインデックス番号
	int m_nCntWalk;				// 歩行カウンター
	STATE m_state;				// 状態
	CShadow *m_pShadow;			// 影の情報
	SMotionFrag m_sMotionFrag;	// モーションのフラグ
private:

	//=============================
	// 関数リスト
	//=============================
	typedef void(CPlayer::* STATE_FUNC)(void);
	static STATE_FUNC m_StateFunc[];

	//=============================
	// メンバ関数
	//=============================
	// 状態関数
	void StateNone(void);		// なし
	void StateInvincible(void);	// 無敵
	void StateDamage(void);		// ダメージ
	void StateKnockBack(void);	// ノックバック
	void StateDead(void);		// 死亡
	void StateDeadWait(void);	// 死亡待機
	void StateFadeOut(void);	// フェードアウト
	void StateRespawn(void);	// リスポーン
	void StateCounter(void);	// カウンター中
	void StateAvoid(void);		// 回避

	// その他関数
	virtual void Controll(void);	// 操作
	void LimitPos(void);			// 位置制限
	void MotionBySetState(void);	// モーション別の状態設定
	void ResetFrag(void);			// フラグリセット
	void RockOn(void);				// ロックオン

	// モーション系関数
	void AttackAction(CMotion::AttackInfo ATKInfo, int nCntATK) override;	// 攻撃時処理
	void AttackInDicision(CMotion::AttackInfo* pATKInfo, int nCntATK) override;			// 攻撃判定中処理

	//=============================
	// メンバ変数
	//=============================
	STATE m_Oldstate;				// 前回の状態
	D3DXCOLOR m_mMatcol;			// マテリアルの色
	MyLib::Vector3 m_posKnokBack;	// ノックバックの位置
	MyLib::Vector3 m_KnokBackMove;	// ノックバックの移動量
	int m_nCntState;				// 状態遷移カウンター
	int m_nComboStage;				// コンボの段階
	int m_nIdxRockOn;				// ロックオン対象のインデックス番号
	bool m_bLockOnAtStart;			// カウンター開始時にロックオンしていたか
	bool m_bReadyDashAtk;				// ダッシュアタックのフラグ
	bool m_bAttacking;				// 攻撃中
	bool m_bCounterAccepting;		// カウンター受付中
	bool m_bDash;					// ダッシュ判定
	float m_fDashTime;				// ダッシュ時間
	int m_nRespawnPercent;			// リスポーン確率
	bool m_bTouchBeacon;			// ビーコンに触れてる判定
	float m_fGuardStaminaSubValue;	// ガード時のスタミナ減少量
	float m_fCounterStaminaSubValue;	// カウンター時のスタミナ減少量
	float m_fCounterStaminaHealValue;	// カウンター時のスタミナ回復量
	CSkillPoint* m_pSkillPoint;		// スキルポイントのオブジェクト
	CHP_GaugePlayer* m_pHPGauge;		// HPゲージのポインタ
	CStaminaGauge_Player* m_pStaminaGauge;	// スタミナゲージのポインタ
	
	// パターン用インスタンス
	CEndCounterSetting* m_pEndCounterSetting;	// カウンター終了時の設定
	CPlayerControlAttack* m_pControlAtk;		// 攻撃操作
	CPlayerControlDefence* m_pControlDefence;	// 防御操作
	CPlayerControlAvoid* m_pControlAvoid;		// 回避操作
	CPlayerGuard* m_pGuard;						// ガード

	Effekseer::Handle *m_pWeaponHandle;		// エフェクトの武器ハンドル
	static CListManager<CPlayer> m_List;	// リスト
};


//==========================================================================
// カウンター終了時の設定クラス
//==========================================================================
class CEndCounterSetting
{
public:
	CEndCounterSetting() {}

	virtual void EndSetting(CPlayer* player);	// 終了時の設定
};

class CEndTurn : public CEndCounterSetting
{
	//void EndSetting(CPlayer* player) override {}
};

class CEndAttack : public CEndCounterSetting
{
	void EndSetting(CPlayer* player) override;
};



#endif