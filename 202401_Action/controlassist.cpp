//=============================================================================
// 
// ����⏕���� [controlassist.cpp]
// Author : ���n�Ή�
// 
//=============================================================================
#include "controlassist.h"
#include "texture.h"
#include "calculation.h"

//==========================================================================
// �萔��`
//==========================================================================
namespace
{
	const char* TEXTURE[] =
	{ 
		"data\\TEXTURE\\control\\rockon.png",	// ���b�N�I��
		"data\\TEXTURE\\control\\counter.png",	// ����
	};
	const float WIDTH = 280.0f;	// ��
	const MyLib::Vector3 CREATE_POSITION = MyLib::Vector3(WIDTH - 5.0f, 200.0f, 0.0f);	// �����ʒu
}
CControlAssist* CControlAssist::m_pThisPtr= nullptr;		// ���g�̃|�C���^

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CControlAssist::CControlAssist(int nPriority) : CObject(nPriority)
{
	// �l�̃N���A
	m_nNumDisp = 0;	// �`�搔
	memset(m_pText, 0, sizeof(m_pText));
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CControlAssist::~CControlAssist()
{

}

//==========================================================================
// ��������
//==========================================================================
CControlAssist* CControlAssist::Create()
{
	if (m_pThisPtr != nullptr)
	{
		return m_pThisPtr;
	}

	// �������̊m��
	m_pThisPtr = DEBUG_NEW CControlAssist;

	if (m_pThisPtr != nullptr)
	{// �������̊m�ۂ��o���Ă�����

		// ����������
		m_pThisPtr->Init();
	}

	return m_pThisPtr;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CControlAssist::Init()
{
	HRESULT hr;

	// ��ނ̐ݒ�
	SetType(TYPE_OBJECT2D);

	for (int i = 0; i < CONTROLTYPE_MAX; i++)
	{
		m_pText[i] = CObject2D::Create(GetPriority());
		if (m_pText[i] == nullptr)
		{
			continue;
		}
		m_pText[i]->SetType(CObject::TYPE_OBJECT2D);

		// �e�N�X�`���̊��蓖��
		int nIdxTex = CTexture::GetInstance()->Regist(TEXTURE[i]);
		m_pText[i]->BindTexture(nIdxTex);

		// �����ݒ�
		m_pText[i]->SetPosition(CREATE_POSITION);

		// �e�N�X�`���T�C�Y�擾
		D3DXVECTOR2 size = CTexture::GetInstance()->GetImageSize(nIdxTex);
		m_pText[i]->SetSize(UtilFunc::Transformation::AdjustSizeByWidth(size, WIDTH));

		// �`��I�t
		m_pText[i]->SetEnableDisp(false);
	}

	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CControlAssist::Uninit()
{

	m_pThisPtr = nullptr;

	for (int i = 0; i < CONTROLTYPE_MAX; i++)
	{
		if (m_pText[i] == nullptr)
		{
			continue;
		}
		m_pText[i]->Uninit();
		m_pText[i] = nullptr;
	}

	// �I������
	Release();
}

//==========================================================================
// �e�L�X�g�ݒ�
//==========================================================================
void CControlAssist::SetText(CONTROLTYPE text)
{
	int type = static_cast<int>(text);
	if (m_pText[type] == nullptr)
	{
		return;
	}

	// �`��ON
	m_pText[type]->SetEnableDisp(true);
	m_pText[type]->SetPosition(CREATE_POSITION);

	// �ʒu�ݒ�
	float height = m_pText[type]->GetSize().y * 2.0f;
	m_pText[type]->AddPosition(MyLib::Vector3(0.0f, m_nNumDisp * height, 0.0f));

	// �`�搔���Z
	m_nNumDisp++;
}

//==========================================================================
// �e�L�X�g���Z�b�g
//==========================================================================
void CControlAssist::ResetText()
{
	// �`�搔���Z�b�g
	m_nNumDisp = 0;

	for (const auto& text : m_pText)
	{
		text->SetEnableDisp(false);
	}
}
