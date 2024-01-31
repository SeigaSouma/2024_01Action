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
};

//=============================
// スタミナのステージ
//=============================
CAbillityStamina::STAGE_FUNC CAbillityStamina::m_StageFunc[] =
{
	&CAbillityStamina::Add01,
	&CAbillityStamina::AutoHeal01,
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
	&CAbillityCounter::SubNeedStamina,
};

//=============================
// リスポーンのステージ
//=============================
CAbillityRespawn::STAGE_FUNC CAbillityRespawn::m_StageFunc[] =
{
	&CAbillityRespawn::AddRespawnPercent,	// リスポーン確率加算
};




//=============================
// アンロック系の関数リスト
//=============================
std::vector<CUnlock::CREATE_FUNC> CUnlock::m_CreateFunc =
{
	[](CSkillTree_Icon::sSkillIcon iconinfo) { return DEBUG_NEW CUnlockCombo(); }
};

//=============================
// ステージ関数リストの初期化
//=============================
CUnlockCombo::STAGE_FUNC CUnlockCombo::m_StageFunc[] =
{
	&CUnlockCombo::State01,
};
