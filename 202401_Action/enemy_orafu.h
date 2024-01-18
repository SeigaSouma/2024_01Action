//=============================================================================
// 
//  オラフ敵ヘッダー [enemy_orafu.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _ENEMYORAFU_H_
#define _ENEMYORAFU_H_	// 二重インクルード防止

#include "enemy.h"

//==========================================================================
// クラス定義
//==========================================================================
// オラフ敵クラス定義
class CEnemyOrafu : public CEnemy
{
public:

	//=============================
	// 列挙型定義
	//=============================
	//モーション列挙
	enum MOTION
	{
		MOTION_DEF = 0,		// ニュートラルモーション
		MOTION_WALK,		// 移動モーション
		MOTION_PUNCH,		// パンチモーション
		MOTION_DMG,			// ダメージ
		MOTION_KNOCKBACK,	// やられモーション
	};

	// 行動列挙
	enum ACTION
	{
		ACTION_CHASE = 0,	// 追従
		ACTION_PROXIMITY,	// 近接攻撃
		ACTION_WAIT,		// 待機
		ACTION_MAX
	};

	CEnemyOrafu(int nPriority = mylib_const::ENEMY_PRIORITY);
	~CEnemyOrafu();

	// オーバーライドされた関数
	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;
	void Kill(void) override;

private:


	//=============================
	// 関数リスト
	//=============================
	typedef void(CEnemyOrafu::*ACT_FUNC)(void);
	static ACT_FUNC m_ActFuncList[];	// 行動関数リスト

	//=============================
	// メンバ関数
	//=============================
	// 行動関数
	void ActionSet(void) override;		// 行動の設定
	void UpdateAction(void) override;	// 行動更新
	void ActChase(void);				// 追い掛け
	void ActAttackProximity(void);		// 近接攻撃
	void ActWait(void);					// 待機

	// 行動内関数
	void ChaseNormal(void);		// 歩き追い掛け
	void AttackPunch(void);		// パンチ攻撃

	// その他関数
	void MotionSet(void) override;	// モーションの設定
	void RotationTarget(void);		// ターゲットの方を向く
	void AttackAction(CMotion::AttackInfo ATKInfo, int nCntATK) override;		// 攻撃時処理
	void AttackInDicision(CMotion::AttackInfo ATKInfo, int nCntATK) override;	// 攻撃判定中処理

	//=============================
	// メンバ変数
	//=============================
	ACTION m_Action;		// 行動
	float m_fActTime;		// 行動カウンター
	bool m_bCatchUp;	// 追い着き判定
};



#endif