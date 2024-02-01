//=============================================================================
// 
//  �v���C���[���� [player.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "player.h"
#include "game.h"
#include "camera.h"
#include "manager.h"
#include "debugproc.h"
#include "renderer.h"
#include "input.h"
#include "enemy.h"
#include "calculation.h"
#include "score.h"
#include "texture.h"
#include "Xload.h"
#include "model.h"
#include "hp_gauge.h"
#include "hp_gauge_player.h"
#include "stamina_gauge_player.h"
#include "elevation.h"
#include "shadow.h"
#include "particle.h"
#include "3D_Effect.h"
#include "ballast.h"
#include "impactwave.h"
#include "sound.h"
#include "stage.h"
#include "objectX.h"
#include "instantfade.h"
#include "fade.h"
#include "listmanager.h"
#include "collisionobject.h"
#include "limitarea.h"
#include "particle.h"
#include "MyEffekseer.h"
#include "skillpoint.h"
#include "damagepoint.h"
#include "deadmanager.h"

//==========================================================================
// �萔��`
//==========================================================================
namespace
{
	const char* CHARAFILE = "data\\TEXT\\character\\player\\tyuuni\\setup_player.txt";	// �L�����N�^�[�t�@�C��
	const float JUMP = 20.0f * 1.5f;	// �W�����v�͏����l
	const int INVINCIBLE_INT = 2;		// ���G�̊Ԋu
	const int INVINCIBLE_TIME = 90;		// ���G�̎���
	const int DEADTIME = 120;			// ���S���̎���
	const int FADEOUTTIME = 60;			// �t�F�[�h�A�E�g�̎���
	const float MULTIPLIY_DASH = 1.875f;	// �_�b�V���̔{��
	const float RADIUS_STAGE = 2300.0f;	// �X�e�[�W�̔��a
	const float TIME_DASHATTACK = 0.3f;		// �_�b�V���U���ɕK�v�Ȏ���
	const int DEFAULT_STAMINA = 200;	// �X�^�~�i�̃f�t�H���g�l
	const float SUBVALUE_DASH = 0.3f;		// �_�b�V���̌��Z��
	const float SUBVALUE_AVOID = 30.0f;		// ����̌��Z��
	const float SUBVALUE_COUNTER = 40.0f;	// �J�E���^�[�̌��Z��
	const int DEFAULT_RESPAWN_PERCENT = 80;	// �����m���̃f�t�H���g�l
}

//==========================================================================
// �֐��|�C���^
//==========================================================================
CPlayer::STATE_FUNC CPlayer::m_StateFunc[] =
{
	&CPlayer::StateNone,		// �Ȃ�
	&CPlayer::StateInvincible,	// ���G
	&CPlayer::StateDamage,		// �_���[�W
	&CPlayer::StateKnockBack,	// �m�b�N�o�b�N
	&CPlayer::StateDead,		// ���S
	&CPlayer::StateDeadWait,	// ���S�ҋ@
	&CPlayer::StateFadeOut,		// �t�F�[�h�A�E�g
	&CPlayer::StateRespawn,		// ���X�|�[��
	&CPlayer::StateCounter,		// �J�E���^�[��
	&CPlayer::StateAvoid,		// ���
};

