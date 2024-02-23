//=============================================================================
// 
//  ボスの体力ゲージヘッダー [hp_gauge_boss.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _HP_GAUGE_BOSS_H_
#define _HP_GAUGE_BOSS_H_	// 二重インクルード防止

#include "main.h"
#include "object2D.h"

//==========================================================================
// クラス定義
//==========================================================================
// ボスの体力ゲージクラス定義
class CHP_GaugeBoss : public CObject
{
public:

	CHP_GaugeBoss(int nPriority = 7);
	~CHP_GaugeBoss();


	// オーバーライドされた関数
	HRESULT Init();
	void Uninit();
	void Update();
	void Draw();
	void SetVtx(int nCntGauge);

	void Kill();
	void SetLife(int nLife);

	static CHP_GaugeBoss *Create(MyLib::Vector3 pos, int nMaxLife);
private:

	// 列挙型定義
	typedef enum
	{
		VTXTYPE_BASE = 0,	// 黒ゲージ
		VTXTYPE_DIFFGAUGE,	// 差分ゲージ
		VTXTYPE_GAUGE,		// ピンクゲージ
		VTXTYPE_FRAM,		// 枠
		VTXTYPE_MAX
	}VTXTYPE;

	// 構造体定義
	struct SHP_Gauge
	{
		CObject2D *pObj2D;	// オブジェクト2Dのオブジェクト
		float fMaxWidth;	// 幅の最大値
		float fWidthDest;	// 幅の差分
		float fMaxHeight;	// 高さの最大値
	};

	// メンバ関数
	void GaugeDecrement(int nCntGauge);		// 減少処理
	void AlwaysDecrement();					// 常に減少の処理

	// メンバ変数
	SHP_Gauge m_HPGauge[VTXTYPE_MAX];		// HPゲージの情報
	int m_nLife;							// 体力
	int m_nMaxLife;							// 最大体力
};


#endif