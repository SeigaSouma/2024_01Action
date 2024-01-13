//=============================================================================
// 
//  ゲームヘッダー [game.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _GAME_H_
#define _GAME_H_	// 二重インクルード防止

#include "main.h"
#include "scene.h"
#include "gamemanager.h"

//==========================================================================
// 前方宣言
//==========================================================================
class CPlayer;
class CScore;
class CTimer;
class CPowerGauge;
class CEditEnemyBase;
class CStage;
class CHitScore;
class CStatusWindow;
class CUltWindow;
class CEnemyManager;
class CItemManager;
class CLimitArea;
class CPlayer;
class CEnemyBase;

//==========================================================================
// クラス定義
//==========================================================================
// ゲームクラス定義
class CGame : public CScene
{
public:

	// エディットタイプ
	enum EEditType
	{
		EDITTYPE_OFF = 0,			// オフ
		EDITTYPE_ENEMYBASE,			// 敵拠点
		EDITTYPE_MAX
	};

	CGame();
	~CGame();

	// オーバーライドされた関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	static void Reset(void);
	static EEditType GetEditType(void) { return m_EditType; }
	static CScore *GetScore(void);
	static CTimer *GetTimer(void) { return m_pTimer; }	// タイマー取得
	static CEditEnemyBase *GetEditEnemyBase(void);
	static CItemManager *GetItemManager(void);
	static CStage *GetStage(void);
	static CGameManager *GetGameManager(void);
	static CStatusWindow *GetStatusWindow(int nIdx);
	static CUltWindow *GetUltWindow(int nIdx);		// 必殺のウィンドウ取得
	static void SetEnableClear(bool bClear) { m_clear = bClear; }	// クリアのフラグ
	static bool IsClearFrag(void) { return m_clear; } // クリアフラグの取得

	static CEnemyManager *GetEnemyManager(void);
	static CEnemyBase *GetEnemyBase(void);		// 敵の拠点
	static bool IsEdit(void) { return m_bEdit; }	// エディットの判定

private:

	// メンバ関数
	void EditReset(void);

	// メンバ変数
	static CScore *m_pScore;						// スコアのオブジェクト
	static CTimer *m_pTimer;						// タイマーのオブジェクト
	static CItemManager *m_pItemManager;			// アイテムマネージャのオブジェクト
	static CLimitArea *m_pLimitArea;				// エリア制限のオブジェクト
	static CEditEnemyBase *m_pEditEnemyBase;		// 敵の拠点エディター
	static CStage *m_pStage;						// ステージのオブジェクト
	static CGameManager *m_pGameManager;			// ゲームマネージャのオブジェクト
	static EEditType m_EditType;					// エディットの種類
	static CEnemyManager *m_pEnemyManager;			// 敵マネージャのオブジェクト
	static CEnemyBase *m_pEnemyBase;				// 敵の拠点
	static CStatusWindow *m_pStatusWindow[mylib_const::MAX_PLAYER];	// ステータスウィンドウのオブジェクト
	static CUltWindow *m_pUltWindow[mylib_const::MAX_PLAYER];		// 必殺のウィンドウ
	static bool m_bEdit;				// エディットの判定
	static bool m_clear; // クリア判定

};



#endif