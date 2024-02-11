//==========================================
//
//  勝利メッセージ(message_win.h)
//  Author : Tomoya Kanazaki
//
//==========================================
#ifndef _MESSAGE_WIN_H_
#define _MESSAGE_WIN_H_
#include "object3D.h"

//==========================================
//  クラス定義
//==========================================
class CMessageWin : public CObject3D
{
public:

	//メンバ関数
	CMessageWin();
	~CMessageWin();

	HRESULT Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;

	// 静的メンバ関数
	static CMessageWin* Create();

private:

	// メンバ関数
	void Move(); //移動

	// メンバ変数
	float m_fSec; // 経過時間
	bool m_bMove; // 移動完了フラグ
	int m_nIdx; // 自身のインデックス
	MyLib::Vector3 pos_target; // 目標位置

	// 静的メンバ変数
	static int nIdx;

};

#endif
