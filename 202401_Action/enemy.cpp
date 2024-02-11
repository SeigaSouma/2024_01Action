//=============================================================================
// 
//  �v���C���[���� [enemy.cpp]
//  Author : ���n�Ή�
// 
//=============================================================================
#include "enemy.h"
#include "manager.h"
#include "renderer.h"
#include "game.h"
#include "tutorial.h"
#include "texture.h"
#include "input.h"
#include "calculation.h"
#include "explosion.h"
#include "particle.h"
#include "motion.h"
#include "model.h"
#include "elevation.h"
#include "player.h"
#include "camera.h"
#include "ballast.h"
#include "3D_Effect.h"
#include "hp_gauge.h"
#include "shadow.h"
#include "sound.h"
#include "impactwave.h"
#include "score.h"
#include "bullet.h"
#include "stage.h"
#include "objectX.h"
#include "collisionobject.h"
#include "limitarea.h"
#include "santabag.h"

#include "rockon_marker.h"

// �q�N���X
#include "enemy_boss.h"
#include "enemy_cookie.h"
#include "enemy_orafu.h"

//==========================================================================
// �������O���
//==========================================================================
namespace
{
	const float PLAYER_SERCH = 800.0f;	// �v���C���[�T���͈�
	const float CHACE_DISTABCE = 50.0f;	// �ǂ��|���鎞�̊Ԋu
	const float TIME_DMG = static_cast<float>(10) / static_cast<float>(mylib_const::DEFAULT_FPS);	// �_���[�W����
	const float TIME_DEAD = static_cast<float>(40) / static_cast<float>(mylib_const::DEFAULT_FPS);	// ���S����
	const float TIME_DOWN = static_cast<float>(150) / static_cast<float>(mylib_const::DEFAULT_FPS);	// �_�E������

}

//==========================================================================
// �ÓI�����o�ϐ��錾
//==========================================================================
CListManager<CEnemy> CEnemy::m_List = {};	// ���X�g


//==========================================================================
// �R���X�g���N�^
//==========================================================================
CEnemy::CEnemy(int nPriority) : CObjectChara(nPriority)
{
	// �l�̃N���A
	m_posOrigin = MyLib::Vector3(0.0f, 0.0f, 0.0f);	// �ŏ��̈ʒu
	memset(&m_sFormationInfo, NULL, sizeof(m_sFormationInfo));	// ����̏��
	m_posKnokBack = MyLib::Vector3(0.0f, 0.0f, 0.0f);	// �m�b�N�o�b�N�̈ʒu
	m_rotOrigin = MyLib::Vector3(0.0f, 0.0f, 0.0f);	// �ŏ��̌���
	m_type = TYPE_BOSS;	// ���
	m_state = STATE_NONE;	// ���
	m_Oldstate = m_state;	// �O��̏��
	m_mMatcol = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);	// �}�e���A���̐F
	m_TargetPosition = mylib_const::DEFAULT_VECTOR3;	// �ڕW�̈ʒu
	m_pWeaponHandle = 0;		// �G�t�F�N�g�̕���n���h��

	m_fStateTime = 0.0f;	// ��ԑJ�ڃJ�E���^�[
	m_nTexIdx = 0;			// �e�N�X�`���̃C���f�b�N�X�ԍ�
	m_nNumChild = 0;		// ���̐�
	m_nTargetPlayerIndex = 0;	// �ǂ��|����v���C���[�̃C���f�b�N�X�ԍ�
	m_fActCounter = 0.0f;		// �ړ��J�E���^�[
	m_bActionable = false;		// �s���\��
	m_fDownTime = 0.0f;			// �_�E���J�E���^�[
	m_fRockOnDistance = 0.0f;	// ���b�N�I���̋���
	m_bAddScore = false;		// �X�R�A���Z���邩�̔���
	m_bRockOnAccepting = false;	// ���b�N�I����t
	m_nBallastEmission = 0;	// ���I�̔����J�E���^�[
	m_sMotionFrag.bJump = false;		// �W�����v�����ǂ���
	m_sMotionFrag.bKnockback = false;	// �m�b�N�o�b�N�����ǂ���
	m_sMotionFrag.bMove = false;		// �ړ������ǂ���
	m_sMotionFrag.bATK = false;			// �U�������ǂ���
	m_pParent = nullptr;					// �e�̃|�C���^
	m_pHPGauge = nullptr;					// HP�Q�[�W�̏��
	m_pShadow = nullptr;
	m_pRockOnMarker = nullptr;		// ���b�N�I���}�[�J�[

	memset(&m_pChild[0], NULL, sizeof(m_pChild));	// �q�̃|�C���^
}

//==========================================================================
// �f�X�g���N�^
//==========================================================================
CEnemy::~CEnemy()
{

}

//==========================================================================
// ��������
//==========================================================================
CEnemy *CEnemy::Create(const char *pFileName, MyLib::Vector3 pos, TYPE type)
{
	// �����p�̃I�u�W�F�N�g
	CEnemy *pEnemy = nullptr;

	if (pEnemy == nullptr)
	{// nullptr��������

		// �������̊m��
		switch (type)
		{
		case TYPE_BOSS:
			pEnemy = DEBUG_NEW CEnemyBoss;
			break;

		case TYPE_COOKIE:
			pEnemy = DEBUG_NEW CEnemyCookie;
			break;

		case TYPE_ORAFU:
			pEnemy = DEBUG_NEW CEnemyOrafu;
			break;

		default:
			return nullptr;
			break;
		}

		if (pEnemy != nullptr)
		{// �������̊m�ۂ��o���Ă�����

			// ���
			pEnemy->m_type = type;

			// �ŏ��̈ʒu�ݒ�
			pEnemy->m_posOrigin = pos;

			// �ʒu�ݒ�
			pEnemy->SetPosition(pos);

			// �e�L�X�g�ǂݍ���
			HRESULT hr = pEnemy->RoadText(pFileName);
			if (FAILED(hr))
			{// ���s���Ă�����
				return nullptr;
			}

			// ����������
			pEnemy->Init();
		}

		return pEnemy;
	}

	return nullptr;
}

