//=============================================================================
// 
// �X�L���c���[�R�}���h���� [skilltree_command.cpp]
// Author : ���n�Ή�
// 
//=============================================================================
#include "skilltree_command.h"
#include "texture.h"
#include "calculation.h"

//==========================================================================
// �萔��`
//==========================================================================
namespace
{
	const char* TEXTURE[] =
	{ 
		"data\\TEXTURE\\skilltree\\skilltree_back.png",
		"data\\TEXTURE\\skilltree\\skilltree_get.png",
		"data\\TEXTURE\\skilltree\\skilltree_reset.png",
	};
	const MyLib::Vector3 CREATE_POSITION = MyLib::Vector3(100.0f, 650.0f, 0.0f);	// �����ʒu
	const float DISTANCE = 180.0f;	// �Ԋu
}

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CSkillTree_Command::CSkillTree_Command(int nPriority, const LAYER layer) : CObject(nPriority, layer)
{
	// �l�̃N���A
	memset(m_pIcon, 0, sizeof(m_pIcon));
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CSkillTree_Command::~CSkillTree_Command()
{

}

//==========================================================================
// ��������
//==========================================================================
CSkillTree_Command* CSkillTree_Command::Create()
{
	// �������̊m��
	CSkillTree_Command* pEffect = DEBUG_NEW CSkillTree_Command;

	if (pEffect != nullptr)
	{// �������̊m�ۂ��o���Ă�����

		// ����������
		pEffect->Init();
	}

	return pEffect;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CSkillTree_Command::Init()
{
	HRESULT hr;

	// ��ނ̐ݒ�
	SetType(TYPE_OBJECT2D);

	for (int i = 0; i < ICONTYPE_MAX; i++)
	{
		m_pIcon[i] = CObject2D::Create(GetPriority());
		if (m_pIcon[i] == nullptr)
		{
			continue;
		}
		m_pIcon[i]->SetType(CObject::TYPE_OBJECT2D);

		// �e�N�X�`���̊��蓖��
		int nIdxTex = CTexture::GetInstance()->Regist(TEXTURE[i]);
		m_pIcon[i]->BindTexture(nIdxTex);

		// �����ݒ�
		MyLib::Vector3 pos = CREATE_POSITION;
		pos.x += DISTANCE * i;
		m_pIcon[i]->SetPosition(pos);

		// �e�N�X�`���T�C�Y�擾
		D3DXVECTOR2 size = CTexture::GetInstance()->GetImageSize(nIdxTex);
		m_pIcon[i]->SetSize(UtilFunc::Transformation::AdjustSizeByHeight(size, 50.0f));
	}

	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CSkillTree_Command::Uninit()
{
	for (int i = 0; i < ICONTYPE_MAX; i++)
	{
		if (m_pIcon[i] == nullptr)
		{
			continue;
		}
		m_pIcon[i]->Uninit();
		m_pIcon[i] = nullptr;
	}

	// �I������
	Release();
}

//==========================================================================
// �s�����x�ݒ�
//==========================================================================
void CSkillTree_Command::SetAlpha(float alpha)
{
	for (int i = 0; i < ICONTYPE_MAX; i++)
	{
		if (m_pIcon[i] == nullptr)
		{
			continue;
		}
		D3DXCOLOR col = m_pIcon[i]->GetColor();
		col.a = alpha;
		m_pIcon[i]->SetColor(col);
	}
}
