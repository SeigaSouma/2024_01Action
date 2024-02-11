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
		ACTION_DEF = 0,	// 待機行動
		ACTION_ATTACK,	// 攻撃行動
		ACTION_SEARCH,	// 索敵行動
		ACTION_MAX
	};

	CEnemy(int nPriority = mylib_const::ENEMY_PRIORITY);
	virtual ~CEnemy();


	// オーバーライドされた関数
	virtual HRESULT Init() override;
	virtual void Uninit() override;
	virtual void Update() override;
	virtual void Draw() override;

	void SetState(STATE state);		// 状態設定
	STATE GetState() { return m_state; }
	virtual bool Hit(const int nValue, CGameManager::AttackType atkType = CGameManager::ATTACK_NORMAL);
	virtual void NormalHitResponse();	// ヒット時の反応
	virtual void CounterHitResponse();	// ヒット時の反応

	void SetSpawnPosition(MyLib::Vector3 pos);	// スポーン地点設定
	MyLib::Vector3 GetSpawnPosition();	// スポーン地点取得
	void SetTargetPosition(MyLib::Vector3 pos) { m_TargetPosition = pos; }	// 目標の位置設定
	void SetEnableRockOn(bool bSet) { m_bRockOnAccepting = bSet; }
	bool IsRockOnAccept() { return m_bRockOnAccepting; }


	// エフェクシア
	Effekseer::Handle GetEfkHandle() { return m_pWeaponHandle; }
	void SetEfkHandle(Effekseer::Handle handle) { m_pWeaponHandle = handle; }

	// モーション
	void SetMotion(int motionIdx);	// モーションの設定

	HRESULT RoadText(const char *pFileName);
	virtual void Kill();	// 削除
	void SetParent(CEnemy *pParent);		// 親のポインタ設定
	void SetOriginRotation(MyLib::Vector3 rot);	// 元の向き
	CEnemy *GetEnemy();
	TYPE GetType() { return m_type; }	// 種類取得
	static CListManager<CEnemy> GetListObj() { return m_List; }	// リスト取得
	static CEnemy* Create(const char* pFileName, MyLib::Vector3 pos, TYPE type = TYPE_BOSS);

protected:

	// 拠点位置の種類
	enum eBaseType
	{
		BASETYPE_MAP = 0,	// マップに沿っている
		BASETYPE_ORIGIN,	// 出現位置
		BASETYPE_MAX
	};

	// モーションの判定
	struct SMotionFrag
	{
		bool bJump;			// ジャンプ中かどうか
		bool bATK;			// 攻撃中かどうか
		bool bKnockback;	// ノックバック中かどうか
		bool bMove;			// 移動中かどうか
		bool bCharge;		// チャージ中かどうか
	};

	// 隊列の構造体定義
	struct SFormationInfo
	{
		MyLib::Vector3 pos;	// 隊列の位置
		float fAngle;		// 向き
		float fLength;		// 長さ
	};

	virtual void ProcessLanding();	// 着地時処理
	virtual void AttackAction(CMotion::AttackInfo ATKInfo, int nCntATK) override;		// 攻撃時処理
	virtual void AttackInDicision(CMotion::AttackInfo* pATKInfo, int nCntATK) override;	// 攻撃判定中処理
	virtual void ChaseMove(float fMove);	// 追い掛け移動
	virtual void RotationTarget();		// ターゲットの方を向く
	virtual bool CalcLenPlayer(float fLen); // プレイヤーとの距離判定
	virtual void MoveRotation();		// 移動方向を向く
	virtual void Move();				// 移動

	virtual void UpdateAction();		// 行動更新
	virtual void ActionSet() = 0;		// 行動の設定

	// 状態更新系
	virtual void StateNone();			// 何もない状態
	virtual void SpawnWait();			// スポーン待機
	virtual void Spawn();				// スポーン
	virtual void Damage();				// ダメージ
	virtual void Dead();				// 死亡
	virtual void FadeOut();				// フェードアウト
	virtual void PlayerChase();			// プレイヤー追従
	virtual void ParentChase();			// 親追従
	virtual void StateAttack();			// 攻撃処理
	virtual void TriggerChasePlayer();	// プレイヤー追従ONにするトリガー
	virtual void ChangeToAttackState();	// 攻撃状態移行処理
	virtual void StateWait();			// 待機処理
	virtual void StateDown();			// ダウン状態

	STATE m_state;							// 状態
	STATE m_Oldstate;						// 前回の状態
	float m_fStateTime;						// 状態カウンター
	int m_nTargetPlayerIndex;				// 追い掛けるプレイヤーのインデックス番号
	float m_fActCounter;					// 移動カウンター
	bool m_bActionable;						// 行動可能か
	float m_fDownTime;						// ダウンカウンター
	float m_fRockOnDistance;				// ロックオンの距離
	MyLib::Vector3 m_posOrigin;				// 最初の位置
	MyLib::Vector3 m_posKnokBack;			// ノックバックの位置
	SMotionFrag m_sMotionFrag;				// モーションのフラグ
	CHP_Gauge *m_pHPGauge;					// HPゲージの情報
	CEnemy *m_pParent;						// 親のポインタ
	D3DXCOLOR m_mMatcol;					// マテリアルの色
	MyLib::Vector3 m_TargetPosition;		// 目標の位置
	Effekseer::Handle m_pWeaponHandle;		// エフェクトの武器ハンドル
private:

	enum MOTION
	{
		MOTION_DEF = 0,		// ニュートラルモーション
		MOTION_WALK,		// 移動モーション
		MOTION_ATK,			// 攻撃
		MOTION_DMG,			// ダメージ
		MOTION_DOWN,		// ダウン
		MOTION_KNOCKBACK,	// やられモーション
		MOTION_FADEOUT,		// 土帰還
		MOTION_MAX
	};
	
	void UpdateState();					// 状態更新処理
	virtual void UpdateStateByType();	// 種類別状態更新処理
	virtual void UpdateByType();		// 種類別更新処理
	void Collision();					// 当たり判定
	virtual void MotionSet() = 0;		// モーションの設定
	void RegistrChild(CEnemy *pChild);
	void ResetChild(CEnemy *pChild);
	void LimitArea(); // 大人の壁判定

	//=============================
	// メンバ変数
	//=============================
	TYPE m_type;			// 種類
	SFormationInfo m_sFormationInfo;	// 隊列の情報
	MyLib::Vector3 m_rotOrigin;	// 最初の向き
	int m_nTexIdx;				// テクスチャのインデックス番号
	int m_nNumChild;			// 子の数
	bool m_bAddScore;			// スコア加算するかの判定
	bool m_bRockOnAccepting;	// ロックオン受付
	int m_nBallastEmission;		// 瓦礫の発生カウンター
	CEnemy *m_pChild[mylib_const::MAX_ENEMY];	// 子のポインタ
	CShadow *m_pShadow;			// 影の情報
	CRockOnMarker* m_pRockOnMarker;		// ロックオンマーカー
	static CListManager<CEnemy> m_List;	// リスト
};



#endif