//=============================================================================
// 
// スキルツリーカーソル処理 [skilltree_cursor.h]
// Author : 相馬靜雅
// 
//=============================================================================

#ifndef _SKILLTREE_CURSOR_H_
#define _SKILLTREE_CURSOR_H_		// 二重インクルード防止のマクロを定義する

#include "object2D.h"

//==========================================================================
// クラス定義
//==========================================================================
// スキルツリーカーソルクラス定義
class CSkillTree_Cursor : public CObject2D
{
public:

	CSkillTree_Cursor(int nPriority = 8);
	~CSkillTree_Cursor();
	
	//  オーバーライド関数
	HRESULT Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;
	void SetVtx() override;

	static CSkillTree_Cursor* Create(int nIdx);
private:

	//=============================
	// メンバ関数
	//=============================
	void Controll();
	void CollisionIcon();	// アイコンとの当たり判定

	//=============================
	// メンバ変数
	//=============================
	int m_nMyPlayerIdx;		// プレイヤーインデックス番号
	MyLib::Vector3 m_WorldPos;	// 絶対座標
	MyLib::Vector3 m_DestPos;	// 目標座標
	bool m_bHitIcon;			// アイコンの接触フラグ
	bool m_bControllKeyboard;	// キーボードの操作フラグ
};

#endif