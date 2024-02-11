//=============================================================================
// 
// �X�L���c���[�E�B���h�E���� [skilltree_window.cpp]
// Author : ���n�Ή�
// 
//=============================================================================
#include "skilltree_window.h"
#include "renderer.h"
#include "texture.h"
#include "manager.h"
#include "calculation.h"

//==========================================================================
// �}�N����`
//==========================================================================
namespace
{
	const char* TEXTURE = "data\\TEXTURE\\skilltree\\skilltree_window3.png";
}

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CSkillTree_Window::CSkillTree_Window(int nPriority) : CObject2D(nPriority)
{
	// �l�̃N���A
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CSkillTree_Window::~CSkillTree_Window()
{

}

//==========================================================================
// ��������
//==========================================================================
CSkillTree_Window* CSkillTree_Window::Create()
{
	// �����p�̃I�u�W�F�N�g
	CSkillTree_Window* pWindow = nullptr;

	// �������̊m��
	pWindow = DEBUG_NEW CSkillTree_Window;

	if (pWindow != nullptr)
	{
		// ����������
		pWindow->Init();
	}

	return pWindow;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CSkillTree_Window::Init()
{
	// ����������
	HRESULT hr = CObject2D::Init();
	if (FAILED(hr))
	{// ���s�����Ƃ�
		return E_FAIL;
	}

	// ��ނ̐ݒ�
	SetType(TYPE_OBJECT2D);

	// �e�N�X�`���̊��蓖��
	int nIdx = CTexture::GetInstance()->Regist(TEXTURE);
	BindTexture(nIdx);

	// �T�C�Y�ݒ�
	D3DXVECTOR2 size = CTexture::GetInstance()->GetImageSize(nIdx);
	size = UtilFunc::Transformation::AdjustSizeByHeight(size, 300.0f);
	SetSize(size);
	SetSizeOrigin(GetSize());

	// �ʒu�ݒ�
	SetPosition(MyLib::Vector3(1050.0f, 450.0f, 0.0f));

	// �F�ݒ�
	SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));

	return S_OK;
}

