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
	const char* TEXTURE = "data\\TEXTURE\\load\\runningman102.png";
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
CLoadScreen *CLoadScreen::Create(void)
{
	// �����p�̃I�u�W�F�N�g
	CLoadScreen *pFade = NULL;

	if (pFade == NULL)
	{// NULL��������

		// �������̊m��
		pFade = DEBUG_NEW CLoadScreen;

		if (pFade != NULL)
		{// �������̊m�ۂ��o���Ă�����

			// ����������
			if (FAILED(pFade->Init()))
			{// ���s���Ă�����
				return NULL;
			}
		}

		return pFade;
	}

	return NULL;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CLoadScreen::Init(void)
{
	// ��������
	m_aObject2D = CObject2D_Anim::Create(0.0f, 5, 2, 2, false);
	if (m_aObject2D == NULL)
	{// ���s���Ă�����
		return E_FAIL;
	}
	
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
void CLoadScreen::Uninit(void)
{
	if (m_aObject2D != NULL)
	{// NULL����Ȃ�������

		// �I������
		m_aObject2D->Uninit();
		m_aObject2D = NULL;
	}
}

//==========================================================================
// �X�V����
//==========================================================================
void CLoadScreen::Update(void)
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
void CLoadScreen::Draw(void)
{
	// �`�揈��
	m_aObject2D->Draw();
}

//==========================================================================
// �I�u�W�F�N�g2D�I�u�W�F�N�g�̎擾
//==========================================================================
CObject2D_Anim*CLoadScreen::GetMyObject(void)
{
	return m_aObject2D;
}