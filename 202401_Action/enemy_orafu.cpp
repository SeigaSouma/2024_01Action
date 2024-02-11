//==========================================================================
// 
//  �I���t�G���� [enemy_orafu.cpp]
//  Author : ���n�Ή�
// 
//==========================================================================
#include "enemy_orafu.h"
#include "manager.h"
#include "debugproc.h"
#include "calculation.h"
#include "particle.h"
#include "santabag.h"
#include "player.h"

//==========================================================================
// �萔��`
//==========================================================================
namespace
{
	const float LENGTH_PUNCH = 110.0f;		// �p���`�̒���
	const float LENGTH_PLAYERCHASE = 600.0f;	// �v���C���[�ǂ������鋗��
	const float VELOCITY_WALK = 1.0f;		// ����
	const float TIME_WAIT = 2.0f;			// �ҋ@
}

//==========================================================================
// �֐��|�C���^
//==========================================================================
CEnemyOrafu::ACT_FUNC CEnemyOrafu::m_ActFuncList[] =
{
	&CEnemyOrafu::ActChase,			// �ǂ��|��
	&CEnemyOrafu::ActAttackProximity,	// �ߐ�
	&CEnemyOrafu::ActWait,				// �ҋ@
};

//==========================================================================
// �R���X�g���N�^
//==========================================================================
CEnemyOrafu::CEnemyOrafu(int nPriority) : CEnemy(nPriority)
{
	m_Action = ACTION_CHASE;	// �s��
	m_fActTime = 0.0f;			// �s���J�E���^�[
	m_bCatchUp = false;			// �ǂ���������
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CEnemyOrafu::~CEnemyOrafu()
{

}

//==========================================================================
// ����������
//==========================================================================
HRESULT CEnemyOrafu::Init()
{
	//����������
	CEnemy::Init();

	// �s��
	m_Action = ACTION_PROXIMITY;

	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CEnemyOrafu::Uninit()
{
	// �I������
	CEnemy::Uninit();
}

//==========================================================================
// �E��
//==========================================================================
void CEnemyOrafu::Kill()
{
	// ���S����
	CEnemy::Kill();
}

//==========================================================================
// �X�V����
//==========================================================================
void CEnemyOrafu::Update()
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
// �s���ݒ�
//==========================================================================
void CEnemyOrafu::ActionSet()
{

}

//==========================================================================
// �s���X�V
//==========================================================================
void CEnemyOrafu::UpdateAction()
{
	if (!m_bActionable)
	{
		return;
	}

	// ��ԕʏ���
	(this->*(m_ActFuncList[m_Action]))();
}

//==========================================================================
// �ҋ@
//==========================================================================
void CEnemyOrafu::ActWait()
{
	// ���[�V�����擾
	CMotion* pMotion = GetMotion();
	if (pMotion == nullptr)
	{
		return;
	}

	// �ҋ@���[�V�����ݒ�
	pMotion->Set(MOTION_DEF);

	// �^�[�Q�b�g�̕�������
	RotationTarget();

	// �s���J�E���^�[���Z
	m_fActTime += CManager::GetInstance()->GetDeltaTime();

	if (TIME_WAIT <= m_fActTime)
	{// �ҋ@���Ԓ�������

		// �ߐڍU��
		m_fActTime = 0.0f;
		m_Action = ACTION_PROXIMITY;

		// �v���C���[���
		CPlayer* pPlayer = CPlayer::GetListObj().GetData(m_nTargetPlayerIndex);
		if (pPlayer != nullptr)
		{
			m_TargetPosition = pPlayer->GetPosition();
		}

		// �ǂ���������
		m_bCatchUp = UtilFunc::Collision::CircleRange3D(GetPosition(), m_TargetPosition, LENGTH_PUNCH, 0.0f);
	}
}


//==========================================================================
// �ǂ��|��
//==========================================================================
void CEnemyOrafu::ActChase()
{
	// �ړ��t���O�𗧂Ă�
	m_sMotionFrag.bMove = true;

	// �ʒu�擾
	MyLib::Vector3 pos = GetPosition();

	// �v���C���[���
	CPlayer* pPlayer = CPlayer::GetListObj().GetData(m_nTargetPlayerIndex);
	if (pPlayer == nullptr)
	{
		return;
	}
	m_TargetPosition = pPlayer->GetPosition();

	// �~�̔���
	if (UtilFunc::Collision::CircleRange3D(GetPosition(), pPlayer->GetPosition(), LENGTH_PLAYERCHASE, 0.0f))
	{

	}

	// �^�[�Q�b�g�̕�������
	RotationTarget();

	// �s���ʈړ�����
	ChaseNormal();
}

//==========================================================================
// �����ǂ��|��
//==========================================================================
void CEnemyOrafu::ChaseNormal()
{
	// ���擾
	MyLib::Vector3 move = GetMove();
	MyLib::Vector3 rot = GetRotation();
	float fMove = GetVelocity();

	// �ړ��ʐݒ�
	move.x = sinf(D3DX_PI + rot.y) * fMove * VELOCITY_WALK;
	move.z = cosf(D3DX_PI + rot.y) * fMove * VELOCITY_WALK;

	// �ړ��ʐݒ�
	SetMove(move);
}

//==========================================================================
// �ߐڍU��
//==========================================================================
void CEnemyOrafu::ActAttackProximity()
{
	if (m_bCatchUp == false)
	{// �ǂ������ĂȂ���

		// �ʒu�擾
		MyLib::Vector3 pos = GetPosition();

		// �v���C���[���
		CPlayer* pPlayer = CPlayer::GetListObj().GetData(m_nTargetPlayerIndex);
		if (pPlayer == nullptr)
		{
			return;
		}
		m_TargetPosition = pPlayer->GetPosition();

		// �^�[�Q�b�g�̕�������
		RotationTarget();

		// �ړ��t���O�𗧂Ă�
		m_sMotionFrag.bMove = true;

		// �s������ׂ̍s���ʈړ�����
		ChaseNormal();

		// �s���ʈړ�����
		float fLength = 0.0f;
		fLength = LENGTH_PUNCH;

		// �ǂ���������
		m_bCatchUp = UtilFunc::Collision::CircleRange3D(GetPosition(), m_TargetPosition, fLength, 0.0f);
	}
	else
	{// �U���̒�����

		// �U���t���O�𗧂Ă�
		m_sMotionFrag.bATK = true;

		// �s���ʈړ�����
		AttackPunch();
	}
}

//==========================================================================
// �p���`�U��
//==========================================================================
void CEnemyOrafu::AttackPunch()
{
	// ���[�V�����擾
	CMotion* pMotion = GetMotion();
	if (pMotion == nullptr)
	{
		return;
	}

	int nType = pMotion->GetType();
	if (nType == MOTION_PUNCH && pMotion->IsFinish() == true)
	{// �p���`�U�����I����Ă���

		// �ҋ@�s��
		m_Action = ACTION_WAIT;

		// �ҋ@���[�V�����ݒ�
		pMotion->Set(MOTION_DEF);

		// �ǂ������ĂȂ�����
		m_bCatchUp = false;
		return;
	}

	if (nType != MOTION_PUNCH)
	{
		// �p���`���[�V�����ݒ�
		pMotion->Set(MOTION_PUNCH);
	}

	// �U���t���O�𗧂Ă�
	m_sMotionFrag.bATK = true;
}

//==========================================================================
// �`�揈��
//==========================================================================
void CEnemyOrafu::Draw()
{
	// �`�揈��
	CEnemy::Draw();
}

//==========================================================================
// ���[�V�����Z�b�g
//==========================================================================
void CEnemyOrafu::MotionSet()
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
		else if (m_sMotionFrag.bATK == true)
		{// �U�����Ă�����

			// �U������OFF
			m_sMotionFrag.bATK = false;

			// �s���ʐݒ菈��
			pMotion->Set(MOTION_PUNCH);
		}
		//else
		//{
		//	// �j���[�g�������[�V����
		//	pMotion->Set(MOTION_DEF);
		//}
	}
}

//==========================================================================
// �U��������
//==========================================================================
void CEnemyOrafu::AttackAction(CMotion::AttackInfo ATKInfo, int nCntATK)
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
	case MOTION_PUNCH:
		break;
	}
}

//==========================================================================
// �U�����蒆����
//==========================================================================
void CEnemyOrafu::AttackInDicision(CMotion::AttackInfo* pATKInfo, int nCntATK)
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
	case MOTION_PUNCH:
		break;
	}
}
