//=============================================================================
// 
// �X�L���c���[���Z�b�g���� [skilltree_reset.cpp]
// Author : ���n�Ή�
// 
//=============================================================================
#include "skilltree_reset.h"
#include "skilltree.h"
#include "renderer.h"
#include "texture.h"
#include "manager.h"
#include "sound.h"
#include "input.h"
#include "game.h"
#include "calculation.h"
#include "player.h"
#include "skillpoint.h"
#include "skilltree_reset_impossible.h"

//==========================================================================
// �}�N����`
//==========================================================================
namespace
{
	const char* TEXTURE = "data\\TEXTURE\\skilltree\\resetwindow.png";
	const char* SELECT_TEXTURE[] =
	{
		"data\\TEXTURE\\skilltree\\reset_select01.png",
		"data\\TEXTURE\\skilltree\\reset_select02.png",
		"data\\TEXTURE\\skilltree\\reset_select03.png",
		"data\\TEXTURE\\skilltree\\reset_select04.png",
	};
	const float DISTANCE = 140.0f;
}
std::vector<int> CSkillTree_Reset::m_nTexIdx = {};		// �e�N�X�`���̃C���f�b�N�X�ԍ�

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CSkillTree_Reset::CSkillTree_Reset(int nPriority) : CObject2D(nPriority)
{
	// �l�̃N���A
	m_nSelect = 0;	// �I����
	m_fFlashTime = 0.0f;	// �_�Ŏ���
	memset(m_pSelectObj, NULL, sizeof(m_pSelectObj));	// �I�����̃I�u�W�F�N�g
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CSkillTree_Reset::~CSkillTree_Reset()
{

}

//==========================================================================
// ��������
//==========================================================================
CSkillTree_Reset* CSkillTree_Reset::Create()
{
	// �������̊m��
	CSkillTree_Reset* pReset = DEBUG_NEW CSkillTree_Reset;

	if (pReset != nullptr)
	{
		// ����������
		if (FAILED(pReset->Init()))
		{
			return nullptr;
		}
	}

	return pReset;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CSkillTree_Reset::Init()
{
	// ����������
	HRESULT hr = CObject2D::Init();
	if (FAILED(hr))
	{// ���s�����Ƃ�
		return E_FAIL;
	}

	// ��ނ̐ݒ�
	SetType(TYPE_OBJECT2D);

	if (m_nTexIdx.empty())
	{
		for (const auto& tex : SELECT_TEXTURE)
		{
			// �e�N�X�`���ǂݍ���
			int nIdx = CTexture::GetInstance()->Regist(tex);
			m_nTexIdx.push_back(nIdx);
		}
	}

	// �e�N�X�`���̊��蓖��
	int nIdx = CTexture::GetInstance()->Regist(TEXTURE);
	BindTexture(nIdx);

	// �T�C�Y�ݒ�
	D3DXVECTOR2 size = CTexture::GetInstance()->GetImageSize(nIdx);
	size = UtilFunc::Transformation::AdjustSizeByHeight(size, 150.0f);
	SetSize(size);
	SetSizeOrigin(GetSize());

	// �ʒu�ݒ�
	SetPosition(MyLib::Vector3(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f, 0.0f));

	// �I��������
	CreateSelect();

	return S_OK;
}

//==========================================================================
// �I��������
//==========================================================================
void CSkillTree_Reset::CreateSelect()
{
	for (int i = 0; i < SELECT_MAX; i++)
	{
		m_pSelectObj[i] = CObject2D::Create(GetPriority());

		// ��ނ̐ݒ�
		m_pSelectObj[i]->SetType(TYPE_OBJECT2D);

		// �e�N�X�`�����蓖��
		m_pSelectObj[i]->BindTexture(m_nTexIdx[i]);

		// �T�C�Y�ݒ�
		D3DXVECTOR2 size = CTexture::GetInstance()->GetImageSize(m_nTexIdx[i]);
		size = UtilFunc::Transformation::AdjustSizeByHeight(size, 32.0f);
		m_pSelectObj[i]->SetSize(size);
		m_pSelectObj[i]->SetSizeOrigin(GetSize());

		// �ʒu�ݒ�
		m_pSelectObj[i]->SetPosition(MyLib::Vector3((SCREEN_WIDTH * 0.5f) - DISTANCE + (DISTANCE * i) * 2.0f, 410.0f, 0.0f));
	}

	int selectIdx = static_cast<int>(SELECT_CANCEL);
	m_pSelectObj[selectIdx]->BindTexture(m_nTexIdx[selectIdx] + 2);

}

//==========================================================================
// �I������
//==========================================================================
void CSkillTree_Reset::Uninit()
{
	// �C���f�b�N�X�ԍ����Z�b�g
	m_nTexIdx.clear();

	for (int i = 0; i < SELECT_MAX; i++)
	{
		if (m_pSelectObj[i] == nullptr)
		{
			continue;
		}
		m_pSelectObj[i] = nullptr;
		m_pSelectObj[i] = nullptr;
	}

	// �I������
	CObject2D::Uninit();
}

//==========================================================================
// �폜����
//==========================================================================
void CSkillTree_Reset::Kill()
{
	for (int i = 0; i < SELECT_MAX; i++)
	{
		if (m_pSelectObj[i] == nullptr)
		{
			continue;
		}
		m_pSelectObj[i]->Uninit();
		m_pSelectObj[i] = nullptr;
	}

	// �I������
	CObject2D::Uninit();
}

//==========================================================================
// �X�V����
//==========================================================================
void CSkillTree_Reset::Update()
{

	m_fFlashTime += CManager::GetInstance()->GetDeltaTime();	// �_�Ŏ���

	// �I�����X�V
	UpdateSelect();
	if (IsDeath())
	{
		return;
	}

	for (int i = 0; i < SELECT_MAX; i++)
	{
		D3DXCOLOR col = m_pSelectObj[i]->GetColor();
		if (m_nSelect == i)
		{
			col = UtilFunc::Transformation::HSVtoRGB(0.0f, 0.0f, 0.7f + fabsf(sinf(D3DX_PI * (m_fFlashTime / 1.0f)) * 0.3f));
			m_pSelectObj[i]->BindTexture(m_nTexIdx[i] + 2);
		}
		else
		{
			// �����ۂ�����
			col = D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.0f);
			m_pSelectObj[i]->BindTexture(m_nTexIdx[i]);
		}
		m_pSelectObj[i]->SetColor(col);
	}

	// �X�V����
	CObject2D::Update();
}

//==========================================================================
// �I�����X�V
//==========================================================================
void CSkillTree_Reset::UpdateSelect()
{
	// �L�[�{�[�h���擾
	CInputKeyboard* pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();

	// �Q�[���p�b�h���擾
	CInputGamepad* pInputGamepad = CManager::GetInstance()->GetInputGamepad();

	if (pInputKeyboard->GetTrigger(DIK_D) ||
		pInputGamepad->GetTrigger(CInputGamepad::BUTTON_RIGHT, 0) ||
		(pInputGamepad->GetLStickTrigger(CInputGamepad::STICK_X) && pInputGamepad->GetStickMoveL(0).x > 0))
	{// �E

		// �p�^�[��No.���X�V
		m_nSelect = (m_nSelect + (SELECT_MAX - 1)) % SELECT_MAX;

		// �T�E���h�Đ�
		//CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL_SE_CURSOR);
	}
	else if (pInputKeyboard->GetTrigger(DIK_A) ||
		pInputGamepad->GetTrigger(CInputGamepad::BUTTON_LEFT, 0) ||
		(pInputGamepad->GetLStickTrigger(CInputGamepad::STICK_X) && pInputGamepad->GetStickMoveL(0).x < 0))
	{// ��

		// �p�^�[��No.���X�V
		m_nSelect = (m_nSelect + 1) % SELECT_MAX;

		// �T�E���h�Đ�
		//CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL_SE_CURSOR);
	}

	// ����
	if (pInputGamepad->GetTrigger(CInputGamepad::BUTTON_A, 0) ||
		pInputKeyboard->GetTrigger(DIK_RETURN))
	{
		switch (m_nSelect)
		{
		case SELECT_CANCEL:
			CSkillTree::GetInstance()->OutReset();
			return;
			break;

		case SELECT_OK:

			// �K���󋵎擾
			std::vector<CSkillTree_Icon::eMastering> masterings = CSkillTree::GetInstance()->GetMastering();

			bool bReset = false;
			for (const auto& master : masterings)
			{
				// 1�ł��K���ς݂�����΃��Z�b�g
				if (master == CSkillTree_Icon::MASTERING_DONE)
				{
					bReset = true;
					break;
				}
			}

			// ���K��
			if (!bReset)
			{
				CSkillTree_ResetImpossible::Create(CSkillTree_ResetImpossible::ERRORTYPE_NOGET);
			}

			// �c��|�C���g�擾
			CPlayer* pPlayer = CPlayer::GetListObj().GetData(0);
			int point = pPlayer->GetSkillPoint()->GetPoint();

			// �|�C���g�Ȃ����烊�Z�b�g�s��
			if (point == 0)
			{
				bReset = false;
				CSkillTree_ResetImpossible::Create(CSkillTree_ResetImpossible::ERRORTYPE_POINT);
				return;
			}

			if (bReset)
			{
				// �|�C���g����
				pPlayer->GetSkillPoint()->SubPoint();
				int backPoint = pPlayer->GetSkillPoint()->GetPoint();

				// �A�C�R�����擾
				std::vector<CSkillTree_Icon::sSkillIcon> iconInfo = CSkillTree::GetInstance()->GetIconInfo();
				for (const auto& info : iconInfo)
				{
					if (info.mastering == CSkillTree_Icon::MASTERING_DONE)
					{
						backPoint += info.needpoint;
					}
				}
				pPlayer->GetSkillPoint()->SetPoint(backPoint);

				// �������Z�b�g
				pPlayer->ResetEnhance();

				// �K���󋵃��Z�b�g
				CSkillTree::GetInstance()->ResetMastering();
				CSkillTree::GetInstance()->OutReset();
				return;
			}

			break;
		}
	}

	// �߂�
	if (pInputGamepad->GetTrigger(CInputGamepad::BUTTON_B, 0) ||
		pInputKeyboard->GetTrigger(DIK_BACK))
	{
		CSkillTree::GetInstance()->OutReset();
		return;
	}
}

//==========================================================================
// �`�揈��
//==========================================================================
void CSkillTree_Reset::Draw()
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
