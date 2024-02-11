//=============================================================================
// 
//  �I�u�W�F�N�g2D���� [object2D_Multi.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "object2D_Multi.h"
#include "manager.h"
#include "renderer.h"
#include "player.h"
#include "enemy.h"
#include "bg.h"
#include "texture.h"
#include "pause.h"

//==========================================================================
// �}�N����`
//==========================================================================
#define TEXTURE_NAME	"data\\TEXTURE\\titlelog_00.png"	// �e�N�X�`����
#define POLYGON_TOP		(4)								// �|���S���̒��_��
#define WIDTH			(80.0f)							// ����
#define HEIGHT			(60.0f)							// �c��
#define ANIM_SPEED		(10)							// �ǂݍ��݊Ԋu
#define MAX_PATTERN_U	(5)								// U�̕�����
#define MAX_PATTERN_V	(2)								// V�̕�����
#define MAX_PATTERN		(MAX_PATTERN_U * MAX_PATTERN_V)
#define MOVE_U			(1.0f / (float)MAX_PATTERN_U)	// U���W�ړ���
#define MOVE_V			(1.0f / (float)MAX_PATTERN_V)	// V���W�ړ���

//==========================================================================
// �ÓI�����o�ϐ��錾
//==========================================================================

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CObject2DMulti::CObject2DMulti(int nPriority) : CObject(nPriority)
{
	m_col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);	// �F
	m_size = D3DXVECTOR2(0.0f, 0.0f);			// �T�C�Y
	m_sizeOrigin = D3DXVECTOR2(0.0f, 0.0f);		// ���̃T�C�Y
	m_fLength = 0.0f;							// �Ίp���̒���
	m_fAngle = 0.0f;							// �Ίp���̌���
	m_pVtxBuff = NULL;							// ���_�o�b�t�@
	m_nTexIdx = 0;								// �e�N�X�`���̃C���f�b�N�X�ԍ�

	for (int nCntVtx = 0; nCntVtx < 32; nCntVtx++)
	{
		m_fTex[nCntVtx] = D3DXVECTOR2(0.0f, 0.0f);			// �e�N�X�`�����W
		m_VtxPos[nCntVtx] = MyLib::Vector3(0.0f, 0.0f, 0.0f);	// ���_���W
	}

}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CObject2DMulti::~CObject2DMulti()
{

}

//==========================================================================
// �e�N�X�`���̊��蓖��
//==========================================================================
void CObject2DMulti::BindTexture(int nIdx)
{
	// ���蓖�Ă�
	m_nTexIdx = nIdx;
}

//==========================================================================
// ��������
//==========================================================================
CObject2DMulti *CObject2DMulti::Create()
{
	// �����p�̃I�u�W�F�N�g
	CObject2DMulti *pObject2D = NULL;

	if (pObject2D == NULL)
	{// NULL��������

		// �������̊m��
		pObject2D = DEBUG_NEW CObject2DMulti;

		if (pObject2D != NULL)
		{// �������̊m�ۂ��o���Ă�����

			// ����������
			pObject2D->Init();
		}

		return pObject2D;
	}

	return NULL;
}

