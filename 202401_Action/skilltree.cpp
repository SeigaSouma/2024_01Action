//=============================================================================
// 
//  �X�L���c���[���� [skilltree.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "skilltree.h"
#include "manager.h"
#include "renderer.h"
#include "texture.h"
#include "object2D.h"
#include "calculation.h"
#include "input.h"
#include "skilltree_screen.h"
#include "skilltree_cursor.h"
#include "skilltree_line.h"
#include "skilltree_command.h"
#include "skilltree_window.h"
#include "skilltree_description.h"
#include "skilltree_obj.h"
#include "skilltree_reset.h"
#include "camera.h"
#include "player.h"
#include "game.h"
#include "transferBeacon.h"

//==========================================================================
// �萔��`
//==========================================================================
namespace
{
	const char* LOADTEXT = "data\\TEXT\\skilltree\\iconinfo.json";
	const char* AUTOSAVE_MASTERING = "data\\TEXT\\skilltree\\mastering.json";
	const float TIME_FADE = 0.65f;	// �t�F�[�h�A�E�g����
	const int MAX_ICON = 4;		// �A�C�R��
}

//==========================================================================
// �ÓI�����o�ϐ���`
//==========================================================================
CSkillTree* CSkillTree::m_pThisPtr = nullptr;	// ���g�̃|�C���^

