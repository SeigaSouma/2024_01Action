//==========================================================================
// 
//  �{�X���� [enemy_boss.cpp]
//  Author : ���n�Ή�
// 
//==========================================================================
#include "enemy_boss.h"
#include "manager.h"
#include "debugproc.h"
#include "blackframe.h"
#include "beam.h"
#include "particle.h"
#include "hp_gauge_boss.h"
#include "camera.h"
#include "sound.h"
#include "player.h"
#include "3D_effect.h"
#include "bullet_obstacle.h"
#include "ballast.h"
#include "model.h"
#include "game.h"

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
	m_bInSight = false;			// ���E������
	m_pBossHPGauge = nullptr;	// �{�X��HP�Q�[�W

	m_pAtkPattern.push_back(DEBUG_NEW CBossSideSwipeCombo());	// ���Ȃ��R���{
	m_pAtkPattern.push_back(DEBUG_NEW CBossOverHead());			// �U�艺�낵
	m_pAtkPattern.push_back(DEBUG_NEW CBossLaunchBallast());	// ���I��΂�
	m_pAtkPattern.push_back(DEBUG_NEW CBossHandSlap());			// ��B
	m_pAtkPattern.push_back(DEBUG_NEW CBossRolling());			// ���[�����O
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
HRESULT CEnemyBoss::Init()
{
	//����������
	CEnemy::Init();

	// HP�̐ݒ�
	m_pBossHPGauge = CHP_GaugeBoss::Create(MyLib::Vector3(640.0f, 50.0f, 0.0f), GetLifeOrigin());	// �{�X��HP�Q�[�W

	// ���b�N�I���̋���
	m_fRockOnDistance = 700.0f;

	// ���t���[���J����
	//CManager::GetInstance()->GetBlackFrame()->SetState(CBlackFrame::STATE_OUT);

	// �U�����I
	//DrawingRandomAction();
	// 
	// �s���\����
	//m_bActionable = true;

	// �_�E�����A
	m_pReturnDown = DEBUG_NEW CReturnDown_Boss();


	// ���E�E�ǂ������t���O���Z�b�g
	m_bCatchUp = false;
	m_bInSight = false;

	// �U���؂�ւ�
	ChangeATKState(m_pAtkPattern[0]);
	//m_pATKState->ChangeMotionIdx(this);

	// �X�[�p�[�A�[�}�[
	m_bActiveSuperArmor = true;

	// ���Z�b�g����
	CGame::GetInstance()->ResetBeforeBoss();


	return S_OK;
}

//==========================================================================
//  �I������
//==========================================================================
void CEnemyBoss::Uninit()
{
	// �I������
	CEnemy::Uninit();

	/*for (int i = 0; i < static_cast<int>(m_pAtkPattern.size()); i++)
	{
		delete m_pAtkPattern[i];
		m_pAtkPattern[i] = nullptr;
	}*/
}

//==========================================================================
// �E��
//==========================================================================
void CEnemyBoss::Kill()
{
	if (m_pBossHPGauge != nullptr)
	{
		m_pBossHPGauge->Kill();
		m_pBossHPGauge = nullptr;
	}

	/*for (int i = 0; i < static_cast<int>(m_pAtkPattern.size()); i++)
	{
		delete m_pAtkPattern[i];
		m_pAtkPattern[i] = nullptr;
	}*/
	// ���S����
	CEnemy::Kill();
}

//==========================================================================
//  �X�V����
//==========================================================================
void CEnemyBoss::Update()
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

	// HP�X�V
	m_pBossHPGauge->SetLife(GetLife());

	if (GetMotion()->GetType() == CEnemyBoss::MOTION_ROLLING)
	{
		MyLib::Vector3 pos = UtilFunc::Transformation::WorldMtxChangeToPosition(GetModel()[2]->GetWorldMtx());

		CMyEffekseer::GetInstance()->SetPosition(m_pWeaponHandle, pos);
	}

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
	// �_���[�W�ݒ�
	m_pBossHPGauge->SetDamage(GetLife());

	// �_���[�W��Ԃɂ���
	m_state = STATE_DMG;

	// �J�ڃJ�E���^�[�ݒ�
	m_fStateTime = TIME_DMG;

	// �q�b�g�X�g�b�v
	//CManager::GetInstance()->SetEnableHitStop(5);

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
	// �_���[�W�ݒ�
	m_pBossHPGauge->SetDamage(GetLife());

	// �_�E����Ԃɂ���
	m_state = STATE_DOWN;

	// �J�ڃJ�E���^�[�ݒ�
	m_fStateTime = TIME_DMG;

	// �_�E���J�E���^�[�ݒ�
	m_fDownTime = TIME_DOWN;

	// ���ꃂ�[�V����
	GetMotion()->Set(MOTION_DOWN);

	// �q�b�g�X�g�b�v
	CManager::GetInstance()->SetEnableHitStop(5);

	// �U��
	CManager::GetInstance()->GetCamera()->SetShake(10, 15.0f, 0.0f);

	// �m�b�N�o�b�N�̈ʒu�X�V
	m_posKnokBack = GetPosition();
}

