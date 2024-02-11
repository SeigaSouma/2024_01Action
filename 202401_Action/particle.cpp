//=============================================================================
// 
//  �p�[�e�B�N������ [my_particle.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "particle.h"
#include "calculation.h"
#include "2D_effect.h"
#include "3D_effect.h"
using namespace my_particle;	// ���O��Ԃ��w��

//==========================================================================
// �ÓI�����o�ϐ��錾
//==========================================================================
MyLib::Vector3 m_pos;		// �ʒu
MyLib::Vector3 m_move;		// �ړ���
D3DXCOLOR m_col;		// �F
float m_fRadius;		// ���a
float m_fAngle;			// ����
float m_fMove;			// �ړ���
int m_nLife;			// ����
TYPE m_nType;			// ���

HRESULT Init();
void Smoke();
void SmokeRed();
void SmokeYel();
void EnemyFade();
void OffSetting();
void Fire_WallTorch();
void Fire_StandTorch();

//==========================================================================
// �p�[�e�B�N���̏���������
//==========================================================================
HRESULT Init()
{

	// �p�[�e�B�N���̏��̏�����
	m_pos = MyLib::Vector3(0.0f, 0.0f, 0.0f);		// �ʒu
	m_move = MyLib::Vector3(0.0f, 0.0f, 0.0f);		// �ړ���
	m_col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	m_fRadius = 0.0f;
	m_nLife = 0;
	m_nType = TYPE_NONE;

	return S_OK;
}

//==========================================================================
// �p�[�e�B�N���̐ݒ菈��
//==========================================================================
void my_particle::Create(const MyLib::Vector3& pos, TYPE nType)
{
	Init();

	// �ʒu
	m_pos = pos;

	// ���
	m_nType = nType;

	switch (m_nType)
	{
	case TYPE_EXPLOSION:
		m_nLife = 30;
		Smoke();
		SmokeRed();
		SmokeYel();
		break;

	case TYPE_SMOKE:

		m_nLife = 30;
		Smoke();
		break;

	case TYPE_SMOKE_RED:

		m_nLife = 30;
		SmokeRed();
		break;


	case TYPE_SMOKE_YEL:

		m_nLife = 30;
		SmokeYel();
		break;

	case TYPE_OFFSETTING:
		m_nLife = 30;
		OffSetting();
		break;

	case TYPE_ENEMY_FADE:
		m_nLife = 60;
		EnemyFade();
		break;

	case TYPE_FIRE_WALLTORCH:
		Fire_WallTorch();
		break;

	case TYPE_FIRE_STANDTORCH:
		Fire_StandTorch();
		break;

	}
}

//==========================================================================
// �`�F�b�N�p
//==========================================================================
void Check()
{
	for (int nCntUse = 0; nCntUse < 1; nCntUse++)
	{
		// �ړ��ʂ̐ݒ�
		m_move = mylib_const::DEFAULT_VECTOR3;

		m_col = D3DXCOLOR(1.0f, 0.6f, 0.2f, 1.0f);

		m_fRadius = 10.0f;

		// �G�t�F�N�g�̐ݒ�
		CEffect3D::Create(
			m_pos,
			m_move,
			m_col,
			m_fRadius,
			m_nLife,
			CEffect3D::MOVEEFFECT_NONE, CEffect3D::TYPE_NORMAL);
	}
}

//==========================================================================
// �`�F�b�N�p
//==========================================================================
void Check2D()
{
	for (int nCntUse = 0; nCntUse < 5; nCntUse++)
	{

		float fMove = 0.0f;		// �ړ���
		float fAngle = 0.0f;	// �p�x

		// �ړ��ʂ̐ݒ�
		m_move.x = 0.0f;
		m_move.y = 0.0f;
		m_move.z = 0.0f;

		m_col = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);

		m_fRadius = 20.0f;

		// �G�t�F�N�g�̐ݒ�
		CEffect2D::Create(
			m_pos,
			m_move,
			m_col,
			m_fRadius,
			m_nLife,
			CEffect2D::MOVEEFFECT_NONE, CEffect2D::TYPE_NORMAL);
	}
}

