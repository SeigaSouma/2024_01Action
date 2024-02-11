//=============================================================================
// 
//  ��Q���e�w�b�_�[ [bullet_obstacle.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _BULLET_OBSTACLE_H_
#define _BULLET_OBSTACLE_H_	// ��d�C���N���[�h�h�~

#include "objectX.h"

//==========================================================================
// �N���X��`
//==========================================================================
// ��Q���e�N���X��`
class CBulletObstacle : public CObjectX
{
public:

	CBulletObstacle(int nPriority = 6);
	~CBulletObstacle();

	// �I�[�o�[���C�h���ꂽ�֐�
	virtual HRESULT Init() override;
	virtual void Uninit() override;
	virtual void Update() override;
	virtual void Draw() override;

	static CBulletObstacle* Create(const MyLib::Vector3& pos, const MyLib::Vector3& rot, const D3DXVECTOR2& paraboramove, const float size);	// ����

private:
	void UpdatePos();			// �ړ�
	void CollisionPlayer();		// �v���C���[�Ƃ̔���
	void LimitPos();			// �ʒu����

	float m_fRadius;		// ���a
	int m_nCntMove;	// �ړ��J�E���^�[

};


#endif