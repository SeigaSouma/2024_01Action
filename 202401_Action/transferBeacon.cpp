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

//==========================================================================
// �萔��`
//==========================================================================
namespace
{
	const char* BAGMODEL = "data\\MODEL\\compactcore_01.x";
	const float RADIUS = 80.0f;	// ���a
	const float TIME_DMG = static_cast<float>(30) / static_cast<float>(mylib_const::DEFAULT_FPS);		// �_���[�W���� 
	const float TIME_INVICIBLE = static_cast<float>(60) / static_cast<float>(mylib_const::DEFAULT_FPS);	// ���G����
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
	m_fStateTime = 0.0f;	// ��ԃJ�E���^�[
	m_state = STATE_NONE;	// ���
	m_TransType = TRANSTYPE_ENHANCE;	// �]�ڎ��
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
CTransferBeacon *CTransferBeacon::Create(eTransType transtype, const MyLib::Vector3 pos)
{
	// �����p�̃I�u�W�F�N�g
	CTransferBeacon *pBag = NULL;

	// �������̊m��
	pBag = DEBUG_NEW CTransferBeacon;

	if (pBag != NULL)
	{// �������̊m�ۂ��o���Ă�����

		// �]�ڎ��
		pBag->m_TransType = transtype;

		// �ʒu���蓖��
		pBag->SetPosition(pos);

		// ����������
		pBag->Init();
	}

	return pBag;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CTransferBeacon::Init(void)
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
	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CTransferBeacon::Uninit(void)
{
	// ���X�g����폜
	m_List.Delete(this);

	// �I������
	CObjectX::Uninit();
}

//==========================================================================
// �X�V����
//==========================================================================
void CTransferBeacon::Update(void)
{
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
void CTransferBeacon::CollisionPlayer(void)
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
}

//==========================================================================
// �Ȃɂ��Ȃ����
//==========================================================================
void CTransferBeacon::StateNone(void)
{
	m_fStateTime = 0.0f;
}

//==========================================================================
// �]�ڏ��
//==========================================================================
void CTransferBeacon::StateTransfer(void)
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
void CTransferBeacon::Draw(void)
{
	// �`��
	CObjectX::Draw();
}

