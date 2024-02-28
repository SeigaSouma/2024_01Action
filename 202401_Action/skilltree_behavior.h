//=============================================================================
// 
// スキルツリー能力処理 [skilltree_behavior.h]
// Author : 相馬靜雅
// 
//=============================================================================

#ifndef _SKILLTREE_BEHAVIOR_H_
#define _SKILLTREE_BEHAVIOR_H_		// 二重インクルード防止のマクロを定義する

#include "skilltree_icon.h"
#include "player.h"
#include "playercontrol.h"

//==========================================================================
// スキルツリー能力のStrategy
//==========================================================================
class CAbillityStrategy
{
public:

	virtual void  BindAbillity() = 0;	// 能力を付与する純粋仮想関数

	// インスタンス生成
	static CAbillityStrategy* CreateInstance(CSkillTree_Icon::sSkillIcon iconinfo, CPlayer* pPlayer)
	{
		// カテゴリー別生成
		CAbillityStrategy* pAbillity = m_CreateFunc[iconinfo.skillCategory](iconinfo);
		pAbillity->m_nStage = iconinfo.skillStage;	// 段階
		pAbillity->m_pPlayer = pPlayer;

		return pAbillity;
	}

protected:
	int m_nStage;	// 段階
	CPlayer* m_pPlayer;	// 強化するプレイヤーのポインタ

private:

	//=============================
	// カテゴリー関数リスト
	//=============================
	using CREATE_FUNC = std::function<CAbillityStrategy* (CSkillTree_Icon::sSkillIcon)>;
	static std::vector<CREATE_FUNC> m_CreateFunc;
};

//==========================================================================
// 強化系
//==========================================================================
class CEnhance : public CAbillityStrategy
{
public:
	virtual void BindAbillity() = 0;	// 能力を付与する純粋仮想関数

	// インスタンス生成
	static CEnhance* CreateInstance(CSkillTree_Icon::sSkillIcon iconinfo)
	{
		// タイプ別生成
		return m_CreateFunc[iconinfo.skillType](iconinfo);
	}

private:

	//=============================
	// タイプ関数リスト
	//=============================
	using CREATE_FUNC = std::function<CEnhance* (CSkillTree_Icon::sSkillIcon)>;
	static std::vector<CREATE_FUNC> m_CreateFunc;
};


//==========================================================================
// 攻撃強化
//==========================================================================
class CAbillityAttack : public CEnhance
{
	virtual void BindAbillity() override	// 能力を付与する関数
	{
		// 状態別処理
		(this->*(m_StageFunc[m_nStage]))();
	}

	//=============================
	// ステージ関数リスト
	//=============================
	typedef void(CAbillityAttack::* STAGE_FUNC)();
	static STAGE_FUNC m_StageFunc[];

	//=============================
	// メンバ関数
	//=============================
	// 段階別
	void AddPower01() { m_pPlayer->SetAttackMultiply(1.15f); }
	void ShorterChargeTime01() { m_pPlayer->SetChargeTime(0.8f); }
	void ShorterChargeTime02() { m_pPlayer->SetChargeTime(0.7f); }
	void NotFlinch() { m_pPlayer->SetToggleFlinch(false); }
	void AddPower02() { m_pPlayer->SetAttackMultiply(1.3f); }
	void AddPower03() { m_pPlayer->SetAttackMultiply(1.5f); }

};

//==========================================================================
// スタミナ強化
//==========================================================================
class CAbillityStamina : public CEnhance
{
	virtual void BindAbillity() override	// 能力を付与する関数
	{
		// 状態別処理
		(this->*(m_StageFunc[m_nStage]))();
	}

	//=============================
	// ステージ関数リスト
	//=============================
	typedef void(CAbillityStamina::* STAGE_FUNC)();
	static STAGE_FUNC m_StageFunc[];

	//=============================
	// メンバ関数
	//=============================
	// 段階別
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
// ライフ強化
//==========================================================================
class CAbillityLife : public CEnhance
{
	virtual void BindAbillity() override	// 能力を付与する関数
	{
		// 状態別処理
		(this->*(m_StageFunc[m_nStage]))();
	}

	//=============================
	// ステージ関数リスト
	//=============================
	typedef void(CAbillityLife::* STAGE_FUNC)();
	static STAGE_FUNC m_StageFunc[];

