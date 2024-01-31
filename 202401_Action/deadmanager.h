//=============================================================================
// 
// 死亡マネージャ処理 [deadmanager.h]
// Author : 相馬靜雅
// 
//=============================================================================

#ifndef _DEAD_MANAGER_H_
#define _DEAD_MANAGER_H_		// 二重インクルード防止のマクロを定義する

#include "object.h"

class CDeadScreen;	// スクリーン
class CDeadText;		// 数字
class CRevivalPercentText;	// リスポーン確率

//==========================================================================
// クラス定義
//==========================================================================
// 死亡マネージャ
class CDeadManager : public CObject
{
public:

	CDeadManager(int nPriority = 8);
	~CDeadManager();
	
	//  オーバーライドされた関数
	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;

	// メンバ関数
	bool IsRespawnPossible() { return m_bRespawnPossible; }	// 復活出来るかのフラグ取得
	void SetFadeOut(void);	// フェードアウト設定

	// 静的関数
	static CDeadManager* GetInstance(void);	// インスタンス取得
	static CDeadManager* Create(void);		// 生成処理

private:

	//=============================
	// メンバ関数
	//=============================
	// その他関数
	bool DrawingRevival(int winningPercent);	// 復活抽選

	//=============================
	// メンバ変数
	//=============================
	float m_fTime;				// カウンター
	bool m_bSetScreen;			// スクリーンのセットフラグ
	bool m_bSetText;			// テキストのセットフラグ
	bool m_bSetRevivalText;		// 復活確率文字のセットフラグ
	bool m_bRespawnPossible;	// 復活できるかのフラグ

	// 生成するオブジェ
	CDeadScreen* m_pScreen;	// スクリーンのオブジェ
	CDeadText* m_pText;		// 数字のオブジェクト
	CRevivalPercentText* m_pRespawnPercent;	// リスポーン確率

	static CDeadManager* m_pThisPtr;	// 自身のポインタ

};

#endif