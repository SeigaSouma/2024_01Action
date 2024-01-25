//=============================================================================
// 
// �X�L���c���[�A�C�R������ [skilltree_icon.cpp]
// Author : ���n�Ή�
// 
//=============================================================================
#include "skilltree_icon.h"
#include "renderer.h"
#include "texture.h"
#include "manager.h"
#include "sound.h"
#include "input.h"
#include "game.h"
#include "calculation.h"

//==========================================================================
// �}�N����`
//==========================================================================
namespace
{
	const char* TEXTURE[] =
	{
		"data\\TEXTURE\\skilltree\\icon_life.png",
		"data\\TEXTURE\\skilltree\\icon_power.png",
		"data\\TEXTURE\\skilltree\\icon_stamina.png",
	};
	const float SIZE_ICON = 50.0f;		// �A�C�R���T�C�Y
	const float TIME_FADE = 0.5f;	// �t�F�[�h�A�E�g����
}

//==========================================================================
// �֐��|�C���^
//==========================================================================
CSkillTree_Icon::STATE_FUNC CSkillTree_Icon::m_StateFuncList[] =
{
	&CSkillTree_Icon::StateNone,		// �Ȃɂ��Ȃ�
};

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CSkillTree_Icon::CSkillTree_Icon(int nPriority) : CObject2D(nPriority)
{
	// �l�̃N���A
	m_fStateTime = 0;				// ��ԑJ�ڃJ�E���^�[
	m_state = STATE_NONE;			// ���
	m_Mastering = MASTERING_YET;	// �K�����
	m_SkillIconInfo = sSkillIcon();	// �X�L���A�C�R��
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CSkillTree_Icon::~CSkillTree_Icon()
{

}

//==========================================================================
// ��������
//==========================================================================
CSkillTree_Icon* CSkillTree_Icon::Create(void)
{
	// �����p�̃I�u�W�F�N�g
	CSkillTree_Icon* pEffect = nullptr;

	// �������̊m��
	pEffect = DEBUG_NEW CSkillTree_Icon;

	if (pEffect != nullptr)
	{
		// ����������
		pEffect->Init();
	}

	return pEffect;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CSkillTree_Icon::Init(void)
{
	// ����������
	HRESULT hr = CObject2D::Init();
	if (FAILED(hr))
	{// ���s�����Ƃ�
		return E_FAIL;
	}

	// ��ނ̐ݒ�
	SetType(TYPE_OBJECT2D);

	// �e�N�X�`���̊��蓖��
	int nIdx = CTexture::GetInstance()->Regist(TEXTURE[0]);
	BindTexture(nIdx);

	// �T�C�Y�ݒ�
	SetSize(D3DXVECTOR2(SIZE_ICON, SIZE_ICON));
	SetSizeOrigin(GetSize());

	// ��ԃJ�E���^�[
	m_fStateTime = 0.0f;

	// �ʒu�ݒ�
	SetPosition(MyLib::Vector3(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f, 0.0f));

	// �F�ݒ�
	SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));

	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CSkillTree_Icon::Uninit(void)
{
	// �I������
	CObject2D::Uninit();
}

//==========================================================================
// �X�V����
//==========================================================================
void CSkillTree_Icon::Update(void)
{
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
void CSkillTree_Icon::StateNone(void)
{

}

//==========================================================================
// �`�揈��
//==========================================================================
void CSkillTree_Icon::Draw(void)
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
// �A�C�R�����ݒ�
//==========================================================================
void CSkillTree_Icon::SetIconInfo(sSkillIcon iconinfo)
{
	m_SkillIconInfo = iconinfo;
}

//==========================================================================
// �A�C�R�����擾
//==========================================================================
CSkillTree_Icon::sSkillIcon CSkillTree_Icon::GetIconInfo(void)
{
	return m_SkillIconInfo;
}

//==========================================================================
// ���_���ݒ菈��
//==========================================================================
void CSkillTree_Icon::SetVtx(void)
{
	// ���_�ݒ�
	CObject2D::SetVtx();
}

