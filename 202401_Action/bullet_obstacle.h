//=============================================================================
// 
//  障害物弾ヘッダー [bullet_obstacle.h]
//  Author : 相馬靜雅
// 
//=============================================================================

#ifndef _BULLET_OBSTACLE_H_
#define _BULLET_OBSTACLE_H_	// 二重インクルード防止

#include "objectX.h"

//==========================================================================
// クラス定義
//==========================================================================
// 障害物弾クラス定義
class CBulletObstacle : public CObjectX
{
public:

	CBulletObstacle(int nPriority = 6);
	~CBulletObstacle();

	// オーバーライドされた関数
	virtual HRESULT Init() override;
	virtual void Uninit() override;
	virtual void Update() override;
	virtual void Draw() override;

	static CBulletObstacle* Create(const MyLib::Vector3& pos, const MyLib::Vector3& rot, const D3DXVECTOR2& paraboramove, const float size);	// 生成

private:
	void UpdatePos();			// 移動
	void CollisionPlayer();		// プレイヤーとの判定
	void LimitPos();			// 位置制限

	float m_fRadius;		// 半径
	int m_nCntMove;	// 移動カウンター

};


#endif