//=============================================================================
// 
//  スキルポイントヘッダー [skillpoint.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _SKILLPOINT_H_
#define _SKILLPOINT_H_	// 二重インクルード防止

class CMultiNumber;

#include "object2D.h"

//==========================================================================
// クラス定義
//==========================================================================
// スキルポイントクラス定義
class CSkillPoint : public CObject2D
{
public:

	enum STATE
	{
		STATE_ARRIVE = 0,		// 到着
		STATE_SLIDEIN,			// スライドイン
		STATE_SLIDECOMPLETION,	// スライド完了
		STATE_SLIDEOUT,			// スライドアウト
		STATE_WAIT,				// 待機
		STATE_ENHANCE,			// 強化
		STATE_MAX
	};

	CSkillPoint(int nPriority = 9);
	~CSkillPoint();

	HRESULT Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;

	void Kill();

	// ポイント関連
	void SetPoint(int nValue);					// ポイント設定
	int GetPoint() const { return m_nPoint; }	// ポイント取得
	void AddPoint();							// ポイント追加
	void AddPoint(int nValue);					// ポイント追加
	void SubPoint();							// ポイント削除
	void SubPoint(int nValue);					// ポイント削除

	// その他
	void SetSlideIn();	// スライドイン設定
	void SetState(STATE state);	// 状態設定

	static CSkillPoint* Create();	// 生成処理

private:

	//=============================
	// 関数リスト
	//=============================
	typedef void(CSkillPoint::* STATE_FUNC)();
	static STATE_FUNC m_StateFunc[];	// 状態関数

	//=============================
	// メンバ関数
	//=============================
	// 状態関数
	void StateArrive();				// 到着
	void StateSlideIn();			// スライドイン
	void StateSlideCompletion();	// スライド完了
	void StateSlideOut();			// スライドアウト
	void StateWait();				// 待機
	void StateEnhance();			// 強化

	//=============================
	// メンバ変数
	//=============================
	int m_nPoint;				// ポイント
	int m_nVisualValue;			// 見た目上の数値
	float m_fStateTime;			// 状態タイマー
	float m_fSlideTime;			// スライドタイマー
	STATE m_State;				// 状態
	CMultiNumber* m_apNumber;	// 数字のオブジェクト
};



#endif