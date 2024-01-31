//=============================================================================
// 
//  プレイヤーの体力ゲージヘッダー [hp_gauge_player.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _HP_GAUGE_PLAYER_H_
#define _HP_GAUGE_PLAYER_H_	// 二重インクルード防止

#include "object2D.h"
#include "object2D_gauge.h"

//==========================================================================
// クラス定義
//==========================================================================
// プレイヤーの体力ゲージクラス定義
class CHP_GaugePlayer : public CObject
{
public:

	CHP_GaugePlayer(int nPriority = 7);
	~CHP_GaugePlayer();


	// オーバーライドされた関数
	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;


	// 強化関数
	int UpgradeMaxValue(int addvalue);	// 最大値のアップグレード

	void Kill(void);
	void SetLife(int nLife);
	int GetMaxLife() { return m_nMaxLifeValue; }
	static CHP_GaugePlayer* Create(MyLib::Vector3 pos, int nMaxLife);

private:

	// 列挙型定義
	enum VTXTYPE
	{
		VTXTYPE_BLACK = 0,	// 黒ゲージ
		VTXTYPE_PINK,		// ピンクゲージ
		VTXTYPE_FRAM,		// 枠
		VTXTYPE_MAX
	};

	// 構造体定義
	struct SHP_Gauge
	{
		CObject2D *pObj2D;	// オブジェクト2Dのオブジェクト
		float fMaxWidth;			// 幅の最大値
		float fWidthDest;			// 幅の差分
		float fMaxHeight;			// 高さの最大値
	};

	// メンバ関数
	void ChangeColor(int nCntGauge);		// 色変更

	// メンバ変数
	int m_nLifeValue;			// 値
	int m_nMaxLifeValue;		// 最大値
	int m_nOriginLifeValue;	// 初期値
	CObject2D_Gauge* m_pObj2DGauge[VTXTYPE_MAX];	// HPゲージのオブジェクト
};


#endif