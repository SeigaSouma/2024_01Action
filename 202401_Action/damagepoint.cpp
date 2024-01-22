//=============================================================================
// 
//  �_���[�W�|�C���g���� [damagepoint.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "damagepoint.h"
#include "manager.h"
#include "texture.h"
#include "renderer.h"
#include "multinumber.h"

//==========================================================================
// �}�N����`
//==========================================================================
namespace
{
	const char* TEXTURE = "data\\TEXTURE\\number\\number_blackclover_01.png";
	const float SIZE_NUMBER = 20.0f;	// �T�C�Y
	const int MAX_DIGIT = 3;			// �ő包
	const float MAX_LIFE = 0.7f;		// �ő����
	const float VELOCITY_UP = 40.0f;	// �㏸���x
}

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CDamagePoint::CDamagePoint(int nPriority)
{
	// �l�̃N���A
	m_fLife = 0.0f;	// ����
	m_apNumber = nullptr;
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CDamagePoint::~CDamagePoint()
{

}

//==========================================================================
// ��������
//==========================================================================
CDamagePoint* CDamagePoint::Create(MyLib::Vector3 pos, int nDamage)
{
	// �����p�̃I�u�W�F�N�g
	CDamagePoint* pDMGPoint = nullptr;

	// �������̊m��
	pDMGPoint = DEBUG_NEW CDamagePoint;

	if (pDMGPoint != nullptr)
	{
		// ����������
		pDMGPoint->SetPosition(pos);
		pDMGPoint->Init();

		// �l�̐ݒ菈��
		pDMGPoint->m_apNumber->SetValue(nDamage);
	}

	return pDMGPoint;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CDamagePoint::Init(void)
{
	SetType(CObject::TYPE_OBJECT2D);

	// �����ݒ�
	m_fLife = MAX_LIFE;

	// ��������
	m_apNumber = CMultiNumber::Create(GetPosition(), D3DXVECTOR2(SIZE_NUMBER, SIZE_NUMBER), MAX_DIGIT, CNumber::OBJECTTYPE_BILLBOARD, TEXTURE, true);
	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CDamagePoint::Uninit(void)
{
	// �����̃I�u�W�F�N�g�̏I������
	if (m_apNumber != nullptr)
	{
		m_apNumber->Uninit();
		delete m_apNumber;
		m_apNumber = nullptr;
	}

	// �J������
	Release();
}

//==========================================================================
// �X�V����
//==========================================================================
void CDamagePoint::Update(void)
{
	float deltatime = CManager::GetInstance()->GetDeltaTime();

	// �ʒu�ݒ�
	MyLib::Vector3 pos = GetPosition();
	pos.y += VELOCITY_UP * deltatime;
	SetPosition(pos);

	// �ʒu�X�V
	m_apNumber->SetPosition(GetPosition());
	m_apNumber->Update();

	// �������Z
	m_fLife -= deltatime;
	if (m_fLife <= 0.0f)
	{
		Uninit();
		return;
	}

	// �F�ݒ�
	D3DXCOLOR col = m_apNumber->GetColor();
	col.a = m_fLife / MAX_LIFE;
	m_apNumber->SetColor(col);
}

//==========================================================================
// �`�揈��
//==========================================================================
void CDamagePoint::Draw(void)
{
	if (m_apNumber != nullptr)
	{
		m_apNumber->Draw();
	}
}