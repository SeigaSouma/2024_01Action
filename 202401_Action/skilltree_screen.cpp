//=============================================================================
// 
// �X�L���c���[��ʃg���� [skilltree_screen.cpp]
// Author : ���n�Ή�
// 
//=============================================================================
#include "skilltree_screen.h"
#include "renderer.h"
#include "texture.h"
#include "manager.h"
#include "sound.h"
#include "calculation.h"

//==========================================================================
// �}�N����`
//==========================================================================
namespace
{
	const char* TEXTURE = "data\\TEXTURE\\stageclear_01.png";
	const float TIME_FADE = 1.0f;		// �t�F�[�h�A�E�g����
}

//==========================================================================
// �֐��|�C���^
//==========================================================================
CSkillTree_Screen::STATE_FUNC CSkillTree_Screen::m_StateFuncList[] =
{
	&CSkillTree_Screen::StateNone,		// �Ȃɂ��Ȃ�
	&CSkillTree_Screen::StateFadeIn,	// �t�F�[�h�C��
	&CSkillTree_Screen::StateFadeOut,	// �t�F�[�h�A�E�g
};

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CSkillTree_Screen::CSkillTree_Screen(int nPriority) : CObject2D(nPriority)
{
	// �l�̃N���A
	m_fStateTime = 0;		// ��ԑJ�ڃJ�E���^�[
	m_state = STATE_NONE;	// ���
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CSkillTree_Screen::~CSkillTree_Screen()
{

}

//==========================================================================
// ��������
//==========================================================================
CSkillTree_Screen* CSkillTree_Screen::Create(void)
{
	// �����p�̃I�u�W�F�N�g
	CSkillTree_Screen* pEffect = NULL;

	// �������̊m��
	pEffect = DEBUG_NEW CSkillTree_Screen;

	if (pEffect != NULL)
	{
		// ����������
		pEffect->Init();
	}

	return pEffect;
}

//==================================================================================
// ����������
//==================================================================================
HRESULT CSkillTree_Screen::Init(void)
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
	SetSize(D3DXVECTOR2(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f));
	SetSizeOrigin(GetSize());

	// ��ԃJ�E���^�[
	m_fStateTime = TIME_FADE;
	m_state = STATE_FADEIN;

	// �ʒu�ݒ�
	SetPosition(MyLib::Vector3(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f, 0.0f));

	return S_OK;
}

//==================================================================================
// �I������
//==================================================================================
void CSkillTree_Screen::Uninit(void)
{
	// �I������
	CObject2D::Uninit();
}

//==================================================================================
// �X�V����
//==================================================================================
void CSkillTree_Screen::Update(void)
{
	if (IsDeath())
	{
		return;
	}

	// ��ԕʏ���
	(this->*(m_StateFuncList[m_state]))();

	if (IsDeath())
	{
		return;
	}

	// ���_���W�̐ݒ�
	SetVtx();
}

//==================================================================================
// �����Ȃ����
//==================================================================================
void CSkillTree_Screen::StateNone(void)
{
	m_fStateTime = 0;
}

//==================================================================================
// �t�F�[�h�C�����
//==================================================================================
void CSkillTree_Screen::StateFadeIn(void)
{
	// �F�擾
	D3DXCOLOR col = GetColor();

	// ��ԑJ�ڃJ�E���^�[���Z
	m_fStateTime -= CManager::GetInstance()->GetDeltaTime();

	// �s�����x�X�V
	col.a = 1.0f - (m_fStateTime / TIME_FADE);

	// �F�ݒ�
	SetColor(col);

	if (m_fStateTime <= 0)
	{
		m_fStateTime = 0;
		return;
	}
}

//==================================================================================
// �t�F�[�h�A�E�g���
//==================================================================================
void CSkillTree_Screen::StateFadeOut(void)
{
	// �F�擾
	D3DXCOLOR col = GetColor();

	// ��ԑJ�ڃJ�E���^�[���Z
	m_fStateTime -= CManager::GetInstance()->GetDeltaTime();

	// �s�����x�X�V
	col.a = m_fStateTime / TIME_FADE;

	// �F�ݒ�
	SetColor(col);

	if (m_fStateTime <= 0)
	{
		m_fStateTime = 0;
		Uninit();
		return;
	}
}

//==================================================================================
// �`�揈��
//==================================================================================
void CSkillTree_Screen::Draw(void)
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

//==========================================================================
// ���_���ݒ菈��
//==========================================================================
void CSkillTree_Screen::SetVtx(void)
{
	// ���_�ݒ�
	CObject2D::SetVtx();
}