//==========================================================================
// ����������
//==========================================================================
HRESULT CEnemy::Init()
{
	// �e��ϐ��̏�����
	m_state = STATE_NONE;	// ���
	m_Oldstate = STATE_PLAYERCHASE;
	m_fStateTime = 0.0f;			// ��ԑJ�ڃJ�E���^�[
	m_posKnokBack = m_posOrigin;	// �m�b�N�o�b�N�̈ʒu
	m_bAddScore = true;	// �X�R�A���Z���邩�̔���
	m_fRockOnDistance = 0.0f;	// ���b�N�I���̋���

	// ��ނ̐ݒ�
	SetType(TYPE_ENEMY);

	// �����ݒ�
	SetRotation(MyLib::Vector3(0.0f, 0.0f, 0.0f));

	// ���X�g�ɒǉ�
	m_List.Regist(this);

	return S_OK;
}

//==========================================================================
// �e�L�X�g�ǂݍ���
//==========================================================================
HRESULT CEnemy::RoadText(const char *pFileName)
{
	// �L�����쐬
	HRESULT hr = SetCharacter(pFileName);
	if (FAILED(hr))
	{// ���s���Ă�����
		return E_FAIL;
	}

	return S_OK;
}

//==========================================================================
// �q�o�^
//==========================================================================
void CEnemy::RegistrChild(CEnemy *pChild)
{
	if (m_nNumChild >= mylib_const::MAX_ENEMY)
	{
		return;
	}

	if (m_pChild[m_nNumChild] == nullptr)
	{
		m_pChild[m_nNumChild] = pChild;
		m_nNumChild++;	// �q�̐����Z
	}
	else
	{
		int n = 0;
	}
}

//==========================================================================
// �q�̃��Z�b�g
//==========================================================================
void CEnemy::ResetChild(CEnemy *pChild)
{
	int nMax = m_nNumChild;

	for (int nCntChild = 0; nCntChild < nMax; nCntChild++)
	{
		if (m_pChild[nCntChild] == nullptr)
		{
			continue;
		}

		if (m_pChild[nCntChild] == pChild)
		{
			m_pChild[nCntChild] = nullptr;
		}
	}
}


//==========================================================================
// �e�̃|�C���^�ݒ�
//==========================================================================
void CEnemy::SetParent(CEnemy *pParent)
{
	if (this != nullptr)
	{
		// �q�̊��蓖��
		if (pParent != nullptr)
		{
			pParent->RegistrChild(this);
		}

		// �e�̃|�C���^�n��
		m_pParent = pParent;
		
	}
	else
	{
		int n = 0;
	}
}

//==========================================================================
// �I������
//==========================================================================
void CEnemy::Uninit()
{
	
	// �e������
	if (m_pShadow != nullptr)
	{
		m_pShadow = nullptr;
	}

	if (m_pRockOnMarker != nullptr)
	{
		m_pRockOnMarker = nullptr;
	}

	// ���X�g����폜
	m_List.Delete(this);

	// �I������
	CObjectChara::Uninit();
}

//==========================================================================
// ���S����
//==========================================================================
void CEnemy::Kill()
{
	for (int nCntEnemy = 0; nCntEnemy < mylib_const::MAX_ENEMY; nCntEnemy++)
	{// �q�̐�����

		if (m_pChild[nCntEnemy] != nullptr)
		{// �q������ꍇ

			// �����̎q�̐e��nullptr�ɂ���
			m_pChild[nCntEnemy]->m_pParent = nullptr;
		}
	}

	if (m_pParent != nullptr)
	{// �����ɐe������ꍇ

		// �e�̎q��nullptr�ɂ���
		m_pParent->ResetChild(this);
	}

	// HP�Q�[�W������
	if (m_pHPGauge != nullptr)
	{
		m_pHPGauge->Uninit();
		m_pHPGauge = nullptr;
	}

	// �e������
	if (m_pShadow != nullptr)
	{
		m_pShadow->Uninit();
		m_pShadow = nullptr;
	}

	// ���b�N�I���}�[�J�[
	if (m_pRockOnMarker != nullptr)
	{
		m_pRockOnMarker->Kill();
		m_pRockOnMarker = nullptr;
	}

	// ���b�N�I����t���Ă���
	if (m_bRockOnAccepting)
	{
		/*CPlayer* pPlayer = CPlayer::GetListObj().GetData(0);
		if (pPlayer != nullptr)
		{
			pPlayer->SwitchRockOnTarget();
		}*/
		CManager::GetInstance()->GetCamera()->SetRockOn(GetPosition(), false);
	}

}

