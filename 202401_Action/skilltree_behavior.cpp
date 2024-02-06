//=============================================================================
// 
// スキルツリー能力処理 [skilltree_behavior.cpp]
// Author : 相馬靜雅
// 
//=============================================================================
#include "skilltree_behavior.h"
#include "renderer.h"
#include "texture.h"
#include "manager.h"
#include "calculation.h"


//=============================
// カテゴリー関数リストの初期化
//=============================
std::vector<CAbillityStrategy::CREATE_FUNC> CAbillityStrategy::m_CreateFunc =
{
	[](CSkillTree_Icon::sSkillIcon iconinfo) {return CEnhance::CreateInstance(iconinfo); },
	[](CSkillTree_Icon::sSkillIcon iconinfo) {return CUnlock::CreateInstance(iconinfo); } ,
};

//=============================
// 強化系の関数リスト
//=============================
std::vector<CEnhance::CREATE_FUNC> CEnhance::m_CreateFunc =
{
	[](CSkillTree_Icon::sSkillIcon iconinfo) { return DEBUG_NEW CAbillityStamina(); },
	[](CSkillTree_Icon::sSkillIcon iconinfo) { return DEBUG_NEW CAbillityLife(); },
	[](CSkillTree_Icon::sSkillIcon iconinfo) { return DEBUG_NEW CAbillityCounter(); },
	[](CSkillTree_Icon::sSkillIcon iconinfo) { return DEBUG_NEW CAbillityRespawn(); },
	[](CSkillTree_Icon::sSkillIcon iconinfo) { return DEBUG_NEW CAbillityGuard(); },
};

//=============================
// スタミナのステージ
//=============================
CAbillityStamina::STAGE_FUNC CAbillityStamina::m_StageFunc[] =
{
	&CAbillityStamina::Add01,
	&CAbillityStamina::AutoHeal01,
	&CAbillityStamina::Add02,
	&CAbillityStamina::AutoHeal02,
	&CAbillityStamina::Add03,
};

//=============================
// ライフのステージ
//=============================
CAbillityLife::STAGE_FUNC CAbillityLife::m_StageFunc[] =
{
	&CAbillityLife::Add01,
	&CAbillityLife::Add02,
};

//=============================
// 反撃のステージ
//=============================
CAbillityCounter::STAGE_FUNC CAbillityCounter::m_StageFunc[] =
{
	&CAbillityCounter::SubNeedStamina01,
	&CAbillityCounter::HealStamina01,
	&CAbillityCounter::SubNeedStamina02,
	&CAbillityCounter::HealStamina02,
	&CAbillityCounter::SubNeedStamina03,
};

//=============================
// リスポーンのステージ
//=============================
CAbillityRespawn::STAGE_FUNC CAbillityRespawn::m_StageFunc[] =
{
	&CAbillityRespawn::AddRespawnPercent,	// リスポーン確率加算
};

//=============================
// ガードのステージ
//=============================
CAbillityGuard::STAGE_FUNC CAbillityGuard::m_StageFunc[] =
{
	&CAbillityGuard::SubNeedStamina01,
	&CAbillityGuard::SubNeedStamina02,
	&CAbillityGuard::KnockbackMitigation01,
	&CAbillityGuard::KnockbackMitigation02,
};




//=============================
// アンロック系の関数リスト
//=============================
std::vector<CUnlock::CREATE_FUNC> CUnlock::m_CreateFunc =
{
	[](CSkillTree_Icon::sSkillIcon iconinfo) { return DEBUG_NEW CUnlockCombo(); },
	[](CSkillTree_Icon::sSkillIcon iconinfo) { return DEBUG_NEW CUnlockCounter(); },
};

//=============================
// ステージ関数リストの初期化[コンボ開放]
//=============================
CUnlockCombo::STAGE_FUNC CUnlockCombo::m_StageFunc[] =
{
	&CUnlockCombo::State01,
};

//=============================
// ステージ関数リストの初期化[反撃開放]
//=============================
CUnlockCounter::STAGE_FUNC CUnlockCounter::m_StageFunc[] =
{
	&CUnlockCounter::State01,
};