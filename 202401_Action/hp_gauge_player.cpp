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
#include "hp_gauge_tip.h"

//==========================================================================
// �萔��`
//==========================================================================
namespace
{
	const float DEFAULT_WIDTH = 150.0f;
	const float DEFAULT_HEIGHT = 18.0f;
	const char* TEXTURE[] =		// �e�N�X�`���̃t�@�C��
	{
		"data\\TEXTURE\\hpgauge\\black.png",
		"data\\TEXTURE\\hpgauge\\hpgauge.png",
	};
	const float LENGTH_TEXTUREREPEAT = 18.0f;	// �e�N�X�`�������s�[�g���钷��
}

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CHP_GaugePlayer::CHP_GaugePlayer(int nPriority) : CObject(nPriority)
{
	// �l�̃N���A
	memset(m_pObj2DGauge, 0, sizeof(m_pObj2DGauge));	// 2D�Q�[�W�̃I�u�W�F�N�g
	m_nLifeValue = 0;			// �l
	m_nMaxLifeValue = 0;		// �ő�l
	m_nOriginLifeValue = 0;	// �����l
	m_pTip = nullptr;	// �Q�[�W�̐�[
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
HRESULT CHP_GaugePlayer::Init()
{
	// �ő�̗�
	m_nLifeValue = m_nOriginLifeValue;

	for (int nCntGauge = 0; nCntGauge < VTXTYPE_MAX; nCntGauge++)
	{
		// ��������
		m_pObj2DGauge[nCntGauge] = CObject2D_Gauge::Create(DEFAULT_WIDTH, DEFAULT_HEIGHT, m_nMaxLifeValue, TEXTURE[nCntGauge], 11);
		if (m_pObj2DGauge[nCntGauge] == nullptr)
		{
			return E_FAIL;
		}

		m_pObj2DGauge[nCntGauge]->SetOriginPosition(GetPosition());

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


	// ��[����
	m_pTip = CHPGaugeTip::Create(GetPosition() - MyLib::Vector3(DEFAULT_WIDTH, 0.0f, 0.0f), GetPosition() + MyLib::Vector3(DEFAULT_WIDTH, 0.0f, 0.0f));

	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CHP_GaugePlayer::Uninit()
{
	for (int nCntGauge = 0; nCntGauge < VTXTYPE_MAX; nCntGauge++)
	{
		m_pObj2DGauge[nCntGauge] = nullptr;
	}

	if (m_pTip != nullptr)
	{
		m_pTip->Uninit();
		m_pTip = nullptr;
	}

	// ���폜
	Release();
}

//==========================================================================
// �I������
//==========================================================================
void CHP_GaugePlayer::Kill()
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

	if (m_pTip != nullptr)
	{
		m_pTip->Kill();
		m_pTip = nullptr;
	}

	// ���폜
	Release();
}

//==========================================================================
// �X�V����
//==========================================================================
void CHP_GaugePlayer::Update()
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


	for (int i = 0; i < VTXTYPE::VTXTYPE_MAX; i++)
	{
		// �T�C�Y�擾
		D3DXVECTOR2 size = m_pObj2DGauge[i]->GetSize();

		D3DXVECTOR2* pTex = m_pObj2DGauge[i]->GetTex();

		float ratio = size.x / LENGTH_TEXTUREREPEAT;

		pTex[1] = D3DXVECTOR2(ratio, 0.0f);
		pTex[3] = D3DXVECTOR2(ratio, 1.0f);

		SetVtx(i);
	}


	// ��[
	if (m_pTip != nullptr) {
		MyLib::Vector3 left, right;
		float maxlen = m_pObj2DGauge[0]->GetMaxWidth();

		left = pos - MyLib::Vector3(maxlen, 0.0f, 0.0f);
		right = pos + MyLib::Vector3(maxlen, 0.0f, 0.0f);
		m_pTip->SetLeftPosition(left);
		m_pTip->SetRightPosition(right);
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

	float ratio = ((float)m_nLifeValue / (float)m_nMaxLifeValue);
	if (ratio <= 0.3f &&
		m_pObj2DGauge[VTXTYPE_PINK] != nullptr)
	{
		m_pObj2DGauge[VTXTYPE_PINK]->SetColor(D3DXCOLOR(1.0f, 0.2f, 0.2f, 1.0f));
	}
	else
	{
		m_pObj2DGauge[VTXTYPE_PINK]->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	}
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

	// �̗͂̍ő�l�㏸
	m_nMaxLifeValue = m_nOriginLifeValue + addvalue;

	for (const auto& gauge : m_pObj2DGauge)
	{
		SetPosition(gauge->UpgradeMaxValue(addvalue, false));
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
void CHP_GaugePlayer::Draw()
{

}

void CHP_GaugePlayer::SetVtx(int nIdx)
{
	// ���_�ݒ�
	m_pObj2DGauge[nIdx]->SetVtx();

	// �ʒu�擾
	MyLib::Vector3 pos = GetPosition();

	D3DXVECTOR2* pTex = m_pObj2DGauge[nIdx]->GetTex();

	// ���_���ւ̃|�C���^
	VERTEX_2D* pVtx;

	// ���_�o�b�t�@�����b�N���A���_���ւ̃|�C���^���擾
	m_pObj2DGauge[nIdx]->GetVtxBuff()->Lock(0, 0, (void**)&pVtx, 0);

	// �T�C�Y�擾
	D3DXVECTOR2 size = m_pObj2DGauge[nIdx]->GetSize();

	// ���_���W�̐ݒ�
	pVtx[0].tex = pTex[0];
	pVtx[1].tex = pTex[1];
	pVtx[2].tex = pTex[2];
	pVtx[3].tex = pTex[3];

	// ���_�o�b�t�@���A�����b�N���b�N
	m_pObj2DGauge[nIdx]->GetVtxBuff()->Unlock();
}
