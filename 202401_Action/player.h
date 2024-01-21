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
class CSkillPoint;

//==========================================================================
// クラス定義
//==========================================================================
// プレイヤークラス定義
class CPlayer : public CObjectChara
{
protected:
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
		MOTION_COUNTER_ACCEPT,		// 反撃受け付け
		MOTION_COUNTER_TURN,		// 反撃受け流し
		MOTION_COUNTER_ATTACK,		// 反撃
		MOTION_MAX
	};

	// モーションの判定
	struct SMotionFrag
	{
		bool bJump;			// ジャンプ中
		bool bATK;			// 攻撃中
		bool bCounter;		// カウンター中
		bool bKnockBack;	// ノックバック中
		bool bDead;			// 死亡中
		bool bMove;			// 移動中
	};

public:

	// 状態定義
	enum STATE
	{
		STATE_NONE = 0,		// なにもない
		STATE_INVINCIBLE,	// 無敵
		STATE_DMG,			// ダメージ
		STATE_KNOCKBACK,	// ノックバック
		STATE_DEAD,			// 死
		STATE_FADEOUT,		// フェードアウト
		STATE_COUNTER,		// 反撃
		STATE_AVOID,		// 回避
		STATE_MAX
	};

	CPlayer(int nPriority = mylib_const::PRIORITY_DEF2D);
	~CPlayer();

	// オーバーライドされた関数
	virtual HRESULT Init(void) override;
	virtual void Uninit(void) override;
	virtual void Update(void) override;
	virtual void Draw(void) override;

	bool Hit(const int nValue, CGameManager::AttackType atkType = CGameManager::ATTACK_NORMAL);	// ヒット処理
	STATE GetState(void);		// 状態取得
	void SetState(STATE state, int nCntState = 0);	// 状態設定
	virtual void Kill(void);			// 死亡処理
	void SwitchRockOnTarget(void);		// ロック対象切り替え

	// 転移ビーコン
	void SetEnableTouchBeacon(bool bTouch) { m_bTouchBeacon = bTouch; }	// ビーコンに触れてる判定設定
	bool IsTouchBeacon(void) { return m_bTouchBeacon; }	// ビーコンに触れてる判定取得

	// スキルポイント
	CSkillPoint* GetSkillPoint(void) { return m_pSkillPoint; }

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

	// 関数リスト
	typedef void(CPlayer::* STATE_FUNC)(void);
	static STATE_FUNC m_StateFunc[];

	// メンバ関数
	void StateNone(void);		// なし
	void StateInvincible(void);	// 無敵
	void StateDamage(void);		// ダメージ
	void StateKnockBack(void);	// ノックバック
	void StateDead(void);		// 死亡
	void StateFadeOut(void);	// フェードアウト
	void StateCounter(void);	// カウンター中
	void StateAvoid(void);		// 回避

	virtual void Controll(void);	// 操作
	void LimitPos(void);			// 位置制限
	void MotionBySetState(void);	// モーション別の状態設定
	void ResetFrag(void);			// フラグリセット
	void RockOn(void);				// ロックオン

	void AttackAction(CMotion::AttackInfo ATKInfo, int nCntATK) override;	// 攻撃時処理
	void AttackInDicision(CMotion::AttackInfo ATKInfo, int nCntATK) override;			// 攻撃判定中処理

	STATE m_Oldstate;				// 前回の状態
	D3DXCOLOR m_mMatcol;			// マテリアルの色
	MyLib::Vector3 m_posKnokBack;	// ノックバックの位置
	MyLib::Vector3 m_KnokBackMove;	// ノックバックの移動量
	int m_nCntState;				// 状態遷移カウンター
	int m_nComboStage;				// コンボの段階
	int m_nIdxRockOn;				// ロックオン対象のインデックス番号
	bool m_bAttacking;				// 攻撃中
	bool m_bCounterAccepting;		// カウンター受付中
	bool m_bDash;					// ダッシュ判定
	float m_fDashTime;				// ダッシュ時間
	bool m_bTouchBeacon;			// ビーコンに触れてる判定
	CSkillPoint* m_pSkillPoint;		// スキルポイントのオブジェクト
	CHP_GaugePlayer* m_pHPGauge;		// HPゲージのポインタ
	Effekseer::Handle *m_pWeaponHandle;	// エフェクトの武器ハンドル
	static CListManager<CPlayer> m_List;	// リスト
};



#endif