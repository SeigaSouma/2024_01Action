//=============================================================================
// 
// �X�L���c���[��ʃg���� [skilltree_screen.cpp]
// Author : ���n�Ή�
// 
//=============================================================================
#include "skilltree_screen.h"
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
	const char* TEXTURE = "data\\TEXTURE\\skilltree\\skilltree_bg4.png";
	const float SCROLL = 0.0001f;
}

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CSkillTree_Screen::CSkillTree_Screen(int nPriority) : CObject2D(nPriority)
{
	// �l�̃N���A
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CSkillTree_Screen::~CSkillTree_Screen()
{

}

//==========================================================================
// ��������
//==========================================================================
CSkillTree_Screen* CSkillTree_Screen::Create(void)
{
	// �����p�̃I�u�W�F�N�g
	CSkillTree_Screen* pEffect = NULL;

	// �������̊m��
	pEffect = DEBUG_NEW CSkillTree_Screen;

	if (pEffect != NULL)
	{
		// ����������
		pEffect->Init();
	}

	return pEffect;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CSkillTree_Screen::Init(void)
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
	int nIdx = CTexture::GetInstance()->Regist(TEXTURE);
	BindTexture(nIdx);

	// �T�C�Y�ݒ�
	SetSize(D3DXVECTOR2(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f));
	SetSizeOrigin(GetSize());

	// �ʒu�ݒ�
	SetPosition(MyLib::Vector3(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f, 0.0f));

	// �F�ݒ�
	SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));

	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CSkillTree_Screen::Uninit(void)
{
	// �I������
	CObject2D::Uninit();
}

//==========================================================================
// �X�V����
//==========================================================================
void CSkillTree_Screen::Update(void)
{
	D3DXVECTOR2* tex = GetTex();
	tex[0].x += SCROLL;
	tex[1].x += SCROLL;
	tex[2].x += SCROLL;
	tex[3].x += SCROLL;

	// ���_���W�̐ݒ�
	SetVtx();
}

//==========================================================================
// �`�揈��
//==========================================================================
void CSkillTree_Screen::Draw(void)
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
void CSkillTree_Screen::SetVtx(void)
{
	// ���_�ݒ�
	CObject2D::SetVtx();
}

