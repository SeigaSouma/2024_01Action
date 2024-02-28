//=============================================================================
// 
//  練習敵(強攻撃)ヘッダー [enemy_practice_SA.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _ENEMYPRACTICE_SA_H_
#define _ENEMYPRACTICE_SA_H_	// 二重インクルード防止

#include "enemy_practice.h"

//==========================================================================
// クラス定義
//==========================================================================
// 練習敵(攻撃)クラス定義
class CEnemyPractice_SA : public CEnemyPractice
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
		MOTION_DOWN,				// ダウン
		MOTION_RETURNDOWN,			// ダウン
		MOTION_KNOCKBACK,			// やられ
		MOTION_FADEOUT,				// フェードアウト
		MOTION_FLINCH_NORMALATK,	// 通常攻撃怯み
		MOTION_MAX
	};

	CEnemyPractice_SA(int nPriority = mylib_const::ENEMY_PRIORITY);
	~CEnemyPractice_SA();

	// オーバーライドされた関数
	HRESULT Init() override;

private:

};

#endif