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
	typedef void(CAbillityStamina::* STAGE_FUNC)(void);
	static STAGE_FUNC m_StageFunc[];

	//=============================
	// メンバ関数
	//=============================
	// 段階別
	void Add01(void) { m_pPlayer->UpgradeMaxStamina(20); }
	void AutoHeal01(void){ m_pPlayer->UpgradeAutoHealStamina(1.3f); }
	void Add02(void) { m_pPlayer->UpgradeMaxStamina(40); }
	void AutoHeal02(void) { m_pPlayer->UpgradeAutoHealStamina(1.75f); }
	void Add03(void) { m_pPlayer->UpgradeMaxStamina(60); }

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
	typedef void(CAbillityLife::* STAGE_FUNC)(void);
	static STAGE_FUNC m_StageFunc[];

	//=============================
	// メンバ関数
	//=============================
	// 段階別
	void Add01(void) { m_pPlayer->UpgradeLife(20); }
	void Add02(void) { m_pPlayer->UpgradeLife(40); }

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
	typedef void(CAbillityCounter::* STAGE_FUNC)(void);
	static STAGE_FUNC m_StageFunc[];

	//=============================
	// メンバ関数
	//=============================
	// 段階別
	void SubNeedStamina01(void) { m_pPlayer->SetCounterSubValue(30.0f); }
	void HealStamina01(void) { m_pPlayer->SetCounterHealValue(10.0f); }
	void SubNeedStamina02(void) { m_pPlayer->SetCounterSubValue(20.0f); }
	void HealStamina02(void) { m_pPlayer->SetCounterHealValue(20.0f); }
	void SubNeedStamina03(void) { m_pPlayer->SetCounterSubValue(10.0f); }

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
	typedef void(CAbillityRespawn::* STAGE_FUNC)(void);
	static STAGE_FUNC m_StageFunc[];

	//=============================
	// メンバ関数
	//=============================
	// 段階別
	void AddRespawnPercent(void) {}

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
	typedef void(CAbillityGuard::* STAGE_FUNC)(void);
	static STAGE_FUNC m_StageFunc[];

	//=============================
	// メンバ関数
	//=============================
	// 段階別
	void SubNeedStamina01(void) { m_pPlayer->SetGuardSubValue(30.0f); }
	void SubNeedStamina02(void) { m_pPlayer->SetGuardSubValue(15.0f); }
	void KnockbackMitigation01(void) { m_pPlayer->ChangeGuardGrade(DEBUG_NEW CPlayerGuard_Level1()); }
	void KnockbackMitigation02(void) { m_pPlayer->ChangeGuardGrade(DEBUG_NEW CPlayerGuard_Level2()); }

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
	typedef void(CUnlockCombo::* STAGE_FUNC)(void);
	static STAGE_FUNC m_StageFunc[];

	//=============================
	// メンバ関数
	//=============================
	// 段階別
	void State01(void) {}

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
	typedef void(CUnlockCounter::* STAGE_FUNC)(void);
	static STAGE_FUNC m_StageFunc[];

	//=============================
	// メンバ関数
	//=============================
	// 段階別
	void State01(void) { m_pPlayer->ChangeDefenceControl(DEBUG_NEW CPlayerControlDefence_Level1()); }

};


#endif