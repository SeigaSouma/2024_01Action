//=============================================================================
// 
//  ゲームヘッダー [game.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _GAME_H_
#define _GAME_H_	// 二重インクルード防止

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
class CEnemyManager;
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
	virtual HRESULT Init() override;
	virtual void Uninit() override;
	virtual void Update() override;
	virtual void Draw() override;

	void ResetBeforeBoss();
	EEditType GetEditType() { return m_EditType; }
	CScore *GetScore();
	CTimer *GetTimer() { return m_pTimer; }	// タイマー取得
	CStage *GetStage();
	CGameManager *GetGameManager();
	void SetEnableClear(bool bClear) { m_clear = bClear; }		// クリアのフラグ
	bool IsClearFrag() { return m_clear; }						// クリアフラグの取得
	CEnemyManager* GetEnemyManager();							// 敵マネージャ
	CEnemyBase* GetEnemyBase();									// 敵の拠点
	bool IsEdit() { return m_bEdit; }							// エディットの判定
	float GetRockOnDistance() { return m_fMaxRokOnDistance; }	// ロックオンの最大距離取得

	// 静的関数
	static CGame* GetInstance();	// インスタンス取得
	static CGame* Create(CScene::MODE mode);		// 生成処理

protected:

	virtual void InitByMode();	// モード別初期化


	//=============================
	// メンバ関数
	//=============================
	void EditReset();

	//=============================
	// メンバ変数
	//=============================
	CScore *m_pScore;						// スコアのオブジェクト
	CTimer *m_pTimer;						// タイマーのオブジェクト
	CLimitArea *m_pLimitArea;				// エリア制限のオブジェクト
	CEditEnemyBase *m_pEditEnemyBase;		// 敵の拠点エディター
	CStage *m_pStage;						// ステージのオブジェクト
	CGameManager *m_pGameManager;			// ゲームマネージャのオブジェクト
	EEditType m_EditType;					// エディットの種類
	CEnemyManager *m_pEnemyManager;			// 敵マネージャのオブジェクト
	CEnemyBase *m_pEnemyBase;				// 敵の拠点
	bool m_bEdit;				// エディットの判定
	bool m_clear; // クリア判定
	float m_fMaxRokOnDistance;			// ロックオンの最大距離

	static CGame* m_pThisPtr;	// 自身のポインタ
};



#endif