//==========================================================================
// �ÓI�����o�ϐ�
//==========================================================================
CListManager<CPlayer> CPlayer::m_List = {};	// ���X�g

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CPlayer::CPlayer(int nPriority) : CObjectChara(nPriority)
{
	// �l�̃N���A
	// ���L�ϐ�
	m_bJump = false;				// �W�����v�����ǂ���
	m_bLandOld = false;				// �ߋ��̒��n���
	m_bHitStage = false;			// �X�e�[�W�̓����蔻��
	m_bLandField = false;			// �t�B�[���h�̒��n����
	m_bHitWall = false;				// �ǂ̓����蔻��
	m_nCntWalk = 0;					// ���s�J�E���^�[
	m_state = STATE_NONE;			// ���

	SMotionFrag initFrag = {};
	m_sMotionFrag = initFrag;		// ���[�V�����̃t���O

	// �v���C�x�[�g�ϐ�
	m_Oldstate = STATE_NONE;						// �O��̏��
	m_mMatcol = mylib_const::DEFAULT_COLOR;			// �}�e���A���̐F
	m_posKnokBack = mylib_const::DEFAULT_VECTOR3;	// �m�b�N�o�b�N�̈ʒu
	m_KnokBackMove = mylib_const::DEFAULT_VECTOR3;	// �m�b�N�o�b�N�̈ړ���
	m_nCntState = 0;								// ��ԑJ�ڃJ�E���^�[
	m_nComboStage = 0;								// �R���{�̒i�K
	m_nIdxRockOn = 0;								// ���b�N�I���Ώۂ̃C���f�b�N�X�ԍ�
	m_bLockOnAtStart = false;						// �J�E���^�[�J�n���Ƀ��b�N�I�����Ă�����
	m_bAttacking = false;							// �U����
	m_bCounterAccepting = false;					// �J�E���^�[��t��
	m_bDash = false;								// �_�b�V������
	m_fDashTime = 0.0f;								// �_�b�V������
	m_nRespawnPercent = 0;							// ���X�|�[���m��
	m_bTouchBeacon = false;							// �r�[�R���ɐG��Ă锻��
	m_nMyPlayerIdx = 0;								// �v���C���[�C���f�b�N�X�ԍ�
	m_pShadow = NULL;								// �e�̏��
	m_pSkillPoint = nullptr;						// �X�L���|�C���g�̃I�u�W�F�N�g
	m_pHPGauge = nullptr;							// HP�Q�[�W�̃|�C���^
	m_pStaminaGauge = nullptr;						// �X�^�~�i�Q�[�W�̃|�C���^
	m_pEndCounterSetting = nullptr;					// �J�E���^�[�I�����̐ݒ�
	m_pWeaponHandle = nullptr;		// �G�t�F�N�g�̕���n���h��
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CPlayer::~CPlayer()
{

}

//==========================================================================
// ��������
//==========================================================================
CPlayer *CPlayer::Create(int nIdx)
{
	// �����p�̃I�u�W�F�N�g
	CPlayer *pPlayer = NULL;

	if (pPlayer == NULL)
	{// NULL��������

		// �������̊m��
		pPlayer = DEBUG_NEW CPlayer;

		if (pPlayer != NULL)
		{// �������̊m�ۂ��o���Ă�����

			// �v���C���[�C���f�b�N�X�ԍ�
			pPlayer->m_nMyPlayerIdx = nIdx;

			// ����������
			pPlayer->Init();
		}

		return pPlayer;
	}

	return NULL;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CPlayer::Init(void)
{
	// ��ނ̐ݒ�
	SetType(TYPE_PLAYER);

	m_state = STATE_NONE;	// ���
	m_nCntState = 0;		// ��ԑJ�ڃJ�E���^�[
	m_bLandOld = true;		// �O��̒��n���
	m_nRespawnPercent = DEFAULT_RESPAWN_PERCENT;	// ���X�|�[���m��

	// �L�����쐬
	HRESULT hr = SetCharacter(CHARAFILE);
	if (FAILED(hr))
	{// ���s���Ă�����
		return E_FAIL;
	}

	// �ʒu�擾
	MyLib::Vector3 pos = GetPosition();

	// �e�̐���
	//m_pShadow = CShadow::Create(pos, 50.0f);

	// ���蓖��
	m_List.Regist(this);

	// �X�L���|�C���g����
	m_pSkillPoint = CSkillPoint::Create();

	// HP�Q�[�W����
	m_pHPGauge = CHP_GaugePlayer::Create({200.0f, 630.0f, 0.0f}, GetLifeOrigin());

	// �X�^�~�i�Q�[�W����
	m_pStaminaGauge = CStaminaGauge_Player::Create(MyLib::Vector3(200.0f, 680.0f, 0.0f), DEFAULT_STAMINA);

	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CPlayer::Uninit(void)
{
	// �e������
	if (m_pShadow != NULL)
	{
		//m_pShadow->Uninit();
		m_pShadow = NULL;
	}

	if (m_pSkillPoint != nullptr)
	{
		m_pSkillPoint = nullptr;
	}

	// HP�Q�[�W
	if (m_pHPGauge != nullptr)
	{
		m_pHPGauge = nullptr;
	}

	// �����I�����̐ݒ�
	if (m_pEndCounterSetting != nullptr)
	{
		delete m_pEndCounterSetting;
		m_pEndCounterSetting = nullptr;
	}

	// �I������
	CObjectChara::Uninit();

	// �폜
	m_List.Delete(this);
}

//==========================================================================
// �I������
//==========================================================================
void CPlayer::Kill(void)
{

	my_particle::Create(GetPosition(), my_particle::TYPE_ENEMY_FADE);

	if (m_pSkillPoint != nullptr)
	{
		m_pSkillPoint->Uninit();
		m_pSkillPoint = nullptr;
	}

	// HP�Q�[�W
	if (m_pHPGauge != nullptr)
	{
		m_pHPGauge->Kill();
		m_pHPGauge = nullptr;
	}

	// �X�^�~�i�Q�[�W
	if (m_pStaminaGauge != nullptr)
	{
		m_pStaminaGauge->Kill();
		m_pStaminaGauge = nullptr;
	}

	// �e������
	if (m_pShadow != NULL)
	{
		m_pShadow->Uninit();
		m_pShadow = NULL;
	}

	// �����I�����̐ݒ�
	if (m_pEndCounterSetting != nullptr)
	{
		delete m_pEndCounterSetting;
		m_pEndCounterSetting = nullptr;
	}

	// ���b�N�I���ݒ�
	CCamera* pCamera = CManager::GetInstance()->GetCamera();
	if (pCamera != nullptr)
	{
		pCamera->SetRockOn(0.0f, false);
	}

	// ���X�g���[�v
	CListManager<CEnemy> enemyList = CEnemy::GetListObj();
	CEnemy* pEnemy = nullptr;
	while (enemyList.ListLoop(&pEnemy))
	{
		pEnemy->SetEnableRockOn(false);
	}

}

//==========================================================================
// �X�V����
//==========================================================================
void CPlayer::Update(void)
{
	if (IsDeath() == true)
	{
		return;
	}

	// �L�[�{�[�h���擾
	CInputKeyboard* pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();
	if (pInputKeyboard->GetTrigger(DIK_F5) == true)
	{// F5�Ń��Z�b�g
		SetPosition(MyLib::Vector3(0.0f, 0.0f, -1000.0f));
		SetMove(MyLib::Vector3(0.0f, 0.0f, 0.0f));
	}

	// �G�f�B�b�g���͔�����
	if (CGame::GetInstance()->GetElevation()->IsEdit())
	{
		return;
	}

	// �G�f�B�b�g���͔�����
	if (CGame::GetInstance()->GetEditType() != CGame::GetInstance()->EDITTYPE_OFF)
	{
		return;
	}

	// �ߋ��̈ʒu�ۑ�
	SetOldPosition(GetPosition());

	// �t���O���Z�b�g
	ResetFrag();

	// �e�̍X�V����
	CObjectChara::Update();

	// ����
	Controll();

	// ���[�V�����̐ݒ菈��
	MotionSet();

	// ���[�V�����ʂ̏�Ԑݒ�
	MotionBySetState();

	// ��ԍX�V
	(this->*(m_StateFunc[m_state]))();

	// �ʒu�擾
	MyLib::Vector3 pos = GetPosition();
	MyLib::Vector3 posCenter = GetCenterPosition();

	// �ړ��ʎ擾
	MyLib::Vector3 move = GetMove();

	// �����擾
	MyLib::Vector3 rot = GetRotation();

	// �J�����̏��擾
	CCamera* pCamera = CManager::GetInstance()->GetCamera();
	pCamera->SetTargetPosition(pos);
	pCamera->SetTargetRotation(rot);

	// �e�̈ʒu�X�V
	if (m_pShadow != NULL)
	{
		m_pShadow->SetPosition(MyLib::Vector3(pos.x, m_pShadow->GetPosition().y, pos.z));
	}

	if (m_pHPGauge != nullptr)
	{
		m_pHPGauge->SetLife(GetLife());
	}


	int nCntEffect = 0;
	int nNumEffect = GetEffectParentNum();
	for (int i = 0; i < mylib_const::MAX_OBJ; i++)
	{
		CEffect3D* pEffect = GetEffectParent(i);
		if (pEffect == NULL)
		{// NULL��������
			continue;
		}

		// �G�t�F�N�g�̈ʒu�X�V
		pEffect->UpdatePosition(GetRotation());
		nCntEffect++;
		if (nNumEffect <= nCntEffect)
		{
			break;
		}
	}

	// �ʒu�̐���
	LimitPos();

#if 1
	// �f�o�b�O�\��
	CManager::GetInstance()->GetDebugProc()->Print(
		"------------------[�v���C���[�̑���]------------------\n"
		"�ʒu�F�yX�F%f, Y�F%f, Z�F%f�z �yW / A / S / D�z\n"
		"�����F�yX�F%f, Y�F%f, Z�F%f�z �yZ / C�z\n"
		"�ړ��ʁF�yX�F%f, Y�F%f, Z�F%f�z\n"
		"�̗́F�y%d�z\n"
		"�R���{�X�e�[�W�F�y%d�z\n"
		"�_�b�V�����ԁF�y%f�z\n"
		, pos.x, pos.y, pos.z, rot.x, rot.y, rot.y, move.x, move.y, move.z, GetLife(), m_nComboStage, m_fDashTime);
#endif

}

//==========================================================================
// ���쏈��
//==========================================================================
void CPlayer::Controll(void)
{

	// �L�[�{�[�h���擾
	CInputKeyboard *pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();

	// �Q�[���p�b�h���擾
	CInputGamepad *pInputGamepad = CManager::GetInstance()->GetInputGamepad();

	// �J�����̏��擾
	CCamera *pCamera = CManager::GetInstance()->GetCamera();

	// �J�����̌����擾
	MyLib::Vector3 Camerarot = pCamera->GetRotation();

	// �ʒu�擾
	MyLib::Vector3 pos = GetPosition();
	MyLib::Vector3 newPosition = GetPosition();
	MyLib::Vector3 sakiPos = GetPosition();

	// �ړ��ʎ擾
	MyLib::Vector3 move = GetMove();

	// �����擾
	MyLib::Vector3 rot = GetRotation();

	// �ڕW�̌����擾
	float fRotDest = GetRotDest();

	// ���݂̎�ގ擾
	CMotion* pMotion = GetMotion();
	int nMotionType = pMotion->GetType();

	// �ړ��ʎ擾
	float fMove = GetVelocity();

	// �o�ߎ��Ԏ擾
	float fCurrentTime = CManager::GetInstance()->GetDeltaTime();

	if (CGame::GetInstance()->GetGameManager()->IsControll())
	{// �s���ł���Ƃ�

		// �_�b�V������
		if (pInputGamepad->GetPress(CInputGamepad::BUTTON_LB, m_nMyPlayerIdx) &&
			pInputGamepad->IsTipStick())
		{// ���X�e�B�b�N���|��Ă�ꍇ
			m_bDash = true;
		}
		else
		{
			m_bDash = false;
		}

		if (m_bDash)
		{
			// �_�b�V���{���|����
			fMove *= MULTIPLIY_DASH;
		}

		if ((pMotion->IsGetMove(nMotionType) == 1 || pMotion->IsGetCancelable()) &&
			!m_sMotionFrag.bATK &&
			m_state != STATE_DMG &&
			m_state != STATE_KNOCKBACK &&
			m_state != STATE_DEAD &&
			m_state != STATE_FADEOUT)
		{// �ړ��\���[�V�����̎�

			if (pInputKeyboard->GetPress(DIK_A) == true)
			{//���L�[�������ꂽ,���ړ�

				// �ړ����ɂ���
				m_sMotionFrag.bMove = true;

				if (pInputKeyboard->GetPress(DIK_W) == true)
				{//A+W,����ړ�

					move.x += sinf(-D3DX_PI * 0.25f + Camerarot.y) * fMove;
					move.z += cosf(-D3DX_PI * 0.25f + Camerarot.y) * fMove;
					fRotDest = D3DX_PI * 0.75f + Camerarot.y;
				}
				else if (pInputKeyboard->GetPress(DIK_S) == true)
				{//A+S,�����ړ�

					move.x += sinf(-D3DX_PI * 0.75f + Camerarot.y) * fMove;
					move.z += cosf(-D3DX_PI * 0.75f + Camerarot.y) * fMove;
					fRotDest = D3DX_PI * 0.25f + Camerarot.y;
				}
				else
				{//A,���ړ�

					move.x += sinf(-D3DX_PI * 0.5f + Camerarot.y) * fMove;
					move.z += cosf(-D3DX_PI * 0.5f + Camerarot.y) * fMove;
					fRotDest = D3DX_PI * 0.5f + Camerarot.y;
				}
			}
			else if (pInputKeyboard->GetPress(DIK_D) == true)
			{//D�L�[�������ꂽ,�E�ړ�

				// �ړ����ɂ���
				m_sMotionFrag.bMove = true;

				if (pInputKeyboard->GetPress(DIK_W) == true)
				{//D+W,�E��ړ�

					move.x += sinf(D3DX_PI * 0.25f + Camerarot.y) * fMove;
					move.z += cosf(D3DX_PI * 0.25f + Camerarot.y) * fMove;
					fRotDest = -D3DX_PI * 0.75f + Camerarot.y;
				}
				else if (pInputKeyboard->GetPress(DIK_S) == true)
				{//D+S,�E���ړ�

					move.x += sinf(D3DX_PI * 0.75f + Camerarot.y) * fMove;
					move.z += cosf(D3DX_PI * 0.75f + Camerarot.y) * fMove;
					fRotDest = -D3DX_PI * 0.25f + Camerarot.y;
				}
				else
				{//D,�E�ړ�

					move.x += sinf(D3DX_PI * 0.5f + Camerarot.y) * fMove;
					move.z += cosf(D3DX_PI * 0.5f + Camerarot.y) * fMove;
					fRotDest = -D3DX_PI * 0.5f + Camerarot.y;
				}
			}
			else if (pInputKeyboard->GetPress(DIK_W) == true)
			{//W�������ꂽ�A��ړ�

				// �ړ����ɂ���
				m_sMotionFrag.bMove = true;
				move.x += sinf(D3DX_PI * 0.0f + Camerarot.y) * fMove;
				move.z += cosf(D3DX_PI * 0.0f + Camerarot.y) * fMove;
				fRotDest = D3DX_PI * 1.0f + Camerarot.y;
			}
			else if (pInputKeyboard->GetPress(DIK_S) == true)
			{//S�������ꂽ�A���ړ�

				// �ړ����ɂ���
				m_sMotionFrag.bMove = true;
				move.x += sinf(D3DX_PI * 1.0f + Camerarot.y) * fMove;
				move.z += cosf(D3DX_PI * 1.0f + Camerarot.y) * fMove;
				fRotDest = D3DX_PI * 0.0f + Camerarot.y;
			}
			else
			{
				// �ړ������ǂ���
				m_sMotionFrag.bMove = false;
			}

			if (pInputGamepad->IsTipStick())
			{// ���X�e�B�b�N���|��Ă�ꍇ

				// �ړ����ɂ���
				m_sMotionFrag.bMove = true;

				// �X�e�B�b�N�̌����擾
				float stickrot = pInputGamepad->GetStickRotL(m_nMyPlayerIdx);
				UtilFunc::Transformation::RotNormalize(stickrot);

				// �ړ��ʂƌ����ݒ�
				move.x += sinf(stickrot + Camerarot.y) * fMove;
				move.z += cosf(stickrot + Camerarot.y) * fMove;
				fRotDest = D3DX_PI + stickrot + Camerarot.y;
			}

			if (m_sMotionFrag.bMove &&
				!m_bJump)
			{// �L�����Z���\ && �ړ���

				pMotion->ToggleFinish(true);
			}

			if (m_bJump == false &&
				(pInputKeyboard->GetTrigger(DIK_SPACE) == true ||
				pInputGamepad->GetTrigger(CInputGamepad::BUTTON_A, m_nMyPlayerIdx)) &&
				!m_bTouchBeacon)
			{// �W�����v

				m_bJump = true;
				m_sMotionFrag.bJump = true;
				move.y += 17.0f;

				// �T�E���h�Đ�
				CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL_SE_JUMP);
			}
		}
		else if (pMotion->IsGetMove(nMotionType) == 0 &&
			m_state != STATE_DEAD &&
			m_state != STATE_FADEOUT)
		{
			if (pInputKeyboard->GetPress(DIK_A) == true)
			{//���L�[�������ꂽ,���ړ�

				if (pInputKeyboard->GetPress(DIK_W) == true)
				{//A+W,����ړ�
					fRotDest = D3DX_PI * 0.75f + Camerarot.y;
				}
				else if (pInputKeyboard->GetPress(DIK_S) == true)
				{//A+S,�����ړ�
					fRotDest = D3DX_PI * 0.25f + Camerarot.y;
				}
				else
				{//A,���ړ�
					fRotDest = D3DX_PI * 0.5f + Camerarot.y;
				}
			}
			else if (pInputKeyboard->GetPress(DIK_D) == true)
			{//D�L�[�������ꂽ,�E�ړ�

				if (pInputKeyboard->GetPress(DIK_W) == true)
				{//D+W,�E��ړ�
					fRotDest = -D3DX_PI * 0.75f + Camerarot.y;
				}
				else if (pInputKeyboard->GetPress(DIK_S) == true)
				{//D+S,�E���ړ�
					fRotDest = -D3DX_PI * 0.25f + Camerarot.y;
				}
				else
				{//D,�E�ړ�
					fRotDest = -D3DX_PI * 0.5f + Camerarot.y;
				}
			}
			else if (pInputKeyboard->GetPress(DIK_W) == true)
			{//W�������ꂽ�A��ړ�
				fRotDest = D3DX_PI * 1.0f + Camerarot.y;
			}
			else if (pInputKeyboard->GetPress(DIK_S) == true)
			{//S�������ꂽ�A���ړ�
				fRotDest = D3DX_PI * 0.0f + Camerarot.y;
			}

			//if (pInputGamepad->IsTipStick())
			//{// ���X�e�B�b�N���|��Ă�ꍇ

			//	fRotDest = D3DX_PI + pInputGamepad->GetStickRotL(m_nMyPlayerIdx) + Camerarot.y;
			//}
		}

		// �U��
		if ((pMotion->IsGetCombiable() || pMotion->IsGetCancelable()) &&
			!m_bJump &&
			!pInputGamepad->GetPress(CInputGamepad::BUTTON_RB, m_nMyPlayerIdx) && 
			pInputGamepad->GetTrigger(CInputGamepad::BUTTON_X, m_nMyPlayerIdx))
		{
			//pMotion->ToggleFinish(true);
			//m_sMotionFrag.bATK = true;		// �U������ON

			if (pInputGamepad->IsTipStick())
			{// ���X�e�B�b�N���|��Ă�ꍇ
				fRotDest = D3DX_PI + pInputGamepad->GetStickRotL(m_nMyPlayerIdx) + Camerarot.y;
			}

			if (m_fDashTime >= TIME_DASHATTACK && m_nComboStage == 0)
			{// �_�b�V�����̏�����2����X�^�[�g
				m_nComboStage++;
				pMotion->Set(MOTION_DASHATK, true);
			}
			else
			{
				// �R���{�i�K���l��
				int nSetType = MOTION_ATK + m_nComboStage;
				pMotion->Set(nSetType, true);
			}

			// �R���{�̒i�K���Z
			m_nComboStage++;
			if (m_nComboStage > MOTION_ATK3 - MOTION_ATK)
			{
				m_nComboStage = 0;
			}
		}

		// �J�E���^�[
		if ((pMotion->IsGetCombiable() || pMotion->IsGetCancelable()) &&
			!m_bJump &&
			pInputGamepad->GetPress(CInputGamepad::BUTTON_RB, m_nMyPlayerIdx) &&
			pInputGamepad->GetTrigger(CInputGamepad::BUTTON_X, m_nMyPlayerIdx))
		{
			pMotion->Set(MOTION_DEF);
			pMotion->Set(MOTION_COUNTER_ACCEPT);
			m_sMotionFrag.bCounter = true;		// �U������ON

			if (pInputGamepad->IsTipStick())
			{// ���X�e�B�b�N���|��Ă�ꍇ

				fRotDest = D3DX_PI + pInputGamepad->GetStickRotL(m_nMyPlayerIdx) + Camerarot.y;
			}

			// �X�^�~�i���Z
			if (m_pStaminaGauge != nullptr)
			{
				m_pStaminaGauge->SubValue(SUBVALUE_COUNTER);
			}
		}


		// �f�o�b�O�\��
		CManager::GetInstance()->GetDebugProc()->Print(
			"------------------[�v���C���[�̑���]------------------\n"
			"�R���{�X�e�[�W�F�y%d�z\n"
			, m_nComboStage);

		// ���
		if (!m_bJump &&
			(pMotion->IsGetCombiable() || pMotion->IsGetCancelable()) &&
			pInputGamepad->GetTrigger(CInputGamepad::BUTTON_B, m_nMyPlayerIdx))
		{
			pMotion->Set(MOTION_DEF);
			pMotion->Set(MOTION_AVOID);

			if (pInputGamepad->IsTipStick())
			{// ���X�e�B�b�N���|��Ă�ꍇ

				fRotDest = D3DX_PI + pInputGamepad->GetStickRotL(m_nMyPlayerIdx) + Camerarot.y;
			}
			m_state = STATE_AVOID;

			// �X�^�~�i���Z
			if (m_pStaminaGauge != nullptr)
			{
				m_pStaminaGauge->SubValue(SUBVALUE_AVOID);
			}
		}
	}

	// �ړ��ʉ��Z
	newPosition.x += move.x;
	newPosition.z += move.z;

	sakiPos.x = newPosition.x + sinf(D3DX_PI + rot.y) * GetRadius();
	sakiPos.z = newPosition.z + cosf(D3DX_PI + rot.y) * GetRadius();

	// �p�x�̐��K��
	UtilFunc::Transformation::RotNormalize(fRotDest);
	SetRotDest(fRotDest);

	// ���݂ƖڕW�̍��������߂�
	float fRotDiff = fRotDest - rot.y;

	// �p�x�̐��K��
	UtilFunc::Transformation::RotNormalize(fRotDiff);

	// �p�x�̕␳������
	rot.y += fRotDiff * 0.25f;
	UtilFunc::Transformation::RotNormalize(rot.y);

	// �����ݒ�
	SetRotation(rot);


	// �d�͏���
	if (m_state != STATE_KNOCKBACK && m_state != STATE_DMG && m_state != STATE_DEAD && m_state != STATE_FADEOUT && m_state != STATE_DEADWAIT)
	{
		move.y -= mylib_const::GRAVITY;

		// �ʒu�X�V
		newPosition.y += move.y;
		sakiPos.y = newPosition.y;
	}

	//**********************************
	// �����蔻��
	//**********************************
	bool bLandStage = Collision(sakiPos, move);

	bool bMove = false;
	if (m_bLandOld == false && bLandStage == true)
	{// �O��͒��n���Ă��Ȃ��āA����͒��n���Ă���ꍇ

		bMove = false;
	}

	if (m_bLandOld == true && bLandStage == true)
	{// �O�����������n���Ă���ꍇ
		bMove = true;
	}

	if (m_bHitWall == false && 
		(bLandStage == false || bMove == true || m_bLandField == true || m_bJump == true || m_sMotionFrag.bKnockBack == true || m_sMotionFrag.bDead == true))
	{
		pos.x = newPosition.x;
		pos.z = newPosition.z;
		pos.y = sakiPos.y;
		Collision(pos, move);

		// �O��͏���Ă����Ƃɂ���
		m_bLandOld = true;
	}
	else
	{
		MyLib::Vector3 posOld = GetOldPosition();
		pos.x = posOld.x;
		pos.z = posOld.z;
		pos = posOld;
		move.x = 0.0f;
		pos.y -= mylib_const::GRAVITY * 7.0f;

		if (m_bJump == false)
		{
			m_bJump = true;
		}

		Collision(pos, move);
	}

	// �����␳
	if (m_state != STATE_KNOCKBACK && m_state != STATE_DMG && m_state != STATE_DEAD && m_state != STATE_FADEOUT)
	{
		move.x += (0.0f - move.x) * 0.25f;
		move.z += (0.0f - move.z) * 0.25f;
	}

	// �ʒu�ݒ�
	SetPosition(pos);

	// �ړ��ʐݒ�
	SetMove(move);

	// ���b�N�I���Ώې؂�ւ�
	if (CManager::GetInstance()->GetCamera()->IsRockOn())
	{
		SwitchRockOnTarget();
	}

	if (pInputGamepad->GetTrigger(CInputGamepad::BUTTON_RSTICKPUSH, m_nMyPlayerIdx))
	{
		if (CManager::GetInstance()->GetCamera()->IsRockOn())
		{// ���b�N�I������

			// ���b�N�I���ݒ�
			CManager::GetInstance()->GetCamera()->SetRockOn(0.0f, false);

			// ���X�g���[�v
			CListManager<CEnemy> enemyList = CEnemy::GetListObj();
			CEnemy* pEnemy = nullptr;
			while (enemyList.ListLoop(&pEnemy))
			{
				pEnemy->SetEnableRockOn(false);
			}
		}
		else
		{
			// ���b�N�I������
			RockOn();
		}
	}

	// ���X�g���[�v
	CListManager<CEnemy> enemyList = CEnemy::GetListObj();
	CEnemy* pEnemy = nullptr;
	while (enemyList.ListLoop(&pEnemy))
	{
		if (CGame::GetInstance()->GetRockOnDistance() <= UtilFunc::Calculation::GetPosLength3D(pos, pEnemy->GetPosition()))
		{
			if (pEnemy->IsRockOnAccept())
			{
				// ���b�N�I���ݒ�
				CManager::GetInstance()->GetCamera()->SetRockOn(0.0f, false);
			}

			pEnemy->SetEnableRockOn(false);
		}
	}


	if (pInputKeyboard->GetTrigger(DIK_LEFT) == true)
	{
		CCollisionObject::Create(GetPosition(), mylib_const::DEFAULT_VECTOR3, 100000.0f, 3, 10000, CCollisionObject::TAG_PLAYER);
	}

	if (pInputKeyboard->GetTrigger(DIK_RETURN) == true)
	{
		m_sMotionFrag.bATK = true;		// �U������OFF

		MyLib::Vector3 weponpos = pos;

		CMyEffekseer::GetInstance()->SetEffect(
			&m_pWeaponHandle,
			"data/Effekseer/debugline_green.efkefc",
			weponpos, rot, 0.0f, 40.0f, true);

		// �f�o�b�O�\��
		CManager::GetInstance()->GetDebugProc()->Print(
			"�킠��������������������������������������������������������������������������������������\n");
	}

	if (pInputKeyboard->GetTrigger(DIK_SPACE) == true)
	{
		MyLib::Vector3 weponpos = pos;
		weponpos.y += 150.0f;

		*m_pWeaponHandle = CMyEffekseer::GetInstance()->SetEffect(
			"data/Effekseer/MyLine.efkefc",
			weponpos, rot, 0.0f, 40.0f);
	}

	if (pInputKeyboard->GetRepeat(DIK_O, 4) == true)
	{
		MyLib::Vector3 weponpos = pos;
		weponpos.y += 150.0f;

		MyLib::Vector3 spawnpos = UtilFunc::Transformation::GetRandomPositionSphere(weponpos, 300.0f);

		CMyEffekseer::GetInstance()->SetEffect(
			"data/Effekseer/Laser01.efkefc",
			spawnpos, UtilFunc::Transformation::GetRandomVecSphere() * D3DX_PI, mylib_const::DEFAULT_VECTOR3, 10.0f);
	}
	if (pInputKeyboard->GetTrigger(DIK_L) == true)
	{
		MyLib::Vector3 weponpos = pos;
		weponpos.y += 150.0f;

		MyLib::Vector3 spawnpos = UtilFunc::Transformation::GetRandomPositionSphere(weponpos, 300.0f);

		CMyEffekseer::GetInstance()->SetEffect(
			"data/Effekseer/CounterParticle_01.efkefc",
			weponpos, MyLib::Vector3(1.57f, 1.57f, 1.57f), 0.0f, 50.0f);
	}

	static float fff = 1.0f;
	if (pInputKeyboard->GetTrigger(DIK_UP) == true)
	{
		fff += 0.1f;
		CManager::GetInstance()->GetSound()->SetFrequency(CSound::LABEL_BGM_GAME, fff);
	}
	if (pInputKeyboard->GetTrigger(DIK_DOWN) == true)
	{
		fff -= 0.1f;
		CManager::GetInstance()->GetSound()->SetFrequency(CSound::LABEL_BGM_GAME, fff);
	}

	if (m_pWeaponHandle != nullptr)
	{

		// ����̈ʒu
		MyLib::Vector3 weponpos = UtilFunc::Transformation::WorldMtxChangeToPosition(GetModel()[16]->GetWorldMtx());

		// ����̃}�g���b�N�X�擾
		D3DXMATRIX weaponWorldMatrix = GetModel()[16]->GetWorldMtx();

		// �O�Ղ̃}�g���b�N�X�ݒ�
		CMyEffekseer::GetInstance()->SetMatrix(*m_pWeaponHandle, weaponWorldMatrix);
		CMyEffekseer::GetInstance()->SetPosition(*m_pWeaponHandle, weponpos);
	}
}

//==========================================================================
// ���[�V�����̐ݒ�
//==========================================================================
void CPlayer::SetMotion(int motionIdx)
{
	// ���[�V�����擾
	CMotion* pMotion = GetMotion();
	if (pMotion == nullptr)
	{
		return;
	}
	pMotion->Set(motionIdx);
}

//==========================================================================
// ���[�V�����̐ݒ�
//==========================================================================
void CPlayer::MotionSet(void)
{
	// ���[�V�����擾
	CMotion* pMotion = GetMotion();
	if (pMotion == NULL)
	{
		return;
	}

	if (pMotion->IsFinish() == true)
	{// �I�����Ă�����

		// ���݂̎�ގ擾
		int nType = pMotion->GetType();
		int nOldType = pMotion->GetOldType();

		if (m_sMotionFrag.bMove == true && m_sMotionFrag.bKnockBack == false && m_bJump == false &&
			m_sMotionFrag.bATK == false && m_sMotionFrag.bATK == false)
		{// �ړ����Ă�����

			m_sMotionFrag.bMove = false;	// �ړ�����OFF

			// �ړ����[�V����
			if (m_bDash)
			{
				pMotion->Set(MOTION_DASH);
			}
			else
			{
				pMotion->Set(MOTION_WALK);
			}
		}
		else if (m_sMotionFrag.bJump == true && m_sMotionFrag.bATK == false && m_sMotionFrag.bKnockBack == false && m_sMotionFrag.bDead == false)
		{// �W�����v��

			// �W�����v�̃t���OOFF
			m_sMotionFrag.bJump = false;

			// �W�����v���[�V����
			pMotion->Set(MOTION_JUMP);
		}
		else if (m_bJump == true && m_sMotionFrag.bJump == false && m_sMotionFrag.bATK == false && m_sMotionFrag.bKnockBack == false && m_sMotionFrag.bDead == false)
		{// �W�����v��&&�W�����v���[�V�������I����Ă鎞

			// �������[�V����
			pMotion->Set(MOTION_FALL);
		}
		else if (m_sMotionFrag.bKnockBack == true)
		{// ���ꒆ��������

			// ���ꃂ�[�V����
			pMotion->Set(MOTION_KNOCKBACK);
		}
		//else if (m_sMotionFrag.bATK == true)
		//{// �U��

		//	m_sMotionFrag.bATK = false;		// �U������OFF

		//	int nSetType = MOTION_ATK + m_nComboStage;
		//	pMotion->Set(nSetType, true);

		//	if (m_nComboStage >= MOTION_ATK3 - MOTION_ATK)
		//	{
		//		m_nComboStage = 0;
		//	}
		//}
		else
		{
			// �j���[�g�������[�V����
			pMotion->Set(MOTION_DEF);
		}
	}
}

//==========================================================================
// ���[�V�����ʂ̏�Ԑݒ�
//==========================================================================
void CPlayer::MotionBySetState(void)
{
	// ���[�V�����擾
	CMotion* pMotion = GetMotion();
	if (pMotion == nullptr)
	{
		return;
	}
	int nType = pMotion->GetType();

	switch (nType)
	{
	case MOTION_DASH:

		// �_�b�V�����ԉ��Z
		m_fDashTime += CManager::GetInstance()->GetDeltaTime();

		if (m_pStaminaGauge != nullptr)
		{
			m_pStaminaGauge->SubValue(SUBVALUE_DASH);
		}
		break;

	default:
		m_fDashTime = 0.0f;
		break;
	}

	switch (nType)
	{
	case MOTION_ATK:
	case MOTION_ATK2:
	case MOTION_ATK3:
	case MOTION_DASHATK:
		m_bAttacking = true;
		break;

	case MOTION_COUNTER_TURN:
	case MOTION_COUNTER_ATTACK:
		// �J�E���^�[��Ԃɐݒ�
		m_state = STATE_COUNTER;
		break;

	default:
		m_bAttacking = false;	// �U��������
		m_nComboStage = 0;		// �R���{�̒i�K
		break;
	}
}

//==========================================================================
// �t���O���Z�b�g
//==========================================================================
void CPlayer::ResetFrag(void)
{
	m_bCounterAccepting = false;	// �J�E���^�[��t��
}

//==========================================================================
// ���b�N�I��
//==========================================================================
void CPlayer::RockOn(void)
{
	// �J�����擾
	CCamera* pCamera = CManager::GetInstance()->GetCamera();

	// �G�̃��X�g�擾
	CListManager<CEnemy> enemyList = CEnemy::GetListObj();
	CEnemy* pEnemy = nullptr;

	float fNearLen = CGame::GetInstance()->GetRockOnDistance();
	int nMaxIdx = 0;
	MyLib::Vector3 targetpos(0.0f);

	// �ʒu�擾
	MyLib::Vector3 pos = GetPosition();

	// ���X�g���[�v
	int i = 0;
	MyLib::Vector3 enemypos(0.0f);
	while (enemyList.ListLoop(&pEnemy))
	{
		// �G�̈ʒu�擾
		enemypos = pEnemy->GetPosition();

		if (pCamera->IsOnScreen(enemypos))
		{
			float len = UtilFunc::Calculation::GetPosLength3D(pos, enemypos);
			if (fNearLen > len)
			{
				fNearLen = len;
				nMaxIdx = i;
			}
		}

		// �C���f�b�N�X���Z
		i++;
	}

	if (fNearLen < CGame::GetInstance()->GetRockOnDistance())
	{// ���b�N�I���������Ȃ�

		// ���b�N�I���ݒ�
		pCamera->SetRockOn(enemyList.GetData(nMaxIdx)->GetPosition(), true);
		enemyList.GetData(nMaxIdx)->SetEnableRockOn(true);

		// �C���f�b�N�X�ԍ��ݒ�
		m_nIdxRockOn = nMaxIdx;
	}
}

//==========================================================================
// ���b�N�I���Ώې؂�ւ�
//==========================================================================
void CPlayer::SwitchRockOnTarget(void)
{
	// �J�����擾
	CCamera* pCamera = CManager::GetInstance()->GetCamera();

	// �ʒu�擾
	MyLib::Vector3 pos = GetPosition();

	// �Q�[���p�b�h���擾
	CInputGamepad* pInputGamepad = CManager::GetInstance()->GetInputGamepad();

	// �G�̃��X�g�擾
	CListManager<CEnemy> enemyList = CEnemy::GetListObj();
	CEnemy* pEnemy = nullptr;

	bool bSwitch = true;
	if (pInputGamepad->GetRStickTrigger(CInputGamepad::STICK_X))
	{// ���E�ǂ��炩�ɐ؂�ւ�

		bool bSwitch = true;

		// ���X�g���[�v
		int i = 0, nMaxIdx = m_nIdxRockOn;
		float fNearLen = CGame::GetInstance()->GetRockOnDistance();
		pEnemy = nullptr;
		MyLib::Vector3 enemypos(0.0f);
		while (enemyList.ListLoop(&pEnemy))
		{
			// �G�̈ʒu�擾
			enemypos = pEnemy->GetPosition();

			if (pCamera->IsOnScreen(enemypos))
			{
				float len = UtilFunc::Calculation::GetPosLength3D(pos, enemypos);
				if (fNearLen > len &&
					m_nIdxRockOn != i)
				{
					fNearLen = len;
					nMaxIdx = i;
				}
			}

			// �C���f�b�N�X���Z
			i++;
		}

		// ���܂Ń��b�N�I�����Ă��Ώۃ��Z�b�g
		enemyList.GetData(m_nIdxRockOn)->SetEnableRockOn(false);

		// ���b�N�I���ݒ�
		CEnemy* pSetEnemy = enemyList.GetData(nMaxIdx);
		CManager::GetInstance()->GetCamera()->SetRockOn(pSetEnemy->GetPosition(), true);
		pSetEnemy->SetEnableRockOn(true);

		// �C���f�b�N�X�ԍ��ݒ�
		m_nIdxRockOn = nMaxIdx;
	}
}

//==========================================================================
// �U��������
//==========================================================================
void CPlayer::AttackAction(CMotion::AttackInfo ATKInfo, int nCntATK)
{
	// ���[�V�����擾
	CMotion* pMotion = GetMotion();
	int nType = pMotion->GetType();

	// ����̈ʒu
	MyLib::Vector3 weponpos = pMotion->GetAttackPosition(GetModel(), ATKInfo);

	switch (nType)
	{
	case MOTION_ATK:	// ��ʂ��E��
		CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL_SE_SNOWGET);
		break;

	case MOTION_WALK:
		CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL_SE_WALK);
		break;

	case MOTION_COUNTER_ATTACK:

		if (nCntATK == 0)
		{
			CMyEffekseer::GetInstance()->SetEffect(
				"data/Effekseer/CounterParticle_01.efkefc",
				weponpos, 0.0f, 0.0f, 50.0f);
		}
		else if (nCntATK != 0)
		{
			CManager::GetInstance()->GetCamera()->SetLenDest(200.0f, 3, 4.0f, 0.3f);

			// �G�փ_�E�����
			CEnemy::GetListObj().GetData(m_nIdxRockOn)->Hit(10, CGameManager::ATTACK_COUNTER);
		}
		break;

	default:
		break;
	}
}

