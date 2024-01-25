//=============================================================================
// 
//  �I�u�W�F�N�g2D�Q�[�W���� [object2D_gauge.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "object2D_gauge.h"
#include "texture.h"
#include "calculation.h"

//==========================================================================
// �萔��`
//==========================================================================
namespace
{
	const float DEFAULT_MOVEFACTOR = 0.15f;	// �f�t�H���g�̈ړ��W��
}

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CObject2D_Gauge::CObject2D_Gauge(int nPriority) : CObject2D(nPriority)
{
	m_fOriginWidth = 0.0f;	// ���̏����l
	m_fMaxWidth = 0.0f;		// ���̍ő�l
	m_fWidthDest = 0.0f;	// �ڕW�̕�
	m_fMaxHeight = 0.0f;	// �����̍ő�l
	m_fMoveFactor = 0.0f;	// �ړ��̌W��
	m_nValue = 0;			// ���݂̒l
	m_nOriginValue = 0;		// �����l
	m_nMaxValue = 0;		// �ő�l
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CObject2D_Gauge::~CObject2D_Gauge()
{

}

//==========================================================================
// ��������
//==========================================================================
CObject2D_Gauge *CObject2D_Gauge::Create(float width, float height, int maxvalue, std::string texturepath)
{
	// �����p�̃I�u�W�F�N�g
	CObject2D_Gauge *pObject2D = NULL;

	if (pObject2D == NULL)
	{// NULL��������

		// �������̊m��
		pObject2D = DEBUG_NEW CObject2D_Gauge;

		if (pObject2D != NULL)
		{// �������̊m�ۂ��o���Ă�����

			// �������
			pObject2D->m_fMaxWidth = width;
			pObject2D->m_fMaxHeight = height;
			pObject2D->m_nMaxValue = maxvalue;


			pObject2D->m_fOriginWidth = width;		// ���̏����l
			pObject2D->m_fMaxHeight = height;		// �����̍ő�l
			pObject2D->m_nOriginValue = maxvalue;	// �����l


			if (texturepath != "")
			{
				int nIdx = CTexture::GetInstance()->Regist(texturepath);

				// �e�N�X�`���̊��蓖��
				pObject2D->BindTexture(nIdx);
			}

			// ����������
			pObject2D->Init();
		}

		return pObject2D;
	}

	return NULL;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CObject2D_Gauge::Init(void)
{
	// ����������
	HRESULT hr = CObject2D::Init();
	if (FAILED(hr))
	{// ���s�����Ƃ�
		return E_FAIL;
	}

	// ���_���ݒ�
	SetVtx();

	// �l�ݒ�
	SetValue(m_nMaxValue);
	m_fMoveFactor = DEFAULT_MOVEFACTOR;	// �ړ��̌W��
	m_fMaxWidth = m_fOriginWidth;	// ���̍ő�l
	m_fWidthDest = m_fOriginWidth;	// �ڕW�̕�
	m_nMaxValue = m_nOriginValue;	// �ő�l
	m_nValue = m_nOriginValue;		// ���݂̒l

	return S_OK;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CObject2D_Gauge::Init(float width, float height, int maxvalue, std::string texturepath)
{
	// �������
	m_fOriginWidth = width;		// ���̏����l
	m_fMaxWidth = width;		// ���̍ő�l
	m_fWidthDest = width;		// �ڕW�̕�
	m_fMaxHeight = height;		// �����̍ő�l
	m_fMoveFactor = DEFAULT_MOVEFACTOR;	// �ړ��̌W��
	m_nOriginValue = maxvalue;	// �����l
	m_nMaxValue = maxvalue;		// �ő�l
	m_nValue = maxvalue;		// ���݂̒l

	// �l�ݒ�
	SetValue(m_nMaxValue);

	// ����������
	HRESULT hr = CObject2D::Init();
	if (FAILED(hr))
	{// ���s�����Ƃ�
		return E_FAIL;
	}

	if (texturepath != "")
	{
		int nIdx = CTexture::GetInstance()->Regist(texturepath);

		// �e�N�X�`���̊��蓖��
		BindTexture(nIdx);
	}

	// ���_���ݒ�
	SetVtx();

	return S_OK;
}

//==========================================================================
// �X�V����
//==========================================================================
void CObject2D_Gauge::Update(void)
{
	// ��������
	GaugeDecrement();

	// �X�V����
	CObject2D::Update();
}

//==========================================================================
// ��������
//==========================================================================
void CObject2D_Gauge::GaugeDecrement(void)
{
	// �T�C�Y�擾
	D3DXVECTOR2 size = GetSize();

	// �����ŏ��X�Ɍ��炵�Ă���
	size.x += (m_fWidthDest - size.x) * m_fMoveFactor;

	// �T�C�Y�ݒ�
	SetSize(size);
}

//==========================================================================
// �l�ݒ�
//==========================================================================
void CObject2D_Gauge::SetValue(int value)
{
	// ���݂̗̑͐ݒ�
	m_nValue = value;

	//�ڕW�̕��ݒ�
	m_fWidthDest = m_fMaxWidth * ((float)m_nValue / (float)m_nMaxValue);
}

//==========================================================================
// �ő�l�̃A�b�v�O���[�h
//==========================================================================
MyLib::Vector3 CObject2D_Gauge::UpgradeMaxValue(int addvalue)
{
	// �ő�l
	m_nMaxValue += addvalue;
	float nowlen = m_fOriginWidth * (static_cast<float>(m_nMaxValue) / static_cast<float>(m_nOriginValue));

	// �ő�̒���
	m_fMaxWidth = nowlen;

	// �Q�[�W���Z�b�g
	D3DXVECTOR2 size = GetSize();
	size.x = nowlen;
	SetSize(size);

	// �l�ݒ�
	SetValue(m_nMaxValue);

	// ���̒����Ƃ̍���
	float difflen = nowlen - m_fOriginWidth;

	// �ʒu�ݒ�
	MyLib::Vector3 newpos = GetOriginPosition();
	newpos.x += difflen;
	SetPosition(newpos);

	return newpos;
}

//==========================================================================
// ���_���ݒ菈��
//==========================================================================
void CObject2D_Gauge::SetVtx(void)
{
	// �X�V����
	CObject2D::SetVtx();

	// ���_���ւ̃|�C���^
	VERTEX_2D *pVtx;

	// ���_�o�b�t�@�����b�N���A���_���ւ̃|�C���^���擾
	GetVtxBuff()->Lock(0, 0, (void**)&pVtx, 0);

	// �ʒu�擾
	MyLib::Vector3 pos = GetPosition();

	// �T�C�Y�擾
	D3DXVECTOR2 size = GetSize();

	D3DXVECTOR2* pTex = GetTex();

	// ���_���W�̐ݒ�
	pVtx[0].pos = MyLib::Vector3(pos.x + -size.x - (m_fMaxWidth - size.x), pos.y + -size.y, 0.0f);
	pVtx[1].pos = MyLib::Vector3(pos.x + size.x - (m_fMaxWidth - size.x), pos.y + -size.y, 0.0f);
	pVtx[2].pos = MyLib::Vector3(pos.x + -size.x - (m_fMaxWidth - size.x), pos.y + size.y, 0.0f);
	pVtx[3].pos = MyLib::Vector3(pos.x + size.x - (m_fMaxWidth - size.x), pos.y + size.y, 0.0f);

	pTex[0] = D3DXVECTOR2(0.0f, 0.0f);
	pTex[1] = D3DXVECTOR2(size.x / m_fMaxWidth, 0.0f);
	pTex[2] = D3DXVECTOR2(0.0f, 1.0f);
	pTex[3] = D3DXVECTOR2(size.x / m_fMaxWidth, 1.0f);

	// ���_�o�b�t�@���A�����b�N���b�N
	GetVtxBuff()->Unlock();
}
