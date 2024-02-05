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
	typedef void(CAbillityStamina::* STAGE_FUNC)(void);
	static STAGE_FUNC m_StageFunc[];

	//=============================
	// �����o�֐�
	//=============================
	// �i�K��
	void Add01(void) { m_pPlayer->UpgradeMaxStamina(20); }
	void AutoHeal01(void){ m_pPlayer->UpgradeAutoHealStamina(1.5f); }
	void Add02(void) { m_pPlayer->UpgradeMaxStamina(20); }
	void AutoHeal02(void) { m_pPlayer->UpgradeAutoHealStamina(1.5f); }
	void Add03(void) { m_pPlayer->UpgradeMaxStamina(20); }

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
	typedef void(CAbillityLife::* STAGE_FUNC)(void);
	static STAGE_FUNC m_StageFunc[];

	//=============================
	// �����o�֐�
	//=============================
	// �i�K��
	void Add01(void) { m_pPlayer->UpgradeLife(20); }
	void Add02(void) { m_pPlayer->UpgradeLife(20); }

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
	typedef void(CAbillityCounter::* STAGE_FUNC)(void);
	static STAGE_FUNC m_StageFunc[];

	//=============================
	// �����o�֐�
	//=============================
	// �i�K��
	void SubNeedStamina01(void) {}
	void SubNeedStamina02(void) {}
	void SubNeedStamina03(void) {}
	void HealStamina01(void) {}
	void HealStamina02(void) {}

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
	typedef void(CAbillityRespawn::* STAGE_FUNC)(void);
	static STAGE_FUNC m_StageFunc[];

	//=============================
	// �����o�֐�
	//=============================
	// �i�K��
	void AddRespawnPercent(void) {}

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
	typedef void(CAbillityGuard::* STAGE_FUNC)(void);
	static STAGE_FUNC m_StageFunc[];

	//=============================
	// �����o�֐�
	//=============================
	// �i�K��
	void SubNeedStamina01(void) {}
	void SubNeedStamina02(void) {}
	void KnockbackMitigation01(void) {}
	void KnockbackMitigation02(void) {}

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
	typedef void(CUnlockCombo::* STAGE_FUNC)(void);
	static STAGE_FUNC m_StageFunc[];

	//=============================
	// �����o�֐�
	//=============================
	// �i�K��
	void State01(void) {}

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
	typedef void(CUnlockCounter::* STAGE_FUNC)(void);
	static STAGE_FUNC m_StageFunc[];

	//=============================
	// �����o�֐�
	//=============================
	// �i�K��
	void State01(void) {}

};


#endif