//==========================================================================
// �U�����蒆����
//==========================================================================
void CPlayer::AttackInDicision(CMotion::AttackInfo ATKInfo, int nCntATK)
{
	// ���[�V�����擾
	CMotion* pMotion = GetMotion();

	// ����̈ʒu
	MyLib::Vector3 weponpos = pMotion->GetAttackPosition(GetModel(), ATKInfo);

	CEffect3D* pEffect = NULL;

	switch (pMotion->GetType())
	{
	case MOTION_ATK:
		break;

	case MOTION_COUNTER_ACCEPT:
		m_bCounterAccepting = true;	// �J�E���^�[��t��
		break;
	}

	if (ATKInfo.fRangeSize == 0.0f)
	{
		return;
	}

#if _DEBUG
	CEffect3D::Create(
		weponpos,
		MyLib::Vector3(0.0f, 0.0f, 0.0f),
		D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f),
		ATKInfo.fRangeSize, 2, CEffect3D::MOVEEFFECT_NONE, CEffect3D::TYPE_NORMAL);
#endif

	// �G�̃��X�g�擾
	CListManager<CEnemy> enemyList = CEnemy::GetListObj();
	CEnemy* pEnemy = nullptr;

	// ���X�g���[�v
	while (enemyList.ListLoop(&pEnemy))
	{
		// �G�̈ʒu�擾
		MyLib::Vector3 TargetPos = pEnemy->GetPosition();

		// ����T�C�Y�擾
		float fTargetRadius = pEnemy->GetRadius();

		MyLib::HitResult hitresult = UtilFunc::Collision::SphereRange(weponpos, TargetPos, ATKInfo.fRangeSize, fTargetRadius);
		if (hitresult.ishit)
		{// ���̔���

			if (pEnemy->Hit(ATKInfo.nDamage) == true)
			{// �������Ă���

				// �ʒu
				MyLib::Vector3 pos = GetPosition();
				MyLib::Vector3 enemypos = pEnemy->GetPosition();

				// �^�[�Q�b�g�ƓG�Ƃ̌���
				float fRot = atan2f((enemypos.x - pos.x), (enemypos.z - pos.z));
				UtilFunc::Transformation::RotNormalize(fRot);

				pEnemy->SetMove(MyLib::Vector3(sinf(fRot) * 8.0f, 0.0f, cosf(fRot) * 8.0f));

				CMyEffekseer::GetInstance()->SetEffect(
					"data/Effekseer/HitParticle_Red_01.efkefc",
					hitresult.hitpos, 0.0f, 0.0f, 50.0f);

				enemypos.y += pEnemy->GetHeight() * 0.5f;
				enemypos += UtilFunc::Transformation::GetRandomPositionSphere(enemypos, fTargetRadius * 0.5f);
				CDamagePoint::Create(hitresult.hitpos, ATKInfo.nDamage);
			}
		}
	}
}

