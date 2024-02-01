//==========================================================================
// 
//  �{�X���� [enemy_boss.cpp]
//  Author : ���n�Ή�
// 
//==========================================================================
#include "enemy_boss.h"
#include "manager.h"
#include "debugproc.h"
#include "calculation.h"
#include "blackframe.h"
#include "beam.h"
#include "particle.h"
#include "hp_gauge_boss.h"
#include "camera.h"
#include "sound.h"
#include "player.h"

//==========================================================================
// �萔��`
//==========================================================================
namespace
{
	// �s�����I�̍\����
	struct sProbability 
	{
		CEnemyBoss::ACTION action;	// �s��
		float fProbability;			// �m��
	};

	const float LENGTH_PUNCH = 600.0f;		// �p���`�̒���
	const float LENGTH_CHASEWALK = 800.0f;	// �����Ǐ]�̒���
	const float VELOCITY_WALK = 1.0f;		// ����
	const float VELOCITY_DASH = 3.5f;		// �_�b�V��
	const float VELOCITY_TACKLE = 6.0f;		// �^�b�N��
	const float TIME_WAIT = 1.0f;			// �ҋ@
	const float TIME_PROXIMITYCHASE = 5.0f;	// �ߐڍU���̒Ǐ]���ԍő�l
	const float TIME_DMG = static_cast<float>(10) / static_cast<float>(mylib_const::DEFAULT_FPS);	// �_���[�W����
	const float TIME_DOWN = static_cast<float>(150) / static_cast<float>(mylib_const::DEFAULT_FPS);	// �_�E������
}

//==========================================================================
// �֐��|�C���^
//==========================================================================

//==========================================================================
//  �R���X�g���N�^
//==========================================================================
CEnemyBoss::CEnemyBoss(int nPriority) : CEnemy(nPriority)
{
	m_TargetPosition = 0.0f;	// �ڕW�̈ʒu
	m_fActTime = 0.0f;			// �s���J�E���^�[
	m_bCatchUp = false;			// �ǂ���������
	m_pBossHPGauge = nullptr;	// �{�X��HP�Q�[�W


	m_pAtkPattern.push_back(DEBUG_NEW CBossSideSwipeCombo());	// ���Ȃ��R���{
	m_pAtkPattern.push_back(DEBUG_NEW CBossOverHead());			// �U�艺�낵
	m_pAtkPattern.push_back(DEBUG_NEW CBossLaunchBallast());	// ���I��΂�
}

//==========================================================================
//  �f�X�g���N�^
//==========================================================================
CEnemyBoss::~CEnemyBoss()
{

}

//==========================================================================
//  ����������
//==========================================================================
HRESULT CEnemyBoss::Init(void)
{
	//����������
	CEnemy::Init();

	// HP�̐ݒ�
	m_pBossHPGauge = CHP_GaugeBoss::Create(MyLib::Vector3(640.0f, 50.0f, 0.0f), GetLifeOrigin());	// �{�X��HP�Q�[�W
	m_pBossHPGauge->SetLife(0);

	// ���t���[���J����
	//CManager::GetInstance()->GetBlackFrame()->SetState(CBlackFrame::STATE_OUT);

	DrawingRandomAction();
	return S_OK;
}

//==========================================================================
//  �I������
//==========================================================================
void CEnemyBoss::Uninit(void)
{
	// �I������
	CEnemy::Uninit();

	for (int i = 0; i < static_cast<int>(m_pAtkPattern.size()); i++)
	{
		delete m_pAtkPattern[i];
		m_pAtkPattern[i] = nullptr;
	}
}

//==========================================================================
// �E��
//==========================================================================
void CEnemyBoss::Kill(void)
{
	if (m_pHPGauge != nullptr)
	{
		m_pBossHPGauge->Kill();
		m_pBossHPGauge = nullptr;
	}

	for (int i = 0; i < static_cast<int>(m_pAtkPattern.size()); i++)
	{
		delete m_pAtkPattern[i];
		m_pAtkPattern[i] = nullptr;
	}
	// ���S����
	CEnemy::Kill();
}

