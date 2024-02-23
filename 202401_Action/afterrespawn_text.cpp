//=============================================================================
// 
// ���X�|�[����e�L�X�g���� [afterrespawn_text.cpp]
// Author : ���n�Ή�
// 
//=============================================================================
#include "afterrespawn_text.h"
#include "manager.h"
#include "calculation.h"

//==========================================================================
// �萔��`
//==========================================================================
namespace
{
	const char* TEXTURE[] =
	{ 
		"data\\TEXTURE\\afterrespawn\\reset.png",				// �������Z�b�g
		"data\\TEXTURE\\afterrespawn\\addpoint.png",			// ��|�C���g
	};
	const float TIME_APPEARANCE = 0.6f;	// �o�ꎞ��
	const float TIME_FADEOUT = 4.0f;	// �t�F�[�h�A�E�g����
	const float TIME_START_FADEOUT = 3.5f;	// �t�F�[�h�A�E�g�����܂ł̎���
	const float WIDTH = 300.0f;	// ��
	const MyLib::Vector3 CREATE_POSITION = MyLib::Vector3(SCREEN_WIDTH - WIDTH + 35.0f, 500.0f, 0.0f);	// �����ʒu
}
CAfterRespawn_Text* CAfterRespawn_Text::m_pThisPtr= nullptr;		// ���g�̃|�C���^

