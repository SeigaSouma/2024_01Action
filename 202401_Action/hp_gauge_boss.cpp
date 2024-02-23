//=============================================================================
// 
//  �{�X�̗̑̓Q�[�W���� [hp_gauge_boss.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "hp_gauge_boss.h"
#include "manager.h"
#include "calculation.h"

//==========================================================================
// �萔��`
//==========================================================================
namespace
{
	const char* TEXTURE[] =	// �e�N�X�`���̃t�@�C��
	{
		"data\\TEXTURE\\bossgauge\\bossHP_base.png",
		"data\\TEXTURE\\bossgauge\\bossHP_white.png",
		"data\\TEXTURE\\bossgauge\\bossHP_gauge.png",
		"data\\TEXTURE\\bossgauge\\bossHP_fram.png",
	};
	const float TIME_DECREMENT = 2.0f;	// ���������܂ł̎���
	const float TIME_DAMAGE = 0.3f;		// �_���[�W��Ԃ̎���
	const float LENGTH_DAMAGE = 9.0f;		// �_���[�W��Ԃ̒���
	const int NUM_SHAKE_DAMAGE = 4;		// �_���[�W�̐U����
}

//==========================================================================
// �֐����X�g
//==========================================================================
CHP_GaugeBoss::STATE_FUNC CHP_GaugeBoss::m_StateFuncList[] =
{
	&CHP_GaugeBoss::StateNone,		// �ʏ�
	&CHP_GaugeBoss::StateDamage,	// �_���[�W
};

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CHP_GaugeBoss::CHP_GaugeBoss(int nPriority) : CObject(nPriority)
{
	// �l�̃N���A
	for (int nCntGauge = 0; nCntGauge < VTXTYPE_MAX; nCntGauge++)
	{
		m_HPGauge[nCntGauge].pObj2D = nullptr;
		m_HPGauge[nCntGauge].fMaxWidth = 0.0f;		// ���̍ő�l
		m_HPGauge[nCntGauge].fMaxHeight = 0.0f;		// �����̍ő�l
		m_HPGauge[nCntGauge].fWidthDest = 0.0f;		// ���̍���
	}

	m_nLife = 0;					// �̗�
	m_nMaxLife = 0;					// �ő�̗�
	m_fDecrementTimer = 0.0f;		// �����^�C�}�[
	m_state = STATE::STATE_NONE;	// ���
	m_fStateTimer = 0.0f;			// ��ԃ^�C�}�[
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CHP_GaugeBoss::~CHP_GaugeBoss()
{

}

//==========================================================================
// ��������
//==========================================================================
CHP_GaugeBoss* CHP_GaugeBoss::Create(MyLib::Vector3 pos, int nMaxLife)
{

	// �������̊m��
	CHP_GaugeBoss* pHPGauge = DEBUG_NEW CHP_GaugeBoss;

	if (pHPGauge != nullptr)
	{// �������̊m�ۂ��o���Ă�����

		// �ő�̗�
		pHPGauge->m_nMaxLife = nMaxLife;

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
HRESULT CHP_GaugeBoss::Init()
{
	// ��ސݒ�
	SetType(CObject::TYPE_OBJECT2D);

	// �ő�̗�
	m_nLife = m_nMaxLife;

	for (int nCntGauge = 0; nCntGauge < VTXTYPE_MAX; nCntGauge++)
	{
		// ��������
		m_HPGauge[nCntGauge].pObj2D = CObject2D::Create(8);
		if (m_HPGauge[nCntGauge].pObj2D == nullptr)
		{
			return E_FAIL;
		}
		m_HPGauge[nCntGauge].pObj2D->SetType(CObject::TYPE::TYPE_NONE);

		// �e�N�X�`���̊��蓖��
		int nTexIdx = CTexture::GetInstance()->Regist(TEXTURE[nCntGauge]);

		// �e�N�X�`���̊��蓖��
		m_HPGauge[nCntGauge].pObj2D->BindTexture(nTexIdx);
	}


	int nTexIdx = CTexture::GetInstance()->Regist(TEXTURE[0]);

	// �T�C�Y
	D3DXVECTOR2 size = CTexture::GetInstance()->GetImageSize(nTexIdx);
	size = UtilFunc::Transformation::AdjustSizeByWidth(size, 350.0f);
	m_HPGauge[VTXTYPE::VTXTYPE_GAUGE].pObj2D->SetSize(size);

	// �T�C�Y
	D3DXVECTOR2 maxsize = m_HPGauge[VTXTYPE::VTXTYPE_GAUGE].pObj2D->GetSize();

	for (auto& info : m_HPGauge)
	{
		info.pObj2D->SetSize(size);

		// �e��ϐ��̏�����
		info.fMaxWidth = maxsize.x;		// ���̍ő�l
		info.fMaxHeight = maxsize.y;	// �����̍ő�l
	}

	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CHP_GaugeBoss::Uninit()
{
	for (int nCntGauge = 0; nCntGauge < VTXTYPE_MAX; nCntGauge++)
	{
		if (m_HPGauge[nCntGauge].pObj2D != nullptr)
		{
			// �I������
			m_HPGauge[nCntGauge].pObj2D->Uninit();
			m_HPGauge[nCntGauge].pObj2D = nullptr;
		}
	}

	// ���폜
	Release();
}

//==========================================================================
// �I������
//==========================================================================
void CHP_GaugeBoss::Kill()
{
	for (int nCntGauge = 0; nCntGauge < VTXTYPE_MAX; nCntGauge++)
	{
		if (m_HPGauge[nCntGauge].pObj2D != nullptr)
		{
			// �I������
			m_HPGauge[nCntGauge].pObj2D->Uninit();
			m_HPGauge[nCntGauge].pObj2D = nullptr;
		}
	}


	// ���폜
	Release();
}

//==========================================================================
// �X�V����
//==========================================================================
void CHP_GaugeBoss::Update()
{
	// ��ԕʏ���
	(this->*(m_StateFuncList[m_state]))();

	// ��������
	GaugeDecrement(VTXTYPE_GAUGE);
	AlwaysDecrement();

	// �ʒu�擾
	MyLib::Vector3 pos = GetPosition();
	for (int nCntGauge = 0; nCntGauge < VTXTYPE_MAX; nCntGauge++)
	{
		if (m_HPGauge[nCntGauge].pObj2D == nullptr)
		{
			continue;
		}

		// �ʒu�ݒ�
		m_HPGauge[nCntGauge].pObj2D->SetPosition(pos);

		// �X�V����
		m_HPGauge[nCntGauge].pObj2D->Update();

		// ���_���W�ݒ�
		SetVtx(nCntGauge);
	}

}

//==========================================================================
// �ʒu�X�V
//==========================================================================
void CHP_GaugeBoss::SetLife(int nLife)
{
	// ���݂̗̑͐ݒ�
	m_nLife = nLife;

	float ratio = ((float)m_nLife / (float)m_nMaxLife);

	if (ratio <= 0.3f &&
		m_HPGauge[VTXTYPE::VTXTYPE_GAUGE].pObj2D != nullptr)
	{
		m_HPGauge[VTXTYPE::VTXTYPE_GAUGE].pObj2D->SetColor(D3DXCOLOR(1.0f, 0.1f, 0.1f, 1.0f));
	}

	for (int nCntGauge = 0; nCntGauge < VTXTYPE_MAX; nCntGauge++)
	{
		if (m_HPGauge[nCntGauge].pObj2D != nullptr)
		{
			//�ڕW�̕��ݒ�
			m_HPGauge[nCntGauge].fWidthDest = m_HPGauge[nCntGauge].fMaxWidth * ratio;
		}
	}
}

//==========================================================================
// ��������
//==========================================================================
void CHP_GaugeBoss::GaugeDecrement(int nCntGauge)
{

	// �T�C�Y�擾
	D3DXVECTOR2 size = m_HPGauge[nCntGauge].pObj2D->GetSize();

	// �����ŏ��X�Ɍ��炵�Ă���
	size.x += (m_HPGauge[nCntGauge].fWidthDest - size.x) * 1.0f;

	// �T�C�Y�ݒ�
	m_HPGauge[nCntGauge].pObj2D->SetSize(size);

	// ���_���W�ݒ�
	SetVtx(nCntGauge);
}

//==========================================================================
// ��Ɍ����̏���
//==========================================================================
void CHP_GaugeBoss::AlwaysDecrement()
{
	SHP_Gauge* gauge = &m_HPGauge[VTXTYPE::VTXTYPE_DIFFGAUGE];

	// �������Ԍ��Z
	m_fDecrementTimer -= CManager::GetInstance()->GetDeltaTime();

	// �T�C�Y�擾
	D3DXVECTOR2 size = gauge->pObj2D->GetSize();
	size.x = UtilFunc::Correction::EasingLinear(m_fDecrementStart, m_fDecrementEnd, 0.0f, TIME_DECREMENT, TIME_DECREMENT - m_fDecrementTimer);

	if (size.x <= m_HPGauge[VTXTYPE::VTXTYPE_GAUGE].fWidthDest)
	{
		size.x = m_HPGauge[VTXTYPE::VTXTYPE_GAUGE].fWidthDest;
	}

	// �T�C�Y�ݒ�
	gauge->pObj2D->SetSize(size);

	// ���_���W�ݒ�
	SetVtx(VTXTYPE::VTXTYPE_DIFFGAUGE);
}

//==========================================================================
// �_���[�W�ݒ�
//==========================================================================
void CHP_GaugeBoss::SetDamage(int nLife)
{
	m_nLife = nLife;

	// �^�C�}�[���Z�b�g
	m_fDecrementTimer = TIME_DECREMENT;

	// ���̒����ݒ�
	float ratio = ((float)m_nLife / (float)m_nMaxLife);
	m_fDecrementStart = m_HPGauge[VTXTYPE::VTXTYPE_DIFFGAUGE].pObj2D->GetSize().x;
	m_fDecrementEnd = m_HPGauge[VTXTYPE::VTXTYPE_GAUGE].fMaxWidth * ratio;

	// �_���[�W��Ԑݒ�
	m_state = STATE::STATE_DAMAGE;
	m_fStateTimer = TIME_DAMAGE;
}

//==========================================================================
// �_���[�W
//==========================================================================
void CHP_GaugeBoss::StateDamage()
{
	// ��Ԏ��Ԍ��Z
	m_fStateTimer -= CManager::GetInstance()->GetDeltaTime();

	if (m_fStateTimer <= 0.0f)
	{
		m_state = STATE_NONE;
		m_fStateTimer = 0.0f;

		// �ʒu�ݒ�
		SetPosition(GetOriginPosition());
		return;
	}

	float ratio = m_fStateTimer / TIME_DAMAGE;
	float len = LENGTH_DAMAGE * ratio;
	float phai = D3DX_PI * (ratio * NUM_SHAKE_DAMAGE);

	MyLib::Vector3 pos = GetOriginPosition();
	pos.x += sinf(D3DX_PI * 0.75f) * (sinf(phai) * len);
	pos.y += cosf(D3DX_PI * 0.75f) * (sinf(phai) * len);
	SetPosition(pos);

}

//==========================================================================
// �`�揈��
//==========================================================================
void CHP_GaugeBoss::Draw()
{
	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();

	for (int nCntGauge = 0; nCntGauge < VTXTYPE_MAX; nCntGauge++)
	{
		// �`�揈��
		m_HPGauge[nCntGauge].pObj2D->Draw();
	}
}

//==========================================================================
// ���_���ݒ菈��
//==========================================================================
void CHP_GaugeBoss::SetVtx(int nCntGauge)
{
	// ���_�ݒ�
	m_HPGauge[nCntGauge].pObj2D->SetVtx();

	// �ʒu�擾
	MyLib::Vector3 pos = GetPosition();

	D3DXVECTOR2 *pTex = m_HPGauge[nCntGauge].pObj2D->GetTex();

	// ���_���ւ̃|�C���^
	VERTEX_2D *pVtx;

	// ���_�o�b�t�@�����b�N���A���_���ւ̃|�C���^���擾
	m_HPGauge[nCntGauge].pObj2D->GetVtxBuff()->Lock(0, 0, (void**)&pVtx, 0);

	// �T�C�Y�擾
	D3DXVECTOR2 size = m_HPGauge[nCntGauge].pObj2D->GetSize();

	// ���_���W�̐ݒ�
	pVtx[0].pos = MyLib::Vector3(pos.x + -size.x - (m_HPGauge[nCntGauge].fMaxWidth - size.x),	pos.y + -size.y, 0.0f);
	pVtx[1].pos = MyLib::Vector3(pos.x + size.x - (m_HPGauge[nCntGauge].fMaxWidth - size.x),	pos.y + -size.y, 0.0f);
	pVtx[2].pos = MyLib::Vector3(pos.x + -size.x - (m_HPGauge[nCntGauge].fMaxWidth - size.x),	pos.y + size.y, 0.0f);
	pVtx[3].pos = MyLib::Vector3(pos.x + size.x - (m_HPGauge[nCntGauge].fMaxWidth - size.x),	pos.y + size.y, 0.0f);

	pTex[0] = D3DXVECTOR2(0.0f, 0.0f);
	pTex[1] = D3DXVECTOR2(size.x / m_HPGauge[nCntGauge].fMaxWidth, 0.0f);
	pTex[2] = D3DXVECTOR2(0.0f, 1.0f);
	pTex[3] = D3DXVECTOR2(size.x / m_HPGauge[nCntGauge].fMaxWidth, 1.0f);

	// ���_�o�b�t�@���A�����b�N���b�N
	m_HPGauge[nCntGauge].pObj2D->GetVtxBuff()->Unlock();

}
