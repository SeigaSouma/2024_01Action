//=============================================================================
// 
//  フェードヘッダー [loadscreen.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _LOADSCREEN_H_
#define _LOADSCREEN_H_	// 二重インクルード防止

#include "main.h"
#include "object2D_Anim.h"
#include "scene.h"

//==========================================================================
// 前方宣言
//==========================================================================

//==========================================================================
// クラス定義
//==========================================================================
// フェードクラス定義
class CLoadScreen
{
public:

	CLoadScreen();
	~CLoadScreen();

	// メンバ関数
	HRESULT Init();
	void Uninit();
	void Update();
	void Draw();
	void Kill();

	CObject2D_Anim*GetMyObject();
	static CLoadScreen *Create();

private:

	CObject2D_Anim* m_aObject2D;		// オブジェクト2Dのオブジェクト
};

#endif