//==========================================================================
// ���̃p�[�e�B�N��
//==========================================================================
void Smoke()
{
	
	for (int nCntUse = 0; nCntUse < 3; nCntUse++)
	{
		m_fMove = (float)(UtilFunc::Transformation::Random(0, 10)) / 10 + 1.0f;		// �ړ���
		m_fAngle = (float)(UtilFunc::Transformation::Random(-50, 50)) / 100.0f;	// �p�x

		// �ړ��ʂ̐ݒ�
		m_move.x = sinf((float)(UtilFunc::Transformation::Random(-314, 314)) / 100.0f) * m_fMove;
		m_move.y = cosf((float)(UtilFunc::Transformation::Random(-314, 314)) / 100.0f) * m_fMove;
		m_move.z = cosf((float)(UtilFunc::Transformation::Random(-314, 314)) / 100.0f) * m_fMove;

		m_pos.x += m_move.x * 0.3f;
		m_pos.y += m_move.y * 0.3f;

		m_col = D3DXCOLOR(0.7f, 0.7f, 0.7f, 0.5f);

		m_fRadius = 60.0f;

		if (rand() % 3 == 0)
		{
			m_fRadius *= 0.95f;
		}
		else if (rand() % 3 == 1)
		{
			m_fRadius *= 0.9f;
		}
		else if(rand() % 3 == 2)
		{
			m_fRadius *= 0.8f;
		}

		// �G�t�F�N�g�̐ݒ�
		CEffect3D::Create(
			m_pos,
			m_move,
			m_col,
			m_fRadius,
			m_nLife,
			CEffect3D::MOVEEFFECT_ADD, CEffect3D::TYPE_SMOKEBLACK);
	}
}

//==========================================================================
// ���̃p�[�e�B�N��(��)
//==========================================================================
void SmokeRed()
{

	for (int nCntUse = 0; nCntUse < 10; nCntUse++)
	{
		m_fMove = (float)(UtilFunc::Transformation::Random(0, 10)) / 10 + 2.0f;		// �ړ���
		m_fAngle = (float)(UtilFunc::Transformation::Random(-314, 314)) / 100.0f;	// �p�x

		// �ړ��ʂ̐ݒ�
		m_move.x = sinf((float)(UtilFunc::Transformation::Random(-314, 314)) / 100.0f) * m_fMove;
		m_move.y = cosf((float)(UtilFunc::Transformation::Random(-314, 314)) / 100.0f) * m_fMove;
		m_move.z = cosf((float)(UtilFunc::Transformation::Random(-314, 314)) / 100.0f) * m_fMove;

		m_pos.x += m_move.x * 0.3f;
		m_pos.y += m_move.y * 0.3f;

		m_col = D3DXCOLOR(1.0f, 0.4f, 0.3f, 1.0f);

		m_fRadius = 30.0f;

		if (rand() % 3 == 0)
		{
			m_fRadius *= 0.95f;
		}
		else if (rand() % 3 == 1)
		{
			m_fRadius *= 0.9f;
		}
		else if (rand() % 3 == 2)
		{
			m_fRadius *= 0.8f;
		}

		// �G�t�F�N�g�̐ݒ�
		CEffect3D::Create(
			m_pos,
			m_move,
			m_col,
			m_fRadius,
			m_nLife,
			CEffect3D::MOVEEFFECT_ADD, CEffect3D::TYPE_SMOKEBLACK);
	}
}

