//=============================================================================
// 
//  �{�X�̗̑̓Q�[�W���� [hp_gauge_boss.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "hp_gauge_boss.h"
#include "texture.h"
#include "manager.h"
#include "renderer.h"
#include "calculation.h"

//==========================================================================
// �}�N����`
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
}

#define WIDTH			(100.0f)	// ����
#define HEIGHT			(25.0f)		// �c��

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CHP_GaugeBoss::CHP_GaugeBoss(int nPriority) : CObject(nPriority)
{
	// �l�̃N���A
	for (int nCntGauge = 0; nCntGauge < VTXTYPE_MAX; nCntGauge++)
	{
		m_HPGauge[nCntGauge].pObj2D = NULL;
		m_HPGauge[nCntGauge].fMaxWidth = 0.0f;		// ���̍ő�l
		m_HPGauge[nCntGauge].fMaxHeight = 0.0f;		// �����̍ő�l
		m_HPGauge[nCntGauge].fWidthDest = 0.0f;		// ���̍���
	}

	m_nLife = 0;	// �̗�
	m_nMaxLife = 0;	// �ő�̗�
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
CHP_GaugeBoss *CHP_GaugeBoss::Create(MyLib::Vector3 pos, int nMaxLife)
{
	// �����p�̃I�u�W�F�N�g
	CHP_GaugeBoss *pHPGauge = NULL;

	if (pHPGauge == NULL)
	{// NULL��������

		// �������̊m��
		pHPGauge = DEBUG_NEW CHP_GaugeBoss;

		if (pHPGauge != NULL)
		{// �������̊m�ۂ��o���Ă�����

			// �ő�̗�
			pHPGauge->m_nMaxLife = nMaxLife;

			// �ʒu�ݒ�
			pHPGauge->SetPosition(pos);

			// ����������
			pHPGauge->Init();
		}

		return pHPGauge;
	}

	return NULL;
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
		if (m_HPGauge[nCntGauge].pObj2D == NULL)
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
		if (m_HPGauge[nCntGauge].pObj2D != NULL)
		{// NULL����Ȃ�������

			// �I������
			m_HPGauge[nCntGauge].pObj2D->Uninit();
			m_HPGauge[nCntGauge].pObj2D = NULL;
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
		if (m_HPGauge[nCntGauge].pObj2D != NULL)
		{// NULL����Ȃ�������

			// �I������
			m_HPGauge[nCntGauge].pObj2D->Uninit();
			m_HPGauge[nCntGauge].pObj2D = NULL;
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
	// �ʒu�擾
	MyLib::Vector3 pos = GetPosition();

	// ��������
	GaugeDecrement(VTXTYPE_GAUGE);
	AlwaysDecrement();

	for (int nCntGauge = 0; nCntGauge < VTXTYPE_MAX; nCntGauge++)
	{
		if (m_HPGauge[nCntGauge].pObj2D == NULL)
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

	for (int nCntGauge = 0; nCntGauge < VTXTYPE_MAX; nCntGauge++)
	{
		if (m_HPGauge[nCntGauge].pObj2D != NULL)
		{// NULL����Ȃ�������

			//�ڕW�̕��ݒ�
			m_HPGauge[nCntGauge].fWidthDest = m_HPGauge[nCntGauge].fMaxWidth * ((float)m_nLife / (float)m_nMaxLife);
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

	// �T�C�Y�擾
	D3DXVECTOR2 size = gauge->pObj2D->GetSize();
	size.x -= 0.08f;

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