//==========================================================================
// ��������
//==========================================================================
CObject2DMulti *CObject2DMulti::Create(int nPriority)
{
	// �����p�̃I�u�W�F�N�g
	CObject2DMulti *pObject2D = NULL;

	if (pObject2D == NULL)
	{// NULL��������

		// �������̊m��
		pObject2D = DEBUG_NEW CObject2DMulti(nPriority);

		if (pObject2D != NULL)
		{// �������̊m�ۂ��o���Ă�����

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
HRESULT CObject2DMulti::Init()
{
	HRESULT hr;

	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();

	m_fTex[0] = D3DXVECTOR2(0.0f, 0.0f);	// �e�N�X�`�����W
	m_fTex[1] = D3DXVECTOR2(1.0f, 0.0f);	// �e�N�X�`�����W
	m_fTex[2] = D3DXVECTOR2(0.0f, 1.0f);	// �e�N�X�`�����W
	m_fTex[3] = D3DXVECTOR2(1.0f, 1.0f);	// �e�N�X�`�����W

	// ���_�o�b�t�@�̐���
	if (m_pVtxBuff != NULL)
	{// ���ɏ�񂪓����Ă�ꍇ

		return E_FAIL;
	}

	hr = pDevice->CreateVertexBuffer(sizeof(VERTEX_2D_MULTITEX) * POLYGON_TOP,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D_MULTITEX,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL);

	if (FAILED(hr))
	{// ���s�����Ƃ�

		return E_FAIL;
	}

	// ���_���ݒ�
	SetVtx();

	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CObject2DMulti::Uninit()
{
	// ���_�o�b�t�@�̔j��
	if (m_pVtxBuff != NULL)
	{
		m_pVtxBuff->Release();
		m_pVtxBuff = NULL;
	}

	// �I�u�W�F�N�g�̔j��
	Release();
}

//==========================================================================
// �X�V����
//==========================================================================
void CObject2DMulti::Update()
{
	// ���_���ݒ�
	SetVtx();
}

//==========================================================================
// �`�揈��
//==========================================================================
void CObject2DMulti::Draw()
{

	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();

	// ���_�o�b�t�@���f�[�^�X�g���[���ɐݒ�
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_2D_MULTITEX));

	// ���_�t�H�[�}�b�g�̐ݒ�
	pDevice->SetFVF(FVF_VERTEX_2D_MULTITEX);

	// �e�N�X�`���̐ݒ�
	pDevice->SetTexture(0, CTexture::GetInstance()->GetAdress(m_nTexIdx));

	// �|���S���̕`��
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}

void CObject2DMulti::Draw(LPDIRECT3DTEXTURE9 mutitex)
{

	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();

	// �e�N�X�`���X�e�[�W1�̐ݒ�
	pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE); // �u�����f�B���O���[�h��MODULATE�ɕύX
	pDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_CURRENT);
	pDevice->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_TEXTURE);
	pDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_MODULATE); // �A���t�@�u�����f�B���O��MODULATE�ɕύX
	pDevice->SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_CURRENT);
	pDevice->SetTextureStageState(1, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);

	// �A���t�@�u�����f�B���O��L���ɂ���
	pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	// ���_�o�b�t�@���f�[�^�X�g���[���ɐݒ�
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_2D_MULTITEX));

	// ���_�t�H�[�}�b�g�̐ݒ�
	pDevice->SetFVF(FVF_VERTEX_2D_MULTITEX);

	// �e�N�X�`���̐ݒ�
	pDevice->SetTexture(0, CTexture::GetInstance()->GetAdress(m_nTexIdx));
	pDevice->SetTexture(1, mutitex);

	// �|���S���̕`��
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);


	// �e�N�X�`���X�e�[�W0�̃f�t�H���g�ݒ�
	pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

	// �e�N�X�`���X�e�[�W1�̃f�t�H���g�ݒ�
	pDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE); // �f�t�H���g�ł̓e�N�X�`���X�e�[�W1�͖���
	pDevice->SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	pDevice->SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_CURRENT);
}

//==========================================================================
// ���_���ݒ菈��
//==========================================================================
void CObject2DMulti::SetVtx()
{

	// ���_���ւ̃|�C���^
	VERTEX_2D_MULTITEX *pVtx;

	// ���_�o�b�t�@�����b�N���A���_���ւ̃|�C���^���擾
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	MyLib::Vector3 pos = GetPosition();
	MyLib::Vector3 rot = GetRotation();
	D3DXCOLOR col = GetColor();
	float fAngle = GetAngle();
	float fLength = GetLength();

	// ���_���W�̐ݒ�
	pVtx[0].pos.x = pos.x + sinf(rot.z - D3DX_PI + fAngle) * fLength;
	pVtx[0].pos.y = pos.y + cosf(rot.z - D3DX_PI + fAngle) * fLength;
	pVtx[0].pos.z = 0.0f;

	pVtx[1].pos.x = pos.x + sinf(rot.z + D3DX_PI - fAngle) * fLength;
	pVtx[1].pos.y = pos.y + cosf(rot.z + D3DX_PI - fAngle) * fLength;
	pVtx[1].pos.z = 0.0f;

	pVtx[2].pos.x = pos.x + sinf(rot.z - fAngle) * fLength;
	pVtx[2].pos.y = pos.y + cosf(rot.z - fAngle) * fLength;
	pVtx[2].pos.z = 0.0f;

	pVtx[3].pos.x = pos.x + sinf(rot.z + fAngle) * fLength;
	pVtx[3].pos.y = pos.y + cosf(rot.z + fAngle) * fLength;
	pVtx[3].pos.z = 0.0f;

	// rhw�̐ݒ�
	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;

	// ���_�J���[�̐ݒ�
	pVtx[0].col = col;
	pVtx[1].col = col;
	pVtx[2].col = col;
	pVtx[3].col = col;

	// �e�N�X�`�����W�̐ݒ�
	pVtx[0].tex = m_fTex[0];
	pVtx[1].tex = m_fTex[1];
	pVtx[2].tex = m_fTex[2];
	pVtx[3].tex = m_fTex[3];

	// �e�N�X�`�����W�̐ݒ�
	pVtx[0].tex2 = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex2 = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex2 = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex2 = D3DXVECTOR2(1.0f, 1.0f);

	// ���_�o�b�t�@���A�����b�N���b�N
	m_pVtxBuff->Unlock();
}