//==========================================================================
// �֐����X�g
//==========================================================================
CAfterRespawn_Text::STATE_FUNC CAfterRespawn_Text::m_StateFuncList[] =
{
	&CAfterRespawn_Text::StateNone,			// �ʏ�
	&CAfterRespawn_Text::StateAppearance,	// �o��
	&CAfterRespawn_Text::StateFadeOut,		// �t�F�[�h�A�E�g
};

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CAfterRespawn_Text::CAfterRespawn_Text(int nPriority) : CObject(nPriority)
{
	// �l�̃N���A
	m_state = STATE::STATE_NONE;	// ���
	m_fStateTimer = 0.0f;			// ��ԃ^�C�}�[
	m_nNumDisp = 0;					// �`�搔
	memset(m_pText, 0, sizeof(m_pText));
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CAfterRespawn_Text::~CAfterRespawn_Text()
{

}

//==========================================================================
// ��������
//==========================================================================
CAfterRespawn_Text* CAfterRespawn_Text::Create()
{
	if (m_pThisPtr != nullptr)
	{
		return m_pThisPtr;
	}

	// �������̊m��
	m_pThisPtr = DEBUG_NEW CAfterRespawn_Text;

	if (m_pThisPtr != nullptr)
	{// �������̊m�ۂ��o���Ă�����

		// ����������
		m_pThisPtr->Init();
	}

	return m_pThisPtr;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CAfterRespawn_Text::Init()
{
	HRESULT hr;

	// ��ނ̐ݒ�
	SetType(TYPE_OBJECT2D);

	for (int i = 0; i < TEXTTYPE_MAX; i++)
	{
		m_pText[i] = CObject2D::Create(10);
		if (m_pText[i] == nullptr)
		{
			continue;
		}
		m_pText[i]->SetType(CObject::TYPE_OBJECT2D);

		// �e�N�X�`���̊��蓖��
		int nIdxTex = CTexture::GetInstance()->Regist(TEXTURE[i]);
		m_pText[i]->BindTexture(nIdxTex);

		// �����ݒ�
		m_pText[i]->SetPosition(CREATE_POSITION);

		// �e�N�X�`���T�C�Y�擾
		D3DXVECTOR2 size = CTexture::GetInstance()->GetImageSize(nIdxTex);
		m_pText[i]->SetSize(UtilFunc::Transformation::AdjustSizeByWidth(size, WIDTH));

		// �`��I�t
		m_pText[i]->SetEnableDisp(false);
	}

	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CAfterRespawn_Text::Uninit()
{

	m_pThisPtr = nullptr;

	for (int i = 0; i < TEXTTYPE_MAX; i++)
	{
		if (m_pText[i] == nullptr)
		{
			continue;
		}
		m_pText[i]->Uninit();
		m_pText[i] = nullptr;
	}

	// �I������
	Release();
}

//==========================================================================
// �X�V����
//==========================================================================
void CAfterRespawn_Text::Update()
{
	// ��ԕʏ���
	(this->*(m_StateFuncList[m_state]))();

	/*for (int i = 0; i < TEXTTYPE_MAX; i++)
	{
		if (m_pText[i] == nullptr ||
			!m_pText[i]->IsDisp())
		{
			continue;
		}
	}*/
}

//==========================================================================
// �o��
//==========================================================================
void CAfterRespawn_Text::StateAppearance()
{

	// �^�C�}�[���Z
	m_fStateTimer -= CManager::GetInstance()->GetDeltaTime();

	if (m_fStateTimer <= 0.0f)
	{
		// �^�C�}�[���Z�b�g
		m_fStateTimer = TIME_FADEOUT;
		m_state = STATE::STATE_FADEOUT;

		for (int i = 0; i < TEXTTYPE_MAX; i++)
		{
			if (m_pText[i] == nullptr ||
				!m_pText[i]->IsDisp())
			{
				continue;
			}
			// �ʒu
			MyLib::Vector3 pos = m_pText[i]->GetPosition();
			pos.x = CREATE_POSITION.x;
			m_pText[i]->SetPosition(pos);
		}
		return;
	}

	for (int i = 0; i < TEXTTYPE_MAX; i++)
	{
		if (m_pText[i] == nullptr ||
			!m_pText[i]->IsDisp())
		{
			continue;
		}

		MyLib::Vector3 pos = m_pText[i]->GetPosition();

		pos.x = UtilFunc::Correction::EasingEaseOut(1600.0f, CREATE_POSITION.x, TIME_FADEOUT, 0.0f, m_fStateTimer);

		// �ʒu�ړ�
		m_pText[i]->SetPosition(pos);
	}
}

//==========================================================================
// �t�F�[�h�A�E�g
//==========================================================================
void CAfterRespawn_Text::StateFadeOut()
{
	m_fStateTimer -= CManager::GetInstance()->GetDeltaTime();
	if (m_fStateTimer <= TIME_FADEOUT - TIME_START_FADEOUT)
	{
		// �s�����x�X�V
		float alpha = UtilFunc::Correction::EasingLinear(1.0f, 0.0f, TIME_FADEOUT - TIME_START_FADEOUT, 0.0f, m_fStateTimer);

		for (int i = 0; i < TEXTTYPE_MAX; i++)
		{
			if (m_pText[i] == nullptr ||
				!m_pText[i]->IsDisp())
			{
				continue;
			}
			m_pText[i]->SetAlpha(alpha);

			// �ʒu�ړ�
			m_pText[i]->AddPosition(MyLib::Vector3(0.0f, -1.0f, 0.0f));
		}
	}

	if (m_fStateTimer <= 0.0f)
	{
		Uninit();
	}
}

//==========================================================================
// �e�L�X�g�ݒ�
//==========================================================================
void CAfterRespawn_Text::SetText(TEXTTYPE text)
{
	int type = static_cast<int>(text);
	if (m_pText[type] == nullptr)
	{
		return;
	}

	// �`��ON
	m_pText[type]->SetEnableDisp(true);

	MyLib::Vector3 setpos = CREATE_POSITION;
	setpos.x = 1600.0f;

	for (int i = 0; i < TEXTTYPE_MAX; i++)
	{
		if (m_pText[i] == nullptr ||
			!m_pText[i]->IsDisp())
		{
			continue;
		}
		float height = m_pText[i]->GetSize().y * 2.0f;
		setpos.y += height;

		m_pText[i]->SetPosition(setpos);
		m_pText[i]->Update();
	}

	// �`�搔���Z
	m_nNumDisp++;

	// �^�C�}�[���Z�b�g
	m_fStateTimer = TIME_APPEARANCE;
	m_state = STATE::STATE_APPEARANCE;
}

//==========================================================================
// �f�t�H���g�e�L�X�g�ݒ�
//==========================================================================
void CAfterRespawn_Text::SetDefaultText()
{
	SetText(TEXTTYPE::TEXTTYPE_RESET);
	SetText(TEXTTYPE::TEXTTYPE_ADDPOINT);
}

//==========================================================================
// �e�L�X�g���Z�b�g
//==========================================================================
void CAfterRespawn_Text::ResetText()
{
	// �`�搔���Z�b�g
	m_nNumDisp = 0;

	for (const auto& text : m_pText)
	{
		text->SetEnableDisp(false);
	}
}
