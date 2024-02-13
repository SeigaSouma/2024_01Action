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
#include "camera.h"
#include "game.h"
#include "player.h"
#include "input.h"
#include "skilltree.h"
#include "instantfade.h"
#include "3D_effect.h"
#include "MyEffekseer.h"
#include "transferBeacon.h"

//==========================================================================
// �萔��`
//==========================================================================
namespace
{
	const char* BAGMODEL = "data\\MODEL\\enhance\\stoneplane.x";
	const MyLib::Vector3 POSITION = MyLib::Vector3(0.0f, 0.0f, 1100.0f);	// ���a
	const float RADIUS = 80.0f;	// ���a
	const float TIME_STARTUP = 1.0f;		// �N������
}
CSkillTree_Obj* CSkillTree_Obj::m_pThisPtr = nullptr;		// ���g�̃|�C���^

//==========================================================================
// �֐��|�C���^
//==========================================================================
CSkillTree_Obj::STATE_FUNC CSkillTree_Obj::m_StateFuncList[] =
{
	&CSkillTree_Obj::StateNone,		// �Ȃɂ��Ȃ�
	&CSkillTree_Obj::StateStartUp,	// �N��
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
// �C���X�^���X�擾
//==========================================================================
CSkillTree_Obj* CSkillTree_Obj::GetInstance()
{
	if (m_pThisPtr == nullptr)
	{
		m_pThisPtr = Create();
	}
	return m_pThisPtr;
}

//==========================================================================
// ��������
//==========================================================================
CSkillTree_Obj *CSkillTree_Obj::Create()
{
	if (m_pThisPtr == nullptr)
	{// �܂��������Ă��Ȃ�������

		// �C���X�^���X����
		m_pThisPtr = DEBUG_NEW CSkillTree_Obj;
		m_pThisPtr->Init();
	}

	return m_pThisPtr;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CSkillTree_Obj::Init()
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
void CSkillTree_Obj::Uninit()
{
	m_pThisPtr = nullptr;

	// �I������
	CObjectX::Uninit();
}

//==========================================================================
// �X�V����
//==========================================================================
void CSkillTree_Obj::Update()
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
void CSkillTree_Obj::CollisionPlayer()
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
		if (pInputGamepad->GetTriggerRT(0) ||
			pInputKeyboard->GetTrigger(DIK_RETURN))
		{
			// �F�胂�[�V�����ݒ�
			pPlayer->SetState(CPlayer::STATE_PRAYER);
			pPlayer->SetMotion(CPlayer::MOTION_PRAYER);

			// �F�菀����Ԃɐݒ�
			CCamera* pCamera = CManager::GetInstance()->GetCamera();
			pCamera->SetControlState(DEBUG_NEW CCameraControlState_BeforePrayer(CManager::GetInstance()->GetCamera()));
		}
	}
}

//==========================================================================
// �N������
//==========================================================================
void CSkillTree_Obj::StartUp()
{
	
	// �N��
	m_state = STATE_STARTUP;

	// �X�L���c���[�ɕύX
	CGame::GetInstance()->GetGameManager()->SetType(CGameManager::SCENE_SKILLTREE);

	// �ڕW�̒����ݒ�
	CCamera* pCamera = CManager::GetInstance()->GetCamera();
	pCamera->SetLenDest(pCamera->GetOriginDistance() + 900.0f, 120, 1000.0f, 0.05f);

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

//==========================================================================
// �ċN������
//==========================================================================
void CSkillTree_Obj::ReStartUp()
{

	// �G�t�F�N�g�S�Ē�~
	CMyEffekseer::GetInstance()->StopAll();
	CMyEffekseer::GetInstance()->StopAll();

	// ���[�v�G�t�F�N�g�Đ�
	MyLib::Vector3 pos = POSITION;
	pos.y += 250.0f;
	pos.z += 50.0f;

	CMyEffekseer::GetInstance()->SetEffect(
		&m_pWeaponHandle,
		CMyEffekseer::EFKLABEL_STONEBASE_LIGHT,
		pos,
		0.0f, 0.0f, 100.0f, false);
}

//==========================================================================
// �Ȃɂ��Ȃ����
//==========================================================================
void CSkillTree_Obj::StateNone()
{
	m_fStateTime = 0.0f;
}

//==========================================================================
// �N�����
//==========================================================================
void CSkillTree_Obj::StateStartUp()
{
	// ��ԃJ�E���^�[���Z
	m_fStateTime += CManager::GetInstance()->GetDeltaTime();

	if (TIME_STARTUP <= m_fStateTime)
	{
		m_state = STATE_NONE;

		// �X�L���c���[����
		CSkillTree::GetInstance()->SetScreen();

		// �����ʒu
		MyLib::Vector3 pos = POSITION;
		pos.z -= 50.0f;

		// �F���Ԃɐݒ�
		CCamera* pCamera = CManager::GetInstance()->GetCamera();
		pCamera->SetTargetPosition(pos);
		pCamera->SetStateCamraR(DEBUG_NEW CStateCameraR_Prayer());
		pCamera->SetControlState(DEBUG_NEW CCameraControlState_Prayer(CManager::GetInstance()->GetCamera()));
	}
}

//==========================================================================
// �`�揈��
//==========================================================================
void CSkillTree_Obj::Draw()
{
	// �`��
	CObjectX::Draw();
}

