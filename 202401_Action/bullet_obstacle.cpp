//=============================================================================
// 
//  ��Q���e���� [bullet_obstacle.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "bullet_obstacle.h"
#include "manager.h"
#include "texture.h"
#include "renderer.h"
#include "3D_effect.h"
#include "sound.h"
#include "particle.h"
#include "calculation.h"
#include "player.h"


//==========================================================================
// �萔��`
//==========================================================================
namespace
{
	const char* BULLETMODEL = "data\\MODEL\\attack\\obstacleballet_01.x";	// ���f���t�@�C��
	const int DAMAGE = 8;	// �_���[�W
}

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CBulletObstacle::CBulletObstacle(int nPriority) : CObjectX(nPriority)
{
	// �l�̃N���A
	m_fRadius = 0.0f;		// ���a
	m_nCntMove = 0;	// �ړ��J�E���^�[
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CBulletObstacle::~CBulletObstacle()
{
	
}

//==========================================================================
// ��������
//==========================================================================
CBulletObstacle* CBulletObstacle::Create(const MyLib::Vector3& pos, const MyLib::Vector3& rot, const D3DXVECTOR2& paraboramove, const float size)
{

	// �������̊m��
	CBulletObstacle* pBullet = DEBUG_NEW CBulletObstacle;

	if (pBullet != NULL)
	{// �������̊m�ۂ��o���Ă�����

		// �ʒu���蓖��
		pBullet->SetPosition(pos);
		pBullet->SetOriginPosition(pos);

		// �ʒu���蓖��
		pBullet->SetRotation(rot);

		// �ړ��ʐݒ�

		MyLib::Vector3 move;
		move.y = paraboramove.y;
		move.x = sinf(D3DX_PI + rot.y) * paraboramove.x;
		move.z = cosf(D3DX_PI + rot.y) * paraboramove.x;

		pBullet->SetMove(move);

		// �T�C�Y�ݒ�
		pBullet->m_fRadius = size;

		// ����������
		pBullet->Init();
	}

	return pBullet;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CBulletObstacle::Init(void)
{
	// ��ނ̐ݒ�
	CObject::SetType(TYPE_BULLET);

	// ����������
	HRESULT hr = CObjectX::Init(BULLETMODEL);
	if (FAILED(hr))
	{// ���s�����Ƃ�
		return E_FAIL;
	}
	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CBulletObstacle::Uninit(void)
{
	// �I������
	CObjectX::Uninit();
}

//==========================================================================
// �X�V����
//==========================================================================
void CBulletObstacle::Update(void)
{
	// �ʒu�X�V
	UpdatePos();

	// �v���C���[�Ƃ̔���
	CollisionPlayer();

	// �ʒu����
	LimitPos();
}

//==========================================================================
// �ړ�����
//==========================================================================
void CBulletObstacle::UpdatePos(void)
{
	// �ʒu�擾
	MyLib::Vector3 pos = GetPosition();
	MyLib::Vector3 posOrigin = GetOriginPosition();

	// �ړ��ʎ擾
	MyLib::Vector3 move = GetMove();

	// �ʒu�X�V
	m_nCntMove++;
	pos = (MyLib::Vector3(0.0f, -0.4f, 0.0f) * (float)(m_nCntMove * m_nCntMove) + move * (float)m_nCntMove) + posOrigin;

	// �ʒu�ݒ�
	SetPosition(pos);

#if _DEBUG
	CEffect3D::Create(
		pos,
		MyLib::Vector3(0.0f, 0.0f, 0.0f),
		D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f),
		m_fRadius, 2, CEffect3D::MOVEEFFECT_NONE, CEffect3D::TYPE_NORMAL);
#endif
}

//==========================================================================
// �ʒu����
//==========================================================================
void CBulletObstacle::LimitPos(void)
{
	MyLib::Vector3 pos = GetPosition();

	float fLength = sqrtf(pos.x * pos.x + pos.z * pos.z);

	if (fLength > mylib_const::RADIUS_STAGE)
	{
		Uninit();
		return;
	}

	if (m_nCntMove > 4 &&
		pos.y <= 0.0f)
	{
		Uninit();
		return;
	}
}

//==========================================================================
// �v���C���[�Ƃ̔���
//==========================================================================
void CBulletObstacle::CollisionPlayer(void)
{
	// �v���C���[���擾
	CPlayer* pPlayer = CPlayer::GetListObj().GetData(0);
	if (pPlayer == NULL)
	{// NULL��������
		return;
	}

	// �v���C���[�̏��擾
	MyLib::Vector3 PlayerPosition = pPlayer->GetCenterPosition();
	MyLib::Vector3 PlayerRotation = pPlayer->GetRotation();
	float fPlayerRadius = pPlayer->GetRadius();

	// ���擾
	MyLib::Vector3 pos = GetPosition();

	if (UtilFunc::Collision::SphereRange(pos, PlayerPosition, m_fRadius, fPlayerRadius).ishit)
	{// �������Ă�����

		// �q�b�g����
		if (pPlayer->Hit(DAMAGE))
		{
			// �I������
			Uninit();
		}
	}

}

//==========================================================================
// �`�揈��
//==========================================================================
void CBulletObstacle::Draw(void)
{
	// �`��
	CObjectX::Draw();
}

