//=============================================================================
// 
//  �������ԃQ�[�W���� [revivaltime_gauge.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "revivaltime_gauge.h"
#include "texture.h"
#include "manager.h"
#include "renderer.h"
#include "calculation.h"

//==========================================================================
// �萔��`
//==========================================================================
namespace
{
	const float DEFAULT_WIDTH = 120.0f;		// �f�t�H���g�̕�
	const float DEFAULT_HEIGHT = 15.0f;		// �f�t�H���g�̍���
	const float DEFAULT_TIMER = 4.0f;		// �f�t�H���g�̃^�C�}�[
	const char* TEXTURE[] =		// �e�N�X�`���̃t�@�C��
	{
		"",
		"",
		"data\\TEXTURE\\hpgauge\\hypnosis_fram.png",
	};
}

//==========================================================================
// �֐��|�C���^
//==========================================================================
CRevivalTimeGauge::STATE_FUNC CRevivalTimeGauge::m_StateFunc[] =
{
	&CRevivalTimeGauge::StateNone,		// �Ȃ�
	&CRevivalTimeGauge::StateFadeIn,	// �t�F�[�h�C��
	&CRevivalTimeGauge::StateFadeOut,	// �t�F�[�h�A�E�g
	&CRevivalTimeGauge::StateTimeComplete,	// ���Ԃ��Ȃ��Ȃ����u��
};

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CRevivalTimeGauge::CRevivalTimeGauge(float fadetime, int nPriority) : m_fFadeTime(fadetime), CObject(nPriority)
{
	// �l�̃N���A
	for (int nCntGauge = 0; nCntGauge < VTXTYPE_MAX; nCntGauge++)
	{
		m_pObj2DGauge[nCntGauge] = nullptr;	// 2D�Q�[�W�̃I�u�W�F�N�g
	}
	m_state = STATE_NONE;	// ���
	m_fStateTime = 0.0f;	// ��ԃJ�E���^�[
	m_fValue = 0.0f;		// �^�C�}�[�̒l
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CRevivalTimeGauge::~CRevivalTimeGauge()
{

}

//==========================================================================
// ��������
//==========================================================================
CRevivalTimeGauge* CRevivalTimeGauge::Create(const float fadetime, MyLib::Vector3 pos)
{
	// �����p�̃I�u�W�F�N�g
	CRevivalTimeGauge* pHPGauge = nullptr;

	// �������̊m��
	pHPGauge = DEBUG_NEW CRevivalTimeGauge(fadetime);

	if (pHPGauge != nullptr)
	{// �������̊m�ۂ��o���Ă�����

		// �ʒu�ݒ�
		pHPGauge->SetPosition(pos);
		pHPGauge->SetOriginPosition(pos);

		// ����������
		pHPGauge->Init();
	}

	return pHPGauge;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CRevivalTimeGauge::Init(void)
{
	// �e��ϐ��̏���������
	m_fValue = DEFAULT_TIMER;	// �^�C�}�[�̒l
	m_fStateTime = 0.0f;
	m_state = STATE_FADEIN;

	// ��ނ̐ݒ�
	SetType(TYPE_HPGAUGE);
	
	D3DXCOLOR col[] =
	{
		D3DXCOLOR(0.3f, 0.3f, 0.3f, 1.0f),
		D3DXCOLOR(1.0f, 0.7f, 0.3f, 1.0f),
		D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f)
	};
	for (int nCntGauge = 0; nCntGauge < VTXTYPE_MAX; nCntGauge++)
	{
		// ��������
		m_pObj2DGauge[nCntGauge] = CObject2D_Gauge::Create(DEFAULT_WIDTH, DEFAULT_HEIGHT, DEFAULT_TIMER * 60, TEXTURE[nCntGauge], GetPriority());
		if (m_pObj2DGauge[nCntGauge] == nullptr)
		{
			return E_FAIL;
		}

		m_pObj2DGauge[nCntGauge]->SetPosition(GetPosition());
		m_pObj2DGauge[nCntGauge]->SetOriginPosition(GetPosition());

		// ���_�J���[�̐ݒ�
		m_pObj2DGauge[nCntGauge]->SetColor(col[nCntGauge]);
		
		// ��ނ̐ݒ�
		m_pObj2DGauge[nCntGauge]->SetType(CObject::TYPE_OBJECT2D);
		m_pObj2DGauge[nCntGauge]->SetMoveFactor(0.25f);	// �ړ��̌W���ݒ�
	}

	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CRevivalTimeGauge::Uninit(void)
{
	for (int nCntGauge = 0; nCntGauge < VTXTYPE_MAX; nCntGauge++)
	{
		m_pObj2DGauge[nCntGauge] = nullptr;
	}

	// ���폜
	Release();
}

//==========================================================================
// �I������
//==========================================================================
void CRevivalTimeGauge::Kill(void)
{
	for (int nCntGauge = 0; nCntGauge < VTXTYPE_MAX; nCntGauge++)
	{
		if (m_pObj2DGauge[nCntGauge] != nullptr)
		{
			// �I������
			m_pObj2DGauge[nCntGauge]->Uninit();
			m_pObj2DGauge[nCntGauge] = nullptr;
		}
	}

	// ���폜
	Release();
}

//==========================================================================
// �X�V����
//==========================================================================
void CRevivalTimeGauge::Update(void)
{
	// �ʒu�擾
	MyLib::Vector3 pos = GetPosition();

	for (const auto& gauge : m_pObj2DGauge)
	{
		D3DXCOLOR col = gauge->GetColor();
		col.a = m_fStateTime / m_fFadeTime;
		gauge->SetColor(col);
	}

	// ��ԍX�V
	(this->*(m_StateFunc[m_state]))();
}

//==========================================================================
// �Ȃɂ��Ȃ�
//==========================================================================
void CRevivalTimeGauge::StateNone(void)
{
	m_fValue -= CManager::GetInstance()->GetDeltaTime();
	m_pObj2DGauge[VTXTYPE_GAUGE]->SetValue(static_cast<int>(m_fValue * 60));
	if (m_fValue <= 0.0f)
	{
		m_fValue = 0.0f;
		m_state = STATE_TIMECOMPLETE;
	}
}

//==========================================================================
// �t�F�[�h�C��
//==========================================================================
void CRevivalTimeGauge::StateFadeIn(void)
{
	// ��ԑJ�ڃJ�E���^�[���Z
	m_fStateTime += CManager::GetInstance()->GetDeltaTime();

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
void CRevivalTimeGauge::StateFadeOut(void)
{
	// ��ԑJ�ڃJ�E���^�[���Z
	m_fStateTime -= CManager::GetInstance()->GetDeltaTime();

	if (m_fStateTime <= 0)
	{
		m_fStateTime = 0.0f;

		// �폜
		Uninit();
		return;
	}
}

//==========================================================================
// ���Ԃ��Ȃ��Ȃ����u��
//==========================================================================
void CRevivalTimeGauge::StateTimeComplete(void)
{

}

//==========================================================================
// �`�揈��
//==========================================================================
void CRevivalTimeGauge::Draw(void)
{

}
