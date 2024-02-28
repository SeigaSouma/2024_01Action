//=============================================================================
// 
//  �^�C�g������ [title.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "title.h"
#include "input.h"
#include "fade.h"
#include "renderer.h"
#include "calculation.h"
#include "debugproc.h"
#include "sound.h"
#include "particle.h"
#include "gallery.h"
#include "torch.h"
#include "MyEffekseer.h"
#include "titlelogo.h"
#include "title_pressenter.h"

//==========================================================================
// �萔��`
//==========================================================================
namespace
{
	const float TIME_FADELOGO = 0.6f;	// ���S�̃t�F�[�h�A�E�g����
	const char* TEXTURE = "data\\TEXTURE\\title\\titlelogo3.png";
}

//==========================================================================
// �ÓI�����o�ϐ��錾
//==========================================================================
CTitle* CTitle::m_pThisPtr = nullptr;	// ���g�̃|�C���^

//==========================================================================
// �֐��|�C���^
//==========================================================================
CTitle::SCENE_FUNC CTitle::m_SceneFunc[] =
{
	&CTitle::SceneNone,			// �Ȃɂ��Ȃ�
	&CTitle::SceneFadeInLogo,	// ���S�t�F�[�h�C��
	&CTitle::SceneFadeOutLoGo,	// ���S�t�F�[�h�A�E�g
};

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CTitle::CTitle()
{
	// �l�̃N���A
	m_SceneType = SCENETYPE::SCENETYPE_NONE;	// �V�[���̎��
	m_fSceneTime = 0.0f;						// �V�[���J�E���^�[
	m_pLogo = nullptr;		// ���S�̃|�C���^
	m_pPressEnter = nullptr;	// �v���X�G���^�[
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CTitle::~CTitle()
{

}

//==========================================================================
// ��������
//==========================================================================
CTitle* CTitle::Create()
{
	if (m_pThisPtr == nullptr)
	{// �܂��������Ă��Ȃ�������

		// �C���X�^���X����
		m_pThisPtr = DEBUG_NEW CTitle;
	}

	return m_pThisPtr;
}

//==========================================================================
// �C���X�^���X�擾
//==========================================================================
CTitle* CTitle::GetInstance()
{
	return m_pThisPtr;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CTitle::Init()
{

	// BGM�Đ�
	CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL_BGM_TITLE);

	// ����������
	if (FAILED(CScene::Init()))
	{// ���s�����ꍇ
		return E_FAIL;
	}


	// �ϏO�ݒ�
	CGallery::SetGallery();

	// �ϏO�̃��X�g�擾
	CListManager<CGallery> galleryList = CGallery::GetList();
	CGallery* pGallery = nullptr;
	while (galleryList.ListLoop(&pGallery))
	{
		pGallery->SetState(CGallery::STATE_CLEARHEAT);
	}

	// �����ݒ�
	CTorch::SetTorch();

	// �^�C�g�����
	//CTitleScreen::Create();

	// �^�C�g�����S����
	m_pLogo = CTitleLogo::Create(1.0f);

	// �v���X�G���^�[
	m_pPressEnter = CTitle_PressEnter::Create(1.0f);

	// �o
	CMyEffekseer::GetInstance()->SetEffect(
		CMyEffekseer::EFKLABEL::EFKLABEL_TITLEBLUR,
		MyLib::Vector3(185.0f, 65.0f, -148.0f),
		0.0f, 0.0f, 10.0f, false);

	// �V�[���̎��
	m_SceneType = SCENETYPE::SCENETYPE_NONE;

	// ����
	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CTitle::Uninit()
{
	m_pThisPtr = nullptr;

	// �I������
	CScene::Uninit();
}

//==========================================================================
// �X�V����
//==========================================================================
void CTitle::Update()
{
	CManager::GetInstance()->GetDebugProc()->Print(
		"���݂̃��[�h�F�y�^�C�g���z\n"
		"�؂�ւ��F�y F �z\n\n");

	// ���͏��擾
	CInputKeyboard* pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();
	CInputGamepad* pInputGamepad = CManager::GetInstance()->GetInputGamepad();

	if (CManager::GetInstance()->GetFade()->GetState() != CFade::STATE_NONE)
	{// �t�F�[�h���͔�����
		return;
	}

	// ��ԕʍX�V����
	(this->*(m_SceneFunc[m_SceneType]))();
}

//==========================================================================
// �Ȃɂ��Ȃ�
//==========================================================================
void CTitle::SceneNone()
{
	// �V�[���J�E���^�[
	m_fSceneTime = TIME_FADELOGO;
}

//==========================================================================
// ���S�t�F�[�h�C��
//==========================================================================
void CTitle::SceneFadeInLogo()
{

}

//==========================================================================
// ���S�t�F�[�h�A�E�g
//==========================================================================
void CTitle::SceneFadeOutLoGo()
{
	// �V�[���J�E���^�[���Z
	m_fSceneTime -= CManager::GetInstance()->GetDeltaTime();

	// �s�����x�X�V
	float alpha = m_fSceneTime / TIME_FADELOGO;
	m_pLogo->SetAlpha(alpha);

	// �G���^�[�̐F
	m_pPressEnter->SetAlpha(alpha);

	if (m_fSceneTime <= 0.0f)
	{
		m_fSceneTime = 0.0f;
		m_SceneType = SCENETYPE_NONE;

		// �s�����x�X�V
		m_pLogo->SetAlpha(1.0f);
		m_pLogo->Uninit();
		m_pLogo = nullptr;

		// �G���^�[�̐F
		m_pPressEnter->SetAlpha(1.0f);
		m_pPressEnter->Uninit();
		m_pPressEnter = nullptr;
		return;
	}
}

//==========================================================================
// �`�揈��
//==========================================================================
void CTitle::Draw()
{

}
