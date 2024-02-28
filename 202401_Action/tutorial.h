//=============================================================================
// 
//  チュートリアルヘッダー [tutorial.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _TUTORIAL_H_
#define _TUTORIAL_H_	// 二重インクルード防止

#include "scene.h"
//#include "tutorialmanager.h"

//==========================================================================
// 前方宣言
//==========================================================================
class CPlayer;
class CEnemyManager;
class CLimitArea;
class CPlayer;
class CEnemyBase;

//==========================================================================
// クラス定義
//==========================================================================
// ゲームクラス定義
class CTutorial : public CScene
{
public:

	// エディットタイプ
	enum EEditType
	{
		EDITTYPE_OFF = 0,			// オフ
		EDITTYPE_ENEMYBASE,			// 敵拠点
		EDITTYPE_MAX
	};

	CTutorial();
	~CTutorial();

	// オーバーライドされた関数
	HRESULT Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;

	void ResetBeforeBoss();
	//CTutorialManager *GetGameManager();
	void SetEnableClear(bool bClear) { m_clear = bClear; }		// クリアのフラグ
	bool IsClearFrag() { return m_clear; }						// クリアフラグの取得
	CEnemyManager* GetEnemyManager();							// 敵マネージャ
	CEnemyBase* GetEnemyBase();									// 敵の拠点
	bool IsEdit() { return m_bEdit; }							// エディットの判定
	float GetRockOnDistance() { return m_fMaxRokOnDistance; }	// ロックオンの最大距離取得

	// 静的関数
	static CTutorial* GetInstance();	// インスタンス取得
	static CTutorial* Create();		// 生成処理

private:

	//=============================
	// メンバ関数
	//=============================
	void EditReset();

	//=============================
	// メンバ変数
	//=============================
	CLimitArea *m_pLimitArea;				// エリア制限のオブジェクト
	//CTutorialManager *m_pGameManager;			// ゲームマネージャのオブジェクト
	CEnemyManager *m_pEnemyManager;			// 敵マネージャのオブジェクト
	CEnemyBase *m_pEnemyBase;				// 敵の拠点
	bool m_bEdit;				// エディットの判定
	bool m_clear; // クリア判定
	float m_fMaxRokOnDistance;			// ロックオンの最大距離

	static CTutorial* m_pThisPtr;	// 自身のポインタ
};



#endif