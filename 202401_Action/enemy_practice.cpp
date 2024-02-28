//==========================================================================
// 
//  ���K�G���� [enemy_practice.cpp]
//  Author : ���n�Ή�
// 
//==========================================================================
#include "enemy_practice.h"
#include "manager.h"
#include "particle.h"
#include "player.h"
#include "game.h"
#include "camera.h"

//==========================================================================
// �萔��`
//==========================================================================
namespace
{
	const float LENGTH_PUNCH = 110.0f;		// �p���`�̒���
	const float VELOCITY_WALK = 1.0f;		// ����
	const float TIME_WAIT = 2.0f;			// �ҋ@
	const float TIME_STRONGATK = 1.0f;		// ���U�������܂ł̃^�C�}�[
}

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CEnemyPractice::CEnemyPractice(int nPriority) : CEnemy(nPriority)
{

}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CEnemyPractice::~CEnemyPractice()
{

}

//==========================================================================
// ����������
//==========================================================================
HRESULT CEnemyPractice::Init()
{
	//����������
	CEnemy::Init();

	// �s��
	m_Action = ACTION_DEF;
	
	// �_�E�����A
	m_pReturnDown = DEBUG_NEW CReturnDown_Practice();

	// ���E�E�ǂ������t���O���Z�b�g
	m_bCatchUp = false;
	m_bInSight = false;

	// ���U���^�C�}�[
	m_fStrongAttackTime = TIME_STRONGATK;

	// �X�[�p�[�A�[�}�[
	m_bActiveSuperArmor = true;

	float angle = GetPosition().AngleXZ(0.0f);
	SetRotation(MyLib::Vector3(0.0f, angle, 0.0f));
	SetRotDest(angle);

	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CEnemyPractice::Uninit()
{
	// �I������
	CEnemy::Uninit();
}

//==========================================================================
// �E��
//==========================================================================
void CEnemyPractice::Kill()
{
	// ���S����
	CEnemy::Kill();
}

//==========================================================================
// �X�V����
//==========================================================================
void CEnemyPractice::Update()
{
	// ���S�̔���
	if (IsDeath() == true)
	{// ���S�t���O�������Ă�����
		return;
	}

	// �X�V����
	CEnemy::Update();

	// ���S�̔���
	if (IsDeath() == true)
	{// ���S�t���O�������Ă�����
		return;
	}

	// �����ʒu�ɌŒ�
	SetPosition(CObject::GetOriginPosition());
	SetRotation(MyLib::Vector3(0.0f, GetPosition().AngleXZ(0.0f), 0.0f));

	if (GetMotion()->GetType() == MOTION::MOTION_ATTACK_STRONG)
	{
		CMyEffekseer::GetInstance()->SetPosition(m_pWeaponHandle, GetCenterPosition());
	}
}


//==========================================================================
// �q�b�g����
//==========================================================================
bool CEnemyPractice::Hit(const int nValue, const MyLib::Vector3& hitpos, CGameManager::AttackType atkType)
{
	bool bHit = false;

	// �����擾
	MyLib::Vector3 rot = GetRotation();
	MyLib::Vector3 pos = GetPosition();

	if (m_bDamageReceived)
	{// �_���[�W��t���̂�

		// ��������
		bHit = true;

		// �s���\����
		if (!m_bActiveSuperArmor)
		{
			m_bActionable = false;
		}

		// �_���[�W���󂯕t���Ȃ�
		m_bDamageReceived = false;

		// �_���[�W���Ԑݒ�
		m_fDamageReciveTime = static_cast<float>(10) / static_cast<float>(mylib_const::DEFAULT_FPS);

		// �ߋ��̏�ԕۑ�
		m_Oldstate = m_state;

		switch (atkType)
		{
		case CGameManager::ATTACK_NORMAL:
			NormalHitResponse();
			break;

		case CGameManager::ATTACK_COUNTER:
			CounterHitResponse();
			break;
		}
	}

	return bHit;
}

//==========================================================================
// �ʏ�q�b�g���̔���
//==========================================================================
void CEnemyPractice::NormalHitResponse()
{
	
	// �_���[�W��Ԃɂ���
	if (m_state != STATE_DOWN &&
		m_state != STATE_STRONGATK)
	{
		m_state = STATE_DMG;
	}

	// �J�ڃJ�E���^�[�ݒ�
	m_fStateTime = static_cast<float>(10) / static_cast<float>(mylib_const::DEFAULT_FPS);

	// �U��
	CManager::GetInstance()->GetCamera()->SetShake(10, 15.0f, 0.0f);
}

//==========================================================================
// �J�E���^�[�q�b�g���̔���
//==========================================================================
void CEnemyPractice::CounterHitResponse()
{
	
	// �_�E����Ԃɂ���
	m_state = STATE_DOWN;

	// �J�ڃJ�E���^�[�ݒ�
	m_fStateTime = static_cast<float>(10) / static_cast<float>(mylib_const::DEFAULT_FPS);

	// �_�E���J�E���^�[�ݒ�
	m_fDownTime = static_cast<float>(150) / static_cast<float>(mylib_const::DEFAULT_FPS);

	// ���ꃂ�[�V����
	GetMotion()->Set(MOTION_DOWN);

	// �q�b�g�X�g�b�v
	CManager::GetInstance()->SetEnableHitStop(5);

	// �U��
	CManager::GetInstance()->GetCamera()->SetShake(10, 15.0f, 0.0f);
}

//==========================================================================
// �_�E�����
//==========================================================================
void CEnemyPractice::StateDown()
{
	// �s���\����
	m_bActionable = false;

	if (m_fDownTime <= 0.0f)
	{
		m_state = STATE_NONE;
		// �s���\����
		m_bActionable = true;

		// �_�E�����A
		if (m_pReturnDown != nullptr)
		{
			ChangeATKState(m_pReturnDown);
			m_pATKState->ChangeMotionIdx(this);
		}
		else
		{
			// �s�����I
			DrawingRandomAction();
		}
		return;
	}

	// ���[�V�����擾
	CMotion* pMotion = GetMotion();
	if (pMotion == nullptr)
	{
		return;
	}

	int nType = pMotion->GetType();
	if (nType != MOTION_DOWN)
	{
		// �_�E�����[�V�����ݒ�
		pMotion->Set(MOTION_DOWN);
	}
}

//==========================================================================
// �`�揈��
//==========================================================================
void CEnemyPractice::Draw()
{
	// �`�揈��
	CEnemy::Draw();
}

//==========================================================================
// ���[�V�����Z�b�g
//==========================================================================
void CEnemyPractice::MotionSet()
{
	// ���[�V�����擾
	CMotion* pMotion = GetMotion();
	if (pMotion == nullptr)
	{
		return;
	}

	if (pMotion->IsFinish() == true)
	{// �I�����Ă�����

		// ���݂̎�ގ擾
		int nType = pMotion->GetType();

		if (m_sMotionFrag.bMove == true && m_sMotionFrag.bKnockback == false && m_sMotionFrag.bATK == false)
		{// �ړ����Ă�����

			// �U�����Ă��Ȃ�
			m_sMotionFrag.bATK = false;

			// �s���ʐݒ菈��
			pMotion->Set(MOTION_WALK);
		}
		else if (m_sMotionFrag.bKnockback == true)
		{// ���ꒆ��������

			// ���ꃂ�[�V����
			pMotion->Set(MOTION_KNOCKBACK);
		}
	}
}

//==========================================================================
// �U��������
//==========================================================================
void CEnemyPractice::AttackAction(CMotion::AttackInfo ATKInfo, int nCntATK)
{
	// ���[�V�����擾
	CMotion* pMotion = GetMotion();
	if (pMotion == nullptr)
	{
		return;
	}

	// ���[�V�������擾
	int nMotionType = pMotion->GetType();
	MyLib::Vector3 weponpos = pMotion->GetAttackPosition(GetModel(), ATKInfo);

	// ���擾
	MyLib::Vector3 pos = GetPosition();
	MyLib::Vector3 rot = GetRotation();

	// ���[�V�����ʏ���
	switch (nMotionType)
	{
	case MOTION_ATTACK_NORMAL:
		CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL::LABEL_SE_ENEMY_SWING_SLOW);
		break;

	case MOTION_ATTACK_STRONG:
		if (nCntATK == 0) {
			m_pWeaponHandle = CMyEffekseer::GetInstance()->SetEffect(
				CMyEffekseer::EFKLABEL_STRONGATK_SIGN,
				GetCenterPosition(), 0.0f, 0.0f, 20.0f);

			CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL::LABEL_SE_STRONGATK, true);
		}
		else {
			CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL::LABEL_SE_ENEMY_SWING_SLOW);
		}
		break;
	}
}

//==========================================================================
// �U�����蒆����
//==========================================================================
void CEnemyPractice::AttackInDicision(CMotion::AttackInfo* pATKInfo, int nCntATK)
{
	// ���̍U�����蒆����
	CEnemy::AttackInDicision(pATKInfo, nCntATK);

	// ���[�V�����擾
	CMotion* pMotion = GetMotion();
	if (pMotion == nullptr)
	{
		return;
	}

	// ���[�V�������擾
	int nMotionType = pMotion->GetType();
	MyLib::Vector3 weponpos = pMotion->GetAttackPosition(GetModel(), *pATKInfo);

	// ���[�V�����ʏ���
	switch (nMotionType)
	{
	case MOTION_ATTACK_NORMAL:
		break;
	}
}


//==========================================================================
// �ߐڍU���̍s��
//==========================================================================
void CAttack_PracticeNormalAttack::Action(CEnemy* boss)
{
	Attack(boss);
}

//==========================================================================
// ���U���̍s��
//==========================================================================
void CAttack_PracticeStrongAttack::Action(CEnemy* boss)
{
	Attack(boss);
}