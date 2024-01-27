//=============================================================================
// 
//  タイトル画面ヘッダー [tutorial_screen.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _TUTORIALSCREEN_H_
#define _TUTORIALSCREEN_H_	// 二重インクルード防止

#include "object.h"

//==========================================================================
// 前方宣言
//==========================================================================
class CObject2D;

//==========================================================================
// クラス定義
//==========================================================================
// 背景クラス定義
class CTutorialScreen : public CObject
{
public:

	CTutorialScreen(int nPriority = 8);
	~CTutorialScreen();

	static CTutorialScreen *Create(void);

	// オーバーライドされた関数
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

private:
	// 列挙型定義
	enum VTX
	{
		VTX_BLACK = 0,	// 黒
		VTX_LOGO_MAX
	};


	// メンバ変数
	int m_nTexIdx[VTX_LOGO_MAX];						// テクスチャのインデックス番号
	CObject2D *m_pObj2D[VTX_LOGO_MAX];					// オブジェクト2Dのオブジェクト
	static const char *m_apTextureFile[VTX_LOGO_MAX];	// テクスチャのファイル
	bool m_bStick;		// スティックの判定
};



#endif