//==========================================================================
// �ʒu����
//==========================================================================
void CPlayer::LimitPos(void)
{
	MyLib::Vector3 pos = GetPosition();

	float fLength = sqrtf(pos.x * pos.x + pos.z * pos.z);

	if (fLength > RADIUS_STAGE)
	{// �␳
		D3DXVec3Normalize(&pos, &pos);

		pos *= RADIUS_STAGE;

		SetPosition(pos);
	}
}

//==========================================================================
// �����蔻��
//==========================================================================
bool CPlayer::Collision(MyLib::Vector3 &pos, MyLib::Vector3 &move)
{
	// �����擾
	MyLib::Vector3 rot = GetRotation();

	// ���n�������ǂ���
	bool bLand = false;
	float fHeight = 0.0f;
	m_bLandField = false;
	m_bHitWall = false;			// �ǂ̓����蔻��

	// �����擾
	if (m_state != STATE_KNOCKBACK && m_state != STATE_DMG && m_state != STATE_DEAD && m_state != STATE_FADEOUT)
	{
		fHeight = CManager::GetInstance()->GetScene()->GetElevation()->GetHeight(pos, &bLand);
	}
	else
	{
		fHeight = pos.y;
	}

	if (fHeight > pos.y)
	{// �n�ʂ̕���������荂��������

		// �n�ʂ̍����ɕ␳
		pos.y = fHeight;
		m_bLandField = true;

		if (bLand == true)
		{// ���n���Ă���

			// �W�����v�g�p�\�ɂ���
			m_bJump = false;
			move.y = 0.0f;
			m_bLandOld = true;
		}
	}


	// X�t�@�C���Ƃ̔���
	CStage *pStage = CGame::GetInstance()->GetStage();
	if (pStage == NULL)
	{// NULL��������
		return false;
	}

	bool bNowLand = false;

	// �X�e�[�W�ɓ�����������
	m_bHitStage = false;
	for (int nCntStage = 0; nCntStage < pStage->GetNumAll(); nCntStage++)
	{
		// �I�u�W�F�N�g�擾
		CObjectX *pObjX = pStage->GetObj(nCntStage);

		if (pObjX == NULL)
		{// NULL��������
			continue;
		}

		// �����擾
		bool bLand = false;
		fHeight = pObjX->GetHeight(pos, bLand);

		if (bLand == true && fHeight > pos.y)
		{// �n�ʂ̕���������荂��������

		 // �n�ʂ̍����ɕ␳
			if (pos.y + 50.0f <= fHeight)
			{// �������ǂ���������
				m_bHitWall = true;
			}
			else
			{
				pos.y = fHeight;
			}

			m_bHitStage = true;
			m_bLandField = false;

			if (bLand == true)
			{// ���n���Ă���

				if ((m_sMotionFrag.bKnockBack || m_bJump == true) && GetPosition().y >= fHeight)
				{
					m_bLandOld = true;
				}

				if (m_bJump == true)
				{// �W�����v����������

					// ���[�V�����擾
					CMotion* pMotion = GetMotion();
					pMotion->ToggleFinish(true);
				}

				// �W�����v�g�p�\�ɂ���
				m_bJump = false;
				move.y = 0.0f;
				bNowLand = true;
				m_sMotionFrag.bJump = false;
			}
		}
	}

	// �G���A�������擾
	CListManager<CLimitArea> limitareaList = CLimitArea::GetListObj();
	CLimitArea* pLimitArea = nullptr;

	while (limitareaList.ListLoop(&pLimitArea))
	{
		CLimitArea::sLimitEreaInfo info = pLimitArea->GetLimitEreaInfo();

		// ��l�̕ǂ�K�p
		if (pos.x + GetRadius() >= info.fMaxX) { pos.x = info.fMaxX - GetRadius(); }
		if (pos.x - GetRadius() <= info.fMinX) { pos.x = info.fMinX + GetRadius(); }
		if (pos.z + GetRadius() >= info.fMaxZ) { pos.z = info.fMaxZ - GetRadius(); }
		if (pos.z - GetRadius() <= info.fMinZ) { pos.z = info.fMinZ + GetRadius(); }
	}

	// �����ݒ�
	SetRotation(rot);

	return bNowLand;
}