//==========================================================================
// �X�V����
//==========================================================================
void CEnemy::Update()
{
	// ���S�̔���
	if (IsDeath() == true)
	{// ���S�t���O�������Ă�����
		return;
	}

	// �G�f�B�b�g���͔�����
	if (CGame::GetInstance()->GetElevation()->IsEdit())
	{
		return;
	}

	if (!CGame::GetInstance()->GetGameManager()->IsControll())
	{// �s���ł���Ƃ�
		return;
	}

	// �e�̏���
	CObjectChara::Update();

	// �ߋ��̈ʒu�ݒ�
	SetOldPosition(GetPosition());

	// �����蔻��
	Collision();

	// ���S�̔���
	if (IsDeath() == true)
	{// ���S�t���O�������Ă�����
		return;
	}

	// ���[�V�����̐ݒ菈��
	MotionSet();

	// ��ԍX�V
	UpdateState();

	// ���S�̔���
	if (IsDeath() == true)
	{// ���S�t���O�������Ă�����
		return;
	}

	if (m_state != STATE_SPAWN &&
		m_state != STATE_SPAWNWAIT)
	{
		// �s���̐ݒ�
		ActionSet();
	}

	// �s���X�V
	UpdateAction();

	// ��ޕʏ���
	UpdateByType();

	// HP�Q�[�W�̈ʒu�X�V
	if (m_pHPGauge != nullptr)
	{
		// �ʒu�擾
		MyLib::Vector3 pos = GetPosition();

		// �̗͎擾
		int nLife = GetLife();

		m_pHPGauge->UpdatePosition(pos, nLife);
	}

	// �e�̈ʒu�X�V
	if (m_pShadow != nullptr)
	{
		// �ʒu�擾
		MyLib::Vector3 pos = GetPosition();
		m_pShadow->SetPosition(MyLib::Vector3(pos.x, m_pShadow->GetPosition().y, pos.z));
	}

	if (GetPosition().y <= mylib_const::KILL_Y)
	{
		// �G�̏I������
		Kill();
		Uninit();
		return;
	}

	// �o�b�O�̃��X�g�擾
	CListManager<CSantaBag> BagList = CSantaBag::GetListObj();
	CSantaBag* pBag = nullptr;

	// ���X�g���[�v
	while (BagList.ListLoop(&pBag))
	{
		m_TargetPosition = pBag->GetPosition();
	}

	// ��l�̕�
	LimitArea();

	// ���b�N�I����t���Ă���
	if (m_bRockOnAccepting)
	{
		MyLib::Vector3 rockonpos = GetPosition();
		rockonpos.y += GetHeight() * 0.5f;
		CManager::GetInstance()->GetCamera()->SetRockOn(rockonpos, true);
		CManager::GetInstance()->GetCamera()->SetRockOnDistance(m_fRockOnDistance);

		// ���b�N�I���}�[�J�[�̈ʒu�ݒ�
		if (m_pRockOnMarker == nullptr)
		{
			m_pRockOnMarker = CRockOnMarker::Create(rockonpos);
		}
		m_pRockOnMarker->SetPosition(rockonpos);
	}
	else
	{
		if (m_pRockOnMarker != nullptr)
		{
			m_pRockOnMarker->Kill();
			m_pRockOnMarker = nullptr;
		}
	}
}

//==========================================================================
// �����蔻��
//==========================================================================
void CEnemy::Collision()
{
	// �ʒu�擾
	MyLib::Vector3 pos = GetPosition();

	// �ړ��ʎ擾
	MyLib::Vector3 move = GetMove();

	// �����擾
	MyLib::Vector3 rot = GetRotation();

	// �d�͏���
	move.y -= mylib_const::GRAVITY;

	// �ʒu�X�V
	pos += move;

	// �����␳
	if (m_state != STATE_SPAWN && m_state != STATE_DMG && m_state != STATE_DEAD)
	{
		move.x += (0.0f - move.x) * 0.25f;
		move.z += (0.0f - move.z) * 0.25f;
	}

	if (move.x >= 0.1f || move.x <= -0.1f ||
		move.z >= 0.1f || move.z <= -0.1f)
	{// �ړ���
		m_sMotionFrag.bMove = true;
	}
	else
	{
		m_sMotionFrag.bMove = false;
	}

	// �����擾
	bool bLand = false;
	float fHeight = CGame::GetInstance()->GetElevation()->GetHeight(pos, &bLand);

	if (fHeight > pos.y)
	{// �n�ʂ̕���������荂��������

		// �n�ʂ̍����ɕ␳
		if (m_state != STATE_DEAD && m_state != STATE_FADEOUT)
		{
			pos.y = fHeight;
		}

		if (bLand == true)
		{// ���n���Ă���

			// ���n������
			ProcessLanding();

			// �W�����v�g�p�\�ɂ���
			if (m_state != STATE_DMG && m_state != STATE_DEAD)
			{
				move.y = 0.0f;
			}
			m_sMotionFrag.bJump = false;
		}
	}


	if (pos.y <= -800.0f)
	{
		pos.y = -800.0f;
	}

	// �ʒu�ݒ�
	SetPosition(pos);

	// �ړ��ʐݒ�
	SetMove(move);
}

//==========================================================================
// ���n���̏���
//==========================================================================
void CEnemy::ProcessLanding()
{
	// �ړ��ʎ擾
	MyLib::Vector3 move = GetMove();

	// �W�����v�g�p�\�ɂ���
	if (m_state != STATE_DMG && m_state != STATE_DEAD)
	{
		move.y = 0.0f;
	}
	m_sMotionFrag.bJump = false;

	// �ړ��ʐݒ�
	SetMove(move);
}

//==========================================================================
// �q�b�g����
//==========================================================================
bool CEnemy::Hit(const int nValue, CGameManager::AttackType atkType)
{
	bool bHit = false;

	// �����擾
	MyLib::Vector3 rot = GetRotation();
	MyLib::Vector3 pos = GetPosition();

	// �̗͎擾
	int nLife = GetLife();

	if (m_state != STATE_DMG && m_state != STATE_DEAD && m_state != STATE_SPAWN && m_state != STATE_FADEOUT)
	{// �Ȃɂ��Ȃ���Ԃ̎�

		// ��������
		bHit = true;

		// �̗͌��炷
		nLife -= nValue;
		UtilFunc::Transformation::ValueNormalize(nLife, GetLifeOrigin(), 0);

		// �̗͐ݒ�
		SetLife(nLife);

		if (nLife > 0)
		{
			// �_���[�W���Đ�
			CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL_SE_DMG01);
		}
		else
		{// �̗͂��Ȃ��Ȃ�����

			// �q�b�g�X�g�b�v
			CManager::GetInstance()->SetEnableHitStop(2);

			// �U��
			CManager::GetInstance()->GetCamera()->SetShake(5, 10.0f, 0.0f);

			// ���S��Ԃɂ���
			m_state = STATE_DEAD;

			// �J�ڃJ�E���^�[�ݒ�
			m_fStateTime = TIME_DEAD;

			// �m�b�N�o�b�N�̈ʒu�X�V
			m_posKnokBack = GetPosition();

			// �m�b�N�o�b�N����ɂ���
			m_sMotionFrag.bKnockback = true;

			// ���ꃂ�[�V����
			//pMotion->Set(MOTION_KNOCKBACK);

			// �����Đ�
			CManager::GetInstance()->GetSound()->PlaySound(CSound::LABEL_SE_ENEMYEXPLOSION);

			/*MyLib::Vector3 move = GetMove();
			move.x = UtilFunc::Transformation::Random(-5, 5) + 20.0f;
			move.y = UtilFunc::Transformation::Random(0, 5) + 15.0f;
			move.z = UtilFunc::Transformation::Random(-5, 5) + 20.0f;
			SetMove(move);*/
			return true;
		}

		// �ߋ��̏�ԕۑ�
		m_Oldstate = m_state;

		switch (atkType)
		{
		case CGameManager::ATTACK_NORMAL:
			NormalHitResponse();
			break;

		case CGameManager::ATTACK_STRONG:
			break;

		case CGameManager::ATTACK_COUNTER:
			CounterHitResponse();
			break;

		default:
			break;
		}
	}

	return bHit;
}

