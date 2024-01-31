//=============================================================================
// 
//  ゲームマネージャヘッダー [gamemanager.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _GAMEMANAGER_H_
#define _GAMEMANAGER_H_	// 二重インクルード防止

#include "constans.h"

class CSkillTree_Ability;

//==========================================================================
// クラス定義
//==========================================================================
// ゲームマネージャクラス定義
class CGameManager
{
public:

	// シーンの種類
	enum SceneType
	{
		SCENE_MAIN = 0,			// メイン
		SCENE_MAINCLEAR,		// メインクリア
		SCENE_ENHANCE,			// 強化
		SCENE_SKILLTREE,		// スキルツリー
		SCENE_BOSS,				// ボス
		SCENE_TRANSITIONWAIT,	// 遷移待機
		SCENE_TRANSITION,		// 遷移
		SCENE_MAX
	};

	enum AttackType
	{
		ATTACK_NORMAL = 0,	// 通常攻撃
		ATTACK_STRONG,		// 強攻撃
		ATTACK_COUNTER,		// 反撃攻撃
		ATTACK_MAX
	};

	CGameManager();
	~CGameManager();

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);

	void SetType(SceneType type);	// シーンの種類設定
	SceneType GetType(void);		// シーンの種類取得
	void SetEnableRush(void) { m_bEndRush = true; }		// ラッシュの終了フラグを有効にする
	bool IsRushEnd(void) { return m_bEndRush; }			// 終了のフラグ取得
	void SetEnableControll(bool bControll) { m_bControll = bControll; }		// 操作フラグを切り替えする
	bool IsControll(void) { return m_bControll; }				// 操作のフラグ取得
	bool IsEndNormalStage(void) { return m_bEndNormalStage; }	// 通常ステージのア終了判定
	void AddNowStage(void);	// ステージの加算
	int GetNowStage(void);	// 現在のステージ取得
	void SetNumStage(int nStage) { m_nNumStage = nStage; }	// ステージの総数設定
	int GetNumStage(void) { return m_nNumStage; }			// ステージの総数取得

	void GameClearSettings(void);	// ゲームクリア時の設定
	static CGameManager *Create(void);	// 生成処理

private:

	// メンバ関数
	void SceneEnhance(void);
	void SetEnemy(void);
	void SetBoss(void);

	// メンバ変数
	CSkillTree_Ability* m_pSkilltreeAbillity;	// スキルツリー能力のポインタ
	SceneType m_SceneType;	// シーンの種類
	bool m_bEndRush;		// ラッシュが終了したか
	bool m_bControll;		// 操作できるか
	bool m_bEndNormalStage;	// 通常ステージが終了したか
	int m_nNowStage;		// 現在のステージ
	int m_nNumStage;		// ステージの総数
};



#endif