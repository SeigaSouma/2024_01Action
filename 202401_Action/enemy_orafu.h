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

	CEnemyOrafu(int nPriority = mylib_const::ENEMY_PRIORITY);
	~CEnemyOrafu();

	// オーバーライドされた関数
	HRESULT Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;
	void Kill() override;

private:


	//=============================
	// 関数リスト
	//=============================

	//=============================
	// メンバ関数
	//=============================

	// その他関数
	void MotionSet() override;	// モーションの設定
	void AttackAction(CMotion::AttackInfo ATKInfo, int nCntATK) override;		// 攻撃時処理
	void AttackInDicision(CMotion::AttackInfo* pATKInfo, int nCntATK) override;	// 攻撃判定中処理

	//=============================
	// メンバ変数
	//=============================
};



#endif