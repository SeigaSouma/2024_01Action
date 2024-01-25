//=============================================================================
// 
//  �v���C���[�̗̑̓Q�[�W���� [hp_gauge_player.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "hp_gauge_player.h"
#include "texture.h"
#include "manager.h"
#include "renderer.h"
#include "calculation.h"

//==========================================================================
// �萔��`
//==========================================================================
namespace
{
	const float DEFAULT_WIDTH = 120.0f;
	const float DEFAULT_HEIGHT = 15.0f;
	const char* TEXTURE[] =		// �e�N�X�`���̃t�@�C��
	{
		/*"data\\TEXTURE\\hpgauge\\gaugeMoto.png",
	"data\\TEXTURE\\hpgauge\\gauge.png",
	"data\\TEXTURE\\hpgauge\\Fram.png",*/
		"",
		"",
		"data\\TEXTURE\\hpgauge\\hypnosis_fram.png",
	};
}

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CHP_GaugePlayer::CHP_GaugePlayer(int nPriority) : CObject(nPriority)
{
	// �l�̃N���A
	for (int nCntGauge = 0; nCntGauge < VTXTYPE_MAX; nCntGauge++)
	{
		m_pObj2DGauge[nCntGauge] = nullptr;
	}

	m_nLifeValue = 0;			// �l
	m_nMaxLifeValue = 0;		// �ő�l
	m_nOriginLifeValue = 0;	// �����l
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CHP_GaugePlayer::~CHP_GaugePlayer()
{

}

//==========================================================================
// ��������
//==========================================================================
CHP_GaugePlayer *CHP_GaugePlayer::Create(MyLib::Vector3 pos, int nMaxLife)
{
	// �����p�̃I�u�W�F�N�g
	CHP_GaugePlayer *pHPGauge = nullptr;

	if (pHPGauge == nullptr)
	{// nullptr��������

		// �������̊m��
		pHPGauge = DEBUG_NEW CHP_GaugePlayer;

		if (pHPGauge != nullptr)
		{// �������̊m�ۂ��o���Ă�����

			pHPGauge->m_nMaxLifeValue = nMaxLife;	// �X�^�~�i�̒l
			pHPGauge->m_nOriginLifeValue = nMaxLife;	// �X�^�~�i�̏����l

			// �ʒu�ݒ�
			pHPGauge->SetPosition(pos);

			// ����������
			pHPGauge->Init();

			// ��ނ̐ݒ�
			pHPGauge->SetType(TYPE_HPGAUGE);
		}

		return pHPGauge;
	}

	return nullptr;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CHP_GaugePlayer::Init(void)
{
	// �ő�̗�
	m_nLifeValue = m_nOriginLifeValue;

	D3DXCOLOR col[] =
	{
		D3DXCOLOR(0.3f, 0.3f, 0.3f, 1.0f),
		D3DXCOLOR(1.0f, 0.0f, 1.0f, 1.0f),
		D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f)
	};
	for (int nCntGauge = 0; nCntGauge < VTXTYPE_MAX; nCntGauge++)
	{
		// ��������
		m_pObj2DGauge[nCntGauge] = CObject2D_Gauge::Create(DEFAULT_WIDTH, DEFAULT_HEIGHT, m_nMaxLifeValue, TEXTURE[nCntGauge]);
		if (m_pObj2DGauge[nCntGauge] == nullptr)
		{
			return E_FAIL;
		}

		m_pObj2DGauge[nCntGauge]->SetOriginPosition(GetPosition());

		// ���_�J���[�̐ݒ�
		m_pObj2DGauge[nCntGauge]->SetColor(col[nCntGauge]);

		// ��ނ̐ݒ�
		m_pObj2DGauge[nCntGauge]->SetType(CObject::TYPE_OBJECT2D);
		m_pObj2DGauge[nCntGauge]->SetSize(D3DXVECTOR2(DEFAULT_WIDTH, DEFAULT_HEIGHT));

		if (nCntGauge == VTXTYPE_PINK)
		{
			m_pObj2DGauge[nCntGauge]->SetMoveFactor(0.15f);	// �ړ��̌W���ݒ�
		}
		else
		{
			m_pObj2DGauge[nCntGauge]->SetMoveFactor(1.0f);	// �ړ��̌W���ݒ�
		}
	}

	// �l�ݒ�
	SetLife(m_nMaxLifeValue);

	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CHP_GaugePlayer::Uninit(void)
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
void CHP_GaugePlayer::Kill(void)
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
void CHP_GaugePlayer::Update(void)
{
	// �ʒu�擾
	MyLib::Vector3 pos = GetPosition();

	for (const auto& gauge : m_pObj2DGauge)
	{
		gauge->SetPosition(pos);

		//// �F�擾
		//D3DXCOLOR col = m_pObj2DGauge[nCntGauge]->GetColor();

		//col = UtilFunc::Transformation::HSVtoRGB(0.0f, 0.0f, 1.0f + sinf(D3DX_PI * ((float)m_nCntTkTk / 60.0f)) * 0.3f);

		// �F�ݒ�
		//m_pObj2DGauge[nCntGauge]->SetColor(col);
	}
}

//==========================================================================
// �ʒu�X�V
//==========================================================================
void CHP_GaugePlayer::SetLife(int nLife)
{
	// ���݂̗̑͐ݒ�
	m_nLifeValue = nLife;
	m_pObj2DGauge[VTXTYPE_PINK]->SetValue(m_nLifeValue);
}

//==========================================================================
// �F�X�V
//==========================================================================
void CHP_GaugePlayer::ChangeColor(int nCntGauge)
{


	CObject2D_Gauge* pGauge = m_pObj2DGauge[nCntGauge];

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
	m_pObj2DGauge[nCntGauge]->SetColor(col);

	// �T�C�Y�擾
	m_pObj2DGauge[nCntGauge]->SetSize(size);

}


//==========================================================================
// �ő�l�̃A�b�v�O���[�h
//==========================================================================
int CHP_GaugePlayer::UpgradeMaxValue(int addvalue)
{
	int oldMaxLife = m_nMaxLifeValue;

	// �X�^�~�i�̍ő�l�㏸
	m_nMaxLifeValue += addvalue;

	for (const auto& gauge : m_pObj2DGauge)
	{
		SetPosition(gauge->UpgradeMaxValue(addvalue));
	}

	// ���݂̗̑͐ݒ�
	float ratio = static_cast<float>(m_nMaxLifeValue) / static_cast<float>(oldMaxLife);
	m_nLifeValue *= ratio;

	m_pObj2DGauge[VTXTYPE_PINK]->SetValue(m_nLifeValue);

	return m_nLifeValue;
}

//==========================================================================
// �`�揈��
//==========================================================================
void CHP_GaugePlayer::Draw(void)
{

}
