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
	void UnLoad(void);
	void ResetLoad();

	bool IsLoadComplete();	// ロードが完了ふらぐ

	// 静的関数
	static CLoadManager* Create();

private:

	//=============================
	// メンバ関数
	//=============================
	void LoadInBackground(void);
	void Load();

	// 新しいシーンをセットする前にロードをリセットする内部関数
	void ResetInternalLoad();

	//=============================
	// メンバ変数
	//=============================
	std::thread m_LoadingThread;

	std::mutex isLoadedMutex;  // Mutex for isLoadComplete
	bool isLoadComplete = false;
	CScene::MODE m_ModeNext;
	CLoadScreen* m_pLoadScreen;
};


#endif