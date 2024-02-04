//=============================================================================
// 
// ロードマネージャ [loadmanager.h]
// Author : 相馬靜雅
// 
//=============================================================================

#ifndef _LOADMANAGER_H_
#define _LOADMANAGER_H_		// 二重インクルード防止

#include "scene.h"

class CLoadScreen;

//==========================================================================
// クラス定義
//==========================================================================
// ロードマネージャクラス
class CLoadManager
{
public:

	CLoadManager();
	~CLoadManager();

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	void LoadScene(CScene::MODE mode);
	void UnLoad(void);	// ロード解放
	void ResetLoad();	// ロードリセット
	bool IsLoadComplete();	// ロードが完了フラグ取得

	// 静的関数
	static CLoadManager* Create();

private:

	//=============================
	// メンバ関数
	//=============================
	void LoadInBackground(void);
	void Load();
	void ResetInternalLoad();	// 新しいシーンをセットする前のリセット

	//=============================
	// メンバ変数
	//=============================
	std::thread m_LoadingThread;
	bool m_bLoadComplete;		// ロード完了フラグ
	CScene::MODE m_ModeNext;	// 次のモード
	CLoadScreen* m_pLoadScreen;	// ロード画面
};


#endif