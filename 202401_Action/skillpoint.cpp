//=============================================================================
// 
//  �X�L���|�C���g���� [skillpoint.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "skillpoint.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"
#include "object2D.h"
#include "calculation.h"
#include "multinumber.h"
#include "input.h"

//==========================================================================
// �萔��`
//==========================================================================
namespace
{
	const char* TEXTURE = "data\\TEXTURE\\skillpoint\\skillpoint_01.png";
	const char* TEXTURE2 = "data\\TEXTURE\\leaf002.png";
	const char* NUMBER_TEXTURE = "data\\TEXTURE\\number\\number_blackclover_01.png";
	const float WIDTH_FRAME = 280.0f;	// �t���[���̕�
	const float TIME_SLIDE = 0.5f;		// �X���C�h�ɂ����鎞��
	const float TIME_DECIDE = 1.0f;		// ����
	const float TIME_ARRIVE = 2.0f;		// ����
	const MyLib::Vector3 DISTANCE_NUMBER = MyLib::Vector3(150.0f, 0.0f, 0.0f);
	const MyLib::Vector3 DESTPOSITION_SLIDEIN = MyLib::Vector3(SCREEN_WIDTH - WIDTH_FRAME, 80.0f, 0.0f);	// �X���C�h�C���̖ڕW�ʒu
	const MyLib::Vector3 DESTPOSITION_SLIDEOUT = MyLib::Vector3(SCREEN_WIDTH + WIDTH_FRAME, 80.0f, 0.0f);	// �X���C�h�A�E�g�̖ڕW�ʒu

}

