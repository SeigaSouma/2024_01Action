//=============================================================================
// 
//  プレイヤーコントロールヘッダー [playercontrol.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _PLAYERCONTROL_H_
#define _PLAYERCONTROL_H_	// 二重インクルード防止

#include "player.h"

//==========================================================================
// プレイヤーコントロールクラス定義
//==========================================================================
// 攻撃
class CPlayerControlAttack
{
public:
	virtual void Attack(CPlayer* player);	// 通常攻撃

protected:
	virtual void StageByReset(CPlayer* player)	// 段階毎のリセット処理
	{
		// 段階取得
		int combostage = player->GetComboStage();

		// コンボの段階加算
		combostage++;
		if (combostage > CPlayer::MOTION_ATK3 - CPlayer::MOTION_ATK)
		{
			combostage = 0;
		}
		player->SetComboStage(combostage);
	}
};

class CPlayerControlAttack_Level1 : public CPlayerControlAttack
{
protected:

	// 段階毎のリセット処理
	virtual void StageByReset(CPlayer* player) override
	{
		// 段階取得
		int combostage = player->GetComboStage();

		// コンボの段階加算
		combostage++;
		//if (combostage > CPlayer::MOTION_ATK4 - CPlayer::MOTION_ATK)
		{
			combostage = 0;
		}
		player->SetComboStage(combostage);
	}
};


// 防御
class CPlayerControlDefence
{
public:
	virtual void Defence(CPlayer* player);	// 防御
};

// 防御レベル1
class CPlayerControlDefence_Level1 : public CPlayerControlDefence
{
public:
	virtual void Defence(CPlayer* player) override;	// 防御
};



// 回避
class CPlayerControlAvoid
{
public:
	virtual void Avoid(CPlayer* player);	// 回避
};

#endif