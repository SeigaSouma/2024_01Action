//=============================================================================
// 
// �X�e�[�W�N���A�e�L�X�g���� [stagecleartext.cpp]
// Author : ���n�Ή�
// 
//=============================================================================
#include "stagecleartext.h"
#include "manager.h"
#include "sound.h"
#include "calculation.h"
#include "camera.h"
#include "battleresult.h"
#include "game.h"

//==========================================================================
// �}�N����`
//==========================================================================
namespace
{
	const char* TEXTURE = "data\\TEXTURE\\battlewin\\text_win.png";
	const float TIME_SCALNONE = 0.4f;			// �Ȃɂ��Ȃ�
	const float TIME_SCALEDOWN = 0.16f;			// �k������
	const float TIME_SCALENONE = 2.0f;			// ���킹�鎞��
	const float TIME_FADEOUT = 0.4f;			// �t�F�[�h�A�E�g����
}

//==========================================================================
// �֐��|�C���^
//==========================================================================
CStageClearText::STATE_FUNC CStageClearText::m_StateFuncList[] =
{
	&CStageClearText::StateNone,
	&CStageClearText::StateScaleDOWN,
	&CStageClearText::StateScaleNone,
	&CStageClearText::StateFadeOut,
};

//==========================================================================
// �ÓI�����o�ϐ��錾
//==========================================================================

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CStageClearText::CStageClearText(int nPriority) : CObject2D(nPriority)
{
	// �l�̃N���A
	m_state = STATE_NONE;			// ���
	m_fStateTimer = 0.0f;			// ��ԃ^�C�}�[
	m_bCreateResultWindow = false;	// ���U���g��ʂ̌Ăяo���t���O
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CStageClearText::~CStageClearText()
{

}

//==========================================================================
// ��������
//==========================================================================
CStageClearText* CStageClearText::Create(const MyLib::Vector3 pos)
{
	// �������̊m��
	CStageClearText* pEffect = DEBUG_NEW CStageClearText;

	if (pEffect != nullptr)
	{// �������̊m�ۂ��o���Ă�����

		// ����������
		pEffect->Init();
		pEffect->SetPosition(pos);
	}

	return pEffect;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CStageClearText::Init()
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
	int nTexIdx = CTexture::GetInstance()->Regist(TEXTURE);
	BindTexture(nTexIdx);

	// �T�C�Y�ݒ�
	D3DXVECTOR2 size = CTexture::GetInstance()->GetImageSize(nTexIdx);
	size = UtilFunc::Transformation::AdjustSizeByWidth(size, 1600.0f);
	SetSize(size);
	SetSizeOrigin(size);

	// �k������J�n
	m_fStateTimer = TIME_SCALEDOWN;
	m_state = eState::STATE_SCALEDOWN;

	// �����ݒ�
	SetRotation(MyLib::Vector3(0.0f, 0.0f, -D3DX_PI * 0.35f));
	SetOriginRotation(GetRotation());

	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CStageClearText::Uninit()
{
	// �I������
	CObject2D::Uninit();
}

//==========================================================================
// �X�V����
//==========================================================================
void CStageClearText::Update()
{
	if (IsDeath())
	{
		return;
	}

	// ��ԕʏ���
	(this->*(m_StateFuncList[m_state]))();

	if (IsDeath())
	{
		return;
	}

	// ���_���W�̐ݒ�
	SetVtx();
}

//==========================================================================
// �����Ȃ����
//==========================================================================
void CStageClearText::StateNone()
{
	// ��ԑJ�ڃJ�E���^�[���Z
	m_fStateTimer -= CManager::GetInstance()->GetDeltaTime();

	if (m_fStateTimer <= 0.0f)
	{
		m_fStateTimer = TIME_FADEOUT;
		m_state = eState::STATE_FADEOUT;
	}
}

//==========================================================================
// �k�����
//==========================================================================
void CStageClearText::StateScaleDOWN()
{
	// �T�C�Y�擾
	D3DXVECTOR2 size = GetSize();
	D3DXVECTOR2 sizeOrigin = GetSizeOrigin();
	D3DXVECTOR2 destSize = UtilFunc::Transformation::AdjustSizeByWidth(size, 240.0f);

	// ��ԑJ�ڃJ�E���^�[���Z
	m_fStateTimer -= CManager::GetInstance()->GetDeltaTime();

	if (m_fStateTimer <= 0.0f)
	{
		m_fStateTimer = TIME_SCALENONE;
		m_state = STATE_SCALENONE;

		// �T�C�Y�ݒ�
		SetSize(destSize);
		SetSizeOrigin(destSize);

		// �U��
		CManager::GetInstance()->GetCamera()->SetShake(8, 25.0f, 0.0f);
		return;
	}

	// ���X�Ɍ���
	size.x = UtilFunc::Correction::EasingEaseOut(sizeOrigin.x, destSize.x, 1.0f - (m_fStateTimer / TIME_SCALEDOWN));
	size.y = UtilFunc::Correction::EasingEaseOut(sizeOrigin.y, destSize.y, 1.0f - (m_fStateTimer / TIME_SCALEDOWN));

	// �����ݒ�
	float angle = UtilFunc::Correction::EasingEaseOut(GetOriginRotation().z, 0.0f, 1.0f - (m_fStateTimer / TIME_SCALEDOWN));
	SetRotation(MyLib::Vector3(0.0f, 0.0f, angle));

	// �T�C�Y�ݒ�
	SetSize(size);
}

//==========================================================================
// �������
//==========================================================================
void CStageClearText::StateScaleNone()
{
	// �T�C�Y�擾
	D3DXVECTOR2 size = GetSize();
	D3DXVECTOR2 sizeOrigin = GetSizeOrigin();
	D3DXVECTOR2 destSize = UtilFunc::Transformation::AdjustSizeByWidth(size, 260.0f);

	// ��ԑJ�ڃJ�E���^�[���Z
	m_fStateTimer -= CManager::GetInstance()->GetDeltaTime();

	if (m_fStateTimer <= 0.0f)
	{
		m_fStateTimer = TIME_SCALNONE;
		m_state = eState::STATE_NONE;

		// �T�C�Y�ݒ�
		SetSize(destSize);
		SetSizeOrigin(destSize);
		return;
	}

	// ���X�Ɍ���
	size.x = UtilFunc::Correction::EasingEaseOut(sizeOrigin.x, destSize.x, 1.0f - (m_fStateTimer / TIME_SCALENONE));
	size.y = UtilFunc::Correction::EasingEaseOut(sizeOrigin.y, destSize.y, 1.0f - (m_fStateTimer / TIME_SCALENONE));

	// �T�C�Y�ݒ�
	SetSize(size);
}

//==========================================================================
// �t�F�[�h�A�E�g���
//==========================================================================
void CStageClearText::StateFadeOut()
{
	// ��ԑJ�ڃJ�E���^�[���Z
	m_fStateTimer -= CManager::GetInstance()->GetDeltaTime();

	// �s�����x�X�V
	float alpha = m_fStateTimer / TIME_FADEOUT;
	SetAlpha(alpha);

	if (TIME_FADEOUT * 0.5f <= m_fStateTimer &&
		!m_bCreateResultWindow)
	{
		m_bCreateResultWindow = true;

		// ��ʐ���
		CBattleResult::Create();
		CGame::GetInstance()->GetGameManager()->SetType(CGameManager::SceneType::SCENE_DURING_MAINRESULT);
	}

	if (m_fStateTimer <= 0.0f)
	{
		m_fStateTimer = 0.0f;
		Uninit();
		return;
	}
}

//==========================================================================
// �`�揈��
//==========================================================================
void CStageClearText::Draw()
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
void CStageClearText::SetVtx()
{
	// ���_�ݒ�
	CObject2D::SetVtx();
}

