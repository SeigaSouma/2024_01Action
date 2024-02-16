//=============================================================================
// 
//  �]�ڃr�[�R������ [transferBeacon.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "transferBeacon.h"
#include "manager.h"
#include "texture.h"
#include "renderer.h"
#include "sound.h"
#include "calculation.h"
#include "game.h"
#include "fade.h"
#include "instantfade.h"
#include "player.h"
#include "input.h"
#include "3D_effect.h"
#include "MyEffekseer.h"
#include "pickupguide.h"

//==========================================================================
// �萔��`
//==========================================================================
namespace
{
	const char* BAGMODEL = "data\\MODEL\\transfer\\transferstone.x";
	const float RADIUS = 80.0f;	// ���a
	const float TIME_VERTICAL = 3.0f;		// �㏸����
}

//==========================================================================
// �֐��|�C���^
//==========================================================================
CTransferBeacon::STATE_FUNC CTransferBeacon::m_StateFuncList[] =
{
	&CTransferBeacon::StateNone,		// �Ȃɂ��Ȃ�
	&CTransferBeacon::StateTransfer,	// �]��
};

//==========================================================================
// �ÓI�����o�ϐ�
//==========================================================================
CListManager<CTransferBeacon> CTransferBeacon::m_List = {};	// ���X�g

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CTransferBeacon::CTransferBeacon(int nPriority) : CObjectX(nPriority)
{
	// �l�̃N���A
	m_nCntEffect = 0;
	m_fStateTime = 0.0f;	// ��ԃJ�E���^�[
	m_state = STATE_NONE;	// ���
	m_TransType = TRANSTYPE_ENHANCE;	// �]�ڎ��
	m_pPickupGuide = nullptr;	// �s�b�N�A�b�v�K�C�h�̃|�C���^
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CTransferBeacon::~CTransferBeacon()
{
	
}

//==========================================================================
// ��������
//==========================================================================
CTransferBeacon *CTransferBeacon::Create(eTransType transtype, const MyLib::Vector3& pos)
{
	// �������̊m��
	CTransferBeacon* pBag = DEBUG_NEW CTransferBeacon;

	if (pBag != NULL)
	{// �������̊m�ۂ��o���Ă�����

		// �]�ڎ��
		pBag->m_TransType = transtype;

		// �ʒu���蓖��
		pBag->SetPosition(pos);
		pBag->SetOriginPosition(pos);

		// ����������
		pBag->Init();
	}

	return pBag;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CTransferBeacon::Init()
{
	// ���X�g�ɒǉ�
	m_List.Regist(this);

	// ��ނ̐ݒ�
	CObject::SetType(TYPE_OBJECTX);

	// ����������
	HRESULT hr = CObjectX::Init(BAGMODEL);
	if (FAILED(hr))
	{
		return E_FAIL;
	}

	// �����ʒu
	MyLib::Vector3 spawnpos = GetOriginPosition();
	spawnpos.y -= 150.0f;

	// �]��
	CMyEffekseer::GetInstance()->SetEffect(
		CMyEffekseer::EFKLABEL_TRANSFER,
		spawnpos,
		0.0f, 0.0f, 100.0f, false);

	// �s�b�N�A�b�v�K�C�h����
	if (!m_pPickupGuide)
	{
		m_pPickupGuide = CPickupGuide::Create(GetOriginPosition(), CPickupGuide::TYPE_TRANSFER);
	}

	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CTransferBeacon::Uninit()
{
	// �s�b�N�A�b�v�K�C�h
	m_pPickupGuide = nullptr;

	// ���X�g����폜
	m_List.Delete(this);

	// �I������
	CObjectX::Uninit();
}

//==========================================================================
// �폜
//==========================================================================
void CTransferBeacon::Kill()
{
	// �s�b�N�A�b�v�K�C�h
	if (m_pPickupGuide)
	{
		m_pPickupGuide->Uninit();
		m_pPickupGuide = nullptr;
	}

	// ���X�g����폜
	m_List.Delete(this);

	// �I������
	CObjectX::Uninit();
}

//==========================================================================
// �X�V����
//==========================================================================
void CTransferBeacon::Update()
{
	// ��ԃJ�E���^�[���Z
	m_fStateTime += CManager::GetInstance()->GetDeltaTime();

	MyLib::Vector3 pos = GetPosition();
	pos.y = 150.0f + sinf(D3DX_PI * (m_fStateTime / TIME_VERTICAL)) * 10.0f;
	SetPosition(pos);

	m_nCntEffect = (m_nCntEffect + 1) % 6;
	if (m_nCntEffect == 0)
	{
		float move = UtilFunc::Transformation::Random(-100, 100) * 0.01f;
		float moveY = UtilFunc::Transformation::Random(20, 30) * 0.1f;
		float radius = UtilFunc::Transformation::Random(50, 80);

		pos.y -= 20.0f;
		CEffect3D* pEffect = CEffect3D::Create(
			pos,
			MyLib::Vector3(move, moveY, move),
			D3DXCOLOR(0.2f, 1.0f, 0.4f, 1.0f),
			radius, 60, CEffect3D::MOVEEFFECT_SUB, CEffect3D::TYPE_SMOKE);
		//pEffect->SetDisableZSort();
	}

	if (m_fStateTime >= TIME_VERTICAL * 2.0f)
	{
		m_fStateTime = 0.0f;
	}


	// �v���C���[�Ƃ̓����蔻��
	CollisionPlayer();

	// ��ԕʏ���
	(this->*(m_StateFuncList[m_state]))();

	// �J�ڂȂ��t�F�[�h�擾
	if (CManager::GetInstance()->GetInstantFade()->GetState() == CInstantFade::STATE_FADECOMPLETION)
	{
		Kill();
	}
}

//==========================================================================
// �v���C���[�Ƃ̓����蔻�菈��
//==========================================================================
void CTransferBeacon::CollisionPlayer()
{
	if (!CGame::GetInstance()->GetGameManager()->IsControll())
	{// �s���ł��Ȃ��Ƃ�
		return;
	}

	if (CManager::GetInstance()->GetInstantFade()->GetState() != CInstantFade::STATE_NONE)
	{
		return;
	}

	// �v���C���[�擾
	CListManager<CPlayer> playerList = CPlayer::GetListObj();
	CPlayer* pPlayer = nullptr;

	MyLib::Vector3 pos = GetPosition();

	bool bHit = false;
	CPickupGuide::STATE guideState = CPickupGuide::STATE_FADEOUT;

	// ���X�g���[�v
	while (playerList.ListLoop(&pPlayer))
	{
		pPlayer->SetEnableTouchBeacon(false);
		if (UtilFunc::Collision::CircleRange3D(pos, pPlayer->GetPosition(), RADIUS, pPlayer->GetRadius()))
		{
			bHit = true;
			pPlayer->SetEnableTouchBeacon(true);
			guideState = CPickupGuide::STATE_FADEIN;
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
			// �J�ڂȂ��t�F�[�h�ǉ�
			CManager::GetInstance()->GetInstantFade()->SetFade(D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f), 40);

			switch (m_TransType)
			{
			case CTransferBeacon::TRANSTYPE_ENHANCE:
				// ������ԂɕύX
				CGame::GetInstance()->GetGameManager()->SetType(CGameManager::SCENE_ENHANCE);
				break;

			case CTransferBeacon::TRANSTYPE_GAMEMAIN:
				// �J�ڏ�ԂɕύX
				CGame::GetInstance()->GetGameManager()->SetType(CGameManager::SCENE_TRANSITION);
				break;
			}

			// ���X�g���[�v
			pPlayer = nullptr;
			while (playerList.ListLoop(&pPlayer))
			{
				pPlayer->SetEnableTouchBeacon(false);
			}
		}
	}

	// �s�b�N�A�b�v�K�C�h�̏�Ԑݒ�
	if (m_pPickupGuide)
	{
		m_pPickupGuide->SetState(guideState);
	}
}

//==========================================================================
// �Ȃɂ��Ȃ����
//==========================================================================
void CTransferBeacon::StateNone()
{
	
}

//==========================================================================
// �]�ڏ��
//==========================================================================
void CTransferBeacon::StateTransfer()
{

	// �����擾
	MyLib::Vector3 rot = GetRotation();
	rot.y += D3DX_PI * 0.05f;
	UtilFunc::Transformation::RotNormalize(rot.y);

	// �����ݒ�
	SetRotation(rot);
}

//==========================================================================
// �G�t�F�N�g�ݒ�
//==========================================================================
void CTransferBeacon::SetEffect()
{
	// �����ʒu
	MyLib::Vector3 spawnpos = GetOriginPosition();
	spawnpos.y -= 150.0f;

	// �]��
	CMyEffekseer::GetInstance()->SetEffect(
		CMyEffekseer::EFKLABEL_TRANSFER,
		spawnpos,
		0.0f, 0.0f, 100.0f, false);
}

//==========================================================================
// �`�揈��
//==========================================================================
void CTransferBeacon::Draw()
{
	// �`��
	CObjectX::Draw();
}

