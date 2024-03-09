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
#include "camera.h"

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
CEnemyGolem::CEnemyGolem() : CEnemy()
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
	m_pAtkPattern.push_back(DEBUG_NEW CGolemStrongAttack());	// ���U��

	// �_�E�����A
	m_pReturnDown = DEBUG_NEW CReturnDown_Golem();

	// ���b�N�I���̋���
	m_fRockOnDistance = 400.0f;

	// ���E�E�ǂ������t���O���Z�b�g
	m_bCatchUp = false;
	m_bInSight = false;

	// �U���؂�ւ�
	ChangeATKState(m_pAtkPattern[0]);

	// �X�[�p�[�A�[�}�[
	m_bActiveSuperArmor = true;
	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CEnemyGolem::Uninit()
{
	/*for (int i = 0; i < static_cast<int>(m_pAtkPattern.size()); i++)
	{
		delete m_pAtkPattern[i];
		m_pAtkPattern[i] = nullptr;
	}*/

	// �I������
	CEnemy::Uninit();
}

//==========================================================================
// �E��
//==========================================================================
void CEnemyGolem::Kill()
{
	/*for (int i = 0; i < static_cast<int>(m_pAtkPattern.size()); i++)
	{
		delete m_pAtkPattern[i];
		m_pAtkPattern[i] = nullptr;
	}*/

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

	// ���S�̔���
	if (IsDeath() == true)
	{// ���S�t���O�������Ă�����
		return;
	}

	if (GetMotion()->GetType() == MOTION::MOTION_ATTACK_STRONG)
	{
		CMyEffekseer::GetInstance()->SetPosition(m_pWeaponHandle, GetCenterPosition());
	}
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
void CEnemyGolem::AttackAction(CMotion::AttackInfo ATKInfo, int nCntATK)
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

	switch (nMotionType)
	{
	case MOTION::MOTION_ATTACK_NORMAL:
	case MOTION::MOTION_ATTACK_SIDESWIPE2:
	case MOTION::MOTION_ATTACK_SIDESWIPE:
	case MOTION::MOTION_ATTACK_UPSWIPE2:
		CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL::LABEL_SE_ENEMY_SWING_SLOW);
		break;

	case MOTION_ATTACK_STRONG:
		if (nCntATK == 0)
		{
			m_pWeaponHandle = CMyEffekseer::GetInstance()->SetEffect(
				CMyEffekseer::EFKLABEL_STRONGATK_SIGN,
				GetCenterPosition(), 0.0f, 0.0f, 20.0f);

			CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL::LABEL_SE_STRONGATK);
		}
		else if (nCntATK == 1 || nCntATK == 2)
		{
			CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL::LABEL_SE_ENEMY_SWING_FAST);
		}
		else
		{
			// �U��
			CManager::GetInstance()->GetCamera()->SetShake(8, 25.0f, 0.0f);
			CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL::LABEL_SE_ENEMY_FIELD_STRONG);
			CMyEffekseer::GetInstance()->SetEffect(
				CMyEffekseer::EFKLABEL::EFKLABEL_ENEMYATK_SAND,
				weponpos, 0.0f, 0.0f, 20.0f);
		}
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
	if (pMotion == nullptr)
	{
		return;
	}

	// ���[�V�������擾
	int nMotionType = pMotion->GetType();
	MyLib::Vector3 weponpos = pMotion->GetAttackPosition(GetModel(), *pATKInfo);

}