//==========================================================================
// �q�b�g����
//==========================================================================
bool CPlayer::Hit(const int nValue, CGameManager::AttackType atkType)
{
	bool bHit = false;

	CCamera* pCamera = CManager::GetInstance()->GetCamera();

	if (m_bCounterAccepting)
	{// �J�E���^�[�󂯕t����
		
		if (atkType == CGameManager::ATTACK_NORMAL)
		{
			// �󂯗���
			GetMotion()->Set(MOTION_COUNTER_TURN);

			// �󂯗����̐ݒ�
			m_pEndCounterSetting = DEBUG_NEW CEndTurn;
		}
		else
		{
			if (GetMotion()->GetType() != MOTION_COUNTER_ATTACK)
			{
				// �J�n���̃t���O�R�s�[
				m_bLockOnAtStart = pCamera->IsRockOn();

				// ����
				GetMotion()->Set(MOTION_COUNTER_ATTACK);
				pCamera->SetRockOnState(CCamera::RockOnState::ROCKON_COUNTER);
				m_pSkillPoint->AddPoint();

				// �U���̐ݒ�
				m_pEndCounterSetting = DEBUG_NEW CEndAttack;
			}
		}
		return false;
	}
	
	// ���ʂ̃q�b�g����
	bHit = ProcessHit(nValue);

	// �������������Ԃ�
	return bHit;
}