//==========================================================================
// �֐��|�C���^
//==========================================================================
CSkillTree::STATE_FUNC CSkillTree::m_StateFuncList[] =
{
	&CSkillTree::StateNone,		// �Ȃɂ��Ȃ�
	&CSkillTree::StateFadeIn,	// �t�F�[�h�C��
	&CSkillTree::StateFadeOut,	// �t�F�[�h�A�E�g
	&CSkillTree::StateReset,	// ���Z�b�g
};

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CSkillTree::CSkillTree(int nPriority) : CObject(nPriority)
{
	// �l�̃N���A
	m_fAlpha = 0.0f;		// �s�����x
	m_fStateTime = 0.0f;	// ��ԑJ�ڃJ�E���^�[
	m_state = STATE_NONE;	// ���
	m_SkillInfo.clear();	// �X�L���A�C�R��
	m_pScreen = nullptr;	// �X�N���[���̃I�u�W�F�N�g
	m_pCursor = nullptr;	// �J�[�\���̃I�u�W�F�N�g
	m_pCommand = nullptr;	// �R�}���h�̃I�u�W�F�N�g
	m_pWindow = nullptr;	// �E�B���h�E�̃I�u�W�F�N�g
	m_pDescription = nullptr;	// �������̃I�u�W�F�N�g
	m_pReset = nullptr;			// ���Z�b�g�p�̃I�u�W�F�N�g
	m_bOnScreen = false;	// �X�N���[����ɂ��邩�̃t���O
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CSkillTree::~CSkillTree()
{

}

//==========================================================================
// ��������
//==========================================================================
CSkillTree *CSkillTree::Create()
{
	if (m_pThisPtr == nullptr)
	{// �܂��������Ă��Ȃ�������

		// �C���X�^���X����
		m_pThisPtr = DEBUG_NEW CSkillTree;

		// ����������
		m_pThisPtr->Init();
	}
	else
	{
		// �C���X�^���X�擾
		m_pThisPtr->Init();
		m_pThisPtr->GetInstance();
	}

	return m_pThisPtr;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CSkillTree::Init()
{
	// ��ނ̐ݒ�
	SetType(CObject::TYPE_OBJECT2D);

	// ���json�ǂݍ��݂ɕύX����
#if 0
	CSkillTree_Icon::sSkillIcon info = CSkillTree_Icon::sSkillIcon();

	for (int i = 0; i < MAX_ICON; i++)
	{
		info.ID = i;
		info.parentID = i - 1;
		info.needpoint = 5;
		info.texID = 0;
		info.pos = MyLib::Vector3(200.0f+i * 200.0f, 360.0f, 0.0f);

		m_SkillInfo.push_back(info);
	}

	// Json�ւ̏�������
	SaveJson();
#else
	// Json����̓ǂݍ���
	LoadJson();
#endif

	// �X�L���A�C�R���̏K����
	for (auto& mastering : m_SkillInfo)
	{
		mastering.mastering = CSkillTree_Icon::MASTERING_YET;
	}

	// ��ԃJ�E���^�[
	m_fStateTime = 0.0f;
	m_state = STATE_NONE;
	m_bOnScreen = false;	// �X�N���[����ɂ��邩�̃t���O

	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CSkillTree::Uninit()
{
	m_pThisPtr = nullptr;

	// ���폜
	Release();
}

//==========================================================================
// �폜
//==========================================================================
void CSkillTree::Kill()
{
	// �A�C�R���̏I��
	for (auto const& iconinfo : m_pSkillIcon)
	{
		iconinfo->Uninit();
	}
	m_pSkillIcon.clear();	// �X�L���A�C�R��

	// �X�N���[���̏I��
	if (m_pScreen != nullptr)
	{
		m_pScreen->Uninit();
		m_pScreen = nullptr;
	}

	// �J�[�\���I��
	m_pCursor->Uninit();
	m_pCursor = nullptr;

	// �R�}���h�I��
	if (m_pCommand != nullptr)
	{
		m_pCommand->Uninit();
		m_pCommand = nullptr;
	}

	// �E�B���h�E�I��
	if (m_pWindow != nullptr)
	{
		m_pWindow->Uninit();
		m_pWindow = nullptr;
	}

	// �������I��
	if (m_pDescription != nullptr)
	{
		m_pDescription->Uninit();
		m_pDescription = nullptr;
	}

	// ���Z�b�g�I��
	if (m_pReset != nullptr)
	{
		m_pReset->Uninit();
		m_pReset = nullptr;
	}


	m_pThisPtr = nullptr;
	// ���폜
	Release();
}

//==========================================================================
// �X�V����
//==========================================================================
void CSkillTree::Update()
{
	if (IsDeath())
	{
		return;
	}

	if (!m_bOnScreen)
	{
		return;
	}

	// ��ԕʏ���
	(this->*(m_StateFuncList[m_state]))();

	// �ʒu�擾
	MyLib::Vector3 pos = GetPosition();

	if (!m_pSkillIcon.empty())
	{
		int i = 0;
		for (auto const& iconinfo : m_SkillInfo)
		{
			// �A�C�R���̈ʒu�ݒ�
			m_pSkillIcon[i]->SetPosition(pos + iconinfo.pos);
			i++;
		}
	}


	// ���C���̃��X�g�擾
	CListManager<CSkillTree_Line> lineList = CSkillTree_Line::GetListObj();
	CSkillTree_Line* pLine = nullptr;

	// ���X�g���[�v
	while (lineList.ListLoop(&pLine))
	{
		pLine->SetPosition(pos);
		pLine->SetVtx();
	}

}

//==========================================================================
// �����Ȃ����
//==========================================================================
void CSkillTree::StateNone()
{
	if (!m_bOnScreen)
	{
		return;
	}

	// �Q�[���p�b�h���擾
	CInputGamepad* pInputGamepad = CManager::GetInstance()->GetInputGamepad();
	CInputKeyboard* pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();
	
	// ���Z�b�g
	if (pInputGamepad->GetTrigger(CInputGamepad::BUTTON_X, 0))
	{
		// ���Z�b�g
		m_state = STATE_RESET;
		return;
	}

	// �߂�
	if (pInputGamepad->GetTrigger(CInputGamepad::BUTTON_B, 0) ||
		pInputKeyboard->GetTrigger(DIK_BACK))
	{
		m_state = STATE_FADEOUT;

		// �ʏ��Ԃɐݒ�
		CManager::GetInstance()->GetCamera()->SetStateCamraR(DEBUG_NEW CStateCameraR());

		// �F����Ԃɐݒ�
		CCamera* pCamera = CManager::GetInstance()->GetCamera();
		pCamera->SetControlState(DEBUG_NEW CCameraControlState_BeforePrayer(CManager::GetInstance()->GetCamera()));

		// �v���C���[�擾
		CListManager<CPlayer> playerList = CPlayer::GetListObj();
		CPlayer* pPlayer = nullptr;

		// ���X�g���[�v
		while (playerList.ListLoop(&pPlayer))
		{
			pPlayer->SetState(CPlayer::STATE_NONE);
		}

		// �ċN������
		CSkillTree_Obj::GetInstance()->ReStartUp();

		// �]�ڃr�[�R���̃G�t�F�N�g�ݒ�
		CListManager<CTransferBeacon> beaconList = CTransferBeacon::GetListObj();
		CTransferBeacon* pBeacon = nullptr;
		while (beaconList.ListLoop(&pBeacon))
		{
			pBeacon->SetEffect();
		}

	}
}

//==========================================================================
// �t�F�[�h�C�����
//==========================================================================
void CSkillTree::StateFadeIn()
{
	// ��ԑJ�ڃJ�E���^�[���Z
	m_fStateTime += CManager::GetInstance()->GetDeltaTime();

	// �s�����x�X�V
	m_fAlpha = m_fStateTime / TIME_FADE;

	for (auto const& icon : m_pSkillIcon)
	{
		// �F�擾
		D3DXCOLOR col = icon->GetColor();

		// �s�����x�ݒ�
		col.a = m_fAlpha;

		// �F�ݒ�
		icon->SetColor(col);
	}

	// �X�N���[���̓����x�ݒ�
	D3DXCOLOR col = m_pScreen->GetColor();
	col.a = m_fAlpha;
	m_pScreen->SetColor(col);

	// �J�[�\���̓����x�ݒ�
	col = m_pCursor->GetColor();
	col.a = m_fAlpha;
	m_pCursor->SetColor(col);

	// �R�}���h�̓����x�ݒ�
	m_pCommand->SetAlpha(m_fAlpha);

	// �E�B���h�E�̓����x�ݒ�
	col = m_pWindow->GetColor();
	col.a = m_fAlpha;
	m_pWindow->SetColor(col);

	// �������̓����x�ݒ�
	col = m_pDescription->GetColor();
	col.a = m_fAlpha;
	m_pDescription->SetColor(col);

	// ���C���̃��X�g�擾
	CListManager<CSkillTree_Line> lineList = CSkillTree_Line::GetListObj();
	CSkillTree_Line* pLine = nullptr;

	// ���X�g���[�v
	while (lineList.ListLoop(&pLine))
	{
		// �J�[�\���̓����x�ݒ�
		col = pLine->GetColor();
		col.a = m_fAlpha;
		pLine->SetColor(col);
	}


	if (m_fStateTime >= TIME_FADE)
	{
		m_fStateTime = TIME_FADE;
		m_state = STATE_NONE;
		return;
	}
}

//==========================================================================
// �t�F�[�h�A�E�g���
//==========================================================================
void CSkillTree::StateFadeOut()
{
	// ��ԑJ�ڃJ�E���^�[���Z
	m_fStateTime -= CManager::GetInstance()->GetDeltaTime();

	// �s�����x�X�V
	m_fAlpha = m_fStateTime / TIME_FADE;

	for (auto const& icon : m_pSkillIcon)
	{
		// �F�擾
		D3DXCOLOR col = icon->GetColor();

		// �s�����x�ݒ�
		col.a = m_fAlpha;

		// �F�ݒ�
		icon->SetColor(col);
	}

	// �X�N���[���̓����x�ݒ�
	D3DXCOLOR col = m_pScreen->GetColor();
	col.a = m_fAlpha;
	m_pScreen->SetColor(col);

	// �J�[�\���̓����x�ݒ�
	col = m_pCursor->GetColor();
	col.a = m_fAlpha;
	m_pCursor->SetColor(col);

	// �R�}���h�̓����x�ݒ�
	m_pCommand->SetAlpha(m_fAlpha);

	// �E�B���h�E�̓����x�ݒ�
	col = m_pWindow->GetColor();
	col.a = m_fAlpha;
	m_pWindow->SetColor(col);

	// �������̓����x�ݒ�
	col = m_pDescription->GetColor();
	col.a = m_fAlpha;
	m_pDescription->SetColor(col);


	// ���C���̃��X�g�擾
	CListManager<CSkillTree_Line> lineList = CSkillTree_Line::GetListObj();
	CSkillTree_Line* pLine = nullptr;

	// ���X�g���[�v
	while (lineList.ListLoop(&pLine))
	{
		// �J�[�\���̓����x�ݒ�
		col = pLine->GetColor();
		col.a = m_fAlpha;
		pLine->SetColor(col);
	}

	if (m_fStateTime <= 0)
	{
		m_fStateTime = 0.0f;

		// �X�L���c���[�ɕύX
		CGame::GetInstance()->GetGameManager()->SetType(CGameManager::SCENE_ENHANCE);

		// �ʏ��Ԃɖ߂�
		CManager::GetInstance()->GetCamera()->SetControlState(DEBUG_NEW CCameraControlState_Normal(CManager::GetInstance()->GetCamera()));

		// �X�N���[������J����
		OutScreen();

		// �폜
		//Kill();
		return;
	}
}

//==========================================================================
// ���Z�b�g�̍X�V
//==========================================================================
void CSkillTree::StateReset()
{
	if (m_pReset == nullptr)
	{
		// ���Z�b�g����
		m_pReset = CSkillTree_Reset::Create();
	}

	// �Q�[���p�b�h���擾
	CInputGamepad* pInputGamepad = CManager::GetInstance()->GetInputGamepad();
	CInputKeyboard* pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();

	//// ���Z�b�g
	//if (pInputGamepad->GetTrigger(CInputGamepad::BUTTON_B, 0))
	//{
	//	// ���Z�b�g����
	//	m_pReset = CSkillTree_Reset::Create();
	//}
}

//==========================================================================
// Json����̃��[�h
//==========================================================================
void CSkillTree::LoadJson()
{
	// �t�@�C������JSON��ǂݍ���
	std::ifstream file(LOADTEXT);
	if (!file.is_open())
	{
		return;
	}

	nlohmann::json jsonData;
	file >> jsonData;	// json�f�[�^��^����

	// json�f�[�^����ǂݍ���
	from_json(jsonData);
}

//==========================================================================
// Json�ւ̃Z�[�u
//==========================================================================
void CSkillTree::SaveJson()
{
	// �t�@�C���ɃL�����N�^�[�̃f�[�^����������
	std::ofstream outFile(LOADTEXT);
	if (!outFile.is_open())
	{
		return;
	}

	// �o�̓f�[�^
	nlohmann::json outputData;
	to_json(outputData);

	// �t�@�C���Ƀf�[�^����������
	outFile << std::setw(4) << outputData << std::endl;
}

//==========================================================================
// �`�揈��
//==========================================================================
void CSkillTree::Draw()
{
	
}

//==========================================================================
// �A�C�R���擾
//==========================================================================
std::vector<CSkillTree_Icon*> CSkillTree::GetIcon() const
{
	return m_pSkillIcon;
}

//==========================================================================
// �K���󋵐ݒ�
//==========================================================================
void CSkillTree::SetMastering(std::vector<CSkillTree_Icon::eMastering> mastering)
{
	for (int i = 0; i < static_cast<int>(m_SkillInfo.size()); i++)
	{
		m_SkillInfo[i].mastering = mastering[i];
	}
}

//==========================================================================
// �K���󋵐ݒ�
//==========================================================================
void CSkillTree::SetMastering(int nIdx, CSkillTree_Icon::eMastering mastering)
{
	if (m_SkillInfo.empty() ||
		static_cast<int>(m_SkillInfo.size()) <= nIdx)
	{
		return;
	}

	// �K���󋵐ݒ�
	m_SkillInfo[nIdx].mastering = mastering;
}

//==========================================================================
// �K���󋵎擾
//==========================================================================
std::vector<CSkillTree_Icon::eMastering> CSkillTree::GetMastering()
{
	if (m_SkillInfo.empty())
	{
		return std::vector<CSkillTree_Icon::eMastering>();
	}

	std::vector<CSkillTree_Icon::eMastering> mastering;
	for (const auto& info : m_SkillInfo)
	{
		mastering.push_back(info.mastering);
	}

	return mastering;
}

//==========================================================================
// �K���󋵃��Z�b�g
//==========================================================================
void CSkillTree::ResetMastering()
{
	// �S�ă��Z�b�g
	for (auto& info : m_SkillInfo)
	{
		info.mastering = CSkillTree_Icon::MASTERING_YET;
	}

	// �S�ă��Z�b�g
	if (!m_pSkillIcon.empty())
	{
		for (auto const& icon : m_pSkillIcon)
		{
			icon->SetMastering(CSkillTree_Icon::MASTERING_YET);
		}
	}
}

//==========================================================================
// �X�N���[����ɐݒ�
//==========================================================================
void CSkillTree::SetScreen()
{
	if (m_bOnScreen)
	{
		return;
	}

	// ��ʐ���
	m_pScreen = CSkillTree_Screen::Create();

	for (auto const& iconinfo : m_SkillInfo)
	{
		int nIdx = static_cast<int>(m_pSkillIcon.size());
		m_pSkillIcon.push_back(nullptr);

		// �X�L���A�C�R���ݒ�
		m_pSkillIcon[nIdx] = CSkillTree_Icon::Create(iconinfo);

		// �K���ς݂̂��͔̂\�͕t�^
		m_pSkillIcon[nIdx]->SetMastering(m_SkillInfo[nIdx].mastering);

		if (iconinfo.parentID != -1)
		{
			CSkillTree_Line::Create(iconinfo.pos, m_SkillInfo[iconinfo.parentID].pos);
		}
	}

	// �J�[�\������
	m_pCursor = CSkillTree_Cursor::Create(0);

	// �R�}���h����
	m_pCommand = CSkillTree_Command::Create();

	// �E�B���h�E����
	m_pWindow = CSkillTree_Window::Create();

	// ����������
	m_pDescription = CSkillTree_Description::Create();

	// ��ԃJ�E���^�[
	m_fStateTime = 0.0f;
	m_state = STATE_FADEIN;

	// �X�N���[����ɂ��邩�̃t���O
	m_bOnScreen = true;
}

//==========================================================================
// �X�N���[������J����
//==========================================================================
void CSkillTree::OutScreen()
{
	if (!m_bOnScreen)
	{
		return;
	}

	// �A�C�R���̏I��
	for (auto const& iconinfo : m_pSkillIcon)
	{
		iconinfo->Kill();
	}
	m_pSkillIcon.clear();

	// �X�N���[���̏I��
	m_pScreen->Uninit();
	m_pScreen = nullptr;

	// �J�[�\���I��
	m_pCursor->Uninit();
	m_pCursor = nullptr;

	// �R�}���h�I��
	m_pCommand->Uninit();
	m_pCommand = nullptr;

	// �E�B���h�E�I��
	m_pWindow->Uninit();
	m_pWindow = nullptr;

	// �������I��
	m_pDescription->Uninit();
	m_pDescription = nullptr;

	// �X�N���[����ɂ��邩�̃t���O
	m_bOnScreen = false;
}

//==========================================================================
// ���Z�b�g�J����
//==========================================================================
void CSkillTree::OutReset()
{
	if (m_pReset == nullptr)
	{
		return;
	}

	// �폜
	m_pReset->Kill();
	m_pReset = nullptr;
	m_state = STATE_NONE;
}
