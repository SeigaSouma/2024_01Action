//=============================================================================
// 
// リスポーン後テキスト処理 [afterrespawn_text.h]
// Author : 相馬靜雅
// 
//=============================================================================

#ifndef _AFTERRESPAWN_TEXT_H_
#define _AFTERRESPAWN_TEXT_H_		// 二重インクルード防止のマクロを定義する

#include "object2D.h"

//==========================================================================
// クラス定義
//==========================================================================
// リスポーン後テキストクラス
class CAfterRespawn_Text : public CObject
{
public:

	//=============================
	// 列挙型定義
	//=============================
	enum TEXTTYPE
	{
		TEXTTYPE_RESET = 0,		// 強化リセット
		TEXTTYPE_ADDPOINT,		// 情けポイント
		TEXTTYPE_MAX
	};

	CAfterRespawn_Text(int nPriority = 0);
	~CAfterRespawn_Text();

	//  オーバーライド関数
	HRESULT Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override {}

	void SetDefaultText();			// デフォルトテキスト設定
	void SetText(TEXTTYPE text);	// テキスト設定
	void ResetText();	// テキストリセット

	static CAfterRespawn_Text* Create();
	static CAfterRespawn_Text* GetInstance() { return m_pThisPtr; }
private:

	//=============================
	// 列挙型定義
	//=============================
	enum STATE
	{
		STATE_NONE = 0,
		STATE_APPEARANCE,	// 登場
		STATE_FADEOUT,	// フェードアウト
		STATE_MAX
	};

	//=============================
	// 関数リスト
	//=============================
	typedef void(CAfterRespawn_Text::* STATE_FUNC)();	// 状態処理のリスト
	static STATE_FUNC m_StateFuncList[];

	//=============================
	// メンバ関数
	//=============================
	// 状態関数
	void StateNone() {}	// 通常
	void StateAppearance();	// 登場
	void StateFadeOut();	// フェードアウト

	//=============================
	// メンバ変数
	//=============================
	int m_nNumDisp;	// 描画数
	STATE m_state;			// 状態
	float m_fStateTimer;	// 状態タイマー
	CObject2D* m_pText[TEXTTYPE::TEXTTYPE_MAX];	// テキストのオブジェクト
	static CAfterRespawn_Text* m_pThisPtr;		// 自身のポインタ
};
#endif