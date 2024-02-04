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
#include "player.h"
#include "skilltree.h"
#include "skillpoint.h"
#include "skilltree_behavior.h"
#include "multinumber.h"

//==========================================================================
// �}�N����`
//==========================================================================
namespace
{
	const D3DXCOLOR COLOR[] =
	{
		D3DXCOLOR(0.4f, 0.4f, 0.4f, 1.0f),	// ���K��
		D3DXCOLOR(1.0f, 0.4f, 1.0f, 1.0f),	// �K���ς�
		D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f),	// �K���\
	};
	const char* LOADTEXT = "data\\TEXT\\skilltree\\icontexture.txt";
	const char* NUMBER_TEXTURE = "data\\TEXTURE\\number\\number_blackclover_01.png";
	const float SIZE_ICON = 50.0f;	// �A�C�R���T�C�Y
	const float TIME_FADE = 0.5f;	// �t�F�[�h�A�E�g����
	const MyLib::Vector3 DISTANCE = MyLib::Vector3(SIZE_ICON, SIZE_ICON, 0.0f);
}

std::vector<int> CSkillTree_Icon::m_nTexIdx = {};	// �e�N�X�`���C���f�b�N�X�ԍ�
bool CSkillTree_Icon::m_bLoadComplete = false;		// ���[�h�����̃t���O
CListManager<CSkillTree_Icon> CSkillTree_Icon::m_List = {};	// ���X�g

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
	m_pAbillity = nullptr;			// �\�͂̃I�u�W�F�N�g
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
CSkillTree_Icon* CSkillTree_Icon::Create(sSkillIcon iconinfo)
{
	// �����p�̃I�u�W�F�N�g
	CSkillTree_Icon* pEffect = nullptr;

	// �������̊m��
	pEffect = DEBUG_NEW CSkillTree_Icon;

	if (pEffect != nullptr)
	{
		// �A�C�R�����ݒ�
		pEffect->m_SkillIconInfo = iconinfo;

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

	// �e�N�X�`���ǂݍ���
	if (!m_bLoadComplete)
	{
		ReadTexture();
	}

	// �e�N�X�`���̊��蓖��
	BindTexture(m_nTexIdx[m_SkillIconInfo.texID]);

	// �T�C�Y�ݒ�
	SetSize(D3DXVECTOR2(SIZE_ICON, SIZE_ICON));
	SetSizeOrigin(GetSize());

	// ��ԃJ�E���^�[
	m_fStateTime = 0.0f;

	// �ʒu�ݒ�
	SetPosition(MyLib::Vector3(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.5f, 0.0f));

	// �F�ݒ�
	SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));

	// �\�͊��蓖��
	m_pAbillity = CAbillityStrategy::CreateInstance(m_SkillIconInfo, CPlayer::GetListObj().GetData(0));

	// ���X�g�ɒǉ�
	m_List.Regist(this);


	// ��������
	m_apNumber = CMultiNumber::Create(GetPosition() + DISTANCE, GetSize() * 0.5f, 1, CNumber::OBJECTTYPE_2D, NUMBER_TEXTURE, true);
	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CSkillTree_Icon::Uninit(void)
{

	// ���X�g����폜
	m_List.Delete(this);

	if (m_pAbillity != nullptr)
	{
		delete m_pAbillity;
		m_pAbillity = nullptr;
	}

	if (m_apNumber != nullptr)
	{
		m_apNumber->Uninit();
		m_apNumber = nullptr;
	}

	// �I������
	CObject2D::Uninit();
}