//==========================================================================
// ���X�e�b�v
//==========================================================================
bool CEnemyBoss::SmallStep()
{
	bool bEnd = false;

	// ���[�V�����擾
	CMotion* pMotion = GetMotion();
	if (pMotion == nullptr)
	{
		return bEnd;
	}

	int nType = pMotion->GetType();
	if (nType == CEnemyBoss::MOTION_BACKSTEP_SMALL && pMotion->IsFinish() == true)
	{// �X�e�b�v�I��

		// �ҋ@���[�V�����ݒ�
		pMotion->Set(CEnemyBoss::MOTION_DEF);

		// �I���t���O
		bEnd = true;
		return bEnd;
	}

	if (nType != CEnemyBoss::MOTION_BACKSTEP_SMALL)
	{
		// ���[�V�����ݒ�
		pMotion->Set(CEnemyBoss::MOTION_BACKSTEP_SMALL);
	}
	return bEnd;
}

//==========================================================================
// �`�揈��
//==========================================================================
void CEnemyBoss::Draw()
{
	// �`�揈��
	CEnemy::Draw();
}

//==========================================================================
// ���[�V�����Z�b�g
//==========================================================================
void CEnemyBoss::MotionSet()
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
			pMotion->Set(MOTION_WALK);
		}
	}
}

//==========================================================================
// �U��������
//==========================================================================
void CEnemyBoss::AttackAction(CMotion::AttackInfo ATKInfo, int nCntATK)
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
	case MOTION::MOTION_OVERHEADATK:
		if (nCntATK == 0) {
			// �U��
			CManager::GetInstance()->GetCamera()->SetShake(8, 25.0f, 0.0f);
			CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL::LABEL_SE_ENEMY_FIELD_STRONG);
			CBallast::Create(weponpos, MyLib::Vector3(5.0f, 12.0f, 5.0f), 20, 3.0f);

			CMyEffekseer::GetInstance()->SetEffect(
				CMyEffekseer::EFKLABEL::EFKLABEL_ENEMYATK_SAND,
				weponpos, 0.0f, 0.0f, 20.0f);
		}
		else{
			CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL::LABEL_SE_ENEMY_SWING_FAST);
		}
		break;

	case MOTION_SIDESWIPE:
		CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL::LABEL_SE_ENEMY_SWING_SLOW);
		break;

	case MOTION_LAUNCHBALLAST:
	{
		// �v���C���[�擾
		CListManager<CPlayer> playerList = CPlayer::GetListObj();
		CPlayer* pPlayer = playerList.GetData(0);
		if (pPlayer != nullptr)
		{
			if (weponpos.y <= 0.0f)
			{
				weponpos.y = 0.0f;
			}

			MyLib::Vector3 playerpos = pPlayer->GetPosition();

			float angle = weponpos.AngleXZ(playerpos);
			CBulletObstacle::Create(weponpos, 
				MyLib::Vector3(0.0f, angle, 0.0f), 
				D3DXVECTOR2(60.0f, 15.0f), 150.0f);
			CBallast::Create(weponpos, MyLib::Vector3(5.0f, 12.0f, 5.0f), 20, 3.0f);

			// �U��
			CManager::GetInstance()->GetCamera()->SetShake(8, 25.0f, 0.0f);
			CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL::LABEL_SE_ENEMY_FIELD_SHORT);
		}
	}
		break;

	case MOTION_HANDSLAP:
		if (nCntATK == 0 || nCntATK == 1) {
			CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL::LABEL_SE_ENEMY_SWING_SLOW);
		}
		else {
			CBallast::Create(weponpos, MyLib::Vector3(5.0f, 12.0f, 5.0f), 20, 3.0f);

			// �U��
			CManager::GetInstance()->GetCamera()->SetShake(8, 25.0f, 0.0f);
			CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL::LABEL_SE_ENEMY_FIELD_SHORT);

			CMyEffekseer::GetInstance()->SetEffect(
				CMyEffekseer::EFKLABEL::EFKLABEL_ENEMYATK_SAND,
				weponpos, 0.0f, 0.0f, 20.0f);
		}
		break;

	case MOTION_ROLLING:
		if (nCntATK == 0)
		{
			MyLib::Vector3 pos = GetPosition();
			pos.y += 150.0f;

			m_pWeaponHandle = CMyEffekseer::GetInstance()->SetEffect(
				CMyEffekseer::EFKLABEL_STRONGATK_SIGN,
				pos, 0.0f, 0.0f, 50.0f);


			CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL::LABEL_SE_STRONGATK);
		}
		else
		{
			CMyEffekseer::GetInstance()->SetEffect(
				CMyEffekseer::EFKLABEL_BOSS_ROLLING,
				weponpos,
				MyLib::Vector3(0.0f, D3DX_PI + rot.y, 0.0f), 0.0f, 80.0f, true);
		}
		break;
	}
}

