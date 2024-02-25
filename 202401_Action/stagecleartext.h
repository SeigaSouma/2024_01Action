//=============================================================================
// 
// ステージクリアテキスト処理 [stagecleartext.h]
// Author : 相馬靜雅
// 
//=============================================================================

#ifndef _STAGECLEARTEXT_H_
#define _STAGECLEARTEXT_H_		// 二重インクルード防止のマクロを定義する

#include "main.h"
#include "object2D.h"

//==========================================================================
// クラス定義
//==========================================================================
// カーソルクラス定義
class CStageClearText : public CObject2D
{
public:

	CStageClearText(int nPriority = 8);
	~CStageClearText();

	static CStageClearText *Create(const MyLib::Vector3 pos);
	
	//  オーバーライドされた関数
	HRESULT Init();
	void Uninit();
	void Update();
	void Draw();
	void SetVtx();

private:

	// 状態列挙
	enum eState
	{
		STATE_NONE = 0,		// なにもなし
		STATE_SCALEDOWN,	// 縮小
		STATE_SCALENONE,	// 整い
		STATE_FADEOUT,		// フェードアウト
		STATE_MAX
	};

	// メンバ関数
	void StateNone();		// 何もない状態
	void StateScaleUP();	// 拡大状態
	void StateScaleDOWN();	// 縮小状態
	void StateScaleNone();	// 整い状態
	void StateFadeOut();	// フェードアウト状態

	// メンバ変数
	eState m_state;		// 状態
	float m_fStateTimer;	// 状態タイマー
	bool m_bCreateResultWindow;	// リザルト画面の呼び出しフラグ

	typedef void(CStageClearText::*STATE_FUNC)();	// 状態処理のリスト
	static STATE_FUNC m_StateFuncList[];
};

#endif