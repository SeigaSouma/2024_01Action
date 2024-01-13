//=============================================================================
// 
//  タイトル画面ヘッダー [title_screen.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _TITLESCREEN_H_
#define _TITLESCREEN_H_	// 二重インクルード防止

#include "main.h"
#include "object.h"

//==========================================================================
// 前方宣言
//==========================================================================
class CObject2D;

//==========================================================================
// マクロ定義
//==========================================================================

//==========================================================================
// クラス定義
//==========================================================================
// 背景クラス定義
class CTitleScreen : public CObject
{
public:

	CTitleScreen(int nPriority = 8);
	~CTitleScreen();

	static CTitleScreen *Create(void);

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
		VTX_LOGO,		// タイトルロゴ
		VTX_LOGOALPHA1,		// 透明なロゴ
		VTX_LOGOALPHA2,		// 透明なロゴ
		VTX_LOGOPERFECT,	// 完成版のロゴ
		VTX_LOGOPERFECTALPHA,	// 完成版のロゴ
		VTX_LOGO_MAX
	};

	// 列挙型定義
	enum VTXSELECT
	{
		VTXSELECT_SELECTSTART = 0,		// スタート
		VTXSELECT_MAX
	};

	// メンバ関数
	void UpdateLogo(int nCntSelect);	// ロゴの更新処理
	void UpdateSelect(int nCntSelect);	// 選択肢の更新処理
	void UpdateSlash();
	void UpdateSlashAlpha(int nCntSelect);

	// メンバ変数
	int m_nCntAlpha;		// 不透明度のカウンター
	int m_nCntLogo;		// ロゴ移動のカウント
	int m_nTexIdx[VTX_LOGO_MAX];						// テクスチャのインデックス番号
	int m_nTexIdx_Select[VTXSELECT_MAX];						// テクスチャのインデックス番号
	CObject2D *m_pObj2D[VTX_LOGO_MAX];					// オブジェクト2Dのオブジェクト
	CObject2D *m_pSelect2D[VTXSELECT_MAX];				// 選択肢のオブジェクト
	static const char *m_apTextureFile[VTX_LOGO_MAX];	// テクスチャのファイル
	static const char *m_apTextureFile_Select[VTXSELECT_MAX];	// テクスチャのファイル
	bool m_bStick;		// スティックの判定
};



#endif