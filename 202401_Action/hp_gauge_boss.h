//=============================================================================
// 
//  ボスの体力ゲージヘッダー [hp_gauge_boss.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _HP_GAUGE_BOSS_H_
#define _HP_GAUGE_BOSS_H_	// 二重インクルード防止

#include "object2D.h"

//==========================================================================
// クラス定義
//==========================================================================
// ボスの体力ゲージクラス
class CHP_GaugeBoss : public CObject
{
public:

	CHP_GaugeBoss(int nPriority = 7);
	~CHP_GaugeBoss();

	// オーバーライドされた関数
	HRESULT Init() override;
	void Uninit()override;
	void Update()override;
	void Draw()override;

	void SetVtx(int nCntGauge);
	void Kill();
	void SetLife(int nLife);
	void SetDamage(int nLife);	// ダメージ設定

	static CHP_GaugeBoss *Create(MyLib::Vector3 pos, int nMaxLife);
private:

	//=============================
	// 列挙型定義
	//=============================
	enum VTXTYPE
	{
		VTXTYPE_BASE = 0,	// 黒ゲージ
		VTXTYPE_DIFFGAUGE,	// 差分ゲージ
		VTXTYPE_GAUGE,		// ピンクゲージ
		VTXTYPE_FRAM,		// 枠
		VTXTYPE_MAX
	};

	// 状態
	enum STATE
	{
		STATE_NONE = 0,	// 通常
		STATE_DAMAGE,	// ダメージ
		STATE_MAX
	};

	//=============================
	// 構造体定義
	//=============================
	struct SHP_Gauge
	{
		CObject2D *pObj2D;	// オブジェクト2Dのオブジェクト
		float fMaxWidth;	// 幅の最大値
		float fWidthDest;	// 幅の差分
		float fMaxHeight;	// 高さの最大値
	};

	//=============================
	// 関数リスト
	//=============================
	typedef void(CHP_GaugeBoss::* STATE_FUNC)();	// 状態処理のリスト
	static STATE_FUNC m_StateFuncList[];

	//=============================
	// メンバ関数
	//=============================
	// 状態関数
	void StateNone() {}	// 通常
	void StateDamage();	// ダメージ

	// その他
	void GaugeDecrement(int nCntGauge);		// 減少処理
	void AlwaysDecrement();					// 常に減少の処理

	//=============================
	// メンバ変数
	//=============================
	SHP_Gauge m_HPGauge[VTXTYPE_MAX];		// HPゲージの情報
	int m_nLife;							// 体力
	int m_nMaxLife;							// 最大体力
	float m_fDecrementTimer;				// 減少タイマー
	float m_fDecrementStart;				// 減少開始時
	float m_fDecrementEnd;					// 減少終了時
	STATE m_state;							// 状態
	float m_fStateTimer;					// 状態タイマー

};


#endif