//==========================================================================
// �I������
//==========================================================================
void CSkillTree_Icon::Kill(void)
{
	// ���X�g����폜
	m_List.Delete(this);

	if (m_pAbillity != nullptr)
	{
		delete m_pAbillity;
		m_pAbillity = nullptr;
	}

	if (m_apNumber != nullptr)
	{
		m_apNumber->Release();
		m_apNumber = nullptr;
	}

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

	if (m_Mastering != MASTERING_DONE &&
		(m_SkillIconInfo.parentID == -1 ||
		m_List.GetData(m_SkillIconInfo.parentID)->GetMastering() == MASTERING_DONE))
	{
		// �K���\�I
		m_Mastering = MASTERING_POSSIBLE;
	}

	D3DXCOLOR col = COLOR[m_Mastering];
	col.a = GetColor().a;
	SetColor(col);

	if (m_apNumber != nullptr)
	{
		m_apNumber->SetValue(m_SkillIconInfo.needpoint);
		m_apNumber->SetPosition(GetPosition() + DISTANCE);
		m_apNumber->SetColor(D3DXCOLOR(1.0f, 1.0f, 1.0f, GetColor().a));
	}


	// ��ԕۑ�
	CSkillTree::GetInstance()->SetMastering(m_SkillIconInfo.ID, m_Mastering);

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
// �����\�͊��蓖��
//==========================================================================
void CSkillTree_Icon::BindStartAvillity(void)
{
	// �\�͊��蓖��
	m_pAbillity->BindAbillity();

	// �K���ς݂ɂ���
	m_Mastering = MASTERING_DONE;
}

//==========================================================================
// �\�͊��蓖��
//==========================================================================
bool CSkillTree_Icon::BindAvillity(void)
{
	if (m_Mastering != MASTERING_POSSIBLE)
	{// �K���\�ȊO�̓��^�[��
		return false;
	}

	CPlayer* pPlayer = CPlayer::GetListObj().GetData(0);
	if (m_Mastering == MASTERING_POSSIBLE &&
		pPlayer->GetSkillPoint()->GetPoint() >= m_SkillIconInfo.needpoint)
	{// �K���\ && �|�C���g����Ă�

		// �\�͊��蓖��
		m_pAbillity->BindAbillity();

		// �X�L���|�C���g���Z
		pPlayer->GetSkillPoint()->SubPoint(m_SkillIconInfo.needpoint);

		// �K���ς݂ɂ���
		m_Mastering = MASTERING_DONE;
		return true;
	}
	return false;
}

//==========================================================================
// ���_���ݒ菈��
//==========================================================================
void CSkillTree_Icon::SetVtx(void)
{
	// ���_�ݒ�
	CObject2D::SetVtx();
}


//==========================================================================
// �e�N�X�`���ǂݍ��ݏ���
//==========================================================================
HRESULT CSkillTree_Icon::ReadTexture(void)
{
	char aComment[MAX_COMMENT] = {};	// �R�����g�p
	int nTexNum = 0;					// �t�@�C���̐�
	int nCntTexture = 0;				// �e�N�X�`���ǂݍ��݃J�E���g

	// �t�@�C���|�C���^
	FILE* pFile = NULL;

	//�t�@�C�����J��
	pFile = fopen(LOADTEXT, "r");

	if (pFile == NULL)
	{//�t�@�C�����J�����ꍇ
		return E_FAIL;
	}

	while (1)
	{// END_SCRIPT������܂ŌJ��Ԃ�

		// ������̓ǂݍ���
		fscanf(pFile, "%s", &aComment[0]);

		// �e�N�X�`�����̐ݒ�
		if (strcmp(&aComment[0], "NUM_TEXTURE") == 0)
		{// NUM_MODEL��������

			fscanf(pFile, "%s", &aComment[0]);	// =�̕�
			fscanf(pFile, "%d", &nTexNum);	// �e�N�X�`����
		}

		while (nCntTexture != nTexNum)
		{// �e�N�X�`���̐����ǂݍ��ނ܂ŌJ��Ԃ�

			fscanf(pFile, "%s", &aComment[0]);	// =�̕�

			if (strcmp(&aComment[0], "TEXTURE_FILENAME") == 0)
			{// TEXTURE_FILENAME��������

				fscanf(pFile, "%s", &aComment[0]);	// =�̕�
				fscanf(pFile, "%s", &aComment[0]);	// �t�@�C����
				
				// �e�N�X�`���̊��蓖��
				int nIdx = CTexture::GetInstance()->Regist(&aComment[0]);
				m_nTexIdx.push_back(nIdx);

				nCntTexture++;	// �e�N�X�`�������Z
			}
		}

		if (strcmp(&aComment[0], "END_SCRIPT") == 0)
		{// �I�������Ń��[�v�𔲂���

			break;
		}
	}

	// �t�@�C�������
	fclose(pFile);

	// ���[�h�����̃t���O�𗧂Ă�
	m_bLoadComplete = true;
	return S_OK;
}