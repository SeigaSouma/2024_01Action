//=============================================================================
// 
// �X�L���c���[�\�͏��� [skilltree_behavior.h]
// Author : ���n�Ή�
// 
//=============================================================================

#ifndef _SKILLTREE_BEHAVIOR_H_
#define _SKILLTREE_BEHAVIOR_H_		// ��d�C���N���[�h�h�~�̃}�N�����`����

#include "skilltree_icon.h"
#include "player.h"
#include "playercontrol.h"

//==========================================================================
// �X�L���c���[�\�͂�Strategy
//==========================================================================
class CAbillityStrategy
{
public:

	virtual void  BindAbillity() = 0;	// �\�͂�t�^���鏃�����z�֐�

	// �C���X�^���X����
	static CAbillityStrategy* CreateInstance(CSkillTree_Icon::sSkillIcon iconinfo, CPlayer* pPlayer)
	{
		// �J�e�S���[�ʐ���
		CAbillityStrategy* pAbillity = m_CreateFunc[iconinfo.skillCategory](iconinfo);
		pAbillity->m_nStage = iconinfo.skillStage;	// �i�K
		pAbillity->m_pPlayer = pPlayer;

		return pAbillity;
	}

protected:
	int m_nStage;	// �i�K
	CPlayer* m_pPlayer;	// ��������v���C���[�̃|�C���^

private:

	//=============================
	// �J�e�S���[�֐����X�g
	//=============================
	using CREATE_FUNC = std::function<CAbillityStrategy* (CSkillTree_Icon::sSkillIcon)>;
	static std::vector<CREATE_FUNC> m_CreateFunc;
};

//==========================================================================
// �����n
//==========================================================================
class CEnhance : public CAbillityStrategy
{
public:
	virtual void BindAbillity() = 0;	// �\�͂�t�^���鏃�����z�֐�

	// �C���X�^���X����
	static CEnhance* CreateInstance(CSkillTree_Icon::sSkillIcon iconinfo)
	{
		// �^�C�v�ʐ���
		return m_CreateFunc[iconinfo.skillType](iconinfo);
	}

private:

	//=============================
	// �^�C�v�֐����X�g
	//=============================
	using CREATE_FUNC = std::function<CEnhance* (CSkillTree_Icon::sSkillIcon)>;
	static std::vector<CREATE_FUNC> m_CreateFunc;
};


//==========================================================================
// �U������
//==========================================================================
class CAbillityAttack : public CEnhance
{
	virtual void BindAbillity() override	// �\�͂�t�^����֐�
	{
		// ��ԕʏ���
		(this->*(m_StageFunc[m_nStage]))();
	}

	//=============================
	// �X�e�[�W�֐����X�g
	//=============================
	typedef void(CAbillityAttack::* STAGE_FUNC)();
	static STAGE_FUNC m_StageFunc[];

	//=============================
	// �����o�֐�
	//=============================
	// �i�K��
	void AddPower01() { m_pPlayer->SetAttackMultiply(1.15f); }
	void ShorterChargeTime01() { m_pPlayer->SetChargeTime(0.8f); }
	void ShorterChargeTime02() { m_pPlayer->SetChargeTime(0.7f); }
	void NotFlinch() { m_pPlayer->SetToggleFlinch(false); }
	void AddPower02() { m_pPlayer->SetAttackMultiply(1.3f); }
	void AddPower03() { m_pPlayer->SetAttackMultiply(1.5f); }

};

//==========================================================================
// �X�^�~�i����
//==========================================================================
class CAbillityStamina : public CEnhance
{
	virtual void BindAbillity() override	// �\�͂�t�^����֐�
	{
		// ��ԕʏ���
		(this->*(m_StageFunc[m_nStage]))();
	}

	//=============================
	// �X�e�[�W�֐����X�g
	//=============================
	typedef void(CAbillityStamina::* STAGE_FUNC)();
	static STAGE_FUNC m_StageFunc[];

	//=============================
	// �����o�֐�
	//=============================
	// �i�K��
	void Add01() { m_pPlayer->UpgradeMaxStamina(20); }
	void AutoHeal01(){ m_pPlayer->UpgradeAutoHealStamina(1.3f); }
	void Add02() { m_pPlayer->UpgradeMaxStamina(40); }
	void AutoHeal02() { m_pPlayer->UpgradeAutoHealStamina(1.75f); }
	void Add03() { m_pPlayer->UpgradeMaxStamina(60); }
	void CounterHeal01() { m_pPlayer->SetCounterHealValue(10.0f); }
	void CounterHeal02() { m_pPlayer->SetCounterHealValue(20.0f); }
	void SubGuard01() { m_pPlayer->SetGuardSubValue(20.0f); }
	void SubGuard02() { m_pPlayer->SetGuardSubValue(10.0f); }

};

//==========================================================================
// ���C�t����
//==========================================================================
class CAbillityLife : public CEnhance
{
	virtual void BindAbillity() override	// �\�͂�t�^����֐�
	{
		// ��ԕʏ���
		(this->*(m_StageFunc[m_nStage]))();
	}

	//=============================
	// �X�e�[�W�֐����X�g
	//=============================
	typedef void(CAbillityLife::* STAGE_FUNC)();
	static STAGE_FUNC m_StageFunc[];

	//=============================
	// �����o�֐�
	//=============================
	// �i�K��
	void Add01() { m_pPlayer->UpgradeLife(20); }
	void Add02() { m_pPlayer->UpgradeLife(40); }
	void Reapawn01() { m_pPlayer->SetRespawnHeal(0.4f); }
	void Reapawn02() { m_pPlayer->SetRespawnHeal(0.5f); }
	void Reapawn03() { m_pPlayer->SetRespawnHeal(0.8f); }

};

