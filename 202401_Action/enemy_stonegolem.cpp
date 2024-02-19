//==========================================================================
// 
//  �X�g�[���S�[�����G���� [enemy_stonegolem.cpp]
//  Author : ���n�Ή�
// 
//==========================================================================
#include "enemy_stonegolem.h"
#include "manager.h"
#include "debugproc.h"
#include "particle.h"

//==========================================================================
// �萔��`
//==========================================================================
namespace
{
	const float LENGTH_PUNCH = 110.0f;		// �p���`�̒���
	const float VELOCITY_WALK = 1.0f;		// ����
	const float TIME_WAIT = 0.1f;			// �ҋ@
}

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CEnemyGolem::CEnemyGolem(int nPriority) : CEnemy(nPriority)
{
	
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CEnemyGolem::~CEnemyGolem()
{

}

//==========================================================================
// ����������
//==========================================================================
HRESULT CEnemyGolem::Init()
{
	//����������
	CEnemy::Init();

	// �s��
	m_Action = ACTION_DEF;
	m_pAtkPattern.push_back(DEBUG_NEW CEnemyGolemUPSwipe());	// 1�i��
	m_pAtkPattern.push_back(DEBUG_NEW CEnemyGolemSideSwipe());	// 2�i��

	// ���b�N�I���̋���
	m_fRockOnDistance = 400.0f;

	// ���E�E�ǂ������t���O���Z�b�g
	m_bCatchUp = false;
	m_bInSight = false;

	// �U���؂�ւ�
	ChangeATKState(m_pAtkPattern[0]);
	m_pATKState->ChangeMotionIdx(this);

	// �X�[�p�[�A�[�}�[
	m_bActiveSuperArmor = true;
	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CEnemyGolem::Uninit()
{
	for (int i = 0; i < static_cast<int>(m_pAtkPattern.size()); i++)
	{
		delete m_pAtkPattern[i];
		m_pAtkPattern[i] = nullptr;
	}

	// �I������
	CEnemy::Uninit();
}

//==========================================================================
// �E��
//==========================================================================
void CEnemyGolem::Kill()
{
	for (int i = 0; i < static_cast<int>(m_pAtkPattern.size()); i++)
	{
		delete m_pAtkPattern[i];
		m_pAtkPattern[i] = nullptr;
	}

	// ���S����
	CEnemy::Kill();
}

//==========================================================================
// �X�V����
//==========================================================================
void CEnemyGolem::Update()
{
	// ���S�̔���
	if (IsDeath() == true)
	{// ���S�t���O�������Ă�����
		return;
	}

	// �X�V����
	CEnemy::Update();
}

//==========================================================================
// �`�揈��
//==========================================================================
void CEnemyGolem::Draw()
{
	// �`�揈��
	CEnemy::Draw();
}

//==========================================================================
// ���[�V�����Z�b�g
//==========================================================================
void CEnemyGolem::MotionSet()
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
void CEnemyGolem::AttackAction(CMotion::AttackInfo ATKInfo, int nCntATK)
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

	switch (nMotionType)
	{
	case MOTION_ATTACK_STRONG:
		break;

	default:
		break;
	}

}

//==========================================================================
// �U�����蒆����
//==========================================================================
void CEnemyGolem::AttackInDicision(CMotion::AttackInfo* pATKInfo, int nCntATK)
{
	// ���̍U�����蒆����
	CEnemy::AttackInDicision(pATKInfo, nCntATK);

	// ���[�V�����擾
	CMotion* pMotion = GetMotion();
	if (pMotion == NULL)
	{
		return;
	}

	// ���[�V�������擾
	int nMotionType = pMotion->GetType();
	MyLib::Vector3 weponpos = pMotion->GetAttackPosition(GetModel(), *pATKInfo);

}
