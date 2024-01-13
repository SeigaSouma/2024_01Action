//=============================================================================
// 
//  爆発ヘッダー [explosion.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _EXPLOSION_H_
#define _EXPLOSION_H_	// 二重インクルード防止

#include "meshsphere.h"

//==========================================================================
// クラス定義
//==========================================================================
// 爆発クラス定義
class CExplosion : public CMeshSphere
{
public:

	// 列挙型定義
	enum TYPE
	{
		TYPE_PLAYER = 0,	// プレイヤーの爆発
		TYPE_ENEMY,			// 敵の爆発
		TYPE_MAX
	};

	enum STATE
	{
		STATE_NONE = 0,	// なにもなし
		STATE_DMG,		// ダメージ
		STATE_FADEOUT,	// フェードアウト
		STATE_MAX
	};

	CExplosion(int nPriority = 6);
	~CExplosion();


	// オーバーライドされた関数
	virtual HRESULT Init(void);
	virtual void Uninit(void);
	virtual void Update(void);
	virtual void Draw(void);
	void SetVtx(void);

	TYPE GetType(void);			// 種類取得
	void SetState(STATE state, int nCntState);	// 状態設定
	STATE GetState(void);	// 状態取得

	static CExplosion *Create(TYPE type, const MyLib::Vector3 pos, const float fSize);

private:

	// メンバ関数
	void UpdatePos(void);		// 移動
	void CollisionPlayer(void);		// プレイヤーとの判定
	void CollisionEnemy(void);		// 敵との判定
	
	// 状態更新
	void StateNone(void);		// 何もない状態
	void StateDamage(void);		// ダメージ状態処理
	void StateFadeout(void);	// フェードアウト状態処理

	// メンバ変数
	TYPE m_type;		// 爆発の種類
	STATE m_state;		// 状態
	int m_nCntState;	// 状態遷移カウンター
	int m_nLife;		// 寿命
	int m_nLifeMax;		// 寿命の最大値
	int m_nTexIdx;		// テクスチャのインデックス番号
	int m_nCntEmission;	// 発生物のカウンター
	float m_fDestSize;	// 目標のサイズ

	typedef void(CExplosion::*STATE_FUNC)(void);
	typedef void(CExplosion::*COLLISION_FUNC)(void);
	static STATE_FUNC m_FuncList[];
	static COLLISION_FUNC m_CollisionFuncList[];	// 当たり判定のリスト
};


#endif