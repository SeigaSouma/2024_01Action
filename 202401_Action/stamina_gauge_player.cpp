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
#include "hp_gauge_tip.h"

//==========================================================================
// �萔��`
//==========================================================================
namespace
{
	const float DEFAULT_AUTOHEAL = 0.15f;	// �f�t�H���g�̎�����
	const float DEFAULT_WIDTH = 150.0f;		// �f�t�H���g�̕�
	const float DEFAULT_HEIGHT = 18.0;		// �f�t�H���g�̍���
	const float TIME_QUICKHEAL = 2.6f;		// �}���񕜂̎���
	const float TIME_STATESUB = static_cast<float>(50) / static_cast<float>(mylib_const::DEFAULT_FPS);		// ���Z��Ԃ̎���
	const char* TEXTURE[] =		// �e�N�X�`���̃t�@�C��
	{
		"data\\TEXTURE\\hpgauge\\black.png",
		"data\\TEXTURE\\hpgauge\\staminagauhge.png",
	};
	const float LENGTH_TEXTUREREPEAT = 18.0f;	// �e�N�X�`�������s�[�g���钷��
}

//==========================================================================
// �֐��|�C���^
//==========================================================================
CStaminaGauge_Player::STATE_FUNC CStaminaGauge_Player::m_StateFunc[] =
{
	&CStaminaGauge_Player::StateNormal,	// �ʏ�
	&CStaminaGauge_Player::StateSub,	// ���Z
	&CStaminaGauge_Player::StateQuickHealing,	// �}����
};

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CStaminaGauge_Player::CStaminaGauge_Player(int nPriority) : CObject(nPriority)
{
	// �l�̃N���A
	memset(m_pObj2DGauge, 0, sizeof(m_pObj2DGauge));	// 2D�Q�[�W�̃I�u�W�F�N�g
	m_state = STATE_NORMAL;		// ���
	m_fStateTime = 0.0f;		// ��Ԃ̃J�E���^�[
	m_fStaminaValue = 0.0f;		// �X�^�~�i�̒l
	m_fMaxStaminaValue = 0.0f;	// �X�^�~�i�̍ő�l
	m_fOriginStaminaValue = 0.0f;	// �X�^�~�i�̏����l
	m_fAutoHeal = 0.0f;			// ������
	m_pTip = nullptr;	// �Q�[�W�̐�[
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
HRESULT CStaminaGauge_Player::Init()
{
	// �e��ϐ��̏���������
	m_fStaminaValue = m_fMaxStaminaValue;	// �X�^�~�i�̒l
	m_fAutoHeal = DEFAULT_AUTOHEAL;			// ������

	// ��ނ̐ݒ�
	SetType(TYPE_HPGAUGE);
	
	for (int nCntGauge = 0; nCntGauge < VTXTYPE_MAX; nCntGauge++)
	{
		// ��������
		m_pObj2DGauge[nCntGauge] = CObject2D_Gauge::Create(DEFAULT_WIDTH, DEFAULT_HEIGHT, static_cast<int>(m_fMaxStaminaValue), TEXTURE[nCntGauge]);
		if (m_pObj2DGauge[nCntGauge] == nullptr)
		{
			return E_FAIL;
		}

		m_pObj2DGauge[nCntGauge]->SetOriginPosition(GetPosition());
		
		// ��ނ̐ݒ�
		m_pObj2DGauge[nCntGauge]->SetType(CObject::TYPE_OBJECT2D);
		m_pObj2DGauge[nCntGauge]->SetSize(D3DXVECTOR2(DEFAULT_WIDTH, DEFAULT_HEIGHT));
		m_pObj2DGauge[nCntGauge]->SetMoveFactor(0.25f);	// �ړ��̌W���ݒ�
	}

	// �l�ݒ�
	SetValue(m_fMaxStaminaValue);

	// ��[����
	m_pTip = CHPGaugeTip::Create(GetPosition() - MyLib::Vector3(DEFAULT_WIDTH, 0.0f, 0.0f), GetPosition() + MyLib::Vector3(DEFAULT_WIDTH, 0.0f, 0.0f));
	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CStaminaGauge_Player::Uninit()
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
void CStaminaGauge_Player::Kill()
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
void CStaminaGauge_Player::Update()
{
	// �ʒu�擾
	MyLib::Vector3 pos = GetPosition();

	for (const auto& gauge : m_pObj2DGauge)
	{
		gauge->SetPosition(pos);
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

	// ��[����
	if (m_pTip != nullptr) {
		MyLib::Vector3 left, right;
		float maxlen = m_pObj2DGauge[0]->GetMaxWidth();

		left = pos - MyLib::Vector3(maxlen, 0.0f, 0.0f);
		right = pos + MyLib::Vector3(maxlen, 0.0f, 0.0f);
		m_pTip->SetLeftPosition(left);
		m_pTip->SetRightPosition(right);
	}

	// ��ԍX�V
	(this->*(m_StateFunc[m_state]))();
}

//==========================================================================
// �ʏ���
//==========================================================================
void CStaminaGauge_Player::StateNormal()
{
	// �J�E���^�[���Z�b�g
	m_fStateTime = 0.0f;

	// ������
	AddValue(m_fAutoHeal);
}

//==========================================================================
// ���Z���
//==========================================================================
void CStaminaGauge_Player::StateSub()
{
	// �J�E���^�[���Z
	m_fStateTime -= CManager::GetInstance()->GetDeltaTime();
	if (m_fStateTime <= 0.0f)
	{
		m_state = STATE_NORMAL;
	}
}

//==========================================================================
// �}���񕜏��
//==========================================================================
void CStaminaGauge_Player::StateQuickHealing()
{
	// �J�E���^�[���Z�b�g
	m_fStateTime += CManager::GetInstance()->GetDeltaTime();
	UtilFunc::Transformation::ValueNormalize(m_fStateTime, TIME_QUICKHEAL, 0.0f);

	SetValue((m_fStateTime / TIME_QUICKHEAL) * m_fMaxStaminaValue);

	if (m_fMaxStaminaValue == m_fStaminaValue)
	{
		m_state = STATE_NORMAL;
	}
}

//==========================================================================
// �F�X�V
//==========================================================================
void CStaminaGauge_Player::ChangeColor()
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
void CStaminaGauge_Player::Draw()
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
	// �X�^�~�i�̍ő�l�㏸
	m_fMaxStaminaValue = m_fOriginStaminaValue + addvalue;

	for (const auto& gauge : m_pObj2DGauge)
	{
		SetPosition(gauge->UpgradeMaxValue(addvalue, false));
	}
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
void CStaminaGauge_Player::CorrectionValue()
{
	UtilFunc::Transformation::ValueNormalize(m_fStaminaValue, m_fMaxStaminaValue, 0.0f);

	m_pObj2DGauge[VTXTYPE_GAUGE]->SetValue(static_cast<int>(m_fStaminaValue));
}

//==========================================================================
// �l�擾
//==========================================================================
float CStaminaGauge_Player::GetValue()
{
	return m_fStaminaValue;
}

//==========================================================================
// ��Ԑݒ�
//==========================================================================
void CStaminaGauge_Player::SetState(STATE state)
{ 
	m_fStateTime = 0.0f;
	m_state = state;
}

//==========================================================================
// ��Ԑݒ�
//==========================================================================
void CStaminaGauge_Player::SetVtx(int nIdx)
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
