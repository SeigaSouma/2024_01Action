//=============================================================================
// 
// ���S��ʏ��� [deadscreen.cpp]
// Author : ���n�Ή�
// 
//=============================================================================
#include "deadscreen.h"
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
	const char* TEXTURE = "data\\TEXTURE\\dead\\BG_01.jpg";
	const float FADE_ALPHA = 0.4f;	// �t�F�[�h�̓����x
	const float TIME_FADE = static_cast<float>(40) / static_cast<float>(mylib_const::DEFAULT_FPS);	// �t�F�[�h�ɂ����鎞��
}

//==========================================================================
// �֐��|�C���^
//==========================================================================
CDeadScreen::STATE_FUNC CDeadScreen::m_StateFunc[] =
{
	&CDeadScreen::StateNone,	// �Ȃ�
	&CDeadScreen::StateFadeIn,	// �t�F�[�h�C��
	&CDeadScreen::StateFadeOut,	// �t�F�[�h�A�E�g
};

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CDeadScreen::CDeadScreen(int nPriority) : CObject2D(nPriority)
{
	// �l�̃N���A
	m_state = STATE_NONE;	// ���
	m_fStateTime = 0.0f;	// ��ԃJ�E���^�[
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CDeadScreen::~CDeadScreen()
{

}

//==========================================================================
// ��������
//==========================================================================
CDeadScreen* CDeadScreen::Create(void)
{
	// �����p�̃I�u�W�F�N�g
	CDeadScreen* pScreen = NULL;

	// �������̊m��
	pScreen = DEBUG_NEW CDeadScreen;

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
HRESULT CDeadScreen::Init(void)
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
void CDeadScreen::Uninit(void)
{
	// �I������
	CObject2D::Uninit();
}

//==========================================================================
// �X�V����
//==========================================================================
void CDeadScreen::Update(void)
{
	// ��ԕʍX�V����
	(this->*(m_StateFunc[m_state]))();

	// �X�V����
	CObject2D::Update();
}

//==========================================================================
// �Ȃɂ��Ȃ�
//==========================================================================
void CDeadScreen::StateNone(void)
{

}

//==========================================================================
// �t�F�[�h�C��
//==========================================================================
void CDeadScreen::StateFadeIn(void)
{
	// ��ԑJ�ڃJ�E���^�[���Z
	m_fStateTime += CManager::GetInstance()->GetDeltaTime();

	// �s�����x�X�V
	D3DXCOLOR col = GetColor();
	col.a = FADE_ALPHA * (m_fStateTime / TIME_FADE);
	SetColor(col);

	if (m_fStateTime >= TIME_FADE)
	{
		m_fStateTime = TIME_FADE;
		m_state = STATE_NONE;
		return;
	}
}

//==========================================================================
// �t�F�[�h�A�E�g
//==========================================================================
void CDeadScreen::StateFadeOut(void)
{
	// ��ԑJ�ڃJ�E���^�[���Z
	m_fStateTime -= CManager::GetInstance()->GetDeltaTime();

	// �s�����x�X�V
	D3DXCOLOR col = GetColor();
	col.a = FADE_ALPHA * (m_fStateTime / TIME_FADE);
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
void CDeadScreen::Draw(void)
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
