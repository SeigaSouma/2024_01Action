//=============================================================================
// 
//  ゲームマネージャヘッダー [gamemanager.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _GAMEMANAGER_H_
#define _GAMEMANAGER_H_	// 二重インクルード防止

#include "constans.h"
#include "skilltree_icon.h"

class CSkillTree_Ability;
class CGameRating;

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
		SCENE_MAINRESULT,		// メインリザルト
		SCENE_DURING_MAINRESULT,		// メインリザルト中
		SCENE_MAINCLEAR,		// メインクリア
		SCENE_BEFOREBATTLE,		// 戦闘準備
		SCENE_BATTLESTART,		// 戦闘開始
		SCENE_ENHANCE,			// 強化
		SCENE_SKILLTREE,		// スキルツリー
		SCENE_BOSS,				// ボス
		SCENE_TRANSITIONWAIT,	// 遷移待機
		SCENE_TRANSITION,		// 転移
		SCENE_REASPAWN,			// 復活
		SCENE_RESULT,			// リザルト
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

	HRESULT Init();
	void Uninit();
	void Update();

	void SetType(SceneType type);	// シーンの種類設定
	SceneType GetType();		// シーンの種類取得
	void SetEnableRush() { m_bEndRush = true; }		// ラッシュの終了フラグを有効にする
	bool IsRushEnd() { return m_bEndRush; }			// 終了のフラグ取得
	void SetEnableControll(bool bControll) { m_bControll = bControll; }		// 操作フラグを切り替えする
	bool IsControll() { return m_bControll; }				// 操作のフラグ取得
	bool IsEndNormalStage() { return m_bEndNormalStage; }	// 通常ステージのア終了判定
	void AddNowStage();	// ステージの加算
	int GetNowStage();	// 現在のステージ取得
	void SetNumStage(int nStage);				// ステージの総数設定
	int GetNumStage() { return m_nNumStage; }	// ステージの総数取得

	void SetPrevEnhance();	// 前回の強化内容設定

	CGameRating* GetGameRating();	// ゲーム評価取得

	void GameClearSettings();	// ゲームクリア時の設定
	void GameResultSettings();	// ゲームリザルトの設定
	static CGameManager *Create();	// 生成処理

private:

	// メンバ関数
	void SceneTransition();	// 遷移中
	void SceneEnhance();
	void SceneReaspawn();	// 復活
	void SetEnemy();
	void SetBoss();

	void UpdateGalleryVolume();	// 観客音量更新

	// メンバ変数
	CSkillTree_Ability* m_pSkilltreeAbillity;	// スキルツリー能力のポインタ
	SceneType m_SceneType;	// シーンの種類
	bool m_bEndRush;		// ラッシュが終了したか
	bool m_bControll;		// 操作できるか
	bool m_bEndNormalStage;	// 通常ステージが終了したか
	bool m_bGameStart;		// ゲーム開始時のフラグ
	int m_nNowStage;		// 現在のステージ
	int m_nNumStage;		// ステージの総数
	int m_nPrevPoint;		// 前回のポイント
	std::vector<CSkillTree_Icon::eMastering> m_PrevSkillIconMastering;	// 前回のスキルアイコンの習得状況
	std::vector<CSkillTree_Icon*> m_p_PrevSkillIcon;	// 前回のスキルアイコン
	std::vector<CGameRating*> m_pGameRating;			// ゲーム評価
};



#endif