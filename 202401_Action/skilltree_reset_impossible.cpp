//=============================================================================
// 
// �X�L���c���[���Z�b�g�s�\���� [skilltree_reset_impossible.cpp]
// Author : ���n�Ή�
// 
//=============================================================================
#include "skilltree_reset_impossible.h"
#include "renderer.h"
#include "texture.h"
#include "manager.h"
#include "sound.h"
#include "calculation.h"

//==========================================================================
// �萔��`
//==========================================================================
namespace
{
	const char* TEXTURE[] =
	{
		"data\\TEXTURE\\skilltree\\reset_inpossible_01.png",
		"data\\TEXTURE\\skilltree\\reset_inpossible02.png",
	};
	const float SPEED_VERTICAL = 0.3f;		// �㏸���x
	const float TIME_START_FADEOUT = 0.9f;	// �������Ă���t�F�[�h�A�E�g�����܂ł̎���
	const float LIFE = 1.2f;				// ����
}
CSkillTree_ResetImpossible* CSkillTree_ResetImpossible::m_pThisPtr = nullptr;	// ���g�̃|�C���^

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CSkillTree_ResetImpossible::CSkillTree_ResetImpossible(int nPriority) : CObject2D(nPriority)
{
	// �l�̃N���A
	m_ErrorType = ERRORTYPE_POINT;	// �G���[�̎��
	m_fAlphaTime = 0.0f;			// �s�����x�^�C�}�[
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CSkillTree_ResetImpossible::~CSkillTree_ResetImpossible()
{

}

//==========================================================================
// ��������
//==========================================================================
CSkillTree_ResetImpossible* CSkillTree_ResetImpossible::Create(ERRORTYPE error)
{
	if (m_pThisPtr != nullptr)
	{
		return m_pThisPtr;
	}

	// �������̊m��
	m_pThisPtr = DEBUG_NEW CSkillTree_ResetImpossible;

	if (m_pThisPtr != nullptr)
	{
		m_pThisPtr->m_ErrorType = error;	// �G���[�̎��

		// ����������
		if (FAILED(m_pThisPtr->Init()))
		{
			return nullptr;
		}
	}

	return m_pThisPtr;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CSkillTree_ResetImpossible::Init()
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
	int nIdx = CTexture::GetInstance()->Regist(TEXTURE[static_cast<int>(m_ErrorType)]);
	BindTexture(nIdx);

	// �T�C�Y�ݒ�
	D3DXVECTOR2 size = CTexture::GetInstance()->GetImageSize(nIdx);
	size = UtilFunc::Transformation::AdjustSizeByHeight(size, 50.0f);
	SetSize(size);
	SetSizeOrigin(GetSize());

	// �ʒu�ݒ�
	SetPosition(MyLib::Vector3(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f, 0.0f));

	// �F�ݒ�
	//SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));

	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CSkillTree_ResetImpossible::Uninit()
{
	m_pThisPtr = nullptr;

	// �I������
	CObject2D::Uninit();
}

//==========================================================================
// �X�V����
//==========================================================================
void CSkillTree_ResetImpossible::Update()
{

	// �ʒu�ړ�
	AddPosition(MyLib::Vector3(0.0f, -SPEED_VERTICAL, 0.0f));

	// �s�����x���ԉ��Z
	m_fAlphaTime += CManager::GetInstance()->GetDeltaTime();

	if (m_fAlphaTime >= TIME_START_FADEOUT)
	{
		float time = m_fAlphaTime - TIME_START_FADEOUT;
		float timeleft = LIFE - TIME_START_FADEOUT;	// �c�莞��

		// �s�����x�X�V
		D3DXCOLOR col = GetColor();
		col.a = 1.0f - (time / timeleft);
		SetColor(col);
	}

	// �X�V����
	CObject2D::Update();

	if (m_fAlphaTime >= LIFE)
	{
		Uninit();
	}
}

//==========================================================================
// �`�揈��
//==========================================================================
void CSkillTree_ResetImpossible::Draw()
{
	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();

	// �A���t�@�e�X�g��L���ɂ���
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);

	// �`�揈��
	CObject2D::Draw();

	// �A���t�@�e�X�g��L���ɂ���
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);
}
