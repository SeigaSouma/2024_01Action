//=============================================================================
// 
// �����m���e�L�X�g���� [revivalpercent_text.cpp]
// Author : ���n�Ή�
// 
//=============================================================================
#include "revivalpercent_text.h"
#include "revivaltime_gauge.h"
#include "renderer.h"
#include "texture.h"
#include "manager.h"
#include "calculation.h"
#include "multinumber.h"

//==========================================================================
// �萔��`
//==========================================================================
namespace
{
	const char* TEXTURE = "data\\TEXTURE\\dead\\respawn_01.png";
	const char* NUMBER_TEXTURE = "data\\TEXTURE\\number\\number_blackclover_01.png";
}

//==========================================================================
// �֐��|�C���^
//==========================================================================
CRevivalPercentText::STATE_FUNC CRevivalPercentText::m_StateFunc[] =
{
	&CRevivalPercentText::StateNone,	// �Ȃ�
	&CRevivalPercentText::StateFadeIn,	// �t�F�[�h�C��
	&CRevivalPercentText::StateFadeOut,	// �t�F�[�h�A�E�g
};

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CRevivalPercentText::CRevivalPercentText(float fadetime, int nPriority) : m_fFadeTime(fadetime), CObject2D(nPriority)
{
	// �l�̃N���A
	m_state = STATE_NONE;	// ���
	m_fStateTime = 0.0f;	// ��ԃJ�E���^�[
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CRevivalPercentText::~CRevivalPercentText()
{

}

//==========================================================================
// ��������
//==========================================================================
CRevivalPercentText* CRevivalPercentText::Create(int respawnpercent, const float fadetime)
{
	// �����p�̃I�u�W�F�N�g
	CRevivalPercentText* pScreen = NULL;

	// �������̊m��
	pScreen = DEBUG_NEW CRevivalPercentText(fadetime);

	if (pScreen != NULL)
	{
		// ����������
		pScreen->Init();
		pScreen->SetNumberObj(respawnpercent);
	}

	return pScreen;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CRevivalPercentText::Init(void)
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
	D3DXVECTOR2 setsize = CTexture::GetInstance()->GetImageSize(nIdx);
	setsize = UtilFunc::Transformation::AdjustSizeByWidth(setsize, 320.0f);
	SetSize(setsize);
	SetSizeOrigin(GetSize());

	// �ʒu�ݒ�
	SetPosition(MyLib::Vector3(SCREEN_WIDTH * 0.5f, 500.0f, 0.0f));

	// �F�ݒ�
	SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));

	// ��ԃJ�E���^�[
	m_fStateTime = 0.0f;
	m_state = STATE_FADEIN;

	// �Q�[�W����
	MyLib::Vector3 spawnpos = GetPosition();
	spawnpos.y += 100.0f;
	CRevivalTimeGauge::Create(m_fFadeTime, spawnpos);
	return S_OK;
}

//==========================================================================
// �����I�u�W�F�N�g�ݒ�
//==========================================================================
void CRevivalPercentText::SetNumberObj(int number)
{
	MyLib::Vector3 spawnpos = GetPosition();
	spawnpos.x += 100.0f;
	m_apNumber = CMultiNumber::Create(spawnpos, D3DXVECTOR2(35.0f, 35.0f), 3, CNumber::OBJECTTYPE_2D, NUMBER_TEXTURE, true);	// �����̃I�u�W�F�N�g
	m_apNumber->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));
	m_apNumber->SetValue(number);
}

//==========================================================================
// �I������
//==========================================================================
void CRevivalPercentText::Uninit(void)
{
	// �I������
	CObject2D::Uninit();

	if (m_apNumber != nullptr)
	{
		m_apNumber->Uninit();
		m_apNumber = nullptr;
	}
}

//==========================================================================
// �폜����
//==========================================================================
void CRevivalPercentText::Kill(void)
{
	// �I������
	CObject2D::Uninit();

	if (m_apNumber != nullptr)
	{
		m_apNumber->Release();
		m_apNumber = nullptr;
	}
}

//==========================================================================
// �X�V����
//==========================================================================
void CRevivalPercentText::Update(void)
{
	// ��ԕʍX�V����
	(this->*(m_StateFunc[m_state]))();
	if (IsDeath())
	{
		return;
	}

	// �X�V����
	CObject2D::Update();

	// �����̍X�V����
	m_apNumber->Update();
}

//==========================================================================
// �Ȃɂ��Ȃ�
//==========================================================================
void CRevivalPercentText::StateNone(void)
{

}

//==========================================================================
// �t�F�[�h�C��
//==========================================================================
void CRevivalPercentText::StateFadeIn(void)
{
	// ��ԑJ�ڃJ�E���^�[���Z
	m_fStateTime += CManager::GetInstance()->GetDeltaTime();

	// �s�����x�X�V
	D3DXCOLOR col = GetColor();
	col.a = m_fStateTime / m_fFadeTime;
	SetColor(col);

	m_apNumber->SetColor(col);

	if (m_fStateTime >= m_fFadeTime)
	{
		m_fStateTime = m_fFadeTime;
		m_state = STATE_NONE;
		return;
	}
}

//==========================================================================
// �t�F�[�h�A�E�g
//==========================================================================
void CRevivalPercentText::StateFadeOut(void)
{
	// ��ԑJ�ڃJ�E���^�[���Z
	m_fStateTime -= CManager::GetInstance()->GetDeltaTime();

	// �s�����x�X�V
	D3DXCOLOR col = GetColor();
	col.a = m_fStateTime / m_fFadeTime;
	SetColor(col);

	m_apNumber->SetColor(col);

	if (m_fStateTime <= 0)
	{
		m_fStateTime = 0.0f;

		// �폜
		Kill();
		return;
	}
}

//==========================================================================
// �`�揈��
//==========================================================================
void CRevivalPercentText::Draw(void)
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