//==========================================================================
// �ʏ�q�b�g���̔���
//==========================================================================
void CEnemy::NormalHitResponse()
{
	if (m_pHPGauge == nullptr)
	{
		// �̗̓Q�[�W
		m_pHPGauge = CHP_Gauge::Create(GetHeight(), GetLifeOrigin(), 3.0f);
	}

	// �_���[�W��Ԃɂ���
	m_state = STATE_DMG;

	// �J�ڃJ�E���^�[�ݒ�
	m_fStateTime = TIME_DMG;

	// ���ꃂ�[�V����
	GetMotion()->Set(MOTION_DMG);

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
void CEnemy::CounterHitResponse()
{
	if (m_pHPGauge == nullptr)
	{
		// �̗̓Q�[�W
		m_pHPGauge = CHP_Gauge::Create(GetHeight(), GetLifeOrigin(), 3.0f);
	}

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
// ��ޕʍX�V����
//==========================================================================
void CEnemy::UpdateByType()
{

}

//==========================================================================
// �s���X�V
//==========================================================================
void CEnemy::UpdateAction()
{

}

//==========================================================================
// �^�[�Q�b�g�̕�������
//==========================================================================
void CEnemy::RotationTarget()
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
// �v���C���[�Ƃ̋����𔻒�
//==========================================================================
bool CEnemy::CalcLenPlayer(float fLen)
{
	// �v���C���[���
	CPlayer* pPlayer = CPlayer::GetListObj().GetData(m_nTargetPlayerIndex);
	if (pPlayer == nullptr)
	{
		return false;
	}

	return UtilFunc::Collision::CircleRange3D(GetPosition(), pPlayer->GetPosition(), fLen, pPlayer->GetRadius());
}

//==========================================================================
// �ړ���������������
//==========================================================================
void CEnemy::MoveRotation()
{
	// �K�v�Ȓl���擾
	MyLib::Vector3 rot = GetRotation();
	MyLib::Vector3 move = GetMove();

	// �������Z�o
	float fRot = atan2f(-move.x, -move.z);

	// �p�x�̐��K��
	UtilFunc::Transformation::RotNormalize(fRot);

	// �p�x�̕␳������
	rot.y += (fRot - rot.y) * 0.025f;

	// �����ݒ�
	SetRotation(rot);
}

//==========================================================================
// �ړ�
//==========================================================================
void CEnemy::Move()
{
	// �ړ��t���O�𗧂Ă�
	m_sMotionFrag.bMove = true;

	// �ړ����x�擾
	float fMove = GetVelocity();

	// �ړ��ʂ�K�p
	MyLib::Vector3 move = GetMove();
	move.x = sinf(m_fActCounter) * fMove;
	move.z = cosf(m_fActCounter) * fMove;
	SetMove(move);

	// �����]��
	MoveRotation();
}

//==========================================================================
// ��ԍX�V����
//==========================================================================
void CEnemy::UpdateState()
{
	// �ʒu�擾
	MyLib::Vector3 pos = GetPosition();
	MyLib::Vector3 pos11 = MyLib::Vector3(GetObjectChara()->GetModel()[0]->GetWorldMtx()._41, GetObjectChara()->GetModel()[0]->GetWorldMtx()._42, GetObjectChara()->GetModel()[0]->GetWorldMtx()._43);

	// �ړ��ʎ擾
	MyLib::Vector3 move = GetMove();

	// �F�ݒ�
	m_mMatcol = D3DXCOLOR(1.0f, 1.0f, 1.0f, m_mMatcol.a);

	// �_�E���J�E���^�[���Z
	m_fDownTime -= CManager::GetInstance()->GetDeltaTime();
	UtilFunc::Transformation::ValueNormalize(m_fDownTime, TIME_DOWN, 0.0f);

	switch (m_state)
	{
	case STATE_NONE:
		StateNone();
		break;

	case STATE_SPAWNWAIT:	// �X�|�[���ҋ@
		SpawnWait();
		break;

	case STATE_SPAWN:
		Spawn();
		break;

	case STATE_DMG:
		Damage();
		break;

	case STATE_DEAD:
		Dead();
		break;

	case STATE_FADEOUT:
		FadeOut();
		break;

	case STATE_PLAYERCHASE:
		PlayerChase();
		break;

	case STATE_PARENTCHASE:
		ParentChase();
		break;

	case STATE_ATTACK:
		StateAttack();
		break;

	case STATE_WAIT:
		StateWait();
		break;

	case STATE_DOWN:
		StateDown();
		break;
	}

	if (m_state != STATE_SPAWNWAIT && IsDisp() == false)
	{
		// �`�悷��
		SetEnableDisp(true);
	}
}

//==========================================================================
// ��ޕʏ�ԍX�V����
//==========================================================================
void CEnemy::UpdateStateByType()
{
	return;
}

//==========================================================================
// �����Ȃ����
//==========================================================================
void CEnemy::StateNone()
{
	// �s���\����
	m_bActionable = true;

	// �F�ݒ�
	m_mMatcol = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	// ��ԑJ�ڃJ�E���^�[���Z
	m_fStateTime -= CManager::GetInstance()->GetDeltaTime();

	if (m_fStateTime <= 0)
	{// �J�ڃJ�E���^�[��0�ɂȂ�����
		m_fStateTime = 0;
	}
}

//==========================================================================
// �X�|�[���ҋ@
//==========================================================================
void CEnemy::SpawnWait()
{
	// �s���\����
	m_bActionable = false;

	// ��ԃJ�E���^�[���Z�b�g
	m_fStateTime = 0.0f;

	// �`�悵�Ȃ�
	SetEnableDisp(false);
}

//==========================================================================
// �o��
//==========================================================================
void CEnemy::Spawn()
{
	// �s���\����
	m_bActionable = false;

}

//==========================================================================
// �_���[�W
//==========================================================================
void CEnemy::Damage()
{
	// �s���\����
	m_bActionable = false;

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

#if _DEBUG
	// �F�ݒ�
	m_mMatcol = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
#endif

	// ��ԑJ�ڃJ�E���^�[���Z
	m_fStateTime -= CManager::GetInstance()->GetDeltaTime();

	if (m_fStateTime <= 0.0f)
	{
		// �ߋ��̏�Ԃɂ���
		m_state = m_Oldstate;
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
// ���S
//==========================================================================
void CEnemy::Dead()
{
	// �s���\����
	m_bActionable = false;

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
	m_fStateTime -= CManager::GetInstance()->GetDeltaTime();

	// �F�ݒ�
	m_mMatcol = D3DXCOLOR(1.0f, 1.0f, 1.0f, m_mMatcol.a);
	m_mMatcol.a -= 1.0f / 80.0f;

	// �d�͂ŗ���
	move.y += -mylib_const::GRAVITY * 0.25f;
	pos.y += move.y;

	// ��]
	rot.y += D3DX_PI * 0.025f;
	rot.x += D3DX_PI * (UtilFunc::Transformation::Random(5, 25) * 0.001f);

	if(CGame::GetInstance()->GetElevation()->IsHit(pos))
	{
		// �p�[�e�B�N������
		my_particle::Create(pos, my_particle::TYPE_ENEMY_FADE);

		// �G�̏I������
		m_state = STATE_FADEOUT;
		//Uninit();
		return;
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
// �t�F�[�h�A�E�g
//==========================================================================
void CEnemy::FadeOut()
{
	// �s���\����
	m_bActionable = false;

	// �ړ��ʎ擾
	float fMove = GetVelocity();

	// �ʒu�擾
	MyLib::Vector3 pos = GetPosition();

	// ���[�V�����擾
	CMotion* pMotion = GetMotion();
	if (pMotion == nullptr)
	{
		return;
	}

	// �t�F�[�h�A�E�g���[�V�����ݒ�
	pMotion->Set(MOTION_FADEOUT);

	// �t�F�[�h�A�E�g�̃t���[����
	int nAllFrame = pMotion->GetMaxAllCount(MOTION_FADEOUT);
	float fFrame = pMotion->GetFrameCount();

	// ���[�V�����̏��擾
	CMotion::Info aInfo = pMotion->GetInfo(pMotion->GetType());

	// �U�����̑����擾
	int nNumAttackInfo = aInfo.nNumAttackInfo;

	// �����擾
	bool bLand = false;
	float fHeight = CGame::GetInstance()->GetElevation()->GetHeight(pos, &bLand);

	m_sMotionFrag.bMove = false;	// �ړ�����OFF
	m_sMotionFrag.bATK = false;		// �U������OFF

	// �J�ڃJ�E���^�[���Z
	m_fStateTime++;

	// �F�ݒ�
	m_mMatcol.a = 1.0f - ((float)m_fStateTime / (float)nAllFrame);

	if (m_fStateTime >= nAllFrame)
	{// �J�ڃJ�E���^�[�����[�V�����𒴂�����

		// �X�R�A���Z�̔���I�t
		m_bAddScore = false;

		// �G�̏I������
		Kill();
		Uninit();
		return;
	}
}

//==========================================================================
// �v���C���[�Ǐ]
//==========================================================================
void CEnemy::PlayerChase()
{
	// �s���\����
	m_bActionable = true;

	// �ʒu�擾
	MyLib::Vector3 pos = GetPosition();

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

	// �v���C���[���
	CPlayer *pPlayer = CPlayer::GetListObj().GetData(m_nTargetPlayerIndex);

	// �I�u�W�F�N�g���
	CObject *pObj = nullptr;


	// ��ԑJ�ڃJ�E���^�[���Z
	m_fStateTime -= CManager::GetInstance()->GetDeltaTime();

	if (m_fStateTime <= 0)
	{// �J�ڃJ�E���^�[��0�ɂȂ�����
		m_fStateTime = 0;
	}

	if (pPlayer != nullptr)
	{// nullptr����Ȃ��Ƃ�

		// �e�̈ʒu�擾
		MyLib::Vector3 posPlayer = pPlayer->GetPosition();
		CObject *pMyObj = GetObject();

		// �ڕW�̊p�x�����߂�
		fRotDest = atan2f((pos.x - posPlayer.x), (pos.z - posPlayer.z));

		// �ڕW�Ƃ̍���
		fRotDiff = fRotDest - rot.y;

		//�p�x�̐��K��
		UtilFunc::Transformation::RotNormalize(fRotDiff);

		//�p�x�̕␳������
		rot.y += fRotDiff * 0.025f;

		// �p�x�̐��K��
		UtilFunc::Transformation::RotNormalize(rot.y);

		// �����ݒ�
		SetRotation(rot);

		// �U����Ԉڍs����
		ChangeToAttackState();

		// �����Ă�����Ƀ_�b�V��
		if (bLen == false)
		{// �������ۂ���Ă�����

			// �ǂ��|���ړ�����
			ChaseMove(fMove);
		}

		if (UtilFunc::Collision::CircleRange3D(pos, pPlayer->GetPosition(), 200.0f, PLAYER_SERCH) == false)
		{// �v���C���[�����E�����������
			m_state = STATE_NONE;
		}
	}

	// �ʒu�ݒ�
	SetPosition(pos);

	// �ڕW�̌����ݒ�
	SetRotDest(fRotDest);
}

//==========================================================================
// �e�Ǐ]
//==========================================================================
void CEnemy::ParentChase()
{
	// �s���\����
	m_bActionable = true;

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

	// ���Ȃ藣��Ă邩�̔���
	bool bLongDistance = true;

	// �I�u�W�F�N�g���
	CObject *pObj = nullptr;


	// ��ԑJ�ڃJ�E���^�[���Z
	//m_fStateTime++;

	if (m_fStateTime <= 0)
	{// �J�ڃJ�E���^�[��0�ɂȂ�����
		m_fStateTime = 0;
	}

	if (m_pParent != nullptr)
	{// �e������ꍇ

		// �e�̈ړ��ʎ擾
		MyLib::Vector3 moveParent = m_pParent->GetMove();

		// �e�̈ʒu�擾
		MyLib::Vector3 posParent = m_pParent->GetPosition();

		// �e�̌����擾
		MyLib::Vector3 rotParent = m_pParent->GetRotation();

		// �ڕW�̈ʒu
		MyLib::Vector3 posDest;
		posDest = posParent;

		// �ڕW�̊p�x�����߂�
		fRotDest = atan2f((pos.x - posDest.x), (pos.z - posDest.z));

		// �ڕW�Ƃ̍���
		fRotDiff = fRotDest - rot.y;

		//�p�x�̐��K��
		UtilFunc::Transformation::RotNormalize(fRotDiff);

		//�p�x�̕␳������
		rot.y += fRotDiff * 0.025f;

		// �p�x�̐��K��
		UtilFunc::Transformation::RotNormalize(rot.y);

		// �����ݒ�
		SetRotation(rot);

		if (UtilFunc::Collision::CircleRange3D(pos, posDest, 25.0f, CHACE_DISTABCE) == true)
			{// ��苗���Ԃɐe����������
				bLen = true;	// ��������
			}

			// �����Ă�����Ƀ_�b�V��
			if (bLen == false)
			{// �������ۂ���Ă�����

				// �ǂ��|���ړ�����
				ChaseMove(fMove * 1.4f);
			}

	}
	else
	{// �������g���e�̎�

		float fRotDiff = 0.0f;	// ���݂ƖڕW�̍���

		// ��ԑJ�ڃJ�E���^�[�X�V
		m_fStateTime = static_cast<float>((static_cast<int>(m_fStateTime) + 1) % 120);

		// �ڕW�̊p�x�����߂�
		int stateTime = static_cast<int>(m_fStateTime);
		if (stateTime == 0)
		{
			fRotDest = UtilFunc::Transformation::Random(-31, 31) * 0.1f;
		}

		// �ڕW�Ƃ̍���
		fRotDiff = fRotDest - rot.y;

		//�p�x�̐��K��
		UtilFunc::Transformation::RotNormalize(fRotDiff);

		//�p�x�̕␳������
		rot.y += fRotDiff * 0.025f;

		// �p�x�̐��K��
		UtilFunc::Transformation::RotNormalize(rot.y);

		// �����ݒ�
		SetRotation(rot);

		// �ǂ��|���ړ�����
		ChaseMove(fMove);

#if _DEBUG
		// �F�ݒ�
		m_mMatcol = D3DXCOLOR(1.0f, 0.5f, 1.0f, 1.0f);
#endif
	}

	// �v���C���[�Ǐ]�̔���
	TriggerChasePlayer();

	// �ڕW�̌����ݒ�
	SetRotDest(fRotDest);
}

//==========================================================================
// �U������
//==========================================================================
void CEnemy::StateAttack()
{
	// �s���\����
	m_bActionable = true;

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

	// �I�u�W�F�N�g���
	CObject *pObj = nullptr;

	// ���[�V�����擾
	CMotion* pMotion = GetMotion();
	if (pMotion == nullptr)
	{
		return;
	}
	int nType = pMotion->GetType();

	if (m_pParent != nullptr)
	{// �e������ꍇ

		// �e���ǂ��|����Ԃɂ���
		if (m_pParent->m_state != STATE_ATTACK)
		{
			if (m_pParent->m_state != STATE_DMG && m_pParent->m_state != STATE_DEAD)
			{
				m_pParent->m_state = STATE_PLAYERCHASE;
				m_pParent->m_fStateTime = 60;
			}

			for (int nCntEnemy = 0; nCntEnemy < m_pParent->m_nNumChild; nCntEnemy++)
			{// �q�̐�����
				if (m_pParent->m_pChild[nCntEnemy] == nullptr)
				{
					continue;
				}

				if (m_pParent->m_pChild[nCntEnemy]->m_state == STATE_DMG || m_pParent->m_pChild[nCntEnemy]->m_state == STATE_DEAD)
				{
					continue;
				}

				m_pParent->m_pChild[nCntEnemy]->m_state = STATE_PLAYERCHASE;
				m_pParent->m_pChild[nCntEnemy]->m_fStateTime = 60;
			}
		}
	}
	else
	{// �������e�ȏꍇ

		for (int nCntEnemy = 0; nCntEnemy < m_nNumChild; nCntEnemy++)
		{// �q�̐�����

			if (m_pChild[nCntEnemy] == nullptr)
			{// nullptr��������
				continue;
			}

			if (m_pChild[nCntEnemy]->m_state != STATE_ATTACK)
			{// �U����Ԃ̎�

				if (m_pChild[nCntEnemy]->m_state != STATE_DMG && m_pChild[nCntEnemy]->m_state != STATE_DEAD)
				{
					m_pChild[nCntEnemy]->m_state = STATE_PLAYERCHASE;
					m_pChild[nCntEnemy]->m_fStateTime = 60;
				}
			}
		}
	}

	// �v���C���[�擾
	CListManager<CPlayer> playerList = CPlayer::GetListObj();
	CPlayer* pPlayer = nullptr;

	// ���X�g���[�v
	while (playerList.ListLoop(&pPlayer))
	{
		if (nType == 0 && pPlayer != nullptr)
		{// �j���[�g�����ɖ߂��

			if (UtilFunc::Collision::CircleRange3D(pos, pPlayer->GetPosition(), 400.0f, PLAYER_SERCH) == false)
			{// �v���C���[�Ɨ��ꂷ���Ă�����

				// �Ԋu���������Ԃɂ���
				m_state = STATE_NONE;
				break;
			}
			else
			{// �܂��ǂ��鎞

				m_state = STATE_PLAYERCHASE;

				if (m_pParent != nullptr)
				{// �e������ꍇ

					// �e���ǂ��|����Ԃɂ���
					if (m_pParent->m_state != STATE_DMG && m_pParent->m_state != STATE_DEAD)
					{
						m_pParent->m_state = STATE_PLAYERCHASE;
					}

					for (int nCntEnemy = 0; nCntEnemy < m_pParent->m_nNumChild; nCntEnemy++)
					{// �q�̐�����

						if (m_pParent->m_pChild[nCntEnemy] == nullptr)
						{
							continue;
						}

						if (m_pParent->m_pChild[nCntEnemy]->m_state == STATE_DMG || m_pParent->m_pChild[nCntEnemy]->m_state == STATE_DEAD)
						{
							continue;
						}

						m_pParent->m_pChild[nCntEnemy]->m_state = STATE_PLAYERCHASE;
					}
				}
				else
				{// �������e�ȏꍇ

					for (int nCntEnemy = 0; nCntEnemy < m_nNumChild; nCntEnemy++)
					{// �q�̐�����

						if (m_pChild[nCntEnemy] == nullptr)
						{
							continue;
						}

						if (m_pChild[nCntEnemy]->m_state == STATE_DMG || m_pChild[nCntEnemy]->m_state == STATE_DEAD)
						{
							continue;
						}

						m_pChild[nCntEnemy]->m_state = STATE_PLAYERCHASE;
					}
				}
			}
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
// �ҋ@���
//==========================================================================
void CEnemy::StateWait()
{
	// �s���\����
	m_bActionable = true;

	return;
}

//==========================================================================
// �_�E�����
//==========================================================================
void CEnemy::StateDown()
{
	// �s���\����
	m_bActionable = false;

	// �F�ݒ�
	m_mMatcol = D3DXCOLOR(1.0f, 0.5f, 0.1f, 1.0f);

	if (m_fDownTime <= 0.0f)
	{
		m_state = STATE_NONE;
		// �s���\����
		m_bActionable = true;
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
// �v���C���[�Ǐ]ON�ɂ���g���K�[
//==========================================================================
void CEnemy::TriggerChasePlayer()
{



	// �ʒu�擾
	MyLib::Vector3 pos = GetPosition();

	// �v���C���[�擾
	CListManager<CPlayer> playerList = CPlayer::GetListObj();
	CPlayer* pPlayer = nullptr;

	// ���X�g���[�v
	int i = 0;
	while (playerList.ListLoop(&pPlayer))
	{
		float fRadius = PLAYER_SERCH;

		if (UtilFunc::Collision::CircleRange3D(pos, pPlayer->GetPosition(), 200.0f, fRadius) == true)
		{// �v���C���[���͈͂ɓ����
			m_state = STATE_PLAYERCHASE;

			if (m_pParent != nullptr)
			{// �e������ꍇ

				// �e���ǂ��|����Ԃɂ���
				if (m_pParent->m_state != STATE_DMG && m_pParent->m_state != STATE_DEAD)
				{
					m_pParent->m_state = STATE_PLAYERCHASE;
				}

				for (int nCntEnemy = 0; nCntEnemy < m_pParent->m_nNumChild; nCntEnemy++)
				{// �q�̐�����

					if (m_pParent->m_pChild[nCntEnemy] == nullptr)
					{
						continue;
					}

					if (m_pParent->m_pChild[nCntEnemy]->m_state == STATE_DMG || m_pParent->m_pChild[nCntEnemy]->m_state == STATE_DEAD)
					{
						continue;
					}

					// �����̐e�̎q���v���C���[�ǂ��|�����
					m_pParent->m_pChild[nCntEnemy]->m_state = STATE_PLAYERCHASE;
				}
			}
			else
			{// �������e�ȏꍇ

				for (int nCntEnemy = 0; nCntEnemy < m_nNumChild; nCntEnemy++)
				{// �q�̐�����

					if (m_pChild[nCntEnemy] == nullptr)
					{
						continue;
					}

					if (m_pChild[nCntEnemy]->m_state == STATE_DMG || m_pChild[nCntEnemy]->m_state == STATE_DEAD)
					{
						continue;
					}

					m_pChild[nCntEnemy]->m_state = STATE_PLAYERCHASE;
				}
			}

			// �ǂ��|����v���C���[�̔ԍ��ݒ�
			m_nTargetPlayerIndex = i;
			break;
		}

		i++;	// �C���f�b�N�X���Z
	}

}

//==========================================================================
// �U����Ԉڍs����
//==========================================================================
void CEnemy::ChangeToAttackState()
{
	// �ʒu�擾
	MyLib::Vector3 pos = GetPosition();

	// �v���C���[�擾
	CListManager<CPlayer> playerList = CPlayer::GetListObj();
	CPlayer* pPlayer = nullptr;

	// ���X�g���[�v
	int i = 0;
	while (playerList.ListLoop(&pPlayer))
	{
		// �e�̈ʒu�擾
		MyLib::Vector3 posPlayer = pPlayer->GetPosition();

		if (UtilFunc::Collision::CircleRange3D(pos, posPlayer, 400.0f, pPlayer->GetRadius()) == true && m_sMotionFrag.bJump == false)
		{// ��苗���ԂɃv���C���[����������

			// �U����Ԃɂ���
			m_state = STATE_ATTACK;
			m_sMotionFrag.bATK = true;

			// �ǂ��|����v���C���[�̔ԍ��ݒ�
			m_nTargetPlayerIndex = i;
			break;
		}

		i++;
	}
}

//==========================================================================
// �ǂ��|���ړ�
//==========================================================================
void CEnemy::ChaseMove(float fMove)
{
	// �����擾
	MyLib::Vector3 rot = GetRotation();

	// �ړ��ʎ擾
	MyLib::Vector3 move = GetMove();

	// �ړ��ʉ��Z
	move.x += sinf(D3DX_PI + rot.y) * fMove;
	move.z += cosf(D3DX_PI + rot.y) * fMove;

	// �ړ��ʐݒ�
	SetMove(move);
}

//==========================================================================
//  ��l�̕�
//==========================================================================
void CEnemy::LimitArea()
{
	// ���g�̒l���擾
	MyLib::Vector3 pos = GetPosition();

	// ��l�̕ǎ擾
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

	// �l��K�p
	SetPosition(pos);


	float fLength = sqrtf(pos.x * pos.x + pos.z * pos.z);

	if (fLength > mylib_const::RADIUS_STAGE)
	{// �␳
		D3DXVec3Normalize(&pos, &pos);

		pos *= mylib_const::RADIUS_STAGE;

		SetPosition(pos);
	}
}

//==========================================================================
// ���[�V�����̐ݒ�
//==========================================================================
void CEnemy::SetMotion(int motionIdx)
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
// �U��������
//==========================================================================
void CEnemy::AttackAction(CMotion::AttackInfo ATKInfo, int nCntATK)
{
	return;
}

//==========================================================================
// �U�����蒆����
//==========================================================================
void CEnemy::AttackInDicision(CMotion::AttackInfo* pATKInfo, int nCntATK)
{
	// ���[�V�����擾
	CMotion* pMotion = GetMotion();
	if (pMotion == nullptr)
	{
		return;
	}

	// �ʒu�擾
	MyLib::Vector3 pos = GetPosition();

	// ����̈ʒu
	MyLib::Vector3 weponpos = pMotion->GetAttackPosition(GetModel(), *pATKInfo);

	if (pATKInfo->fRangeSize == 0.0f)
	{
		return;
	}

	if (pATKInfo->bEndAtk)
	{
		return;
	}

#if _DEBUG
	CEffect3D::Create(weponpos, MyLib::Vector3(0.0f, 0.0f, 0.0f), D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f), pATKInfo->fRangeSize, 10, CEffect3D::MOVEEFFECT_NONE, CEffect3D::TYPE_NORMAL);
#endif

	//============================
	// �v���C���[�Ɣ���
	//============================
	// �v���C���[�擾
	CListManager<CPlayer> playerList = CPlayer::GetListObj();
	CPlayer* pPlayer = nullptr;

	// ���X�g���[�v
	while (playerList.ListLoop(&pPlayer))
	{
		// �v���C���[�̌���
		MyLib::Vector3 PlayerPos = pPlayer->GetPosition();

		// ����T�C�Y�擾
		float fRadius = pPlayer->GetRadius();

		if (UtilFunc::Collision::SphereRange(weponpos, PlayerPos, pATKInfo->fRangeSize, fRadius).ishit)
		{// ���̔���

			MyLib::HitResult_Character result = pPlayer->Hit(pATKInfo->nDamage, this, pATKInfo->AtkType);

			if (result.ishit)
			{
				pATKInfo->bEndAtk = true;
			}

			if (result.isdamage)
			{// �������Ă���

				// �v���C���[�̌���
				MyLib::Vector3 PlayerRot = pPlayer->GetRotation();

				// �^�[�Q�b�g�ƓG�Ƃ̌���
				float fRot = atan2f((pos.x - PlayerPos.x), (pos.z - PlayerPos.z));

				// �����𐳖ʂɂ���
				fRot = D3DX_PI + fRot;
				UtilFunc::Transformation::RotNormalize(fRot);

				// �����ݒ�
				pPlayer->SetRotation(MyLib::Vector3(PlayerRot.x, fRot, PlayerRot.z));
				pPlayer->SetRotDest(fRot);
				fRot = pPlayer->GetRotation().y;

				// ������΂�
				pPlayer->SetMove(MyLib::Vector3(
					sinf(fRot) * 4.0f,
					12.0f,
					cosf(fRot) * 4.0f));
			}
		}

		//============================
		// �܂Ɣ���
		//============================
		// �o�b�O�̃��X�g�擾
		CListManager<CSantaBag> BagList = CSantaBag::GetListObj();
		CSantaBag* pBag = nullptr;

		// ���X�g���[�v
		while (BagList.ListLoop(&pBag))
		{
			// �����蔻��
			if (UtilFunc::Collision::CircleRange3D(weponpos, pBag->GetPosition(), pATKInfo->fRangeSize, 80.0f))
			{
				pBag->Hit();
			}
		}
	}

}

//==========================================================================
// �`�揈��
//==========================================================================
void CEnemy::Draw()
{
	if (m_state == STATE_FADEOUT)
	{
		CObjectChara::Draw(m_mMatcol.a);
	}
	else if (m_mMatcol != D3DXCOLOR(1.0f, 1.0f, 1.0f, m_mMatcol.a))
	{
		// �I�u�W�F�N�g�L�����̕`��
		CObjectChara::Draw(m_mMatcol);
	}
	else
	{
		// �I�u�W�F�N�g�L�����̕`��
		CObjectChara::Draw();
	}
}

//==========================================================================
// ���̌���
//==========================================================================
void CEnemy::SetOriginRotation(MyLib::Vector3 rot)
{
	m_rotOrigin = rot;
}

//==========================================================================
// �X�|�[���n�_�ݒ�
//==========================================================================
void CEnemy::SetSpawnPosition(MyLib::Vector3 pos)
{
	m_posOrigin = pos;
}

//==========================================================================
// �X�|�[���n�_�擾
//==========================================================================
MyLib::Vector3 CEnemy::GetSpawnPosition()
{
	return m_posOrigin;
}

//==========================================================================
// ��Ԑݒ�
//==========================================================================
void CEnemy::SetState(STATE state)
{
	m_state = state;
}

//==========================================================================
// �G�̏��擾
//==========================================================================
CEnemy *CEnemy::GetEnemy()
{
	// �������g�̃|�C���^���擾
	return this;
}
