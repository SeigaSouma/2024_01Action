//=============================================================================
// 
//  フェードヘッダー [loadscreen.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _LOADSCREEN_H_
#define _LOADSCREEN_H_	// 二重インクルード防止

#include "main.h"
#include "object2D.h"
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
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	CObject2D *GetMyObject(void);
	static CLoadScreen *Create(void);

private:

	CObject2D *m_aObject2D;		// オブジェクト2Dのオブジェクト
};

#endif