//==========================================================================
// �F�ݒ�
//==========================================================================
void CObject2DMulti::SetColor(const D3DXCOLOR col)
{
	m_col = col;
}

//==========================================================================
// �F�擾
//==========================================================================
D3DXCOLOR CObject2DMulti::GetColor() const
{
	return m_col;
}

//==========================================================================
// �T�C�Y�ݒ�
//==========================================================================
void CObject2DMulti::SetSize(const D3DXVECTOR2 size)
{
	m_size = size;		// �T�C�Y
	SetLength(sqrtf(m_size.x * m_size.x + m_size.y * m_size.y));	// �Ίp���̒���
	SetAngle(atan2f(m_size.x, m_size.y));								// �Ίp���̌���
}

//==========================================================================
// �T�C�Y�擾
//==========================================================================
D3DXVECTOR2 CObject2DMulti::GetSize() const
{
	return m_size;
}

//==========================================================================
// ���̃T�C�Y�̐ݒ�
//==========================================================================
void CObject2DMulti::SetSizeOrigin(const D3DXVECTOR2 size)
{
	m_sizeOrigin = size;
}

//==========================================================================
// ���̃T�C�Y�̎擾
//==========================================================================
D3DXVECTOR2 CObject2DMulti::GetSizeOrigin() const
{
	return m_sizeOrigin;
}

//==========================================================================
// �e�N�X�`�����W�ݒ�
//==========================================================================
void CObject2DMulti::SetTex(D3DXVECTOR2 *tex)
{
	memcpy(&m_fTex[0], tex, sizeof(m_fTex));
}

//==========================================================================
// �e�N�X�`�����W�擾
//==========================================================================
D3DXVECTOR2 *CObject2DMulti::GetTex()
{
	return &m_fTex[0];
}

//==========================================================================
// �Ίp���̒����ݒ�
//==========================================================================
void CObject2DMulti::SetLength(const float fLength)
{
	m_fLength = fLength;
}

//==========================================================================
// �Ίp���̒����擾
//==========================================================================
float CObject2DMulti::GetLength() const
{
	return m_fLength;
}

//==========================================================================
// �Ίp���̌����擾
//==========================================================================
void CObject2DMulti::SetAngle(const float fAngle)
{
	m_fAngle = fAngle;
}

//==========================================================================
// �Ίp���̌����擾
//==========================================================================
float CObject2DMulti::GetAngle() const
{
	return m_fAngle;
}


//==========================================================================
// ���_���W�擾
//==========================================================================
MyLib::Vector3 *CObject2DMulti::GetVtxPos()
{
	return &m_VtxPos[0];
}

//==========================================================================
// ���_���W�ݒ�
//==========================================================================
void CObject2DMulti::SetVtxPos(MyLib::Vector3 *pos)
{
	//&m_VtxPos[0] = *pos;
	memcpy(&m_VtxPos[0], pos, sizeof(m_VtxPos));
}

//==========================================================================
// ���_�o�b�t�@�̎擾
//==========================================================================
LPDIRECT3DVERTEXBUFFER9 CObject2DMulti::GetVtxBuff() const
{
	return m_pVtxBuff;
}

//==========================================================================
// �I�u�W�F�N�g2D�I�u�W�F�N�g�̎擾
//==========================================================================
CObject2DMulti *CObject2DMulti::GetObject2DMulti()
{
	return this;
}
