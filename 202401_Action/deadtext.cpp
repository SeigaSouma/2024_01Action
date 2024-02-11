//=============================================================================
// 
// ���S�e�L�X�g���� [deadtext.cpp]
// Author : ���n�Ή�
// 
//=============================================================================
#include "deadtext.h"
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
	const char* TEXTURE = "data\\TEXTURE\\dead\\deadtext_01.png";
}

//==========================================================================
// �֐��|�C���^
//==========================================================================
CDeadText::STATE_FUNC CDeadText::m_StateFunc[] =
{
	&CDeadText::StateNone,		// �Ȃ�
	&CDeadText::StateFadeIn,	// �t�F�[�h�C��
	&CDeadText::StateFadeOut,	// �t�F�[�h�A�E�g
};

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CDeadText::CDeadText(float fadein, float fadeout, int nPriority) : m_fFadeInTime(fadein), m_fFadeOutTime(fadeout), CObject2D(nPriority)
{
	// �l�̃N���A
	m_state = STATE_NONE;	// ���
	m_fStateTime = 0.0f;	// ��ԃJ�E���^�[
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CDeadText::~CDeadText()
{

}

//==========================================================================
// ��������
//==========================================================================
CDeadText* CDeadText::Create(const float fadein, const float fadeout)
{
	// �����p�̃I�u�W�F�N�g
	CDeadText* pScreen = NULL;

	// �������̊m��
	pScreen = DEBUG_NEW CDeadText(fadein, fadeout);

	if (pScreen != NULL)
	{
		// ����������
		pScreen->Init();
	}

	return pScreen;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CDeadText::Init()
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

	// �ʒu�ݒ�
	SetPosition(MyLib::Vector3(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f, 0.0f));

	// �F�ݒ�
	SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));

	// ��ԃJ�E���^�[
	m_fStateTime = 0.0f;
	m_state = STATE_FADEIN;
	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CDeadText::Uninit()
{
	// �I������
	CObject2D::Uninit();
}

//==========================================================================
// �X�V����
//==========================================================================
void CDeadText::Update()
{
	// ��ԕʍX�V����
	(this->*(m_StateFunc[m_state]))();
	if (IsDeath())
	{
		return;
	}

	// �X�V����
	CObject2D::Update();
}

//==========================================================================
// �Ȃɂ��Ȃ�
//==========================================================================
void CDeadText::StateNone()
{

}

//==========================================================================
// �t�F�[�h�C��
//==========================================================================
void CDeadText::StateFadeIn()
{
	// ��ԑJ�ڃJ�E���^�[���Z
	m_fStateTime += CManager::GetInstance()->GetDeltaTime();

	// �s�����x�X�V
	D3DXCOLOR col = GetColor();
	col.a = m_fStateTime / m_fFadeInTime;
	SetColor(col);

	if (m_fStateTime >= m_fFadeInTime)
	{
		m_fStateTime = m_fFadeOutTime;
		m_state = STATE_NONE;
		return;
	}
}

//==========================================================================
// �t�F�[�h�A�E�g
//==========================================================================
void CDeadText::StateFadeOut()
{
	// ��ԑJ�ڃJ�E���^�[���Z
	m_fStateTime -= CManager::GetInstance()->GetDeltaTime();

	// �s�����x�X�V
	D3DXCOLOR col = GetColor();
	col.a = m_fStateTime / m_fFadeOutTime;
	SetColor(col);

	if (m_fStateTime <= 0)
	{
		m_fStateTime = 0.0f;

		// �폜
		Uninit();
		return;
	}
}

//==========================================================================
// �`�揈��
//==========================================================================
void CDeadText::Draw()
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