	//=============================
	// メンバ関数
	//=============================
	// 段階別
	void Add01() { m_pPlayer->UpgradeLife(20); }
	void Add02() { m_pPlayer->UpgradeLife(40); }
	void Reapawn01() { m_pPlayer->SetRespawnHeal(0.4f); }
	void Reapawn02() { m_pPlayer->SetRespawnHeal(0.5f); }
	void Reapawn03() { m_pPlayer->SetRespawnHeal(0.8f); }

};

//==========================================================================
// カウンター強化
//==========================================================================
class CAbillityCounter : public CEnhance
{
	virtual void BindAbillity() override	// 能力を付与する関数
	{
		// 状態別処理
		(this->*(m_StageFunc[m_nStage]))();
	}

	//=============================
	// ステージ関数リスト
	//=============================
	typedef void(CAbillityCounter::* STAGE_FUNC)();
	static STAGE_FUNC m_StageFunc[];

	//=============================
	// メンバ関数
	//=============================
	// 段階別
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
// リスポーン強化
//==========================================================================
class CAbillityRespawn : public CEnhance
{
	virtual void BindAbillity() override	// 能力を付与する関数
	{
		// 状態別処理
		(this->*(m_StageFunc[m_nStage]))();
	}

	//=============================
	// ステージ関数リスト
	//=============================
	typedef void(CAbillityRespawn::* STAGE_FUNC)();
	static STAGE_FUNC m_StageFunc[];

	//=============================
	// メンバ関数
	//=============================
	// 段階別
	void AddRespawnPercent() { m_pPlayer->AddRespawnPercent(5); }

};


//==========================================================================
// ガード強化
//==========================================================================
class CAbillityGuard : public CEnhance
{
	virtual void BindAbillity() override	// 能力を付与する関数
	{
		// 状態別処理
		(this->*(m_StageFunc[m_nStage]))();
	}

	//=============================
	// ステージ関数リスト
	//=============================
	typedef void(CAbillityGuard::* STAGE_FUNC)();
	static STAGE_FUNC m_StageFunc[];

	//=============================
	// メンバ関数
	//=============================
	// 段階別
	void SubNeedStamina01() { m_pPlayer->SetGuardSubValue(45.0f); }
	void SubNeedStamina02() { m_pPlayer->SetGuardSubValue(30.0f); }
	void KnockbackMitigation01() { m_pPlayer->ChangeGuardGrade(DEBUG_NEW CPlayerGuard_Level1()); }
	void KnockbackMitigation02() { m_pPlayer->ChangeGuardGrade(DEBUG_NEW CPlayerGuard_Level2()); }
	void DamageMitigation01() { m_pPlayer->SetGuardMitigation(1.15f); }
	void DamageMitigation02() { m_pPlayer->SetGuardMitigation(1.3f); }
	void DamageMitigation03() { m_pPlayer->SetGuardMitigation(1.5f); }

};




//==========================================================================
// アンロック系
//==========================================================================
class CUnlock : public CAbillityStrategy
{
public:
	virtual void BindAbillity() = 0;	// 能力を付与する純粋仮想関数

	// インスタンス生成
	static CUnlock* CreateInstance(CSkillTree_Icon::sSkillIcon iconinfo)
	{
		// タイプ別生成
		return m_CreateFunc[iconinfo.skillType](iconinfo);
	}

private:

	//=============================
	// タイプ関数リスト
	//=============================
	using CREATE_FUNC = std::function<CUnlock* (CSkillTree_Icon::sSkillIcon)>;
	static std::vector<CREATE_FUNC> m_CreateFunc;
};


//==========================================================================
// コンボ開放
//==========================================================================
class CUnlockCombo : public CUnlock
{
	virtual void BindAbillity() override	// 能力を付与する関数
	{
		// 状態別処理
		(this->*(m_StageFunc[m_nStage]))();
	}

	//=============================
	// ステージ関数リスト
	//=============================
	typedef void(CUnlockCombo::* STAGE_FUNC)();
	static STAGE_FUNC m_StageFunc[];

	//=============================
	// メンバ関数
	//=============================
	// 段階別
	void State01() { m_pPlayer->ChangeAtkControl(DEBUG_NEW CPlayerControlAttack_Level1()); }

};

//==========================================================================
// 反撃開放
//==========================================================================
class CUnlockCounter : public CUnlock
{
	virtual void BindAbillity() override	// 能力を付与する関数
	{
		// 状態別処理
		(this->*(m_StageFunc[m_nStage]))();
	}

	//=============================
	// ステージ関数リスト
	//=============================
	typedef void(CUnlockCounter::* STAGE_FUNC)();
	static STAGE_FUNC m_StageFunc[];

	//=============================
	// メンバ関数
	//=============================
	// 段階別
	void State01() { m_pPlayer->ChangeDefenceControl(DEBUG_NEW CPlayerControlDefence_Level1()); }

};


#endif