//==========================================================================
// �U�����蒆����
//==========================================================================
void CEnemyBoss::AttackInDicision(CMotion::AttackInfo* pATKInfo, int nCntATK)
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
	case MOTION_SIDESWIPE:
		break;

	case MOTION_LAUNCHBALLAST:
		break;
	}
}

//==========================================================================
// �_�E�����
//==========================================================================
void CEnemyBoss::StateDown()
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
// ���S
//==========================================================================
void CEnemyBoss::StateDead()
{
	// �s���\����
	m_bActionable = false;

	// �_���[�W���󂯕t���Ȃ�
	m_bDamageReceived = false;

	SetMove(0.0f);

	// ���[�V�����擾
	CMotion* pMotion = GetMotion();
	if (pMotion == nullptr)
	{
		return;
	}

	int nType = pMotion->GetType();
	if (nType != MOTION::MOTION_KNOCKBACK)
	{
		pMotion->Set(MOTION::MOTION_KNOCKBACK);
	}

	if (nType == MOTION::MOTION_KNOCKBACK && pMotion->IsFinish() == true)
	{
		// ���̍s�����I
		m_state = STATE::STATE_FADEOUT;
		m_fStateTime = 2.0f;
		return;
	}
}

//==========================================================================
// �t�F�[�h�A�E�g
//==========================================================================
void CEnemyBoss::StateFadeOut()
{
	// �s���\����
	m_bActionable = false;

	// �_���[�W���󂯕t���Ȃ�
	m_bDamageReceived = false;

	// ���[�V�����擾
	CMotion* pMotion = GetMotion();
	if (pMotion == nullptr)
	{
		return;
	}

	m_sMotionFrag.bMove = false;	// �ړ�����OFF
	m_sMotionFrag.bATK = false;		// �U������OFF

	// ��ԑJ�ڃJ�E���^�[���Z
	m_fStateTime -= CManager::GetInstance()->GetDeltaTime();

	// �F�ݒ�
	m_mMatcol.a = m_fStateTime / 2.0f;

	if (m_fStateTime <= 0.0f)
	{
		// �G�̏I������
		Kill();
		Uninit();
		return;
	}
}

//==========================================================================
// �_�E���ҋ@���
//==========================================================================
void CEnemyBoss::StateDownWait()
{
	// �_�E���ҋ@���
	CEnemy::StateDownWait();

	// ���[�V�����擾
	CMotion* pMotion = GetMotion();
	if (pMotion == nullptr)
	{
		return;
	}

	int nType = pMotion->GetType();
	if (nType != MOTION_DOWNWAIT)
	{
		// �_�E���ҋ@���[�V�����ݒ�
		pMotion->Set(MOTION_DOWNWAIT);
	}

}

//==========================================================================
// �U���O����
//==========================================================================
void CBossSideSwipeCombo::BeforeAttack(CEnemy* enemy)
{
	CEnemyBoss* bossEnemy = dynamic_cast<CEnemyBoss*>(enemy);
	if (bossEnemy) 
	{
		m_bBeforeAttackAction = bossEnemy->SmallStep();
	}
	else 
	{
		// �U���O�s���t���O�I��
		m_bBeforeAttackAction = true;
	}
}
