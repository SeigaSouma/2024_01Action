//=============================================================================
// 
//  ゴブリン敵ヘッダー [enemy_gobelin.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _ENEMYGOBELIN_H_
#define _ENEMYGOBELIN_H_	// 二重インクルード防止

#include "enemy.h"

//==========================================================================
// クラス定義
//==========================================================================
// ゴブリン敵クラス定義
class CEnemyGobelin : public CEnemy
{
public:

	//=============================
	// 列挙型定義
	//=============================
	//モーション列挙
	enum MOTION
	{
		MOTION_DEF = 0,			// ニュートラル
		MOTION_WALK,			// 移動
		MOTION_ATTACK_NORMAL,	// 通常攻撃
		MOTION_ATTACK_STRONG,	// 強攻撃
		MOTION_DMG,				// ダメージ
		MOTION_KNOCKBACK,		// やられ
		MOTION_FADEOUT,			// フェードアウト
		MOTION_MAX
	};

	CEnemyGobelin(int nPriority = mylib_const::ENEMY_PRIORITY);
	~CEnemyGobelin();

	// オーバーライドされた関数
	HRESULT Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;
	void Kill() override;

private:

	//=============================
	// メンバ関数
	//=============================
	// 状態関数
	virtual void StateDown() override;		// ダウン状態
	//virtual void StateStrongAtk() override;	// 強攻撃状態

	// その他関数
	void MotionSet() override;	// モーションの設定
	void AttackAction(CMotion::AttackInfo ATKInfo, int nCntATK) override;		// 攻撃時処理
	void AttackInDicision(CMotion::AttackInfo* pATKInfo, int nCntATK) override;	// 攻撃判定中処理

};



#endif