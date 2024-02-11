//=============================================================================
// 
//  弾ヘッダー [bullet.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _BULLET_H_
#define _BULLET_H_	// 二重インクルード防止

#include "listmanager.h"
#include "meshsphere.h"

class CThunderRing;
class CObjectBillboard;

//==========================================================================
// クラス定義
//==========================================================================
// 弾クラス定義
class CBullet : public CMeshSphere
{
public:

	// 列挙型定義
	enum TYPE
	{
		TYPE_PLAYER = 0,	// プレイヤーの弾
		TYPE_ENEMY,		// 敵の弾
		TYPE_MAX
	};

	enum MOVETYPE
	{
		MOVETYPE_NORMAL = 0,	// 通常
		MOVETYPE_PARABOLA,		// 放物線
		MOVETYPE_MAX
	};

	enum STATE
	{
		STATE_NONE = 0,	// なにもなし
		STATE_DMG,		// 跳ね返し
		STATE_MAX
	};

	CBullet(int nPriority = 6);
	~CBullet();


	// オーバーライドされた関数
	virtual HRESULT Init();
	virtual void Uninit();
	virtual void Update();
	virtual void Draw();
	void SetVtx();

	void SetType(TYPE type);	// 種類設定
	TYPE GetType();			// 種類取得
	void SetState(STATE state, int nCntState);	// 状態設定
	STATE GetState();	// 状態取得
	void SetTargetPosition(MyLib::Vector3 pos);	// 目標の位置
	void SetParabolaHeight(float fHeight);		// 放物線の最大高さ

	void SetReverseAutoDeath() { m_bAutoDeath = m_bAutoDeath ?  false : true; }	// 自動削除の判定削除
	bool IsFinish() { return m_bFinish; }					// 終了の判定

	static int GetNumAll();
	static CBullet *Create(TYPE type, MOVETYPE movetype, const MyLib::Vector3 pos, const MyLib::Vector3 rot, const MyLib::Vector3 move, const float fSize);
	static CListManager<CBullet> GetListObj() { return m_List; }	// リスト取得

private:
	void UpdatePos();		// 移動
	void UpdateTypePlayer();	// プレイヤー弾の更新
	void CollisionPlayer();		// プレイヤーとの判定
	void CollisionEnemy();		// 敵との判定
	void StateNone();		// 何もない状態
	void StateDamage();		// ダメージ状態処理

	typedef void(CBullet::* STATE_FUNC)();
	typedef void(CBullet::* COLLISION_FUNC)();
	static STATE_FUNC m_FuncList[];
	static COLLISION_FUNC m_CollisionFuncList[];	// 当たり判定のリスト

	TYPE m_type;				// 弾の種類
	STATE m_state;				// 状態
	MOVETYPE m_MoveType;		// 移動の種類
	MyLib::Vector3 m_OriginPosition;	// 元の位置
	MyLib::Vector3 m_TargetPosition;	// 目標の位置
	int m_nCntState;			// 状態遷移カウンター
	int m_nLife;				// 寿命
	int m_nLifeMax;				// 寿命の最大値
	int m_nTexIdx;				// テクスチャのインデックス番号
	int m_nCntEmission;			// 発生物のカウンター
	float m_fMaxParabolaHeight;	// 放物線の最大高さ
	bool m_bAutoDeath;		// 自動削除のフラグ
	bool m_bFinish;			// 終わった判定
	static int m_nNumAll;		// 弾の総数
	static const char *m_apTextureFile[TYPE_MAX];	// テクスチャのファイル
	static CListManager<CBullet> m_List;	// リスト
};


#endif