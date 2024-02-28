//=============================================================================
// 
// �^�C�g���G���^�[���� [title_pressenter.cpp]
// Author : ���n�Ή�
// 
//=============================================================================
#include "title_pressenter.h"
#include "title.h"
#include "manager.h"
#include "sound.h"
#include "calculation.h"
#include "input.h"
#include "fade.h"
#include "tutorial_checkshould.h"

//==========================================================================
// �萔��`
//==========================================================================
namespace
{
	const char* TEXTURE = "data\\TEXTURE\\title\\title_pressbutton.png";
	const float TIME_TUTORIAL_FADEOUT = 0.3f;	// �`���[�g���A���m�F�̃t�F�[�h�A�E�g
}

//==========================================================================
// �֐��|�C���^
//==========================================================================
CTitle_PressEnter::STATE_FUNC CTitle_PressEnter::m_StateFunc[] =
{
	&CTitle_PressEnter::StateNone,		// �Ȃ�
	&CTitle_PressEnter::StateFadeIn,	// �t�F�[�h�C��
	&CTitle_PressEnter::StateTutorial_FadeOut,		// �`���[�g���A���m�F�̃t�F�[�h�A�E�g
};

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CTitle_PressEnter::CTitle_PressEnter(float fadetime, int nPriority) : m_fFadeOutTime(fadetime), CObject2D(nPriority)
{
	// �l�̃N���A
	m_state = STATE::STATE_NONE;		// ���
	m_fStateTime = 0.0f;			// ��ԃJ�E���^�[
	m_pTutorialCheck = nullptr;		// �`���[�g���A����邩�m�F
}

//==========================================================================
// ��������
//==========================================================================
CTitle_PressEnter* CTitle_PressEnter::Create(float fadetime)
{
	// �������̊m��
	CTitle_PressEnter* pScreen = DEBUG_NEW CTitle_PressEnter(fadetime);

	if (pScreen != nullptr)
	{
		// ����������
		pScreen->Init();
	}

	return pScreen;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CTitle_PressEnter::Init()
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

	D3DXVECTOR2 size = CTexture::GetInstance()->GetImageSize(nIdx);
	size = UtilFunc::Transformation::AdjustSizeByWidth(size, 250.0f);

	// �T�C�Y�ݒ�
	SetSize(size);
	SetSizeOrigin(GetSize());

	// �ʒu�ݒ�
	SetPosition(MyLib::Vector3(SCREEN_WIDTH * 0.5f, 600.0f, 0.0f));

	// �F�ݒ�
	SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));

	// ��ԃJ�E���^�[
	m_fStateTime = m_fFadeOutTime;
	m_state = STATE_FADEIN;
	return S_OK;
}

//==========================================================================
// �X�V����
//==========================================================================
void CTitle_PressEnter::Update()
{
	if (CManager::GetInstance()->GetFade()->GetState() != CFade::STATE_NONE)
	{// �t�F�[�h���͔�����
		return;
	}

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
void CTitle_PressEnter::StateNone()
{
	// ���͏��擾
	CInputKeyboard* pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();
	CInputGamepad* pInputGamepad = CManager::GetInstance()->GetInputGamepad();

	if (pInputGamepad->GetTrigger(CInputGamepad::BUTTON::BUTTON_A, 0) ||
		pInputGamepad->GetTrigger(CInputGamepad::BUTTON::BUTTON_B, 0) ||
		pInputGamepad->GetTrigger(CInputGamepad::BUTTON::BUTTON_X, 0) ||
		pInputGamepad->GetTrigger(CInputGamepad::BUTTON::BUTTON_Y, 0) ||
		pInputGamepad->GetTrigger(CInputGamepad::BUTTON::BUTTON_START, 0) ||
		pInputKeyboard->GetTrigger(DIK_RETURN) ||
		pInputKeyboard->GetTrigger(DIK_SPACE) ||
		pInputKeyboard->GetTrigger(DIK_BACKSPACE)
		)
	{
		// �`���[�g���A���m�F�N���X����
		if (m_pTutorialCheck == nullptr)
		{
			m_pTutorialCheck = CTutorialCheckShould::Create();
		}
	}
}

//==========================================================================
// �t�F�[�h�C��
//==========================================================================
void CTitle_PressEnter::StateFadeIn()
{
	// ��ԑJ�ڃJ�E���^�[���Z
	m_fStateTime -= CManager::GetInstance()->GetDeltaTime();

	// �s�����x�X�V
	float alpha = 1.0f - (m_fStateTime / m_fFadeOutTime);
	SetAlpha(alpha);

	if (m_fStateTime <= 0.0f)
	{
		m_fStateTime = 0.0f;
		m_state = STATE_NONE;

		// �s�����x�X�V
		SetAlpha(1.0f);
		return;
	}
}

//==========================================================================
// �`���[�g���A���m�F�̃t�F�[�h�A�E�g
//==========================================================================
void CTitle_PressEnter::StateTutorial_FadeOut()
{
	// ��ԑJ�ڃJ�E���^�[���Z
	m_fStateTime += CManager::GetInstance()->GetDeltaTime();

	if (m_pTutorialCheck == nullptr)
	{
		return;
	}

	float alpha = UtilFunc::Correction::EasingLinear(1.0f, 0.0f, 0.0f, TIME_TUTORIAL_FADEOUT, m_fStateTime);

	m_pTutorialCheck->SetAlpha(alpha);

	if (m_fStateTime >= TIME_TUTORIAL_FADEOUT)
	{
		m_fStateTime = 0.0f;
		m_pTutorialCheck->Kill();
		m_pTutorialCheck = nullptr;
		m_state = STATE_NONE;
	}
}

//==========================================================================
// ��Ԑݒ�
//==========================================================================
void CTitle_PressEnter::SetState(STATE state)
{
	m_state = state;
	m_fStateTime = 0.0f;
}