//==========================================================================
// �q�b�g����
//==========================================================================
bool CPlayer::Hit(const int nValue, CEnemy* pEnemy, CGameManager::AttackType atkType)
{
	bool bHit = false;

	CCamera* pCamera = CManager::GetInstance()->GetCamera();

	if (m_bCounterAccepting)
	{// �J�E���^�[�󂯕t����

		if (atkType == CGameManager::ATTACK_NORMAL)
		{
			// �󂯗���
			GetMotion()->Set(MOTION_COUNTER_TURN);

			// �󂯗����̐ݒ�
			m_pEndCounterSetting = DEBUG_NEW CEndTurn;
		}
		else
		{
			if (GetMotion()->GetType() != MOTION_COUNTER_ATTACK)
			{

				// �J�n���̃t���O�R�s�[
				m_bLockOnAtStart = pCamera->IsRockOn();
				if (!pCamera->IsRockOn())
				{
					// ���b�N�I���ݒ�
					pCamera->SetRockOn(pEnemy->GetPosition(), true);
					pEnemy->SetEnableRockOn(true);

					// �C���f�b�N�X�ԍ��ݒ�
					m_nIdxRockOn = CEnemy::GetListObj().FindIdx(pEnemy);
				}

				// ����
				GetMotion()->Set(MOTION_COUNTER_ATTACK);
				pCamera->SetRockOnState(CCamera::RockOnState::ROCKON_COUNTER);
				m_pSkillPoint->AddPoint();

				// �U���̐ݒ�
				m_pEndCounterSetting = DEBUG_NEW CEndAttack;
			}
		}
		return false;
	}

	// ���ʂ̃q�b�g����
	bHit = ProcessHit(nValue);

	// �������������Ԃ�
	return bHit;
}

