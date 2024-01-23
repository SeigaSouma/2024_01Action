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
#include "calculation.h"
#include "skilltree.h"
#include "debugproc.h"

//==========================================================================
// �萔��`
//==========================================================================
namespace
{
	const char* TEXTURE = "data\\TEXTURE\\tyuuni\\tyuuni_face.png";
	const MyLib::Vector3 LOCK_POSITION = MyLib::Vector3(340.0f, 240.0f, 0.0f);	// �Œ�̈ʒu
	const float MOVE_VELOCITY = 70.0f;	// �ړ����x
	const float RADIUS = 60.0f;	// ���a
}

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CSkillTree_Cursor::CSkillTree_Cursor(int nPriority) : CObject2D(nPriority)
{
	// �l�̃N���A
	m_nMyPlayerIdx = 0;	// �v���C���[�C���f�b�N�X�ԍ�
	m_WorldPos = 0.0f;	// ��΍��W
	m_DestPos = 0.0f;	// �ڕW���W
	m_bHitIcon = false;	// �A�C�R���̐ڐG�t���O
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

//==========================================================================
// ����������
//==========================================================================
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

	// �ʒu�ݒ�
	SetPosition(LOCK_POSITION);
	m_WorldPos = 0.0f;

	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CSkillTree_Cursor::Uninit(void)
{
	// �I������
	CObject2D::Uninit();
}

//==========================================================================
// �X�V����
//==========================================================================
void CSkillTree_Cursor::Update(void)
{
	CManager* pManager = CManager::GetInstance();
	float deltatime = pManager->GetDeltaTime();

	// �ړ����x
	float velocity = MOVE_VELOCITY * deltatime;

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
	m_WorldPos += move;

	// �X�L���c���[�̈ʒu�ݒ�
	MyLib::Vector3 setpos = CSkillTree::GetInstance()->GetPosition();
	setpos = -m_WorldPos;
	CSkillTree::GetInstance()->SetPosition(setpos);

	// ����
	move.x += (0.0f - move.x) * 0.2f;
	move.y += (0.0f - move.y) * 0.2f;

	// �ʒu�E�ړ���
	SetMove(move);

	// ���_���W�̐ݒ�
	SetVtx();

	// �A�C�R���Ƃ̓����蔻��
	CollisionIcon();

	// �f�o�b�O�\��
	CManager::GetInstance()->GetDebugProc()->Print(
		"------------------[�J�[��]------------------\n"
		"�ʒu�F�yX�F%f, Y�F%f, Z�F%f�z\n"
		, m_WorldPos.x, m_WorldPos.y, m_WorldPos.z);
}

//==========================================================================
// �A�C�R���Ƃ̓����蔻��
//==========================================================================
void CSkillTree_Cursor::CollisionIcon(void)
{
	// �A�C�R���擾
	std::vector<CSkillTree_Icon*> iconList = CSkillTree::GetInstance()->GetIcon();

	m_bHitIcon = false;
	for (const auto& icon : iconList)
	{
		MyLib::Vector3 iconpos = icon->GetPosition() - LOCK_POSITION;
		if (UtilFunc::Collision::CircleRange2D(0.0f, iconpos, RADIUS, 1.0f))
		{// �A�C�R���Ƀq�b�g
			
			m_bHitIcon = true;
			m_DestPos = m_WorldPos + iconpos;
			break;
		}
	}

	if (m_bHitIcon &&
		!CManager::GetInstance()->GetInputGamepad()->IsTipStick())
	{// �ڐG && �X�e�B�b�N���|����ĂȂ�
		UtilFunc::Correction::InertiaCorrection(m_WorldPos.x, m_DestPos.x, 0.15f);
		UtilFunc::Correction::InertiaCorrection(m_WorldPos.y, m_DestPos.y, 0.15f);
	}
}

//==========================================================================
// �`�揈��
//==========================================================================
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

