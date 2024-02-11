//=============================================================================
// 
//  復活時間ゲージヘッダー [revivaltime_gauge.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _REVIVALTIME_GAUGE_H_
#define _REVIVALTIME_GAUGE_H_	// 二重インクルード防止

#include "object2D_gauge.h"

//==========================================================================
// クラス定義
//==========================================================================
// 復活時間ゲージクラス定義
class CRevivalTimeGauge : public CObject
{
public:

	CRevivalTimeGauge(float fadetime, int nPriority = 8);
	~CRevivalTimeGauge();

	// オーバーライドされた関数
	HRESULT Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;

	void Kill();	// 削除


	static CRevivalTimeGauge* Create(const float fadetime, MyLib::Vector3 pos);
private:

	//=============================
	// 列挙型定義
	//=============================
	// 頂点列挙
	enum VTXTYPE
	{
		VTXTYPE_BLACK = 0,	// 黒ゲージ
		VTXTYPE_GAUGE,		// ゲージ部分
		VTXTYPE_FRAM,		// 枠
		VTXTYPE_MAX
	};

	// 状態挙
	enum eState
	{
		STATE_NONE = 0, // なにもなし
		STATE_FADEIN,	// フェードイン
		STATE_FADEOUT,	// フェードアウト
		STATE_TIMECOMPLETE,	// 時間がなくなった瞬間
		STATE_MAX
	};

	//=============================
	// 関数ポインタ
	//=============================
	typedef void(CRevivalTimeGauge::* STATE_FUNC)();
	static STATE_FUNC m_StateFunc[];

	//=============================
	// メンバ関数
	//=============================
	void StateNone();		// なにもなし
	void StateFadeIn();		// フェードイン
	void StateFadeOut();	// フェードアウト
	void StateTimeComplete();	// 時間がなくなった瞬間

	//=============================
	// メンバ変数
	//=============================
	eState m_state;		// 状態
	float m_fStateTime;	// 状態のカウンター

	float m_fValue;		// タイマーの値
	const float m_fFadeTime;	// フェードにかかる時間
	CObject2D_Gauge* m_pObj2DGauge[VTXTYPE_MAX];	// HPゲージのオブジェクト
};


#endif