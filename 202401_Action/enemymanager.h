//=============================================================================
// 
//  敵のマネージャヘッダー [enemymanager.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _ENEMYMANAGER_H_
#define _ENEMYMANAGER_H_	// 二重インクルード防止

#include "main.h"
#include "constans.h"

//==========================================================================
// 前方宣言
//==========================================================================
class CEnemy;
class CEnemyBoss;

//==========================================================================
// クラス定義
//==========================================================================
// 敵のマネージャクラス定義
class CEnemyManager
{
public:

	// 構造体定義
	struct EnemyData
	{
		int nType;	// キャラクター種類
		int nStartFrame;	// 初期フレーム
		MyLib::Vector3 pos;		// 位置
	};

	struct Pattern
	{
		int nNumEnemy;	// 敵の数
		int nFixedType;	// 一定の動きの種類
		EnemyData EnemyData[mylib_const::MAX_PATTEN_ENEMY];
	};

	// 列挙型定義
	enum STATE
	{
		STATE_NONE = 0,		// 何もない状態
		STATE_MAX
	};

	CEnemyManager();
	~CEnemyManager();

	HRESULT Init();
	void Uninit();
	void Update();

	static CEnemyManager *Create(const std::string pTextFile);
	HRESULT ReadText(const std::string pTextFile);	// 外部ファイル読み込み処理
	void SetStageEnemy();	// ステージ毎の敵配置
	void SetStageBoss();	// ボスステージの敵配置
	CEnemy **SetEnemy(MyLib::Vector3 pos, MyLib::Vector3 rot, int nPattern);	// 敵配置
	int GetPatternNum();

	bool IsChangeStage() { return m_bChangeStage; }	// ステージ変更中か
	void SetEnableChangeStage(bool bChange) { m_bChangeStage = bChange; }	// ステージ変更の状態切り替え
	STATE GetState() { return m_state; }	// 状態取得
	Pattern GetPattern(int nPattern);	// パターン取得
	CEnemyBoss *GetBoss();		// ボス取得
	const char *GetMotionFilename(int nType);
protected:


private:

	CEnemyBoss *m_pBoss;											// ボス
	Pattern m_aPattern[mylib_const::MAX_PATTEN_ENEMY];			// 配置の種類
	std::string sMotionFileName[mylib_const::MAX_PATTEN_ENEMY];	// モーションファイル名
	STATE m_state;			// 状態
	int m_nPatternNum;		// 出現パターン数
	int m_nNumChara;		// 敵の種類の総数
	bool m_bChangeStage;	// ステージ変更中か
};



#endif