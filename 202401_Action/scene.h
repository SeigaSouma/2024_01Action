//=============================================================================
// 
//  シーンヘッダー [scene.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _SCENE_H_
#define _SCENE_H_	// 二重インクルード防止

#include "elevation.h"

//==========================================================================
// 前方宣言
//==========================================================================
class CCamera;

//==========================================================================
// クラス定義
//==========================================================================
// シーンクラス定義
class CScene
{
public:
	// モードの列挙型定義
	enum MODE
	{
		MODE_NONE = 0,		// なし
		MODE_TITLE,			// タイトル
		MODE_TUTORIAL,		// チュートリアル
		MODE_GAME,			// ゲーム
		MODE_RESULT,		// リザルト
		MODE_RANKING,		// ランキング
		MODE_MAX
	};

	CScene();
	~CScene();

	// オーバーライドする関数
	virtual HRESULT Init();
	virtual void Uninit();
	virtual void Update();
	virtual void Draw();

	void ResetScene();	// シーンのリセット
	static CScene *Create(MODE mode);
	static CElevation *GetElevation();	// オブジェクト3Dの取得
	MODE GetMode();						// 現在のモード取得

protected:

private:

	MODE m_mode;				// 現在のモード
	static CElevation *m_pObject3DMesh;				// オブジェクト3Dメッシュのオブジェクト
};



#endif