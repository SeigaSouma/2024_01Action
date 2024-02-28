//=============================================================================
// 
//  ûKG(U)wb_[ [enemy_practice_attack.h]
//  Author : nèÎë
// 
//=============================================================================

#ifndef _ENEMYPRACTICE_ATTACK_H_
#define _ENEMYPRACTICE_ATTACK_H_	// ñdCN[hh~

#include "enemy_practice.h"

//==========================================================================
// NXè`
//==========================================================================
// ûKG(U)NXè`
class CEnemyPractice_Attack : public CEnemyPractice
{
public:

	//=============================
	// ñ^è`
	//=============================
	//[Vñ
	enum MOTION
	{
		MOTION_DEF = 0,			// j[g
		MOTION_WALK,			// Ú®
		MOTION_ATTACK_NORMAL,	// ÊíU
		MOTION_ATTACK_STRONG,	// ­U
		MOTION_DMG,				// _[W
		MOTION_DOWN,				// _E
		MOTION_RETURNDOWN,			// _E
		MOTION_KNOCKBACK,			// âçê
		MOTION_FADEOUT,				// tF[hAEg
		MOTION_FLINCH_NORMALATK,	// ÊíU¯Ý
		MOTION_MAX
	};

	CEnemyPractice_Attack(int nPriority = mylib_const::ENEMY_PRIORITY);
	~CEnemyPractice_Attack();

	// I[o[Ch³ê½Ö
	HRESULT Init() override;

private:

};

#endif