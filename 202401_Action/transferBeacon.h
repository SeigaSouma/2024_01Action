//=============================================================================
// 
//  転移ビーコンヘッダー [transferBeacon.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _TRANSFERBEACON_H_
#define _TRANSFERBEACON_H_	// 二重インクルード防止

#include "objectX.h"
#include "listmanager.h"

//==========================================================================
// クラス定義
//==========================================================================
// 転移ビーコンクラス
class CTransferBeacon : public CObjectX
{
public:
	//=============================
	// 列挙型定義
	//=============================
	enum eTransType	// 種類
	{
		TRANSTYPE_ENHANCE = 0,	// 強化に転移
		TRANSTYPE_GAMEMAIN,		// ゲームメイン
		TRANSTYPE_MAX
	};

	CTransferBeacon(int nPriority = 6);
	~CTransferBeacon();

	// オーバーライドされた関数
	virtual HRESULT Init() override;
	virtual void Uninit() override;
	virtual void Update() override;
	virtual void Draw() override;

	// 静的関数
	static CTransferBeacon *Create(eTransType transtype, const MyLib::Vector3 pos = MyLib::Vector3(0.0f, 150.0f, 0.0f));
	static CListManager<CTransferBeacon> GetListObj() { return m_List; }	// リスト取得

private:

	//=============================
	// 列挙型定義
	//=============================
	enum STATE	// 状態
	{
		STATE_NONE = 0,	// 何もなし
		STATE_TRANSFER,	// 転移
		STATE_MAX
	};

	//=============================
	// 関数リスト
	//=============================
	typedef void(CTransferBeacon::*STATE_FUNC)();
	static STATE_FUNC m_StateFuncList[];	// 状態関数のリスト

	//=============================
	// メンバ関数
	//=============================
	// 状態関数
	void StateNone();		// なにもなし
	void StateTransfer();	// 転移

	// その他関数
	void CollisionPlayer();	// プレイヤーとの当たり判定処理

	//=============================
	// メンバ変数
	//=============================
	float m_fStateTime;	// 状態カウンター
	STATE m_state;		// 状態
	eTransType m_TransType;	// 転移種類
	static CListManager<CTransferBeacon> m_List;	// リスト

};


#endif