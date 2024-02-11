//=============================================================================
// 
// �X�L���c���[���C������ [skilltree_line.cpp]
// Author : ���n�Ή�
// 
//=============================================================================
#include "skilltree_line.h"
#include "renderer.h"
#include "texture.h"
#include "manager.h"
#include "sound.h"
#include "input.h"
#include "game.h"
#include "calculation.h"
#include "skilltree_icon.h"

//==========================================================================
// �}�N����`
//==========================================================================
namespace
{
	const char* TEXTURE = "data\\TEXTURE\\skilltree\\line_01.png";
	const float SIZE_HEIGHT = 15.0f;
}
CListManager<CSkillTree_Line> CSkillTree_Line::m_List = {};	// ���X�g

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CSkillTree_Line::CSkillTree_Line(int nPriority) : CObject2D(nPriority)
{
	// �l�̃N���A
	m_SkillLine = sSkillLine();	// �X�L�����C��
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CSkillTree_Line::~CSkillTree_Line()
{

}

//==========================================================================
// ��������
//==========================================================================
CSkillTree_Line* CSkillTree_Line::Create(const MyLib::Vector3& myposition, const MyLib::Vector3& parentposition)
{
	// �����p�̃I�u�W�F�N�g
	CSkillTree_Line* pEffect = nullptr;

	// �������̊m��
	pEffect = DEBUG_NEW CSkillTree_Line;

	if (pEffect != nullptr)
	{
		// �X�L�����C���̏��
		pEffect->m_SkillLine.mypos = myposition;
		pEffect->m_SkillLine.parentpos = parentposition;

		// ����������
		pEffect->Init();
	}

	return pEffect;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CSkillTree_Line::Init()
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

	float fSize = UtilFunc::Calculation::GetFabsPosLength2D(m_SkillLine.mypos, m_SkillLine.parentpos);
	fSize *= 0.5f;

	// �T�C�Y�ݒ�
	SetSize(D3DXVECTOR2(fSize, SIZE_HEIGHT));
	SetSizeOrigin(GetSize());


	// �ڕW�̊p�x�����߂�
	float rotZ = atan2f((m_SkillLine.mypos.x - m_SkillLine.parentpos.x), (m_SkillLine.mypos.y - m_SkillLine.parentpos.y));
	rotZ += D3DX_PI * 0.5f;
	UtilFunc::Transformation::RotNormalize(rotZ);

	// �����ݒ�
	SetRotation(MyLib::Vector3(0.0f, 0.0f, rotZ));


	MyLib::Vector3 distance = m_SkillLine.parentpos - m_SkillLine.mypos;
	distance *= 0.5f;
	SetPosition(m_SkillLine.mypos + distance);
	SetOriginPosition(GetPosition());


	// �F�ݒ�
	SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));

	// �ǉ�
	m_List.Regist(this);
	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CSkillTree_Line::Uninit()
{
	// �폜
	m_List.Delete(this);

	// �I������
	CObject2D::Uninit();
}

//==========================================================================
// �X�V����
//==========================================================================
void CSkillTree_Line::Update()
{
	MyLib::Vector3 distance = m_SkillLine.parentpos - m_SkillLine.mypos;
	distance *= 0.5f;
	SetOriginPosition(m_SkillLine.mypos + distance);

	SetPosition(GetPosition() + GetOriginPosition());

	// �X�V����
	CObject2D::Update();
}

//==========================================================================
// ���_�X�V
//==========================================================================
void CSkillTree_Line::SetVtx()
{
	MyLib::Vector3 distance = m_SkillLine.parentpos - m_SkillLine.mypos;
	distance *= 0.5f;
	SetOriginPosition(m_SkillLine.mypos + distance);

	SetPosition(GetPosition() + GetOriginPosition());

	CObject2D::SetVtx();
}

void CSkillTree_Line::SetSkillLineInfo(const MyLib::Vector3& myposition, const MyLib::Vector3& parentposition)
{
	// �X�L�����C���̏��
	m_SkillLine.mypos = myposition;
	m_SkillLine.parentpos = parentposition;
}

//==========================================================================
// �`�揈��
//==========================================================================
void CSkillTree_Line::Draw()
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

