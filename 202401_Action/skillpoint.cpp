//=============================================================================
// 
//  �X�L���|�C���g���� [skillpoint.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "skillpoint.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"
#include "object2D.h"
#include "calculation.h"
#include "multinumber.h"
#include "input.h"

//==========================================================================
// �萔��`
//==========================================================================
namespace
{
	const char* TEXTURE = "data\\TEXTURE\\skillpoint\\cyberwall_03.png";
	const char* NUMBER_TEXTURE = "data\\TEXTURE\\number\\number_blackclover_01.png";
}

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CSkillPoint::CSkillPoint(int nPriority) : CObject2D(nPriority)
{
	// �l�̃N���A
	m_nPoint = 0;	// �|�C���g
	m_apNumber = nullptr;	// �����̃I�u�W�F�N�g
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CSkillPoint::~CSkillPoint()
{

}

//==========================================================================
// ��������
//==========================================================================
CSkillPoint *CSkillPoint::Create(void)
{
	// �����p�̃I�u�W�F�N�g
	CSkillPoint *pTitleScreen = NULL;

	if (pTitleScreen == NULL)
	{// NULL��������

		// �������̊m��
		pTitleScreen = DEBUG_NEW CSkillPoint;

		if (pTitleScreen != NULL)
		{// �������̊m�ۂ��o���Ă�����

			// ����������
			pTitleScreen->Init();
		}

		return pTitleScreen;
	}

	return NULL;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CSkillPoint::Init(void)
{
	// ��ނ̐ݒ�
	SetType(CObject::TYPE_OBJECT2D);

	// ������
	CObject2D::Init();
	SetType(CObject::TYPE_OBJECT2D);

	// �e�N�X�`���̊��蓖��
	int nIdx = CTexture::GetInstance()->Regist(TEXTURE);

	// �e�N�X�`���̊��蓖��
	BindTexture(nIdx);

	D3DXVECTOR2 size;
	// �T�C�Y�擾
	SetSize(CTexture::GetInstance()->GetImageSize(nIdx) * 0.1f);	// �T�C�Y
	SetPosition(MyLib::Vector3(1000.0f, 80.0f, 0.0f));	// �ʒu


	// ��������
	m_apNumber = CMultiNumber::Create({1200.0f, 80.0f, 0.0f}, D3DXVECTOR2(GetSize().x, GetSize().x), 2, CNumber::OBJECTTYPE_2D, NUMBER_TEXTURE);

	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CSkillPoint::Uninit(void)
{
	// �����̃I�u�W�F�N�g�̏I������
	if (m_apNumber != NULL)
	{
		m_apNumber->Uninit();
		delete m_apNumber;
		m_apNumber = NULL;
	}

	// �I������
	CObject2D::Uninit();
}

//==========================================================================
// �X�V����
//==========================================================================
void CSkillPoint::Update(void)
{
	// �X�V����
	CObject2D::Update();

	// �l�̐ݒ菈��
	m_apNumber->SetValue(m_nPoint);
}

//==========================================================================
// �|�C���g�ݒ�
//==========================================================================
void CSkillPoint::SetPoint(int nValue)
{
	m_nPoint = nValue;
	if (m_nPoint <= 0)
	{
		m_nPoint = 0;
	}
}

//==========================================================================
// �|�C���g�ǉ�
//==========================================================================
void CSkillPoint::AddPoint(void)
{
	m_nPoint++;
}

//==========================================================================
// �|�C���g�ǉ�
//==========================================================================
void CSkillPoint::AddPoint(int nValue)
{
	m_nPoint += nValue;
}

//==========================================================================
// �|�C���g�폜
//==========================================================================
void CSkillPoint::SubPoint(void)
{
	m_nPoint--;

	if (m_nPoint <= 0)
	{
		m_nPoint = 0;
	}
}

//==========================================================================
// �`�揈��
//==========================================================================
void CSkillPoint::Draw(void)
{
	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();

	// �A���t�@�e�X�g��L���ɂ���
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);

	// �`�揈��
	CObject2D::Draw();

	// �A���t�@�e�X�g�𖳌��ɂ���
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);
}