//==========================================================================
// �J�E���^�[����
//==========================================================================
class CAbillityCounter : public CEnhance
{
	virtual void BindAbillity() override	// �\�͂�t�^����֐�
	{
		// ��ԕʏ���
		(this->*(m_StageFunc[m_nStage]))();
	}

	//=============================
	// �X�e�[�W�֐����X�g
	//=============================
	typedef void(CAbillityCounter::* STAGE_FUNC)();
	static STAGE_FUNC m_StageFunc[];

	//=============================
	// �����o�֐�
	//=============================
	// �i�K��
	void SubNeedStamina01() { m_pPlayer->SetCounterSubValue(30.0f); }
	void HealStamina01() { m_pPlayer->SetCounterHealValue(10.0f); }
	void SubNeedStamina02() { m_pPlayer->SetCounterSubValue(20.0f); }
	void HealStamina02() { m_pPlayer->SetCounterHealValue(20.0f); }
	void SubNeedStamina03() { m_pPlayer->SetCounterSubValue(10.0f); }
	void ExtensionFrame01() { m_pPlayer->SetCounterExtensionFrame(19); }
	void ExtensionFrame02() { m_pPlayer->SetCounterExtensionFrame(24); }
	void LongerDown01() { m_pPlayer->SetAddDownTime(4.0f); }
	void LongerDown02() { m_pPlayer->SetAddDownTime(5.0f); }

};

//==========================================================================
// ���X�|�[������
//==========================================================================
class CAbillityRespawn : public CEnhance
{
	virtual void BindAbillity() override	// �\�͂�t�^����֐�
	{
		// ��ԕʏ���
		(this->*(m_StageFunc[m_nStage]))();
	}

	//=============================
	// �X�e�[�W�֐����X�g
	//=============================
	typedef void(CAbillityRespawn::* STAGE_FUNC)();
	static STAGE_FUNC m_StageFunc[];

	//=============================
	// �����o�֐�
	//=============================
	// �i�K��
	void AddRespawnPercent() { m_pPlayer->AddRespawnPercent(5); }

};


//==========================================================================
// �K�[�h����
//==========================================================================
class CAbillityGuard : public CEnhance
{
	virtual void BindAbillity() override	// �\�͂�t�^����֐�
	{
		// ��ԕʏ���
		(this->*(m_StageFunc[m_nStage]))();
	}

	//=============================
	// �X�e�[�W�֐����X�g
	//=============================
	typedef void(CAbillityGuard::* STAGE_FUNC)();
	static STAGE_FUNC m_StageFunc[];

	//=============================
	// �����o�֐�
	//=============================
	// �i�K��
	void SubNeedStamina01() { m_pPlayer->SetGuardSubValue(45.0f); }
	void SubNeedStamina02() { m_pPlayer->SetGuardSubValue(30.0f); }
	void KnockbackMitigation01() { m_pPlayer->ChangeGuardGrade(DEBUG_NEW CPlayerGuard_Level1()); }
	void KnockbackMitigation02() { m_pPlayer->ChangeGuardGrade(DEBUG_NEW CPlayerGuard_Level2()); }
	void DamageMitigation01() { m_pPlayer->SetGuardMitigation(1.15f); }
	void DamageMitigation02() { m_pPlayer->SetGuardMitigation(1.3f); }
	void DamageMitigation03() { m_pPlayer->SetGuardMitigation(1.5f); }

};




//==========================================================================
// �A�����b�N�n
//==========================================================================
class CUnlock : public CAbillityStrategy
{
public:
	virtual void BindAbillity() = 0;	// �\�͂�t�^���鏃�����z�֐�

	// �C���X�^���X����
	static CUnlock* CreateInstance(CSkillTree_Icon::sSkillIcon iconinfo)
	{
		// �^�C�v�ʐ���
		return m_CreateFunc[iconinfo.skillType](iconinfo);
	}

private:

	//=============================
	// �^�C�v�֐����X�g
	//=============================
	using CREATE_FUNC = std::function<CUnlock* (CSkillTree_Icon::sSkillIcon)>;
	static std::vector<CREATE_FUNC> m_CreateFunc;
};


//==========================================================================
// �R���{�J��
//==========================================================================
class CUnlockCombo : public CUnlock
{
	virtual void BindAbillity() override	// �\�͂�t�^����֐�
	{
		// ��ԕʏ���
		(this->*(m_StageFunc[m_nStage]))();
	}

	//=============================
	// �X�e�[�W�֐����X�g
	//=============================
	typedef void(CUnlockCombo::* STAGE_FUNC)();
	static STAGE_FUNC m_StageFunc[];

	//=============================
	// �����o�֐�
	//=============================
	// �i�K��
	void State01() { m_pPlayer->ChangeAtkControl(DEBUG_NEW CPlayerControlAttack_Level1()); }

};

//==========================================================================
// �����J��
//==========================================================================
class CUnlockCounter : public CUnlock
{
	virtual void BindAbillity() override	// �\�͂�t�^����֐�
	{
		// ��ԕʏ���
		(this->*(m_StageFunc[m_nStage]))();
	}

	//=============================
	// �X�e�[�W�֐����X�g
	//=============================
	typedef void(CUnlockCounter::* STAGE_FUNC)();
	static STAGE_FUNC m_StageFunc[];

	//=============================
	// �����o�֐�
	//=============================
	// �i�K��
	void State01() { m_pPlayer->ChangeDefenceControl(DEBUG_NEW CPlayerControlDefence_Level1()); }

};


#endif