//==========================================================================
// �֐����X�g
//==========================================================================
CSkillPoint::STATE_FUNC CSkillPoint::m_StateFunc[] =
{
	&CSkillPoint::StateArrive,			// ����
	&CSkillPoint::StateSlideIn,			// �X���C�h�C��
	&CSkillPoint::StateSlideCompletion,	// �X���C�h����
	&CSkillPoint::StateSlideOut,		// �X���C�h�A�E�g
	&CSkillPoint::StateWait,			// �ҋ@
	&CSkillPoint::StateEnhance,			// ����
};

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CSkillPoint::CSkillPoint(int nPriority) : CObject2D(nPriority)
{
	// �l�̃N���A
	m_nPoint = 0;			// �|�C���g
	m_nVisualValue = 0;		// �����ڏ�̐��l
	m_fStateTime = 0.0f;	// ��ԃ^�C�}�[
	m_fSlideTime = 0.0f;	// �X���C�h�^�C�}�[
	m_State = STATE_ARRIVE;	// ���
	m_apNumber = nullptr;	// �����̃I�u�W�F�N�g
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CSkillPoint::~CSkillPoint()
{

}

//==========================================================================
// ��������
//==========================================================================
CSkillPoint* CSkillPoint::Create()
{
	// �������̊m��
	CSkillPoint* pTitleScreen = DEBUG_NEW CSkillPoint;

	if (pTitleScreen != nullptr)
	{
		// ����������
		pTitleScreen->Init();
	}

	return pTitleScreen;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CSkillPoint::Init()
{
	// �e��ϐ�������
	m_State = STATE_WAIT;


	// ������
	CObject2D::Init();
	SetType(CObject::TYPE_OBJECT2D);

	// �e�N�X�`���̊��蓖��
	int nIdx = CTexture::GetInstance()->Regist(TEXTURE);
	BindTexture(nIdx);

	// �T�C�Y�ݒ�
	D3DXVECTOR2 size = CTexture::GetInstance()->GetImageSize(nIdx);
	size = UtilFunc::Transformation::AdjustSizeByWidth(size, WIDTH_FRAME);
	SetSize(size);
	SetSizeOrigin(GetSize());

	// �ʒu�ݒ�
	SetPosition(DESTPOSITION_SLIDEOUT);

	// ��������
	m_apNumber = CMultiNumber::Create(GetPosition() + DISTANCE_NUMBER, D3DXVECTOR2(50.0f, 50.0f), 2, CNumber::OBJECTTYPE_2D, NUMBER_TEXTURE, true, GetPriority());

	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CSkillPoint::Uninit()
{
	// �����̃I�u�W�F�N�g�̏I������
	if (m_apNumber != nullptr)
	{
		m_apNumber->Uninit();
		m_apNumber = nullptr;
	}

	// �I������
	CObject2D::Uninit();
}

//==========================================================================
// �폜����
//==========================================================================
void CSkillPoint::Kill()
{
	// �����̃I�u�W�F�N�g�폜
	if (m_apNumber != nullptr)
	{
		m_apNumber->Release();
		m_apNumber = nullptr;
	}

	// �I������
	CObject2D::Uninit();
}

//==========================================================================
// �X�V����
//==========================================================================
void CSkillPoint::Update()
{
	
	// ��ԍX�V
	(this->*(m_StateFunc[m_State]))();

	// �X�V����
	CObject2D::Update();

	// �l�̐ݒ菈��
	m_apNumber->SetPosition(GetPosition() + DISTANCE_NUMBER);
	m_apNumber->Update();
	m_apNumber->SetValue(m_nVisualValue);
}

//==========================================================================
// ����
//==========================================================================
void CSkillPoint::StateArrive()
{
	// �^�C�}�[�X�V
	m_fStateTime += CManager::GetInstance()->GetDeltaTime();
	m_fSlideTime = TIME_SLIDE;

	if (TIME_ARRIVE <= m_fStateTime)
	{
		m_fStateTime = 0.0f;
		m_State = STATE_SLIDEOUT;
	}
}

//==========================================================================
// �X���C�h�C��
//==========================================================================
void CSkillPoint::StateSlideIn()
{
	// �^�C�}�[���Z
	m_fStateTime += CManager::GetInstance()->GetDeltaTime();
	m_fSlideTime += CManager::GetInstance()->GetDeltaTime();

	// �ʒu�X�V
	MyLib::Vector3 pos = GetPosition();
	pos.x = UtilFunc::Correction::EasingEaseOut(DESTPOSITION_SLIDEOUT.x, DESTPOSITION_SLIDEIN.x, m_fSlideTime / TIME_SLIDE);
	SetPosition(pos);

	if (m_fSlideTime >= TIME_SLIDE)
	{
		m_fSlideTime = TIME_SLIDE;
		m_fStateTime = 0.0f;
		m_State = STATE_SLIDECOMPLETION;
	}
}

//==========================================================================
// �X���C�h����
//==========================================================================
void CSkillPoint::StateSlideCompletion()
{
	// �^�C�}�[���Z
	m_fStateTime += CManager::GetInstance()->GetDeltaTime();

	if (TIME_DECIDE <= m_fStateTime)
	{
		// �����ڏ�̐��l���X�V
		m_nVisualValue = m_nPoint;
		m_fStateTime = 0.0f;
		m_State = STATE_ARRIVE;
	}
}

//==========================================================================
// �X���C�h�A�E�g
//==========================================================================
void CSkillPoint::StateSlideOut()
{
	// �^�C�}�[�X�V
	m_fStateTime += CManager::GetInstance()->GetDeltaTime();
	m_fSlideTime -= CManager::GetInstance()->GetDeltaTime();

	// �ʒu�X�V
	MyLib::Vector3 pos = GetPosition();
	pos.x = UtilFunc::Correction::EasingEaseOut(DESTPOSITION_SLIDEIN.x, DESTPOSITION_SLIDEOUT.x, 1.0f - (m_fSlideTime / TIME_SLIDE));
	SetPosition(pos);

	if (m_fSlideTime <= 0.0f)
	{
		m_fSlideTime = 0.0f;
		m_fStateTime = 0.0f;
		m_State = STATE_WAIT;
	}
}

//==========================================================================
// �ҋ@
//==========================================================================
void CSkillPoint::StateWait()
{
	// �����ڏ�̐��l����ɍX�V
	m_nVisualValue = m_nPoint;

	// �ʒu�ݒ�
	SetPosition(DESTPOSITION_SLIDEOUT);
}

//==========================================================================
// ����
//==========================================================================
void CSkillPoint::StateEnhance()
{
	// �����ڏ�̐��l����ɍX�V
	m_nVisualValue = m_nPoint;

	// �ʒu�ݒ�
	SetPosition(DESTPOSITION_SLIDEIN);
}

//==========================================================================
// �|�C���g�ݒ�
//==========================================================================
void CSkillPoint::SetPoint(int nValue)
{
	m_nPoint = nValue;
	if (m_nPoint <= 0)
	{
		m_nPoint = 0;
	}
}

//==========================================================================
// �|�C���g�ǉ�
//==========================================================================
void CSkillPoint::AddPoint()
{
	m_nPoint++;
}

//==========================================================================
// �|�C���g�ǉ�
//==========================================================================
void CSkillPoint::AddPoint(int nValue)
{
	m_nPoint += nValue;
}

//==========================================================================
// �|�C���g�폜
//==========================================================================
void CSkillPoint::SubPoint()
{
	m_nPoint--;

	if (m_nPoint <= 0)
	{
		m_nPoint = 0;
	}
}

//==========================================================================
// �|�C���g�폜
//==========================================================================
void CSkillPoint::SubPoint(int nValue)
{
	m_nPoint -= nValue;
	if (m_nPoint <= 0)
	{
		m_nPoint = 0;
	}
}

//==========================================================================
// �X���C�h�C���ݒ�
//==========================================================================
void CSkillPoint::SetSlideIn()
{
	m_State = STATE_SLIDEIN;
	m_fStateTime = 0.0f;
	m_fSlideTime = 0.0f;
}

//==========================================================================
// ��Ԑݒ�
//==========================================================================
void CSkillPoint::SetState(STATE state)
{
	m_State = state;
	m_fStateTime = 0.0f;
}

//==========================================================================
// �`�揈��
//==========================================================================
void CSkillPoint::Draw()
{
	// �`�揈��
	CObject2D::Draw();
}

