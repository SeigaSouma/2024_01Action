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
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetVtx(void);

private:

	// 状態列挙
	enum eState
	{
		STATE_NONE = 0,		// なにもなし
		STATE_SCALEUP,		// 拡大
		STATE_SCALEDOWN,	// 縮小
		STATE_SCALENONE,	// 整い
		STATE_FADEOUT,		// フェードアウト
		STATE_MAX
	};

	// メンバ関数
	void StateNone(void);		// 何もない状態
	void StateScaleUP(void);	// 拡大状態
	void StateScaleDOWN(void);	// 縮小状態
	void StateScaleNone(void);	// 整い状態
	void StateFadeOut(void);	// フェードアウト状態

	// メンバ変数
	int m_nTexIdx;		// テクスチャのインデックス番号
	int m_nCntState;	// 状態遷移カウンター
	eState m_state;		// 状態

	typedef void(CStageClearText::*STATE_FUNC)(void);	// 状態処理のリスト
	static STATE_FUNC m_StateFuncList[];
};

#endif