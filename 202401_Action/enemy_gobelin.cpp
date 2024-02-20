//==========================================================================
// 
//  �S�u�����G���� [enemy_gobelin.cpp]
//  Author : ���n�Ή�
// 
//==========================================================================
#include "enemy_gobelin.h"
#include "manager.h"
#include "particle.h"
#include "player.h"

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
CEnemyGobelin::CEnemyGobelin(int nPriority) : CEnemy(nPriority)
{

}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CEnemyGobelin::~CEnemyGobelin()
{

}

//==========================================================================
// ����������
//==========================================================================
HRESULT CEnemyGobelin::Init()
{
	//����������
	CEnemy::Init();

	// �s��
	m_Action = ACTION_DEF;
	m_pAtkPattern.push_back(DEBUG_NEW CAttack_GobelinNormalAttack());	// �ʏ�U��
	m_pAtkPattern.push_back(DEBUG_NEW CEnemyStrongAttack());	// �ʏ�U��

	// ���E�E�ǂ������t���O���Z�b�g
	m_bCatchUp = false;
	m_bInSight = false;

	// �U���؂�ւ�
	ChangeATKState(m_pAtkPattern[0]);
	//m_pATKState->ChangeMotionIdx(this);

	// ���U���^�C�}�[
	m_fStrongAttackTime = TIME_STRONGATK;

	return S_OK;
}

//==========================================================================
// �I������
//==========================================================================
void CEnemyGobelin::Uninit()
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
void CEnemyGobelin::Kill()
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
void CEnemyGobelin::Update()
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
// �_�E�����
//==========================================================================
void CEnemyGobelin::StateDown()
{
	// �s���\����
	m_bActionable = false;

	if (m_fDownTime <= 0.0f)
	{
		m_state = STATE_NONE;
		// �s���\����
		m_bActionable = true;

		// �s�����I
		DrawingRandomAction();
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
void CEnemyGobelin::Draw()
{
	// �`�揈��
	CEnemy::Draw();
}

//==========================================================================
// ���[�V�����Z�b�g
//==========================================================================
void CEnemyGobelin::MotionSet()
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
void CEnemyGobelin::AttackAction(CMotion::AttackInfo ATKInfo, int nCntATK)
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
		break;

	case MOTION_ATTACK_STRONG:
		m_pWeaponHandle = CMyEffekseer::GetInstance()->SetEffect(
			CMyEffekseer::EFKLABEL_STRONGATK_SIGN,
			GetCenterPosition(), 0.0f, 0.0f, 20.0f);
		break;
	}
}

//==========================================================================
// �U�����蒆����
//==========================================================================
void CEnemyGobelin::AttackInDicision(CMotion::AttackInfo* pATKInfo, int nCntATK)
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