//==========================================================================
// ���̃p�[�e�B�N��(��)
//==========================================================================
void SmokeYel()
{

	for (int nCntUse = 0; nCntUse < 10; nCntUse++)
	{
		m_fMove = (float)(UtilFunc::Transformation::Random(0, 10)) / 10 + 2.0f;		// �ړ���
		m_fAngle = (float)(UtilFunc::Transformation::Random(-314, 314)) / 100.0f;	// �p�x

		// �ړ��ʂ̐ݒ�
		m_move.x = sinf((float)(UtilFunc::Transformation::Random(-314, 314)) / 100.0f) * m_fMove;
		m_move.y = cosf((float)(UtilFunc::Transformation::Random(-314, 314)) / 100.0f) * m_fMove;
		m_move.z = cosf((float)(UtilFunc::Transformation::Random(-314, 314)) / 100.0f) * m_fMove;

		m_pos.x += m_move.x * 0.3f;
		m_pos.y += m_move.y * 0.3f;

		m_col = D3DXCOLOR(0.8f, 0.5f, 0.0f, 1.0f);

		m_fRadius = 30.0f;

		if (rand() % 3 == 0)
		{
			m_fRadius *= 0.95f;
		}
		else if (rand() % 3 == 1)
		{
			m_fRadius *= 0.9f;
		}
		else if (rand() % 3 == 2)
		{
			m_fRadius *= 0.8f;
		}

		// �G�t�F�N�g�̐ݒ�
		CEffect3D::Create(
			m_pos,
			m_move,
			m_col,
			m_fRadius,
			m_nLife,
			CEffect3D::MOVEEFFECT_ADD, CEffect3D::TYPE_SMOKEBLACK);
	}
}

//==========================================================================
//�ł������̃p�[�e�B�N��
//==========================================================================
void OffSetting()
{
	float fAngle, fMove;

	for (int nCntUse = 0; nCntUse < 10; nCntUse++)
	{

		fMove = (float)(rand() % 10) / 1 + 3.0f;		// �ړ���
		fAngle = (float)(rand() % 629 - 314) / 100.0f;	// �p�x

		// �ړ��ʂ̐ݒ�
		m_move.x = sinf((float)(rand() % 629 - 314) / 100.0f) * fMove;
		m_move.y = cosf((float)(rand() % 629 - 314) / 100.0f) * fMove;
		m_move.z = cosf((float)(rand() % 629 - 314) / 100.0f) * fMove;

		m_col = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);

		m_fRadius = 40.0f;

		// �G�t�F�N�g�̐ݒ�
		if (CEffect3D::Create(
			m_pos,
			m_move,
			m_col,
			m_fRadius,
			m_nLife,
			CEffect3D::MOVEEFFECT_SUB, CEffect3D::TYPE_NORMAL) == NULL)
		{
			return;
		}
	}
}

//==========================================================================
// �G�t�F�[�h
//==========================================================================
void EnemyFade()
{
	for (int nCntUse = 0; nCntUse < 15; nCntUse++)
	{
		float fMoveXZ = (float)UtilFunc::Transformation::Random(0, 100) * 0.01f;		// �ړ���
		float fMove = (float)UtilFunc::Transformation::Random(0, 60) * 0.1f + 1.0f;	// �ړ���

		// �ړ��ʂ̐ݒ�
		m_move.x = sinf(UtilFunc::Transformation::GetRandomPi()) * fMoveXZ;
		m_move.y = sinf((float)UtilFunc::Transformation::Random(0, 314) / 100.0f) * fMove;
		m_move.z = cosf(UtilFunc::Transformation::GetRandomPi()) * fMoveXZ;

		// �o���n�_�̃u��
		m_pos.y += UtilFunc::Transformation::Random(-100, 100) * 0.1f;
		m_pos.x += UtilFunc::Transformation::Random(-100, 100) * 0.1f;
		m_pos.z += UtilFunc::Transformation::Random(-100, 100) * 0.1f;

		m_fRadius = 20.0f + UtilFunc::Transformation::Random(-50, 50) * 0.1f;

		// �F����
		int nRand = 0;
		//int nRand = UtilFunc::Transformation::Random(0, 1);
		switch (nRand)
		{
		case 0:
			m_col = D3DXCOLOR(
				0.2f + UtilFunc::Transformation::Random(-100, 100) * 0.001f,
				0.8f + UtilFunc::Transformation::Random(-200, 200) * 0.001f,
				0.2f + UtilFunc::Transformation::Random(-100, 100) * 0.001f + UtilFunc::Transformation::Random(0, 20) * 0.01f, 1.0f);
			break;

		case 1:
			m_col = D3DXCOLOR(
				0.8f + UtilFunc::Transformation::Random(-50, 50) * 0.001f,
				0.2f + UtilFunc::Transformation::Random(-50, 50) * 0.001f,
				0.8f + UtilFunc::Transformation::Random(-50, 50) * 0.001f, 1.0f);
			break;

		default:
			break;
		}

		// �G�t�F�N�g�̐ݒ�
		CEffect3D::Create(
			m_pos,
			m_move,
			m_col,
			m_fRadius,
			m_nLife,
			CEffect3D::MOVEEFFECT_SUB, CEffect3D::TYPE_NORMAL);
	}
}

