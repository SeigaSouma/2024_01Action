//=============================================================================
// 
// �`���[�g���A�������邩�m�F���� [tutorial_checkshould.cpp]
// Author : ���n�Ή�
// 
//=============================================================================
#include "tutorial_checkshould.h"
#include "manager.h"
#include "sound.h"
#include "input.h"
#include "calculation.h"
#include "fade.h"

//==========================================================================
// �}�N����`
//==========================================================================
namespace
{
	const char* TEXTURE = "data\\TEXTURE\\shouldtutorial\\window.png";
	const char* SELECT_TEXTURE[] =
	{
		"data\\TEXTURE\\shouldtutorial\\select_no.png",
		"data\\TEXTURE\\shouldtutorial\\select_yes.png",
		"data\\TEXTURE\\shouldtutorial\\select_no2.png",
		"data\\TEXTURE\\shouldtutorial\\select_yes2.png",
	};
	const float DISTANCE = 140.0f;
}
std::vector<int> CTutorialCheckShould::m_nTexIdx = {};		// �e�N�X�`���̃C���f�b�N�X�ԍ�

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CTutorialCheckShould::CTutorialCheckShould(int nPriority) : CObject2D(nPriority)
{
	// �l�̃N���A
	m_nSelect = 0;	// �I����
	m_fFlashTime = 0.0f;	// �_�Ŏ���
	memset(m_pSelectObj, NULL, sizeof(m_pSelectObj));	// �I�����̃I�u�W�F�N�g
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CTutorialCheckShould::~CTutorialCheckShould()
{

}

//==========================================================================
// ��������
//==========================================================================
CTutorialCheckShould* CTutorialCheckShould::Create()
{
	// �������̊m��
	CTutorialCheckShould* pReset = DEBUG_NEW CTutorialCheckShould;

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
HRESULT CTutorialCheckShould::Init()
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
	CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL::LABEL_SE_CURSOR_END);

	return S_OK;
}

//==========================================================================
// �I��������
//==========================================================================
void CTutorialCheckShould::CreateSelect()
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

	int selectIdx = static_cast<int>(SELECT_NO);
	m_pSelectObj[selectIdx]->BindTexture(m_nTexIdx[selectIdx] + 2);

}

//==========================================================================
// �I������
//==========================================================================
void CTutorialCheckShould::Uninit()
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
void CTutorialCheckShould::Kill()
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
void CTutorialCheckShould::Update()
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
void CTutorialCheckShould::UpdateSelect()
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
		CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL::LABEL_SE_CURSOR_MOVE);
	}
	else if (pInputKeyboard->GetTrigger(DIK_A) ||
		pInputGamepad->GetTrigger(CInputGamepad::BUTTON_LEFT, 0) ||
		(pInputGamepad->GetLStickTrigger(CInputGamepad::STICK_X) && pInputGamepad->GetStickMoveL(0).x < 0))
	{// ��

		// �p�^�[��No.���X�V
		m_nSelect = (m_nSelect + 1) % SELECT_MAX;

		// �T�E���h�Đ�
		CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL::LABEL_SE_CURSOR_MOVE);
	}

	// ����
	if (pInputGamepad->GetTrigger(CInputGamepad::BUTTON_A, 0) ||
		pInputKeyboard->GetTrigger(DIK_RETURN))
	{
		switch (m_nSelect)
		{
		case SELECT_NO:
			
			CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL::LABEL_SE_CURSOR_END);

			// ���[�h�ݒ�
			CManager::GetInstance()->GetFade()->SetFade(CScene::MODE_GAME);
			break;

		case SELECT_OK:

			// ���[�h�ݒ�
			CManager::GetInstance()->GetFade()->SetFade(CScene::MODE_GAMETUTORIAL);
			break;
		}
	}

	// �߂�
	if (pInputGamepad->GetTrigger(CInputGamepad::BUTTON_B, 0) ||
		pInputKeyboard->GetTrigger(DIK_BACK))
	{
		CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL::LABEL_SE_CURSOR_END);
		return;
	}
}

//==========================================================================
// �`�揈��
//==========================================================================
void CTutorialCheckShould::Draw()
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