//==========================================================================
// ���ʂ̃q�b�g����
//==========================================================================
bool CPlayer::ProcessHit(const int nValue)
{
	bool bHit = false;

	// �̗͎擾
	int nLife = GetLife();

	CCamera* pCamera = CManager::GetInstance()->GetCamera();

	if (m_state == STATE_COUNTER ||
		m_state == STATE_AVOID)
	{// �_���[�W�󂯂Ȃ����
		return false;
	}

	if (m_state != STATE_DMG &&
		m_state != STATE_KNOCKBACK &&
		m_state != STATE_INVINCIBLE &&
		m_state != STATE_DEAD &&
		m_state != STATE_FADEOUT)
	{// �_���[�W��t��Ԃ̎�

		// ��������
		bHit = true;

		// �̗͌��炷
		nLife -= nValue;

		// �Q�[���p�b�h���擾
		CInputGamepad* pInputGamepad = CManager::GetInstance()->GetInputGamepad();
		pInputGamepad->SetVibration(CInputGamepad::VIBRATION_STATE_DMG, 0);

		m_KnokBackMove.y += 18.0f;
		m_bHitStage = false;

		// �̗͐ݒ�
		SetLife(nLife);

		if (nLife <= 0)
		{// �̗͂��Ȃ��Ȃ�����

			// �����
			m_state = STATE_DEAD;

			m_KnokBackMove.y = 8.0f;

			// �J�ڃJ�E���^�[�ݒ�
			m_nCntState = DEADTIME;

			// �̗͐ݒ�
			SetLife(0);

			// �m�b�N�o�b�N����ɂ���
			m_sMotionFrag.bKnockBack = true;

			// ���ꃂ�[�V����
			//pMotion->Set(MOTION_KNOCKBACK);

			// �m�b�N�o�b�N�̈ʒu�X�V
			MyLib::Vector3 pos = GetPosition();
			MyLib::Vector3 rot = GetRotation();
			m_posKnokBack = pos;

			// �Ռ��g����
			CImpactWave::Create
			(
				MyLib::Vector3(pos.x, pos.y + 80.0f, pos.z),	// �ʒu
				MyLib::Vector3(D3DX_PI * 0.5f, D3DX_PI + rot.y, D3DX_PI),				// ����
				D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.3f),			// �F
				80.0f,										// ��
				80.0f,										// ����
				0.0f,										// ���S����̊Ԋu
				20,											// ����
				10.0f,										// ���̈ړ���
				CImpactWave::TYPE_GIZAWHITE,				// �e�N�X�`���^�C�v
				false										// ���Z�������邩
			);

			CManager::GetInstance()->SetEnableHitStop(18);

			// �U��
			pCamera->SetShake(21, 30.0f, 0.0f);

			// ����
			return true;
		}

		// �ߋ��̏�ԕۑ�
		m_Oldstate = m_state;

		// �F�ݒ�
		m_mMatcol = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);

		// �_���[�W��Ԃɂ���
		m_state = STATE_DMG;

		// �J�ڃJ�E���^�[�ݒ�
		m_nCntState = 0;

		// �m�b�N�o�b�N�̈ʒu�X�V
		MyLib::Vector3 pos = GetPosition();
		MyLib::Vector3 rot = GetRotation();
		m_posKnokBack = pos;

		// �m�b�N�o�b�N����ɂ���
		m_sMotionFrag.bKnockBack = true;

		// ���ꃂ�[�V����
		GetMotion()->Set(MOTION_KNOCKBACK);

		// �Ռ��g����
		CImpactWave::Create
		(
			MyLib::Vector3(pos.x, pos.y + 80.0f, pos.z),	// �ʒu
			MyLib::Vector3(D3DX_PI * 0.5f, D3DX_PI + rot.y, D3DX_PI),				// ����
			D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.3f),			// �F
			80.0f,										// ��
			80.0f,										// ����
			0.0f,										// ���S����̊Ԋu
			20,											// ����
			10.0f,										// ���̈ړ���
			CImpactWave::TYPE_GIZAWHITE,				// �e�N�X�`���^�C�v
			false										// ���Z�������邩
		);

		CManager::GetInstance()->SetEnableHitStop(12);

		// �U��
		pCamera->SetShake(12, 25.0f, 0.0f);

		// �T�E���h�Đ�
		CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL_SE_PLAYERDMG);
	}

	return bHit;
}

//==========================================================================
// �Ȃ�
//==========================================================================
void CPlayer::StateNone(void)
{
	// �F�ݒ�
	m_mMatcol = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
}

//==========================================================================
// ���G���
//==========================================================================
void CPlayer::StateInvincible(void)
{
	// ��ԑJ�ڃJ�E���^�[���Z
	m_nCntState--;

	if (m_nCntState % INVINCIBLE_INT == 0)
	{// �K��Ԋu
		
		// �F�ݒ�
		if (m_mMatcol.a == 1.0f)
		{
			m_mMatcol.a = 0.4f;
		}
		else
		{
			m_mMatcol.a = 1.0f;
		}
	}

	if (m_nCntState <= 0)
	{// �J�ڃJ�E���^�[��0�ɂȂ�����

		// �Ȃɂ��Ȃ���Ԃɂ���
		m_state = STATE_NONE;
		m_nCntState = 0;

		// �F�ݒ�
		m_mMatcol = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	}

}

//==========================================================================
// �_���[�W���
//==========================================================================
void CPlayer::StateDamage(void)
{
	// �ʒu�擾
	MyLib::Vector3 pos = GetPosition();

	// �ړ��ʎ擾
	MyLib::Vector3 move = GetMove();

	// �����擾
	MyLib::Vector3 rot = GetRotation();

	// ��ԑJ�ڃJ�E���^�[���Z
	m_nCntState++;

	// �F�ݒ�
	m_mMatcol = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);

	// �ʒu�X�V
	pos.y = (-0.4f * (float)(m_nCntState * m_nCntState) + m_KnokBackMove.y * (float)m_nCntState) + m_posKnokBack.y;
	pos.x += move.x;
	pos.z += move.z;

	// �N���Ƃ̔���
	if ((CManager::GetInstance()->GetScene()->GetElevation()->IsHit(pos) || m_bHitStage) && m_nCntState >= 10)
	{// �n�ʂƓ������Ă�����
		m_state = STATE_INVINCIBLE;
		m_nCntState = INVINCIBLE_TIME;
		m_KnokBackMove.y = 0.0f;	// �ړ��ʃ[��
		m_bLandOld = true;

		// �F�ݒ�
		m_mMatcol = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

		// �m�b�N�o�b�N�������
		m_sMotionFrag.bKnockBack = false;

		// ���[�V�����擾
		CMotion* pMotion = GetMotion();
		if (pMotion == NULL)
		{// ���[�V������NULL��������
			return;
		}
		pMotion->ToggleFinish(true);


		// X�t�@�C���Ƃ̔���
		CStage *pStage = CGame::GetInstance()->GetStage();
		if (pStage == NULL)
		{// NULL��������
			return;
		}

		// �X�e�[�W�ɓ�����������
		for (int nCntStage = 0; nCntStage < pStage->GetNumAll(); nCntStage++)
		{
			// �I�u�W�F�N�g�擾
			CObjectX *pObjX = pStage->GetObj(nCntStage);

			if (pObjX == NULL)
			{// NULL��������
				continue;
			}

			// �����擾
			bool bLand = false;
			pos.y = pObjX->GetHeight(pos, bLand);
		}


	}
	else if (m_nCntState >= 30)
	{// �J�ڃJ�E���^�[��30�ɂȂ�����

		// �m�b�N�o�b�N��Ԃɂ���
		m_state = STATE_KNOCKBACK;
	}


	// �ʒu�ݒ�
	SetPosition(pos);

	// �ړ��ʐݒ�
	SetMove(move);

	// �����ݒ�
	SetRotation(rot);
}

//==========================================================================
// ���S���
//==========================================================================
void CPlayer::StateDead(void)
{
	// �ʒu�擾
	MyLib::Vector3 pos = GetPosition();

	// �ړ��ʎ擾
	MyLib::Vector3 move = GetMove();

	// �ʒu�X�V
	if (m_nCntState > 0)
	{
		int nCnt = DEADTIME - m_nCntState;
		pos.y = (-0.1f * (float)(nCnt * nCnt) + m_KnokBackMove.y * (float)nCnt) + m_posKnokBack.y;
		pos.x += move.x;
	}

	// ��ԑJ�ڃJ�E���^�[���Z
	m_nCntState--;

	// �N���Ƃ̔���
	if ((CManager::GetInstance()->GetScene()->GetElevation()->IsHit(pos) || m_bHitStage) && m_nCntState >= 10)
	{// �n�ʂƓ������Ă�����

		// ���S�}�l�[�W������
		CDeadManager::Create();

		m_state = STATE_DEADWAIT;	// ���S�ҋ@���
		m_nCntState = FADEOUTTIME;
		m_KnokBackMove.y = 0.0f;	// �ړ��ʃ[��
		m_bLandOld = true;
		move.x = 0.0f;

		// �F�ݒ�
		m_mMatcol = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

		// �m�b�N�o�b�N�������
		m_sMotionFrag.bKnockBack = false;
		m_sMotionFrag.bDead = true;
		//pMotion->ToggleFinish(true);

		// �Ԃ��|�ꃂ�[�V����
		//pMotion->Set(MOTION_DEAD);

		// X�t�@�C���Ƃ̔���
		CStage *pStage = CGame::GetInstance()->GetStage();
		if (pStage == NULL)
		{// NULL��������
			return;
		}

		// �X�e�[�W�ɓ�����������
		for (int nCntStage = 0; nCntStage < pStage->GetNumAll(); nCntStage++)
		{
			// �I�u�W�F�N�g�擾
			CObjectX *pObjX = pStage->GetObj(nCntStage);

			if (pObjX == NULL)
			{// NULL��������
				continue;
			}

			// �����擾
			bool bLand = false;
			pos.y = pObjX->GetHeight(pos, bLand);
		}
	}

	// �ʒu�ݒ�
	SetPosition(pos);

	// �ړ��ʐݒ�
	SetMove(move);
}

//==========================================================================
// ���S�ҋ@
//==========================================================================
void CPlayer::StateDeadWait(void)
{
	// �Ԃ��|�ꃂ�[�V����
	GetMotion()->Set(MOTION_DEAD);
}