//==========================================================================
// �u������
//==========================================================================
void Fire_StandTorch()
{
	for (int nCntUse = 0; nCntUse < 2; nCntUse++)
	{
		// ��
		float fMoveY = 3.5f + UtilFunc::Transformation::Random(-10, 10) * 0.1f;
		float fMove = 0.5f + UtilFunc::Transformation::Random(-20, 20) * 0.01f;
		float fRot = UtilFunc::Transformation::Random(-31, 31) * 0.1f;

		// �ړ��ʂ̐ݒ�
		m_move.x = sinf(fRot) * fMove;
		m_move.y = fMoveY;
		m_move.z = cosf(fRot) * fMove;

		m_fRadius = 70.0f + UtilFunc::Transformation::Random(-50, 50) * 0.1f;

		m_col = D3DXCOLOR(0.9f + UtilFunc::Transformation::Random(-10, 0) * 0.01f, 0.0f, 0.0f, 0.6f);

		CEffect3D::Create(
			m_pos,
			m_move,
			m_col,
			m_fRadius,
			25,
			CEffect3D::MOVEEFFECT_ADD,
			CEffect3D::TYPE_SMOKE);

		// ��
		m_fRadius = 50.0f + UtilFunc::Transformation::Random(-50, 50) * 0.1f;

		m_col = D3DXCOLOR(
			0.8f + UtilFunc::Transformation::Random(-10, 0) * 0.01f,
			0.5f + UtilFunc::Transformation::Random(-10, 0) * 0.01f,
			0.0f, 0.5f);

		CEffect3D::Create(
			m_pos,
			m_move,
			m_col,
			m_fRadius,
			25,
			CEffect3D::MOVEEFFECT_ADD,
			CEffect3D::TYPE_SMOKE);
	}
}

//==========================================================================
// �Ǐ���
//==========================================================================
void Fire_WallTorch()
{
	for (int nCntUse = 0; nCntUse < 2; nCntUse++)
	{
		// ��
		float fMoveY = 3.5f + UtilFunc::Transformation::Random(-10, 10) * 0.1f;
		float fMove = 0.5f + UtilFunc::Transformation::Random(-20, 20) * 0.01f;
		float fRot = UtilFunc::Transformation::Random(-31, 31) * 0.1f;

		// �ړ��ʂ̐ݒ�
		m_move.x = sinf(fRot) * fMove;
		m_move.y = fMoveY;
		m_move.z = cosf(fRot) * fMove;

		m_fRadius = 40.0f + UtilFunc::Transformation::Random(-50, 50) * 0.1f;

		m_col = D3DXCOLOR(0.9f + UtilFunc::Transformation::Random(-10, 0) * 0.01f, 0.0f, 0.0f, 0.6f);

		CEffect3D::Create(
			m_pos,
			m_move,
			m_col,
			m_fRadius,
			25,
			CEffect3D::MOVEEFFECT_ADD,
			CEffect3D::TYPE_SMOKE);

		// ��
		m_fRadius = 30.0f + UtilFunc::Transformation::Random(-50, 50) * 0.1f;

		m_col = D3DXCOLOR(
			0.8f + UtilFunc::Transformation::Random(-10, 0) * 0.01f,
			0.5f + UtilFunc::Transformation::Random(-10, 0) * 0.01f,
			0.0f, 0.6f);

		CEffect3D::Create(
			m_pos,
			m_move,
			m_col,
			m_fRadius,
			25,
			CEffect3D::MOVEEFFECT_ADD,
			CEffect3D::TYPE_SMOKE);
	}
}