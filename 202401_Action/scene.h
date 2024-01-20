//=============================================================================
// 
//  シーンヘッダー [scene.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _SCENE_H_
#define _SCENE_H_	// 二重インクルード防止

#include "main.h"
#include "elevation.h"

//==========================================================================
// 前方宣言
//==========================================================================
class CXLoad;
class CPlayerUnion;
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
	virtual HRESULT Init(void);
	virtual void Uninit(void);
	virtual void Update(void);
	virtual void Draw(void);

	void ResetScene(void);	// シーンのリセット
	static CScene *Create(MODE mode);
	static CXLoad *GetXLoad(void);			// Xファイルのデータ取得
	static CElevation *GetElevation(void);	// オブジェクト3Dの取得
	MODE GetMode(void);						// 現在のモード取得

protected:

private:

	MODE m_mode;				// 現在のモード
	static CXLoad *m_pXLoad;						// Xファイルのオブジェクト
	static CElevation *m_pObject3DMesh;				// オブジェクト3Dメッシュのオブジェクト
};



#endif