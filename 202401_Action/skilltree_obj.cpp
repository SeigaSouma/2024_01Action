//=============================================================================
// 
//  �X�L���c���[�I�u�W�F�N�g���� [skilltree_obj.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "skilltree_obj.h"
#include "manager.h"
#include "texture.h"
#include "renderer.h"
#include "sound.h"
#include "calculation.h"
#include "game.h"
#include "player.h"
#include "input.h"
#include "skilltree.h"

//==========================================================================
// �萔��`
//==========================================================================
namespace
{
	const char* BAGMODEL = "data\\MODEL\\skilltree_obj.x";
	const MyLib::Vector3 POSITION = MyLib::Vector3(0.0f, 0.0f, 1000.0f);	// ���a
	const float RADIUS = 80.0f;	// ���a
	const float TIME_DMG = static_cast<float>(30) / static_cast<float>(mylib_const::DEFAULT_FPS);		// �_���[�W���� 
	const float TIME_INVICIBLE = static_cast<float>(60) / static_cast<float>(mylib_const::DEFAULT_FPS);	// ���G����
}

//==========================================================================
// �֐��|�C���^
//==========================================================================
CSkillTree_Obj::STATE_FUNC CSkillTree_Obj::m_StateFuncList[] =
{
	&CSkillTree_Obj::StateNone,		// �Ȃɂ��Ȃ�
	&CSkillTree_Obj::StateTransfer,	// �]��
};

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CSkillTree_Obj::CSkillTree_Obj(int nPriority) : CObjectX(nPriority)
{
	// �l�̃N���A
	m_fStateTime = 0.0f;	// ��ԃJ�E���^�[
	m_state = STATE_NONE;	// ���
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CSkillTree_Obj::~CSkillTree_Obj()
{
	
}

//==========================================================================
// ��������
//==========================================================================
CSkillTree_Obj *CSkillTree_Obj::Create(void)
{
	// �����p�̃I�u�W�F�N�g
	CSkillTree_Obj *pBag = NULL;

	// �������̊m��
	pBag = DEBUG_NEW CSkillTree_Obj;

	if (pBag != NULL)
	{// �������̊m�ۂ��o���Ă�����

		// ����������
		pBag->Init();
	}

	return pBag;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CSkillTree_Obj::Init(void)
{

	// ��ނ̐ݒ�
	CObject::SetType(TYPE_OBJECTX);

	// ����������
	HRESULT hr = CObjectX::Init(BAGMODEL);
	if (FAILED(hr))
	{
		return E_FAIL;
	}

	// �ʒu�ݒ�
	SetPosition(POSITION);
	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CSkillTree_Obj::Uninit(void)
{

	// �I������
	CObjectX::Uninit();
}

//==========================================================================
// �X�V����
//==========================================================================
void CSkillTree_Obj::Update(void)
{
	// ��ԃJ�E���^�[���Z
	m_fStateTime -= CManager::GetInstance()->GetDeltaTime();

	// �v���C���[�Ƃ̓����蔻��
	CollisionPlayer();

	// ��ԕʏ���
	(this->*(m_StateFuncList[m_state]))();
}

//==========================================================================
// �v���C���[�Ƃ̓����蔻�菈��
//==========================================================================
void CSkillTree_Obj::CollisionPlayer(void)
{
	if (!CGame::GetGameManager()->IsControll())
	{// �s���ł��Ȃ��Ƃ�
		return;
	}

	// �v���C���[�擾
	CListManager<CPlayer> playerList = CPlayer::GetListObj();
	CPlayer* pPlayer = nullptr;

	MyLib::Vector3 pos = GetPosition();

	bool bHit = false;

	// ���X�g���[�v
	while (playerList.ListLoop(&pPlayer))
	{
		pPlayer->SetEnableTouchBeacon(false);
		if (UtilFunc::Collision::CircleRange3D(pos, pPlayer->GetPosition(), RADIUS, pPlayer->GetRadius()))
		{
			bHit = true;
			pPlayer->SetEnableTouchBeacon(true);
			break;
		}
	}

	if (bHit)
	{
		// �Q�[���p�b�h���擾
		CInputGamepad* pInputGamepad = CManager::GetInstance()->GetInputGamepad();
		if (pInputGamepad->GetTrigger(CInputGamepad::BUTTON_A, 0))
		{
			// �X�L���c���[�ɕύX
			CGame::GetGameManager()->SetType(CGameManager::SCENE_SKILLTREE);

			// �X�L���c���[����
			CSkillTree::Create();
		}
	}
}

//==========================================================================
// �Ȃɂ��Ȃ����
//==========================================================================
void CSkillTree_Obj::StateNone(void)
{
	m_fStateTime = 0.0f;
}

//==========================================================================
// �]�ڏ��
//==========================================================================
void CSkillTree_Obj::StateTransfer(void)
{
	m_fStateTime = 0.0f;

	// �����擾
	MyLib::Vector3 rot = GetRotation();
	rot.y += D3DX_PI * 0.05f;
	UtilFunc::Transformation::RotNormalize(rot.y);

	// �����ݒ�
	SetRotation(rot);
}


//==========================================================================
// �`�揈��
//==========================================================================
void CSkillTree_Obj::Draw(void)
{
	// �`��
	CObjectX::Draw();
}

