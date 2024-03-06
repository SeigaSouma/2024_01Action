//=============================================================================
// 
//  �t�F�[�h���� [loadscreen.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "loadscreen.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"
#include "object2D.h"

//==========================================================================
// �萔��`
//==========================================================================
namespace
{
	const char* TEXTURE = "data\\TEXTURE\\load\\loadscreen.jpg";
}


//==========================================================================
// �ÓI�����o�ϐ��錾
//==========================================================================

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CLoadScreen::CLoadScreen()
{
	// �l�̃N���A
	m_aObject2D = nullptr;					// �I�u�W�F�N�g2D�̃I�u�W�F�N�g
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CLoadScreen::~CLoadScreen()
{

}

//==========================================================================
// ��������
//==========================================================================
CLoadScreen *CLoadScreen::Create()
{
	// �����p�̃I�u�W�F�N�g
	CLoadScreen *pFade = nullptr;

	if (pFade == nullptr)
	{// nullptr��������

		// �������̊m��
		pFade = DEBUG_NEW CLoadScreen;

		if (pFade != nullptr)
		{// �������̊m�ۂ��o���Ă�����

			// ����������
			if (FAILED(pFade->Init()))
			{// ���s���Ă�����
				return nullptr;
			}
		}

		return pFade;
	}

	return nullptr;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CLoadScreen::Init()
{
	// ��������
	m_aObject2D = CObject2D_Anim::Create(0.0f, 5, 9, 2, false);
	if (m_aObject2D == nullptr)
	{// ���s���Ă�����
		return E_FAIL;
	}
	m_aObject2D->SetType(CObject::TYPE::TYPE_NONE);

	m_aObject2D->SetSize(D3DXVECTOR2(640.0f, 360.0f));	// �T�C�Y
	m_aObject2D->SetPosition(D3DXVECTOR3(640.0f, 360.0f, 0.0f));	// �ʒu
	m_aObject2D->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));	// �F�ݒ�

	// �e�N�X�`���̊��蓖��
	int nIdx = CTexture::GetInstance()->Regist(TEXTURE);

	// �e�N�X�`���̊��蓖��
	m_aObject2D->BindTexture(nIdx);

	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CLoadScreen::Uninit()
{
	if (m_aObject2D != nullptr)
	{// nullptr����Ȃ�������

		// �I������
		m_aObject2D->Uninit();
		m_aObject2D = nullptr;
	}
}

void CLoadScreen::Kill()
{
	if (m_aObject2D != nullptr)
	{
		// �I������
		m_aObject2D->Uninit();
		delete m_aObject2D;
		m_aObject2D = nullptr;
	}
}

//==========================================================================
// �X�V����
//==========================================================================
void CLoadScreen::Update()
{
	if (m_aObject2D == nullptr)
	{
		return;
	}

	// �F�擾
	D3DXCOLOR col = m_aObject2D->GetColor();

	// �X�V����
	m_aObject2D->Update();

	m_aObject2D->SetVtx();

}

//==========================================================================
// �`�揈��
//==========================================================================
void CLoadScreen::Draw()
{
	// �`�揈��
	m_aObject2D->Draw();
}

//==========================================================================
// �I�u�W�F�N�g2D�I�u�W�F�N�g�̎擾
//==========================================================================
CObject2D_Anim*CLoadScreen::GetMyObject()
{
	return m_aObject2D;
}