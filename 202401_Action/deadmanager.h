//=============================================================================
// 
// 死亡マネージャ処理 [deadmanager.h]
// Author : 相馬靜雅
// 
//=============================================================================

#ifndef _DEAD_MANAGER_H_
#define _DEAD_MANAGER_H_		// 二重インクルード防止のマクロを定義する

#include "object.h"

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

	static CDeadManager* GetInstance(void);	// インスタンス取得
	static CDeadManager* Create(void);		// 生成処理

private:

	//=============================
	// メンバ関数
	//=============================

	//=============================
	// メンバ変数
	//=============================
	float m_fTime;		// カウンター
	bool m_bSetScreen;	// スクリーンのセットフラグ
	bool m_bSetText;	// テキストのセットフラグ
	bool m_bSetRevivalText;	// 復活確率文字のセットフラグ
	static CDeadManager* m_pThisPtr;	// 自身のポインタ

};

#endif