//==========================================================================
//  �X�V����
//==========================================================================
void CEnemyBoss::Update(void)
{
	// ���S�̔���
	if (IsDeath() == true)
	{// ���S�t���O�������Ă�����
		return;
	}

	// �X�V����
	CEnemy::Update();

	if (IsDeath() == true)
	{// ���S�t���O�������Ă�����
		return;
	}

	// �U�����s����
	PerformAttack();

	// HP�X�V
	m_pBossHPGauge->SetLife(GetLife());

	// �v���C���[���
	CPlayer* pPlayer = CPlayer::GetListObj().GetData(m_nTargetPlayerIndex);
	if (pPlayer == nullptr)
	{
		return;
	}
	m_TargetPosition = pPlayer->GetPosition();

	//// ���t���[���J����
	//if (CManager::GetInstance()->GetBlackFrame()->GetState() == CBlackFrame::STATE_INCOMPLETION)
	//{
	//	CManager::GetInstance()->GetBlackFrame()->SetState(CBlackFrame::STATE_OUT);
	//}
}

//==========================================================================
// �ʏ�q�b�g���̔���
//==========================================================================
void CEnemyBoss::NormalHitResponse()
{
	// �_���[�W��Ԃɂ���
	m_state = STATE_DMG;

	// �J�ڃJ�E���^�[�ݒ�
	m_fStateTime = TIME_DMG;

	// �q�b�g�X�g�b�v
	CManager::GetInstance()->SetEnableHitStop(5);

	// �U��
	CManager::GetInstance()->GetCamera()->SetShake(10, 15.0f, 0.0f);

	// �m�b�N�o�b�N�̈ʒu�X�V
	m_posKnokBack = GetPosition();
}

//==========================================================================
// �J�E���^�[�q�b�g���̔���
//==========================================================================
void CEnemyBoss::CounterHitResponse()
{
	// �_�E����Ԃɂ���
	m_state = STATE_DOWN;

	// �J�ڃJ�E���^�[�ݒ�
	m_fStateTime = TIME_DMG;

	// �_�E���J�E���^�[�ݒ�
	m_fDownTime = TIME_DOWN;

	// ���ꃂ�[�V����
	//GetMotion()->Set(MOTION_DOWN);

	// �q�b�g�X�g�b�v
	CManager::GetInstance()->SetEnableHitStop(5);

	// �U��
	CManager::GetInstance()->GetCamera()->SetShake(10, 15.0f, 0.0f);

	// �m�b�N�o�b�N�̈ʒu�X�V
	m_posKnokBack = GetPosition();
}

//==========================================================================
// �s���ݒ�
//==========================================================================
void CEnemyBoss::ActionSet(void)
{

}

//==========================================================================
// �s���X�V
//==========================================================================
void CEnemyBoss::UpdateAction(void)
{

}

//==========================================================================
// �U����Ԑ؂�ւ�
//==========================================================================
void CEnemyBoss::ChangeATKState(CBossState* state)
{
	m_pATKState = state;
}

//==========================================================================
// �U����Ԑ؂�ւ�
//==========================================================================
void CEnemyBoss::PerformAttack()
{
	m_pATKState->Action(this);
}

//==========================================================================
// �U����Ԑ؂�ւ�
//==========================================================================
void CEnemyBoss::DrawingRandomAction()
{
	// �����_���ɃA�N�V�����p�^�[����I�����Ď��s
	if (!m_pAtkPattern.empty())
	{
		int randomIndex = rand() % m_pAtkPattern.size();
		ChangeATKState(m_pAtkPattern[randomIndex]);
		m_bCatchUp = false;

		// ���[�V�����C���f�b�N�X�؂�ւ�
		m_pATKState->ChangeMotionIdx(this);
	}
}


//==========================================================================
// �U������
//==========================================================================
void CBossAttack::Attack(CEnemyBoss* boss)
{
	// ���[�V�����擾
	CMotion* pMotion = boss->GetMotion();
	if (pMotion == NULL)
	{
		return;
	}

	int nType = pMotion->GetType();
	if (nType == m_nIdxMotion && pMotion->IsFinish() == true)
	{// �U�����I����Ă���

		// ���̍s�����I
		boss->DrawingRandomAction();

		// �ҋ@���[�V�����ݒ�
		pMotion->Set(CEnemyBoss::MOTION_DEF);
		return;
	}

	if (nType != m_nIdxMotion)
	{
		// ���[�V�����ݒ�
		pMotion->Set(m_nIdxMotion);
	}

	// �U���t���O�𗧂Ă�
	//m_sMotionFrag.bATK = true;

}


//==========================================================================
// �ߐڍU���̍s��
//==========================================================================
void CBossProximity::Action(CEnemyBoss* boss)
{
	// �͈͊O���Ǐ]
	if (!boss->IsCatchUp())
	{
		// �ǂ��|����
		boss->ActChase();
		return;
	}

	// �U������
	Attack(boss);
}

