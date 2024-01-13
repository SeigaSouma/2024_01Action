//==========================================================================
//
// コリジョンヘッダー[collision.h]
// Author：相馬靜雅
//
//==========================================================================
#ifndef _COLLISION_H_
#define _COLLISION_H_	// 二重インクルード防止

#include "object.h"
#include "listmanager.h"

//==========================================================================
// リスト構造クラスの定義
//==========================================================================
class CCollision
{
public:

	CCollision();	// コンストラクタ
	~CCollision();	// デストラクタ

	enum eCollisionType
	{
		TYPE_SQUARE2D = 0,	// 2D矩形
		TYPE_SQUARE3D,		// 3D矩形
		TYPE_CIRCLE2D,		// 2D円
		TYPE_CIRCLE3D,		// 3D円
		TYPE_SPHERE,		// 球
		TYPE_CYLINDER,		// 筒
		TYPE_MAX
	};


	static CCollision *Create(eCollisionType collisionType);	// 生成処理

private:
	CObject *m_pCollision;	// 当たり判定のオブジェクト
	//CListManager *m_pCollisionList[CObject::TYPE_MAX];	// 当たり判定のオブジェクト
};

//class CCollisionSquare2D : public CCollision
//{
//public:
//
//	CCollisionSquare2D();	// コンストラクタ
//	~CCollisionSquare2D();	// デストラクタ
//
//
//	static CCollisionSquare2D *Create(void);	// 生成処理
//
//private:
//};



#endif