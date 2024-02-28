//=============================================================================
// 
// �X�L���c���[�E�B���h�E���� [skilltree_description.cpp]
// Author : ���n�Ή�
// 
//=============================================================================
#include "skilltree_description.h"
#include "renderer.h"
#include "texture.h"
#include "manager.h"
#include "calculation.h"

//==========================================================================
// �}�N����`
//==========================================================================
namespace
{
	const char* LOADTEXT = "data\\TEXT\\skilltree\\descriptiontexture.txt";
}

//==========================================================================
// �ÓI�����o�ϐ���`
//==========================================================================
std::vector<int> CSkillTree_Description::m_nTexIdx = {};	// �e�N�X�`���C���f�b�N�X�ԍ�
bool CSkillTree_Description::m_bLoadComplete = false;		// ���[�h�����̃t���O

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CSkillTree_Description::CSkillTree_Description(int nPriority) : CObject2D(nPriority)
{
	// �l�̃N���A
	m_nCurrentIdx = 0;	// �e�N�X�`���̃C���f�b�N�X�ԍ�
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CSkillTree_Description::~CSkillTree_Description()
{

}

//==========================================================================
// ��������
//==========================================================================
CSkillTree_Description* CSkillTree_Description::Create()
{
	// �����p�̃I�u�W�F�N�g
	CSkillTree_Description* pWindow = nullptr;

	// �������̊m��
	pWindow = DEBUG_NEW CSkillTree_Description;

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
HRESULT CSkillTree_Description::Init()
{
	// ����������
	HRESULT hr = CObject2D::Init();
	if (FAILED(hr))
	{// ���s�����Ƃ�
		return E_FAIL;
	}

	// ��ނ̐ݒ�
	SetType(TYPE_OBJECT2D);

	// �e�N�X�`���ǂݍ���
	if (!m_bLoadComplete)
	{
		ReadTexture();
	}

	// �e�N�X�`���̊��蓖��
	BindTexture(m_nTexIdx[0]);

	// �T�C�Y�ݒ�
	D3DXVECTOR2 size = CTexture::GetInstance()->GetImageSize(m_nTexIdx[0]);
	size = UtilFunc::Transformation::AdjustSizeByWidth(size, 160.0f);
	SetSize(size);
	SetSizeOrigin(GetSize());

	// �ʒu�ݒ�
	SetPosition(MyLib::Vector3(1050.0f, 430.0f, 0.0f));

	// �F�ݒ�
	SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));

	return S_OK;
}

//==========================================================================
// �X�V����
//==========================================================================
void CSkillTree_Description::Update()
{
	CObject2D::Update();

	// �e�N�X�`���̊��蓖��
	BindTexture(m_nTexIdx[m_nCurrentIdx]);
}

//==========================================================================
// �e�N�X�`���̃C���f�b�N�X�ԍ�
//==========================================================================
void CSkillTree_Description::SetIdxTex(int nIdx)
{ 
	int idx = nIdx;
	if (idx >= static_cast<int>(m_nTexIdx.size()))
	{
		idx = static_cast<int>(m_nTexIdx.size()) - 1;
	}
	m_nCurrentIdx = idx;
}

//==========================================================================
// �e�N�X�`���ǂݍ��ݏ���
//==========================================================================
HRESULT CSkillTree_Description::ReadTexture()
{
	char aComment[MAX_COMMENT] = {};	// �R�����g�p
	int nTexNum = 0;					// �t�@�C���̐�
	int nCntTexture = 0;				// �e�N�X�`���ǂݍ��݃J�E���g

	// �t�@�C���|�C���^
	FILE* pFile = NULL;

	//�t�@�C�����J��
	pFile = fopen(LOADTEXT, "r");

	if (pFile == NULL)
	{//�t�@�C�����J�����ꍇ
		return E_FAIL;
	}

	while (1)
	{// END_SCRIPT������܂ŌJ��Ԃ�

		// ������̓ǂݍ���
		fscanf(pFile, "%s", &aComment[0]);

		// �e�N�X�`�����̐ݒ�
		if (strcmp(&aComment[0], "NUM_TEXTURE") == 0)
		{// NUM_MODEL��������

			fscanf(pFile, "%s", &aComment[0]);	// =�̕�
			fscanf(pFile, "%d", &nTexNum);	// �e�N�X�`����
		}

		while (nCntTexture != nTexNum)
		{// �e�N�X�`���̐����ǂݍ��ނ܂ŌJ��Ԃ�

			fscanf(pFile, "%s", &aComment[0]);	// =�̕�

			if (strcmp(&aComment[0], "TEXTURE_FILENAME") == 0)
			{// TEXTURE_FILENAME��������

				fscanf(pFile, "%s", &aComment[0]);	// =�̕�
				fscanf(pFile, "%s", &aComment[0]);	// �t�@�C����

				// �e�N�X�`���̊��蓖��
				int nIdx = CTexture::GetInstance()->Regist(&aComment[0]);
				m_nTexIdx.push_back(nIdx);

				nCntTexture++;	// �e�N�X�`�������Z
			}
		}

		if (strcmp(&aComment[0], "END_SCRIPT") == 0)
		{// �I�������Ń��[�v�𔲂���

			break;
		}
	}

	// �t�@�C�������
	fclose(pFile);

	// ���[�h�����̃t���O�𗧂Ă�
	m_bLoadComplete = true;
	return S_OK;
}