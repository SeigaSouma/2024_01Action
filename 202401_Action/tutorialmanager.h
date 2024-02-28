//=============================================================================
// 
//  チュートリアルマネージャヘッダー [tutorialmanager.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _TUTORIALMANAGER_H_
#define _TUTORIALMANAGER_H_	// 二重インクルード防止

#include "gamemanager.h"

//==========================================================================
// クラス定義
//==========================================================================
// ゲームマネージャクラス定義
class CTutorialManager : public CGameManager
{
public:

	CTutorialManager();
	~CTutorialManager();

	HRESULT Init() override;
	void Uninit() override;
	void Update() override;

private:

	// メンバ関数
	void SceneTransition() override;	// 遷移中
	void SceneEnhance() override;
	void SceneReaspawn() override;	// 復活
	void SetEnemy() override;

};



#endif