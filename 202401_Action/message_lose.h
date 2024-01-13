//==========================================
//
//  敗北メッセージ(message_lose.h)
//  Author : Tomoya Kanazaki
//
//==========================================
#ifndef _MESSAGE_LOSE_H_
#define _MESSAGE_LOSE_H_
#include "object3D.h"

//==========================================
//  クラス定義
//==========================================
class CMessageLose : public CObject3D
{
public:

	//メンバ関数
	CMessageLose();
	~CMessageLose();

	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;

	// 静的メンバ関数
	static CMessageLose* Create();

private:

	// メンバ関数
	void Move(); //移動

	// メンバ変数
	float m_fSec; // 経過時間
	bool m_bMove; // 移動完了フラグ

};

#endif
