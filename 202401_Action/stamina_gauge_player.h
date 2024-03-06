//=============================================================================
// 
//  プレイヤーのスタミナゲージヘッダー [stamina_gauge_player.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _STAMINA_GAUGE_PLAYER_H_
#define _STAMINA_GAUGE_PLAYER_H_	// 二重インクルード防止

#include "object2D_gauge.h"

class CHPGaugeTip;

//==========================================================================
// クラス定義
//==========================================================================
// プレイヤーのスタミナゲージクラス
class CStaminaGauge_Player : public CObject
{
public:

	//=============================
	// 列挙型定義
	//=============================
	// 状態
	enum STATE
	{
		STATE_NORMAL = 0,	// 通常
		STATE_SUB,			// 減算
		STATE_QUICKHEAL,	// 急速回復
		STATE_MAX
	};

	CStaminaGauge_Player(int nPriority = 6);
	~CStaminaGauge_Player();

	// オーバーライドされた関数
	HRESULT Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;

	void Kill();	// 削除

	// 強化関数
	void UpgradeMaxValue(int addvalue);		// 最大値のアップグレード
	void UpgradeAutoHeal(float multiply);	// 自動回復のアップグレード

	// 値の関数
	void AddValue(float value);	// 値加算
	void SubValue(float value);	// 値減算
	void SetValue(float value);	// 値設定
	float GetValue();			// 値取得
	float GetMaxValue() { return m_fMaxStaminaValue; }	// 最大値取得
	float GetAutoHealValue() { return m_fAutoHeal; }	// 自然回復量取得

	void SetState(STATE state);
	STATE GetState() { return m_state; }

	static CStaminaGauge_Player* Create(MyLib::Vector3 pos, int maxvalue);
private:

	//=============================
	// 列挙型定義
	//=============================
	// 頂点列挙
	enum VTXTYPE
	{
		VTXTYPE_BLACK = 0,	// 黒ゲージ
		VTXTYPE_GAUGE,		// ゲージ部分
		VTXTYPE_MAX
	};

	//=============================
	// 関数リスト
	//=============================
	typedef void(CStaminaGauge_Player::* STATE_FUNC)();
	static STATE_FUNC m_StateFunc[];

	//=============================
	// メンバ関数
	//=============================
	// 状態系
	void StateNormal();	// 通常状態
	void StateSub();	// 減算状態
	void StateQuickHealing();	// 急速回復状態

	// その他
	void ChangeColor();		// 色変更
	void CorrectionValue();	// 値の補正
	void SetVtx(int nIdx);

	//=============================
	// メンバ変数
	//=============================
	STATE m_state;					// 状態
	float m_fStateTime;				// 状態のカウンター
	float m_fStaminaValue;			// スタミナの値
	float m_fMaxStaminaValue;		// スタミナの最大値
	float m_fOriginStaminaValue;	// スタミナの初期値
	float m_fAutoHeal;				// 自動回復
	CObject2D_Gauge* m_pObj2DGauge[VTXTYPE_MAX];	// HPゲージのオブジェクト
	CHPGaugeTip* m_pTip;	// ゲージの先端
};


#endif