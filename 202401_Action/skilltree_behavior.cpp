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
// �J�e�S���[�֐����X�g�̏�����
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
	[](CSkillTree_Icon::sSkillIcon iconinfo) { return new CAbillityStamina(); },
	[](CSkillTree_Icon::sSkillIcon iconinfo) { return new CAbillityLife(); },
	[](CSkillTree_Icon::sSkillIcon iconinfo) { return new CAbillityCounter(); },
};

//=============================
// �X�^�~�i�̃X�e�[�W
//=============================
CAbillityStamina::STAGE_FUNC CAbillityStamina::m_StageFunc[] =
{
	&CAbillityStamina::Add01,
	&CAbillityStamina::AutoHeal01,
};

//=============================
// ���C�t�̃X�e�[�W
//=============================
CAbillityLife::STAGE_FUNC CAbillityLife::m_StageFunc[] =
{
	&CAbillityLife::Add01,
	&CAbillityLife::Add02,
};

//=============================
// �����̃X�e�[�W
//=============================
CAbillityCounter::STAGE_FUNC CAbillityCounter::m_StageFunc[] =
{
	&CAbillityCounter::SubNeedStamina,
};



//=============================
// �A�����b�N�n�̊֐����X�g
//=============================
std::vector<CUnlock::CREATE_FUNC> CUnlock::m_CreateFunc =
{
	[](CSkillTree_Icon::sSkillIcon iconinfo) { return new CUnlockCombo(); }
};

//=============================
// �X�e�[�W�֐����X�g�̏�����
//=============================
CUnlockCombo::STAGE_FUNC CUnlockCombo::m_StageFunc[] =
{
	&CUnlockCombo::State01,
};
