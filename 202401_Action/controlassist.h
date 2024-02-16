//=============================================================================
// 
// 操作補助処理 [controlassist.h]
// Author : 相馬靜雅
// 
//=============================================================================

#ifndef _CONTROLASSIST_H_
#define _CONTROLASSIST_H_		// 二重インクルード防止のマクロを定義する

#include "object2D.h"

//==========================================================================
// クラス定義
//==========================================================================
// 操作補助クラス定義
class CControlAssist : public CObject
{
public:

	//=============================
	// 列挙型定義
	//=============================
	enum CONTROLTYPE
	{
		CONTROLTYPE_DASH = 0,			// ダッシュ
		CONTROLTYPE_ROCKON,		// ロックオン
		CONTROLTYPE_CHANGETARGET,	// ターゲット切替
		CONTROLTYPE_ATTACK_NORMAL,	// 通常攻撃
		CONTROLTYPE_ATTACK_CHARGE,	// チャージ攻撃
		CONTROLTYPE_GUARD,			// ガード
		CONTROLTYPE_COUNTER,		// 刹舞
		CONTROLTYPE_AVOID,			// 回避
		CONTROLTYPE_MAX
	};

	CControlAssist(int nPriority = 0);
	~CControlAssist();

	//  オーバーライド関数
	HRESULT Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override {}

	void SetDefaultText();			// デフォルトテキスト設定
	void SetText(CONTROLTYPE text);	// テキスト設定
	void ResetText();	// テキストリセット

	static CControlAssist* Create();
	static CControlAssist* GetInstance() { return m_pThisPtr; }
private:

	//=============================
	// メンバ変数
	//=============================
	int m_nNumDisp;	// 描画数
	CObject2D* m_pText[CONTROLTYPE_MAX];	// テキストのオブジェクト
	static CControlAssist* m_pThisPtr;		// 自身のポインタ
};
#endif