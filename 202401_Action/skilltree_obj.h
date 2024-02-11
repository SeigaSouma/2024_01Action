//=============================================================================
// 
//  スキルツリーオブジェクトヘッダー [skilltree_obj.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _SKILLTREE_OBJ_H_
#define _SKILLTREE_OBJ_H_	// 二重インクルード防止

#include "objectX.h"
#include "listmanager.h"

//==========================================================================
// クラス定義
//==========================================================================
// スキルツリーオブジェクトクラス
class CSkillTree_Obj : public CObjectX
{
public:

	CSkillTree_Obj(int nPriority = 3);
	~CSkillTree_Obj();

	// オーバーライドされた関数
	virtual HRESULT Init() override;
	virtual void Uninit() override;
	virtual void Update() override;
	virtual void Draw() override;

	// その他関数
	void StartUp();	// 起動処理
	void ReStartUp();	// 再起動処理

	// 静的関数
	static CSkillTree_Obj* GetInstance();		// インスタンス取得
	static CSkillTree_Obj *Create();

private:

	//=============================
	// 列挙型定義
	//=============================
	enum STATE	// 状態
	{
		STATE_NONE = 0,	// 何もなし
		STATE_STARTUP,	// 起動
		STATE_MAX
	};

	//=============================
	// 関数リスト
	//=============================
	typedef void(CSkillTree_Obj::*STATE_FUNC)();
	static STATE_FUNC m_StateFuncList[];	// 状態関数のリスト

	//=============================
	// メンバ関数
	//=============================
	// 状態関数
	void StateNone();		// なにもなし
	void StateStartUp();	// 起動

	// その他関数
	void CollisionPlayer();	// プレイヤーとの当たり判定処理

	//=============================
	// メンバ変数
	//=============================
	float m_fStateTime;	// 状態カウンター
	STATE m_state;		// 状態
	Effekseer::Handle* m_pWeaponHandle;		// エフェクトハンドルのポインタ
	static CSkillTree_Obj* m_pThisPtr;		// 自身のポインタ
};


#endif