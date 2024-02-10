//=============================================================================
// 
//  �v���C���[�̃X�^�~�i�Q�[�W���� [stamina_gauge_player.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "stamina_gauge_player.h"
#include "texture.h"
#include "manager.h"
#include "renderer.h"
#include "calculation.h"

//==========================================================================
// �萔��`
//==========================================================================
namespace
{
	const float DEFAULT_AUTOHEAL = 0.15f;	// �f�t�H���g�̎�����
	const float DEFAULT_WIDTH = 120.0f;		// �f�t�H���g�̕�
	const float DEFAULT_HEIGHT = 15.0f;		// �f�t�H���g�̍���
	const float TIME_STATESUB = static_cast<float>(20) / static_cast<float>(mylib_const::DEFAULT_FPS);		// ���Z��Ԃ̎���
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
CStaminaGauge_Player::STATE_FUNC CStaminaGauge_Player::m_StateFunc[] =
{
	&CStaminaGauge_Player::StateNormal,	// �ʏ�
	&CStaminaGauge_Player::StateSub,	// ���Z
};

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CStaminaGauge_Player::CStaminaGauge_Player(int nPriority) : CObject(nPriority)
{
	// �l�̃N���A
	for (int nCntGauge = 0; nCntGauge < VTXTYPE_MAX; nCntGauge++)
	{
		m_pObj2DGauge[nCntGauge] = nullptr;	// 2D�Q�[�W�̃I�u�W�F�N�g
	}
	m_state = STATE_NORMAL;		// ���
	m_fStateTime = 0.0f;		// ��Ԃ̃J�E���^�[
	m_fStaminaValue = 0.0f;		// �X�^�~�i�̒l
	m_fMaxStaminaValue = 0.0f;	// �X�^�~�i�̍ő�l
	m_fOriginStaminaValue = 0.0f;	// �X�^�~�i�̏����l
	m_fAutoHeal = 0.0f;			// ������
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CStaminaGauge_Player::~CStaminaGauge_Player()
{

}

//==========================================================================
// ��������
//==========================================================================
CStaminaGauge_Player *CStaminaGauge_Player::Create(MyLib::Vector3 pos, int maxvalue)
{
	// �����p�̃I�u�W�F�N�g
	CStaminaGauge_Player *pHPGauge = nullptr;

	if (pHPGauge == nullptr)
	{// nullptr��������

		// �������̊m��
		pHPGauge = DEBUG_NEW CStaminaGauge_Player;

		if (pHPGauge != nullptr)
		{// �������̊m�ۂ��o���Ă�����

			pHPGauge->m_fMaxStaminaValue = static_cast<float>(maxvalue);	// �X�^�~�i�̒l
			pHPGauge->m_fOriginStaminaValue = static_cast<float>(maxvalue);	// �X�^�~�i�̏����l

			// �ʒu�ݒ�
			pHPGauge->SetPosition(pos);
			pHPGauge->SetOriginPosition(pos);

			// ����������
			pHPGauge->Init();
		}

		return pHPGauge;
	}

	return nullptr;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CStaminaGauge_Player::Init(void)
{
	// �e��ϐ��̏���������
	m_fStaminaValue = m_fMaxStaminaValue;	// �X�^�~�i�̒l
	m_fAutoHeal = DEFAULT_AUTOHEAL;			// ������

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
		m_pObj2DGauge[nCntGauge] = CObject2D_Gauge::Create(DEFAULT_WIDTH, DEFAULT_HEIGHT, static_cast<int>(m_fMaxStaminaValue), TEXTURE[nCntGauge]);
		if (m_pObj2DGauge[nCntGauge] == nullptr)
		{// nullptr��������
			return E_FAIL;
		}

		m_pObj2DGauge[nCntGauge]->SetOriginPosition(GetPosition());

		// ���_�J���[�̐ݒ�
		m_pObj2DGauge[nCntGauge]->SetColor(col[nCntGauge]);
		
		// ��ނ̐ݒ�
		m_pObj2DGauge[nCntGauge]->SetType(CObject::TYPE_OBJECT2D);
		m_pObj2DGauge[nCntGauge]->SetSize(D3DXVECTOR2(DEFAULT_WIDTH, DEFAULT_HEIGHT));
		m_pObj2DGauge[nCntGauge]->SetMoveFactor(0.25f);	// �ړ��̌W���ݒ�
	}

	// �l�ݒ�
	SetValue(m_fMaxStaminaValue);
	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CStaminaGauge_Player::Uninit(void)
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
void CStaminaGauge_Player::Kill(void)
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
void CStaminaGauge_Player::Update(void)
{
	// �ʒu�擾
	MyLib::Vector3 pos = GetPosition();

	for (const auto& gauge : m_pObj2DGauge)
	{
		gauge->SetPosition(pos);
	}

	// ��ԍX�V
	(this->*(m_StateFunc[m_state]))();
}

//==========================================================================
// �ʏ���
//==========================================================================
void CStaminaGauge_Player::StateNormal(void)
{
	// �J�E���^�[���Z�b�g
	m_fStateTime = 0.0f;

	// ������
	AddValue(m_fAutoHeal);
}

//==========================================================================
// ���Z���
//==========================================================================
void CStaminaGauge_Player::StateSub(void)
{
	// �J�E���^�[���Z
	m_fStateTime -= CManager::GetInstance()->GetDeltaTime();
	if (m_fStateTime <= 0.0f)
	{
		m_state = STATE_NORMAL;
	}
}

//==========================================================================
// �F�X�V
//==========================================================================
void CStaminaGauge_Player::ChangeColor(void)
{
	CObject2D_Gauge* pGauge = m_pObj2DGauge[VTXTYPE_GAUGE];

	// �T�C�Y�擾
	D3DXVECTOR2 size = pGauge->GetSize();

	// �F�擾
	D3DXCOLOR col = pGauge->GetColor();

	float maxwidth = pGauge->GetMaxWidth();

	if (
		size.x / maxwidth <= 0.95f &&
		size.x / maxwidth > 0.6f)
	{//HP�Q�[�W8��

		col = D3DXCOLOR(0.2f, 0.6f, 0.2f, 1.0f);
	}
	else if (
		size.x / maxwidth <= 0.6f &&
		size.x / maxwidth > 0.4f)
	{//HP�Q�[�W6��

		col = D3DXCOLOR(0.8f, 0.7f, 0.2f, 1.0f);
	}
	else if (
		size.x / maxwidth <= 0.4f &&
		size.x / maxwidth > 0.2f)
	{//HP�Q�[�W4��

		col = D3DXCOLOR(0.8f, 0.5f, 0.2f, 1.0f);
	}
	else if (size.x / maxwidth <= 0.2f)
	{//�댯�G���A

		col = D3DXCOLOR(0.8f, 0.2f, 0.2f, 1.0f);
	}
	else
	{//���^��

		col = D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f);
	}

	// �F�ݒ�
	pGauge->SetColor(col);

}

//==========================================================================
// �`�揈��
//==========================================================================
void CStaminaGauge_Player::Draw(void)
{

}

//==========================================================================
// �l���Z
//==========================================================================
void CStaminaGauge_Player::AddValue(float value)
{
	m_fStaminaValue += value;

	// �l�̕␳
	CorrectionValue();
}

//==========================================================================
// �l���Z
//==========================================================================
void CStaminaGauge_Player::SubValue(float value)
{
	// �X�^�~�i���Z
	m_fStaminaValue -= value;

	// ���Z��Ԃɐݒ�
	m_state = STATE_SUB;
	m_fStateTime = TIME_STATESUB;

	// �l�̕␳
	CorrectionValue();
}

//==========================================================================
// �l�ݒ�
//==========================================================================
void CStaminaGauge_Player::SetValue(float value)
{

	m_fStaminaValue = value;
	
	// �l�̕␳
	CorrectionValue();
}

//==========================================================================
// �ő�l�̃A�b�v�O���[�h
//==========================================================================
void CStaminaGauge_Player::UpgradeMaxValue(int addvalue)
{
#if 0
	// �ő�l
	float oldMaxvalue = m_fMaxStaminaValue;
	m_fMaxStaminaValue += addvalue;
	float nowlen = DEFAULT_WIDTH * (m_fMaxStaminaValue / m_fOriginStaminaValue);

	for (const auto& gauge : m_pObj2DGauge)
	{
		// �ő�l
		gauge->SetMaxValue(static_cast<int>(m_fMaxStaminaValue));

		// �ő�̒���
		gauge->SetMaxWidth(nowlen);

		// �Q�[�W���Z�b�g
		D3DXVECTOR2 size = gauge->GetSize();
		size.x = nowlen;
		gauge->SetSize(size);

		gauge->SetValue(static_cast<int>(m_fMaxStaminaValue));
	}

	// ���̒����Ƃ̍���
	float difflen = nowlen - DEFAULT_WIDTH;

	// �ʒu�ݒ�
	MyLib::Vector3 newpos = GetOriginPosition();
	newpos.x += difflen;
	SetPosition(newpos);

	for (const auto& gauge : m_pObj2DGauge)
	{
		gauge->SetPosition(newpos);
	}
#else

	// �X�^�~�i�̍ő�l�㏸
	m_fMaxStaminaValue = m_fOriginStaminaValue + addvalue;

	for (const auto& gauge : m_pObj2DGauge)
	{
		SetPosition(gauge->UpgradeMaxValue(addvalue));
	}
	
#endif
}

//==========================================================================
// �����񕜂̃A�b�v�O���[�h
//==========================================================================
void CStaminaGauge_Player::UpgradeAutoHeal(float multiply)
{
	// �����񕜋���
	m_fAutoHeal = DEFAULT_AUTOHEAL * multiply;
}

//==========================================================================
// �l�̕␳
//==========================================================================
void CStaminaGauge_Player::CorrectionValue(void)
{
	UtilFunc::Transformation::ValueNormalize(m_fStaminaValue, m_fMaxStaminaValue, 0.0f);

	m_pObj2DGauge[VTXTYPE_GAUGE]->SetValue(static_cast<int>(m_fStaminaValue));
}

//==========================================================================
// �l�擾
//==========================================================================
float CStaminaGauge_Player::GetValue(void)
{
	return m_fStaminaValue;
}