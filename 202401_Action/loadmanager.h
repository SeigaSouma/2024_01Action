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

	void LoadScene(void);
	void UnLoad(void);
	void ResetLoad();

	// ロードが完了したかどうかを返す
	bool IsLoadComplete()
	{
		std::lock_guard<std::mutex>  lock(isLoadedMutex);
		return isLoadComplete;
	}

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
	CLoadScreen* m_pLoadScreen;
};


#endif