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
	const char* TEXTURE2 = "data\\TEXTURE\\leaf002.png";
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
	SetSizeOrigin(GetSize());	// �T�C�Y
	SetPosition(MyLib::Vector3(1000.0f, 80.0f, 0.0f));	// �ʒu


	//// �e�N�X�`���̊��蓖��
	//int nMultiIdx = CTexture::GetInstance()->Regist(TEXTURE2);
	//multi = CTexture::GetInstance()->GetAdress(nMultiIdx);

	// ��������
	m_apNumber = CMultiNumber::Create({1150.0f, 80.0f, 0.0f}, D3DXVECTOR2(GetSize().x, GetSize().x), 2, CNumber::OBJECTTYPE_2D, NUMBER_TEXTURE, true);

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
		m_apNumber = NULL;
	}

	// �I������
	CObject2D::Uninit();
}

//==========================================================================
// �폜����
//==========================================================================
void CSkillPoint::Kill(void)
{
	// �����̃I�u�W�F�N�g�폜
	if (m_apNumber != NULL)
	{
		m_apNumber->Release();
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
	// �L�[�{�[�h���擾
	CInputKeyboard* pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();



	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();


	// �Q�[�W�̈ʒu��T�C�Y�̍X�V�i�����ŃQ�[�W�̈ʒu��T�C�Y��ύX����j
	D3DXVECTOR2 size = GetSize();
	D3DXVECTOR2* pTex = GetTex();

	if (pInputKeyboard->GetPress(DIK_UP) == true)
	{//���L�[�������ꂽ,���ړ�]
		pTex[1].x += 0.01f;
		pTex[3].x += 0.01f;
		/*size.x += 1.0f;*/
	}
	if (pInputKeyboard->GetPress(DIK_DOWN) == true)
	{//���L�[�������ꂽ,���ړ�
		//size.x -= 1.0f;
		pTex[1].x -= 0.01f;
		pTex[3].x -= 0.01f;
	}
	SetSize(size);


	// �N���b�s���O�̈���Q�[�W�̐i���ɍ��킹�ĕύX
	float clipWidth = 2.0f * (GetSize().x / GetSizeOrigin().x) - 1.0f;



	// �X�V����
	CObject2D::Update();

	// �l�̐ݒ菈��
	m_apNumber->Update();
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
// �|�C���g�폜
//==========================================================================
void CSkillPoint::SubPoint(int nValue)
{
	m_nPoint -= nValue;
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

	// �`�揈��
	CObject2D::Draw();

}