//==========================================================================
// �������U���̍s��
//==========================================================================
void CBossRemote::Action(CEnemyBoss* boss)
{
	// �U������
	Attack(boss);
}


//==========================================================================
// �ҋ@
//==========================================================================
void CEnemyBoss::ActWait(void)
{
	// ���[�V�����擾
	CMotion* pMotion = GetMotion();
	if (pMotion == NULL)
	{
		return;
	}

	// �ҋ@���[�V�����ݒ�
	pMotion->Set(MOTION_DEF);

	// �s���J�E���^�[���Z
	m_fActTime += CManager::GetInstance()->GetDeltaTime();

	// �^�[�Q�b�g�̕�������
	RotationTarget();

	if (TIME_WAIT <= m_fActTime)
	{// �ҋ@���Ԓ�������

		// �s�����I
		DrawingRandomAction();
		m_fActTime = 0.0f;
	}
}

//==========================================================================
// �ǂ��|��
//==========================================================================
void CEnemyBoss::ActChase(void)
{
	// �ړ��t���O�𗧂Ă�
	m_sMotionFrag.bMove = true;

	// �^�[�Q�b�g�̕�������
	RotationTarget();

	// ���擾
	MyLib::Vector3 move = GetMove();
	MyLib::Vector3 rot = GetRotation();
	float fMove = GetVelocity();

	// �ړ��ʐݒ�
	move.x += sinf(D3DX_PI + rot.y) * fMove * VELOCITY_WALK;
	move.z += cosf(D3DX_PI + rot.y) * fMove * VELOCITY_WALK;

	// �ړ��ʐݒ�
	SetMove(move);

	// �ǂ���������
	m_bCatchUp = UtilFunc::Collision::CircleRange3D(GetPosition(), m_TargetPosition, LENGTH_PUNCH, 0.0f);
}


//==========================================================================
// �`�揈��
//==========================================================================
void CEnemyBoss::Draw(void)
{
	// �`�揈��
	CEnemy::Draw();
}

//==========================================================================
// ���[�V�����Z�b�g
//==========================================================================
void CEnemyBoss::MotionSet(void)
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

		if (m_sMotionFrag.bMove == true && m_sMotionFrag.bKnockback == false && m_sMotionFrag.bATK == false)
		{// �ړ����Ă�����

			// �U�����Ă��Ȃ�
			m_sMotionFrag.bATK = false;
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
// �^�[�Q�b�g�̕�������
//==========================================================================
void CEnemyBoss::RotationTarget(void)
{
	// �ʒu�擾
	MyLib::Vector3 pos = GetPosition();
	MyLib::Vector3 rot = GetRotation();

	// �ڕW�̊p�x�����߂�
	float fRotDest = atan2f((pos.x - m_TargetPosition.x), (pos.z - m_TargetPosition.z));

	// �ڕW�Ƃ̍���
	float fRotDiff = fRotDest - rot.y;

	//�p�x�̐��K��
	UtilFunc::Transformation::RotNormalize(fRotDiff);

	//�p�x�̕␳������
	rot.y += fRotDiff * 0.1f;
	UtilFunc::Transformation::RotNormalize(rot.y);

	// �����ݒ�
	SetRotation(rot);

	// �ڕW�̌����ݒ�
	SetRotDest(fRotDest);
}

//==========================================================================
// �U��������
//==========================================================================
void CEnemyBoss::AttackAction(CMotion::AttackInfo ATKInfo, int nCntATK)
{
	// ���[�V�����擾
	CMotion* pMotion = GetMotion();
	if (pMotion == NULL)
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
	case MOTION_SIDESWIPE:
		break;

	case MOTION_LAUNCHBALLAST:
		break;
	}
}

//==========================================================================
// �U�����蒆����
//==========================================================================
void CEnemyBoss::AttackInDicision(CMotion::AttackInfo ATKInfo, int nCntATK)
{
	// ���̍U�����蒆����
	CEnemy::AttackInDicision(ATKInfo, nCntATK);

	// ���[�V�����擾
	CMotion* pMotion = GetMotion();
	if (pMotion == NULL)
	{
		return;
	}

	// ���[�V�������擾
	int nMotionType = pMotion->GetType();
	MyLib::Vector3 weponpos = pMotion->GetAttackPosition(GetModel(), ATKInfo);

	// ���[�V�����ʏ���
	switch (nMotionType)
	{
	case MOTION_SIDESWIPE:
		break;

	case MOTION_LAUNCHBALLAST:
		break;
	}
}
