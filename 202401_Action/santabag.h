//=============================================================================
// 
//  サンタの袋ヘッダー [santabag.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _SANTABAG_H_
#define _SANTABAG_H_	// 二重インクルード防止

#include "objectX.h"
#include "listmanager.h"

//==========================================================================
// クラス定義
//==========================================================================
// サンタの袋クラス定義
class CSantaBag : public CObjectX
{
public:

	CSantaBag(int nPriority = 6);
	~CSantaBag();

	// オーバーライドされた関数
	virtual HRESULT Init() override;
	virtual void Uninit() override;
	virtual void Update() override;
	virtual void Draw() override;

	// メンバ関数
	void Hit();	// ヒット処理

	// 静的関数
	static CSantaBag *Create(const MyLib::Vector3 pos);
	static CListManager<CSantaBag> GetListObj() { return m_List; }	// リスト取得

private:

	enum STATE
	{
		STATE_NONE = 0,		// 何もなし
		STATE_DMG,			// ダメージ
		STATE_INVINCIBLE,	// 無敵
		STATE_MAX
	};

	//=============================
	// 関数リスト
	//=============================
	typedef void(CSantaBag::*STATE_FUNC)();
	static STATE_FUNC m_StateFuncList[];	// 状態関数のリスト

	//=============================
	// メンバ関数
	//=============================
	// 状態関数
	void StateNone();		// なにもない状態
	void StateDamage();		// ダメージ状態
	void StateInvicible();	// 無敵状態


	//=============================
	// メンバ変数
	//=============================
	float m_fStateTime;	// 状態カウンター
	STATE m_state;		// 状態
	D3DXCOLOR m_MatCol;	// マテリアルカラー
	int m_nLife;		// 寿命
	int m_nLifeMax;		// 寿命の最大値
	static CListManager<CSantaBag> m_List;	// リスト

};


#endif