//==========================================================================
// �t�F�[�h�A�E�g���
//==========================================================================
void CPlayer::StateFadeOut(void)
{
	// ��ԑJ�ڃJ�E���^�[���Z
	m_nCntState--;

	// �F�ݒ�
	m_mMatcol.a = (float)m_nCntState / (float)FADEOUTTIME;

	// �Ԃ��|�ꃂ�[�V����
	GetMotion()->Set(MOTION_DEAD);

	if (m_nCntState <= 0)
	{// �J�ڃJ�E���^�[��0�ɂȂ�����

		// ���S����
		Kill();

		// �I������
		Uninit();
		return;
	}
}

//==========================================================================
// �m�b�N�o�b�N
//==========================================================================
void CPlayer::StateKnockBack(void)
{
	// �ʒu�擾
	MyLib::Vector3 pos = GetPosition();

	// �ړ��ʎ擾
	MyLib::Vector3 move = GetMove();

	// �����擾
	MyLib::Vector3 rot = GetRotation();

	// �ڕW�̌����擾
	float fRotDest = GetRotDest();

	// �ړ��ʎ擾
	float fMove = GetVelocity();

	// ���݂ƖڕW�̍���
	float fRotDiff = 0.0f;

	// �����̔���
	bool bLen = false;


	// ��ԑJ�ڃJ�E���^�[���Z
	m_nCntState++;

	// �ʒu�X�V
	pos.y = (-0.4f * (float)(m_nCntState * m_nCntState) + m_KnokBackMove.y * (float)m_nCntState) + m_posKnokBack.y;
	pos.x += move.x;
	pos.z += move.z;

	// �N���Ƃ̔���
	if ((CManager::GetInstance()->GetScene()->GetElevation()->IsHit(pos) || m_bHitStage))
	{// �n�ʂƓ������Ă�����
		m_state = STATE_INVINCIBLE;
		m_nCntState = INVINCIBLE_TIME;
		m_KnokBackMove.y = 0.0f;	// �ړ��ʃ[��
		m_bLandOld = true;

		// �F�ݒ�
		m_mMatcol = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		
		// �m�b�N�o�b�N�������
		m_sMotionFrag.bKnockBack = false;

		// ���[�V�����擾
		CMotion* pMotion = GetMotion();
		if (pMotion == NULL)
		{// ���[�V������NULL��������
			return;
		}
		pMotion->ToggleFinish(true);

		// X�t�@�C���Ƃ̔���
		CStage *pStage = CGame::GetInstance()->GetStage();
		if (pStage == NULL)
		{// NULL��������
			return;
		}

		// �X�e�[�W�ɓ�����������
		for (int nCntStage = 0; nCntStage < pStage->GetNumAll(); nCntStage++)
		{
			// �I�u�W�F�N�g�擾
			CObjectX *pObjX = pStage->GetObj(nCntStage);

			if (pObjX == NULL)
			{// NULL��������
				continue;
			}

			// �����擾
			bool bLand = false;
			pos.y = pObjX->GetHeight(pos, bLand);
		}
	}


	// �ʒu�ݒ�
	SetPosition(pos);

	// �ړ��ʐݒ�
	SetMove(move);

	// �����ݒ�
	SetRotation(rot);

	// �ڕW�̌����ݒ�
	SetRotDest(fRotDest);
}

//==========================================================================
// ���X�|�[��
//==========================================================================
void CPlayer::StateRespawn(void)
{
	// ���[�V�����擾
	CMotion* pMotion = GetMotion();
	if (pMotion == nullptr)
	{
		return;
	}

	int nType = pMotion->GetType();
	if (nType != MOTION_RESPAWN)
	{// �������I��
		m_state = STATE_NONE;
		return;
	}

	if (nType != MOTION_RESPAWN)
	{
		// �������[�V�����ݒ�
		pMotion->Set(MOTION_RESPAWN);
	}
}

//==========================================================================
// �J�E���^�[���
//==========================================================================
void CPlayer::StateCounter(void)
{
	// ���[�V�����擾
	CMotion* pMotion = GetMotion();
	if (pMotion == NULL)
	{
		return;
	}

	// �ʒu�擾
	MyLib::Vector3 pos = GetPosition();

	CEnemy* pEnemy = CEnemy::GetListObj().GetData(m_nIdxRockOn);
	if (pEnemy == nullptr)
	{
		// �I�����̐ݒ�
		if (m_pEndCounterSetting != nullptr)
		{
			m_pEndCounterSetting->EndSetting(this);
			delete m_pEndCounterSetting;
			m_pEndCounterSetting = nullptr;
		}
		return;
	}

	MyLib::Vector3 enemypos = pEnemy->GetPosition();

	SetRotDest(atan2f((pos.x - enemypos.x), (pos.z - enemypos.z)));

	int nType = pMotion->GetType();
	if (nType != MOTION_COUNTER_ACCEPT &&
		nType != MOTION_COUNTER_TURN &&
		nType != MOTION_COUNTER_ATTACK)
	{// �J�E���^�[��Ԃ��I��

		// �I�����̐ݒ�
		if (m_pEndCounterSetting != nullptr)
		{
			m_pEndCounterSetting->EndSetting(this);
			delete m_pEndCounterSetting;
			m_pEndCounterSetting = nullptr;
		}
		return;
	}
}

//==========================================================================
// ���
//==========================================================================
void CPlayer::StateAvoid(void)
{
	// ���[�V�����擾
	CMotion* pMotion = GetMotion();
	if (pMotion == NULL)
	{
		return;
	}

	int nType = pMotion->GetType();
	if (nType != MOTION_AVOID)
	{// ������I��
		m_state = STATE_NONE;
		return;
	}
}

//==========================================================================
// �`�揈��
//==========================================================================
void CPlayer::Draw(void)
{

	// �`�揈��
	if (m_state == STATE_DMG)
	{
		CObjectChara::Draw(m_mMatcol);
	}
	else if (m_state == STATE_INVINCIBLE || m_state == STATE_FADEOUT)
	{
		CObjectChara::Draw(m_mMatcol.a);
	}
	else
	{
		CObjectChara::Draw();
	}
}

//==========================================================================
// ��Ԑݒ�
//==========================================================================
void CPlayer::SetState(STATE state, int nCntState)
{
	m_state = state;
	m_nCntState = nCntState;
}

//==========================================================================
// ��Ԏ擾
//==========================================================================
CPlayer::STATE CPlayer::GetState(void)
{
	return m_state;
}

//==========================================================================
// �̗̓A�b�v�O���[�h
//==========================================================================
void CPlayer::UpgradeLife(int addvalue)
{
	// �A�b�v�O���[�h��̗̑͂ɐݒ�
	SetLife(m_pHPGauge->UpgradeMaxValue(addvalue));
	SetLifeOrigin(m_pHPGauge->GetMaxLife());
}

//==========================================================================
// �X�^�~�i�A�b�v�O���[�h
//==========================================================================
void CPlayer::UpgradeMaxStamina(int addvalue)
{
	// �X�^�~�i�ő�l�㏸
	if (m_pStaminaGauge != nullptr)
	{
		m_pStaminaGauge->UpgradeMaxValue(addvalue);
	}
}

//==========================================================================
// �X�^�~�i�����񕜃A�b�v�O���[�h
//==========================================================================
void CPlayer::UpgradeAutoHealStamina(float ratio)
{
	// �X�^�~�i�����񕜒l�㏸
	if (m_pStaminaGauge != nullptr)
	{
		m_pStaminaGauge->UpgradeAutoHeal(ratio);
	}
}

//==========================================================================
// ���X�|�[�����̐ݒ�
//==========================================================================
void CPlayer::RespawnSetting()
{
	SetMotion(CPlayer::MOTION_RESPAWN);
	m_state = STATE_RESPAWN;

	m_nRespawnPercent -= 30;
	UtilFunc::Transformation::ValueNormalize(m_nRespawnPercent, 100, 0);

	SetLife(GetLifeOrigin());
}

//==========================================================================
// ���X�|�[���m���ݒ�
//==========================================================================
void CPlayer::SetRespawnPercent(int value)
{
	m_nRespawnPercent = value;
	UtilFunc::Transformation::ValueNormalize(m_nRespawnPercent, 100, 0);
}


//==========================================================================
// �J�E���^�[�I�����̐ݒ�
//==========================================================================
void CEndCounterSetting::EndSetting(CPlayer* player)
{
	player->SetState(CPlayer::STATE_NONE);
	CManager::GetInstance()->GetCamera()->SetRockOnState(CCamera::RockOnState::ROCKON_NORMAL);
}

void CEndAttack::EndSetting(CPlayer* player)
{
	// �e�̏���
	CEndCounterSetting::EndSetting(player);

	if (!player->IsLockOnAtStart())
	{
		if (CManager::GetInstance()->GetCamera()->IsRockOn())
		{// ���b�N�I������

			// ���b�N�I���ݒ�
			CManager::GetInstance()->GetCamera()->SetRockOn(0.0f, false);

			// ���X�g���[�v
			CListManager<CEnemy> enemyList = CEnemy::GetListObj();
			CEnemy* pEnemy = nullptr;
			while (enemyList.ListLoop(&pEnemy))
			{
				pEnemy->SetEnableRockOn(false);
			}
		}
	}
}