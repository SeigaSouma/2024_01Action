//=============================================================================
// 
// �X�L���c���[�J�[�\������ [skilltree_cursor.cpp]
// Author : ���n�Ή�
// 
//=============================================================================
#include "skilltree_cursor.h"
#include "renderer.h"
#include "texture.h"
#include "manager.h"
#include "sound.h"
#include "input.h"
#include "fade.h"


//==========================================================================
// �萔��`
//==========================================================================
namespace
{
	const char* TEXTURE = "data\\TEXTURE\\tyuuni\\tyuuni_face.png";
	const float MOVE_VELOCITY = 50.0f;	// �ړ����x
}

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CSkillTree_Cursor::CSkillTree_Cursor(int nPriority) : CObject2D(nPriority)
{
	// �l�̃N���A
	m_nMyPlayerIdx = 0;	// �v���C���[�C���f�b�N�X�ԍ�
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CSkillTree_Cursor::~CSkillTree_Cursor()
{

}

//==========================================================================
// ��������
//==========================================================================
CSkillTree_Cursor *CSkillTree_Cursor::Create(int nIdx)
{
	// �����p�̃I�u�W�F�N�g
	CSkillTree_Cursor *pEffect = NULL;

	if (pEffect == NULL)
	{// NULL��������

		// �������̊m��
		pEffect = DEBUG_NEW CSkillTree_Cursor;

		if (pEffect != NULL)
		{// �������̊m�ۂ��o���Ă�����

			// �v���C���[�C���f�b�N�X
			pEffect->m_nMyPlayerIdx = nIdx;

			// ����������
			pEffect->Init();
		}

		return pEffect;
	}

	return NULL;
}

//==================================================================================
// ����������
//==================================================================================
HRESULT CSkillTree_Cursor::Init(void)
{
	HRESULT hr;

	// ����������
	hr = CObject2D::Init();
	if (FAILED(hr))
	{// ���s�����Ƃ�
		return E_FAIL;
	}

	// ��ނ̐ݒ�
	SetType(TYPE_OBJECT2D);

	// �e�N�X�`���̊��蓖��
	int nIdxTex = CTexture::GetInstance()->Regist(TEXTURE);
	BindTexture(nIdxTex);

	// �e�N�X�`���T�C�Y�擾
	D3DXVECTOR2 size = CTexture::GetInstance()->GetImageSize(nIdxTex) * 0.1f;

	// �T�C�Y�ݒ�
	SetSize(size);

	return S_OK;
}

//==================================================================================
// �I������
//==================================================================================
void CSkillTree_Cursor::Uninit(void)
{
	// �I������
	CObject2D::Uninit();
}

//==================================================================================
// �X�V����
//==================================================================================
void CSkillTree_Cursor::Update(void)
{
	CManager* pManager = CManager::GetInstance();
	float deltatime = pManager->GetDeltaTime();

	// �ړ����x
	float velocity = MOVE_VELOCITY * deltatime;

	// �ʒu�擾
	D3DXVECTOR3 pos = GetPosition();

	// �ړ��ʎ擾
	D3DXVECTOR3 move = GetMove();

	// �L�[�{�[�h���擾
	CInputKeyboard *pInputKeyboard = pManager->GetInputKeyboard();

	// �Q�[���p�b�h���擾
	CInputGamepad *pInputGamepad = pManager->GetInputGamepad();

	if (pInputKeyboard->GetPress(DIK_A) == true)
	{//���L�[�������ꂽ,���ړ�

		if (pInputKeyboard->GetPress(DIK_W) == true)
		{//A+W,����ړ�
			move.x += sinf(-D3DX_PI * 0.75f) * velocity;
			move.y += cosf(-D3DX_PI * 0.75f) * velocity;
		}
		else if (pInputKeyboard->GetPress(DIK_S) == true)
		{//A+S,�����ړ�
			move.x += sinf(-D3DX_PI * 0.25f) * velocity;
			move.y += cosf(-D3DX_PI * 0.25f) * velocity;
		}
		else
		{//A,���ړ�
			move.x += sinf(-D3DX_PI * 0.5f) * velocity;
			move.y += cosf(-D3DX_PI * 0.5f) * velocity;
		}
	}
	else if (pInputKeyboard->GetPress(DIK_D) == true)
	{//D�L�[�������ꂽ,�E�ړ�

		if (pInputKeyboard->GetPress(DIK_W) == true)
		{//D+W,�E��ړ�
			move.x += sinf(D3DX_PI * 0.75f) * velocity;
			move.y += cosf(D3DX_PI * 0.75f) * velocity;
		}
		else if (pInputKeyboard->GetPress(DIK_S) == true)
		{//D+S,�E���ړ�
			move.x += sinf(D3DX_PI * 0.25f) * velocity;
			move.y += cosf(D3DX_PI * 0.25f) * velocity;
		}
		else
		{//D,�E�ړ�
			move.x += sinf(D3DX_PI * 0.5f) * velocity;
			move.y += cosf(D3DX_PI * 0.5f) * velocity;
		}
	}
	else if (pInputKeyboard->GetPress(DIK_W) == true)
	{//W�������ꂽ�A��ړ�
		move.x += sinf(D3DX_PI * 1.0f) * velocity;
		move.y += cosf(D3DX_PI * 1.0f) * velocity;
	}
	else if (pInputKeyboard->GetPress(DIK_S) == true)
	{//S�������ꂽ�A���ړ�
		move.x += sinf(D3DX_PI * 0.0f) * velocity;
		move.y += cosf(D3DX_PI * 0.0f) * velocity;
	}

	if (pInputGamepad->IsTipStick())
	{// ���X�e�B�b�N���|��Ă�ꍇ

		// �X�e�B�b�N�̌����擾
		float stickrot = pInputGamepad->GetStickRotL(m_nMyPlayerIdx);

		// �ړ��ʂƌ����ݒ�
		move.x += sinf(stickrot) * velocity;
		move.y += cosf(stickrot) * -velocity;
	}

	// �ړ�
	pos += move;

	// ����
	move.x += (0.0f - move.x) * 0.2f;
	move.y += (0.0f - move.y) * 0.2f;

	// �ʒu�E�ړ���
	SetPosition(pos);
	SetMove(move);

	// ���_���W�̐ݒ�
	SetVtx();
}

//==================================================================================
// �`�揈��
//==================================================================================
void CSkillTree_Cursor::Draw(void)
{
	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetInstance()->GetRenderer()->GetDevice();

	// �A���t�@�e�X�g��L���ɂ���
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);

	// �`�揈��
	CObject2D::Draw();

	// �A���t�@�e�X�g��L���ɂ���
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_ALWAYS);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);
}

//==========================================================================
// ���_���ݒ菈��
//==========================================================================
void CSkillTree_Cursor::SetVtx(void)
{
	// ���_�ݒ�
	CObject2D::SetVtx();
}

