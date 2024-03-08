//=============================================================================
// 
// �X�L���c���[�\�͏��� [skilltree_behavior.cpp]
// Author : ���n�Ή�
// 
//=============================================================================
#include "skilltree_behavior.h"
#include "renderer.h"
#include "texture.h"
#include "manager.h"
#include "calculation.h"


//=============================
// �J�e�S���[�֐����X�g
//=============================
std::vector<CAbillityStrategy::CREATE_FUNC> CAbillityStrategy::m_CreateFunc =
{
	[](CSkillTree_Icon::sSkillIcon iconinfo) {return CEnhance::CreateInstance(iconinfo); },
	[](CSkillTree_Icon::sSkillIcon iconinfo) {return CUnlock::CreateInstance(iconinfo); } ,
};

//=============================
// �����n�̊֐����X�g
//=============================
std::vector<CEnhance::CREATE_FUNC> CEnhance::m_CreateFunc =
{
	[](CSkillTree_Icon::sSkillIcon iconinfo) { return DEBUG_NEW CAbillityStamina(); },
	[](CSkillTree_Icon::sSkillIcon iconinfo) { return DEBUG_NEW CAbillityLife(); },
	[](CSkillTree_Icon::sSkillIcon iconinfo) { return DEBUG_NEW CAbillityCounter(); },
	[](CSkillTree_Icon::sSkillIcon iconinfo) { return DEBUG_NEW CAbillityRespawn(); },
	[](CSkillTree_Icon::sSkillIcon iconinfo) { return DEBUG_NEW CAbillityGuard(); },
	[](CSkillTree_Icon::sSkillIcon iconinfo) { return DEBUG_NEW CAbillityAttack(); },
};

//=============================
// �U���̃X�e�[�W
//=============================
CAbillityAttack::STAGE_FUNC CAbillityAttack::m_StageFunc[] =
{
	&CAbillityAttack::AddPower01,
	&CAbillityAttack::ShorterChargeTime01,
	&CAbillityAttack::ShorterChargeTime02,
	&CAbillityAttack::NotFlinch,
	&CAbillityAttack::AddPower02,
	&CAbillityAttack::AddPower03,
};

//=============================
// �X�^�~�i�̃X�e�[�W
//=============================
CAbillityStamina::STAGE_FUNC CAbillityStamina::m_StageFunc[] =
{
	&CAbillityStamina::Add01,
	&CAbillityStamina::AutoHeal01,
	&CAbillityStamina::Add02,
	&CAbillityStamina::AutoHeal02,
	&CAbillityStamina::Add03,
	&CAbillityStamina::CounterHeal01,
	&CAbillityStamina::CounterHeal02,
	&CAbillityStamina::SubGuard01,
	&CAbillityStamina::SubGuard02,
};

//=============================
// ���C�t�̃X�e�[�W
//=============================
CAbillityLife::STAGE_FUNC CAbillityLife::m_StageFunc[] =
{
	&CAbillityLife::Add01,
	&CAbillityLife::Add02,
	&CAbillityLife::Reapawn01,
	&CAbillityLife::Reapawn02,
	&CAbillityLife::Reapawn03,
};

//=============================
// �����̃X�e�[�W
//=============================
CAbillityCounter::STAGE_FUNC CAbillityCounter::m_StageFunc[] =
{
	&CAbillityCounter::SubNeedStamina01,
	&CAbillityCounter::HealStamina01,
	&CAbillityCounter::SubNeedStamina02,
	&CAbillityCounter::HealStamina02,
	&CAbillityCounter::SubNeedStamina03,
	&CAbillityCounter::ExtensionFrame01,
	&CAbillityCounter::ExtensionFrame02,
	&CAbillityCounter::LongerDown01,
	&CAbillityCounter::LongerDown02,
};

//=============================
// ���X�|�[���̃X�e�[�W
//=============================
CAbillityRespawn::STAGE_FUNC CAbillityRespawn::m_StageFunc[] =
{
	&CAbillityRespawn::AddRespawnPercent,	// ���X�|�[���m�����Z
};

//=============================
// �K�[�h�̃X�e�[�W
//=============================
CAbillityGuard::STAGE_FUNC CAbillityGuard::m_StageFunc[] =
{
	&CAbillityGuard::SubNeedStamina01,
	&CAbillityGuard::SubNeedStamina02,
	&CAbillityGuard::KnockbackMitigation01,
	&CAbillityGuard::DamageMitigation01,
	&CAbillityGuard::DamageMitigation02,
	&CAbillityGuard::DamageMitigation03,
	&CAbillityGuard::KnockbackMitigation02,
};




//=============================
// �A�����b�N�n�̊֐����X�g
//=============================
std::vector<CUnlock::CREATE_FUNC> CUnlock::m_CreateFunc =
{
	[](CSkillTree_Icon::sSkillIcon iconinfo) { return DEBUG_NEW CUnlockCombo(); },
	[](CSkillTree_Icon::sSkillIcon iconinfo) { return DEBUG_NEW CUnlockCounter(); },
};

//=============================
// �X�e�[�W�֐����X�g�̏�����[�R���{�J��]
//=============================
CUnlockCombo::STAGE_FUNC CUnlockCombo::m_StageFunc[] =
{
	&CUnlockCombo::State01,
};

//=============================
// �X�e�[�W�֐����X�g�̏�����[�����J��]
//=============================
CUnlockCounter::STAGE_FUNC CUnlockCounter::m_StageFunc[] =
{
	&CUnlockCounter::State01,
};