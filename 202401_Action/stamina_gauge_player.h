//=============================================================================
// 
//  プレイヤーのスタミナゲージヘッダー [stamina_gauge_player.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _STAMINA_GAUGE_PLAYER_H_
#define _STAMINA_GAUGE_PLAYER_H_	// 二重インクルード防止

#include "object2D_gauge.h"

//==========================================================================
// クラス定義
//==========================================================================
// プレイヤーのスタミナゲージクラス定義
class CStaminaGauge_Player : public CObject
{
public:

	CStaminaGauge_Player(int nPriority = 7);
	~CStaminaGauge_Player();

	static CStaminaGauge_Player *Create(MyLib::Vector3 pos, int maxvalue);

	// オーバーライドされた関数
	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;

	void Kill(void);	// 削除

	// 強化関数
	void UpgradeMaxValue(int addvalue);	// 最大値のアップグレード

	// 値の関数
	void AddValue(float value);	// 値加算
	void SubValue(float value);	// 値減算
	void SetValue(float value);	// 値設定
	float GetValue(void);		// 値取得

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

	// 頂点列挙
	enum STATE
	{
		STATE_NORMAL = 0,	// 通常
		STATE_SUB,			// 減算
		STATE_MAX
	};

	//=============================
	// 関数リスト
	//=============================
	typedef void(CStaminaGauge_Player::* STATE_FUNC)(void);
	static STATE_FUNC m_StateFunc[];

	//=============================
	// メンバ関数
	//=============================
	// 状態系
	void StateNormal(void);	// 通常状態
	void StateSub(void);	// 減算状態

	// その他
	void ChangeColor(void);		// 色変更
	void CorrectionValue(void);	// 値の補正

	//=============================
	// メンバ変数
	//=============================
	STATE m_state;				// 状態
	float m_fStateTime;			// 状態のカウンター
	float m_fStaminaValue;	// スタミナの値
	float m_fMaxStaminaValue;	// スタミナの最大値
	float m_fOriginStaminaValue;	// スタミナの初期値
	float m_fAutoHeal;		// 自動回復
	CObject2D_Gauge* m_pObj2DGauge[VTXTYPE_MAX];	// HPゲージのオブジェクト
};


#endif