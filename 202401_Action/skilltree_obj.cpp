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
#include "instantfade.h"
#include "3D_effect.h"
#include "MyEffekseer.h"

//==========================================================================
// �萔��`
//==========================================================================
namespace
{
	const char* BAGMODEL = "data\\MODEL\\enhance\\stoneplane.x";
	const MyLib::Vector3 POSITION = MyLib::Vector3(0.0f, 0.0f, 1100.0f);	// ���a
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
	m_pWeaponHandle = nullptr;		// �G�t�F�N�g�n���h���̃|�C���^
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

	MyLib::Vector3 pos = POSITION;
	pos.y += 250.0f;
	pos.z += 50.0f;


	//CMyEffekseer::GetInstance()->SetEffect(
		//	&m_pWeaponHandle,
		//	"data/Effekseer/debugline_green.efkefc",
		//	weponpos, rot, 0.0f, 40.0f, true);

	// ���[�v�G�t�F�N�g�Đ�
	CMyEffekseer::GetInstance()->SetEffect(
		&m_pWeaponHandle,
		CMyEffekseer::EFKLABEL_STONEBASE_LIGHT,
		pos,
		0.0f, 0.0f, 100.0f, false);
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
	float distance = static_cast<float>(UtilFunc::Transformation::Random(5, 180)) * 10.0f;
	float diffRadius = static_cast<float>(UtilFunc::Transformation::Random(-10, 10));
	for (int i = 0; i < 1; i++)
	{
		CEffect3D* pEffect = CEffect3D::Create(
			UtilFunc::Transformation::GetRandomPositionSphere(MyLib::Vector3(0.0f, 500.0f, 0.0f), distance),
			UtilFunc::Transformation::Random(-100, 100) * 0.01f,
			D3DXCOLOR(0.2f, 1.0f, 0.4f, 1.0f),
			30.0f + diffRadius, UtilFunc::Transformation::Random(160, 200), CEffect3D::MOVEEFFECT_SUB, CEffect3D::TYPE_POINT);
	}

	// ��ԃJ�E���^�[���Z
	m_fStateTime -= CManager::GetInstance()->GetDeltaTime();

	// �v���C���[�Ƃ̓����蔻��
	CollisionPlayer();

	// ��ԕʏ���
	(this->*(m_StateFuncList[m_state]))();

	// �J�ڂȂ��t�F�[�h�擾
	if (CManager::GetInstance()->GetInstantFade()->GetState() == CInstantFade::STATE_FADECOMPLETION)
	{
		Uninit();
	}
}

//==========================================================================
// �v���C���[�Ƃ̓����蔻�菈��
//==========================================================================
void CSkillTree_Obj::CollisionPlayer(void)
{
	if (!CGame::GetInstance()->GetGameManager()->IsControll())
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
		CInputKeyboard* pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();
		if (pInputGamepad->GetTrigger(CInputGamepad::BUTTON_A, 0) ||
			pInputKeyboard->GetTrigger(DIK_RETURN))
		{
			// �X�L���c���[�ɕύX
			CGame::GetInstance()->GetGameManager()->SetType(CGameManager::SCENE_SKILLTREE);

			// �X�L���c���[����
			CSkillTree::GetInstance()->SetScreen();


			// ���[�v�G�t�F�N�g��~
			if (m_pWeaponHandle != nullptr)
			{
				CMyEffekseer::GetInstance()->Stop(*m_pWeaponHandle);
				m_pWeaponHandle = nullptr;
			}

			// �����ʒu
			MyLib::Vector3 pos = POSITION;
			pos.y += 250.0f;
			pos.z -= 20.0f;

			// �N���G�t�F�N�g
			CMyEffekseer::GetInstance()->SetEffect(
				CMyEffekseer::EFKLABEL_STONEBASE_BEGIN,
				pos,
				0.0f, 0.0f, 100.0f, true);
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

