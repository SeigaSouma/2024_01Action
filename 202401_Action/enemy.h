//=============================================================================
// 
//  �G�w�b�_�[ [enemy.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _ENEMY_H_
#define _ENEMY_H_	// ��d�C���N���[�h�h�~

#include "objectChara.h"
#include "motion.h"
#include "enemymanager.h"
#include "resultmanager.h"
#include "listmanager.h"
#include "myeffekseer.h"
#include "calculation.h"

// �O���錾
class CHP_Gauge;
class CShadow;
class CEffect3D;
class CEnemyFixedMoveManager;
class CRockOnMarker;

class CEnemyState;
class CEnemyAttack;
class CEnemyReturnDown;

//==========================================================================
// �N���X��`
//==========================================================================
// �G�N���X��`
class CEnemy : public CObjectChara
{
public:
	// �G���
	enum TYPE
	{
		TYPE_BOSS = 0,		// �{�X
		TYPE_STONEGOLEM,	// �X�g�[���S�[����
		TYPE_GOBELIN,		// �S�u����
		TYPE_PRACTICE,		// ���K
		TYPE_PRACTICE_A,	// ���K(�U��)
		TYPE_PRACTICE_SA,	// ���K(���U��)
		TYPE_MAX
	};

	// ��ԗ�
	enum STATE
	{
		STATE_NONE = 0,		// �Ȃɂ��Ȃ�
		STATE_SPAWNWAIT,	// �X�|�[���ҋ@
		STATE_SPAWN,		// �X�|�[��
		STATE_DMG,			// �_���[�W
		STATE_DEAD,			// ��
		STATE_FADEOUT,		// �t�F�[�h�A�E�g
		STATE_WAIT,			// �ҋ@
		STATE_DOWN,			// �_�E��
		STATE_STRONGATK,	// ���U��
		STATE_MAX
	};

	// �s����
	enum ACTION
	{
		ACTION_DEF = 0,	// �ʏ�s��
		ACTION_WAIT,	// �ҋ@�s��
		ACTION_MAX
	};

	enum MOTION
	{
		MOTION_DEF = 0,			// �j���[�g����
		MOTION_WALK,			// �ړ�
		MOTION_ATTACK_NORMAL,	// �ʏ�U��
		MOTION_ATTACK_STRONG,	// ���U��
		MOTION_DMG,				// �_���[�W
		MOTION_DOWN,			// �_�E��
		MOTION_RETURNDOWN,			// �_�E��
		MOTION_KNOCKBACK,		// ����
		MOTION_FADEOUT,			// �t�F�[�h�A�E�g
		MOTION_MAX
	};


	CEnemy(int nPriority = mylib_const::ENEMY_PRIORITY);
	virtual ~CEnemy();


	// �I�[�o�[���C�h���ꂽ�֐�
	virtual HRESULT Init() override;
	virtual void Uninit() override;
	virtual void Update() override;
	virtual void Draw() override;

	virtual void Kill();	// �폜

	void SetState(STATE state);		// ��Ԑݒ�
	STATE GetState() { return m_state; }
	void SetStateTime(float time) { m_fStateTime = time; }	// ��Ԏ��Ԑݒ�
	void SetDownTime(float time) { m_fDownTime = time; }	// �_�E�����Ԑݒ�
	virtual bool Hit(const int nValue, const MyLib::Vector3& hitpos, CGameManager::AttackType atkType = CGameManager::AttackType::ATTACK_NORMAL);
	virtual void NormalHitResponse();	// �q�b�g���̔���
	virtual void CounterHitResponse();	// �q�b�g���̔���

	// ���b�N�I���֘A
	void SetTargetPosition(MyLib::Vector3 pos) { m_TargetPosition = pos; }	// �ڕW�̈ʒu�ݒ�
	void SetEnableRockOn(bool bSet) { m_bRockOnAccepting = bSet; }
	bool IsRockOnAccept() { return m_bRockOnAccepting; }

	// �U����Ԑ؂�ւ�
	void ChangeATKState(CEnemyState* state);
	void ChangeNextATKState(CEnemyState* state) { m_pNextATKState = state; }
	CEnemyState* GetATKState() { return m_pATKState; }
	CEnemyState* GetNextATKState() { return m_pNextATKState; }

	void PerformAttack();		// �U�����s����
	void DrawingRandomAction();	// �U�������_�����I
	void ChangeNextAction();	// ���̍U���֐؂�ւ�

	virtual void ActChase(float moveMultiply, float catchLen);	// �ǂ��|��
	virtual void RotationTarget(float range = 90.0f);	// �^�[�Q�b�g�̕�������

	void ToggleCatchUp(bool catchUp) { m_bCatchUp = catchUp; }	// �ǂ���������
	bool IsCatchUp() { return m_bCatchUp; }						// �ǂ���������
	void ToggleInSight(bool inSight) { m_bInSight = inSight; }	// ���E������
	bool IsInSight() { return m_bInSight; }						// ���E������
	bool IsActiveSuperArmor() { return m_bActiveSuperArmor; }	// �X�[�p�[�A�[�}�[�̗L���t���O�擾

	// ���[�V����
	void SetMotion(int motionIdx);	// ���[�V�����̐ݒ�

	HRESULT RoadText(const char *pFileName);
	void SetParent(CEnemy *pParent);		// �e�̃|�C���^�ݒ�
	CEnemy *GetEnemy();
	TYPE GetType() { return m_type; }	// ��ގ擾
	static int GetNumSurvival() { return m_nNumSurvival; }	// �����l��
	static CListManager<CEnemy> GetListObj() { return m_List; }	// ���X�g�擾
	static CEnemy* Create(const char* pFileName, MyLib::Vector3 pos, TYPE type = TYPE_BOSS);

protected:

	//=============================
	// �\���̒�`
	//=============================
	// ���[�V�����̔���
	struct SMotionFrag
	{
		bool bJump;			// �W�����v�����ǂ���
		bool bATK;			// �U�������ǂ���
		bool bKnockback;	// �m�b�N�o�b�N�����ǂ���
		bool bMove;			// �ړ������ǂ���
		bool bCharge;		// �`���[�W�����ǂ���
		SMotionFrag() : bJump(false), bATK(false), bKnockback(false), bMove(false), bCharge(false) {}
	};

	//=============================
	// �����o�֐�
	//=============================
	// �s���X�V�n
	virtual void ActDefault();		// �ʏ�s��
	virtual void ActWait();			// �ҋ@�s��

	// ��ԍX�V�n
	virtual void StateNone();		// �����Ȃ����
	virtual void StateSpawnWait();	// �X�|�[���ҋ@
	virtual void StateSpawn();		// �X�|�[��
	virtual void StateDamage();		// �_���[�W
	virtual void StateDead();		// ���S
	virtual void StateFadeOut();	// �t�F�[�h�A�E�g
	virtual void StateWait();		// �ҋ@����
	virtual void StateDown();		// �_�E�����
	virtual void StateStrongAtk();	// ���U��

	// ���̑��֐�
	virtual void ProcessLanding();	// ���n������
	virtual void AttackAction(CMotion::AttackInfo ATKInfo, int nCntATK) override;		// �U��������
	virtual void AttackInDicision(CMotion::AttackInfo* pATKInfo, int nCntATK) override;	// �U�����蒆����

	//=============================
	// �����o�ϐ�
	//=============================
	ACTION m_Action;			// �s��
	float m_fActTime;			// �s���J�E���^�[
	float m_fStrongAttackTime;	// ���U���̃^�C�}�[

	STATE m_state;							// ���
	STATE m_Oldstate;						// �O��̏��
	float m_fStateTime;						// ��ԃJ�E���^�[
	int m_nTargetPlayerIndex;				// �ǂ��|����v���C���[�̃C���f�b�N�X�ԍ�
	bool m_bDecrementSurvival;				// �����l�������t���O
	bool m_bActionable;						// �s���\��
	bool m_bDamageReceived;					// �_���[�W�󂯕t������
	float m_fDamageReciveTime;				// �_���[�W��t����
	bool m_bActiveSuperArmor;				// �X�[�p�[�A�[�}�[
	float m_fDownTime;						// �_�E���J�E���^�[
	float m_fRockOnDistance;				// ���b�N�I���̋���
	bool m_bRockOnAccepting;				// ���b�N�I����t
	bool m_bCatchUp;						// �ǂ���������
	bool m_bInSight;						// ���E������
	MyLib::Vector3 m_posKnokBack;			// �m�b�N�o�b�N�̈ʒu
	SMotionFrag m_sMotionFrag;				// ���[�V�����̃t���O
	CHP_Gauge *m_pHPGauge;					// HP�Q�[�W�̏��
	CEnemy *m_pParent;						// �e�̃|�C���^
	D3DXCOLOR m_mMatcol;					// �}�e���A���̐F
	MyLib::Vector3 m_TargetPosition;		// �ڕW�̈ʒu
	Effekseer::Handle m_pWeaponHandle;		// �G�t�F�N�g�̕���n���h��
	static int m_nNumSurvival;				// �����l��

	CEnemyState* m_pATKState;		// ���̍s���|�C���^
	CEnemyState* m_pNextATKState;	// ���̍s���|�C���^
	bool m_bStateChanging;			// ��Ԃ��؂�ւ�����u��
	std::vector<CEnemyAttack*> m_pAtkPattern;	// �U���̎��
	CEnemyReturnDown* m_pReturnDown;			// �_�E�����A

private:
	
	//=============================
	// �֐����X�g
	//=============================
	typedef void(CEnemy::* ACT_FUNC)();
	static ACT_FUNC m_ActFuncList[];	// �s���֐����X�g

	typedef void(CEnemy::* STATE_FUNC)();
	static STATE_FUNC m_StateFunc[];	// ��Ԋ֐����X�g

	void UpdateState();					// ��ԍX�V����
	void Collision();					// �����蔻��
	void UpdateDamageReciveTimer();		// �_���[�W��t���ԍX�V
	virtual void MotionSet() = 0;		// ���[�V�����̐ݒ�
	void RegistrChild(CEnemy *pChild);
	void ResetChild(CEnemy *pChild);
	void LimitArea(); // ��l�̕ǔ���

	//=============================
	// �����o�ϐ�
	//=============================
	TYPE m_type;			// ���
	int m_nNumChild;			// �q�̐�
	CEnemy *m_pChild[mylib_const::MAX_ENEMY];	// �q�̃|�C���^
	CShadow *m_pShadow;			// �e�̏��
	CRockOnMarker* m_pRockOnMarker;		// ���b�N�I���}�[�J�[
	static CListManager<CEnemy> m_List;	// ���X�g
};


class CEnemyFlinch;	// ���݃|�C���^

//=============================
// �G�l�~�[�X�e�[�g
//=============================
class CEnemyState
{
public:
	CEnemyState() : 
		m_nIdxMotion(0), 
		m_bCreateFirstTime(false), 
		m_bBeforeAttackAction(false),
		m_bFinchAction(false),
		m_pFlinchAction(nullptr) {}

	~CEnemyState();

	virtual void Action(CEnemy* boss) = 0;	// �s��
	virtual void Attack(CEnemy* boss) = 0;	// �U������
	virtual void ChangeMotionIdx(CEnemy* boss)
	{
		// �h���N���X�ŃC���f�b�N�X�ݒ肳��Ă�O��
		boss->SetMotion(m_nIdxMotion);
		m_bBeforeAttackAction = false;	// �U���O�s���t���O
	}

	virtual void BeforeAttack(CEnemy* boss) { m_bBeforeAttackAction = true; }	// �U���O����

	bool IsCreateFirstTime() { return m_bCreateFirstTime; }	// ���񐶐��̃t���O
	bool IsFlinchAction() { return m_bFinchAction; }	// ���ݍs���t���O

	// ���ݍs���ݒ�
	void SetFlinchAction(CEnemyFlinch* pFlinch);

	// ���݂֐؂�ւ�
	void ChangeFlinchAction(CEnemy* boss);

protected:
	int m_nIdxMotion;				// ���[�V�����ԍ�
	bool m_bCreateFirstTime;		// ���񐶐��̃t���O
	bool m_bBeforeAttackAction;		// �U���O�s���t���O
	bool m_bFinchAction;			// ���ݍs���t���O
	CEnemyFlinch* m_pFlinchAction;	// ���ݍs���̃|�C���^
};


//=============================
// �s���O�A�N�V����
//=============================
class CEnemyBeforeAction : public CEnemyState
{
public:

	CEnemyBeforeAction() {}

	virtual void Action(CEnemy* boss) override;		// �s��
	virtual void Attack(CEnemy* boss) override {}	// �U������

	// ���[�V�����C���f�b�N�X�؂�ւ�
	virtual void ChangeMotionIdx(CEnemy* boss) override
	{
		CEnemyState::ChangeMotionIdx(boss);
	}
};


//=============================
// ����
//=============================
class CEnemyFlinch : public CEnemyState
{
public:
	CEnemyFlinch() {}

	virtual void Action(CEnemy* boss) override;		// �s��
	virtual void Attack(CEnemy* boss) override {}	// �U������

	// ���[�V�����C���f�b�N�X�؂�ւ�
	virtual void ChangeMotionIdx(CEnemy* boss) override
	{
		CEnemyState::ChangeMotionIdx(boss);
	}
};


//=============================
// �_�E�����A
//=============================
class CEnemyReturnDown : public CEnemyState
{
public:
	CEnemyReturnDown() { m_bCreateFirstTime = true; }

	virtual void Action(CEnemy* boss) override;		// �s��
	virtual void Attack(CEnemy* boss) override {}	// �U������

	// ���[�V�����C���f�b�N�X�؂�ւ�
	virtual void ChangeMotionIdx(CEnemy* boss) override
	{
		CEnemyState::ChangeMotionIdx(boss);
	}
};


//=============================
// �U���N���X
//=============================
class CEnemyAttack : public CEnemyState
{
public:

	CEnemyAttack() : 
		m_bWillDirectlyTrans(true), 
		m_bSetAngleBeforeAttack(true), 
		m_bSetAngleNotAttacking(false), 
		m_bChainAttack(false), 
		m_pChainAttackPtr(nullptr)
	{
		m_bCreateFirstTime = true;
	}

	~CEnemyAttack()
	{
		if (m_pChainAttackPtr != nullptr)
		{
			delete m_pChainAttackPtr;
			m_pChainAttackPtr = nullptr;
		}
	}

	virtual void Action(CEnemy* boss) override = 0;	// �s��
	virtual void Attack(CEnemy* boss) override;		// �U������

	// ���[�V�����C���f�b�N�X�؂�ւ�
	virtual void ChangeMotionIdx(CEnemy* boss) override
	{
		CEnemyState::ChangeMotionIdx(boss);
	}

	// �J�ڑO����
	virtual void BeforeTransitionProcess(CEnemy* boss)
	{
		boss->RotationTarget();
		boss->ActChase(1.0f, 600.0f);
	}	// �J�ڑO����

	bool IsDirectlyTrans() { return m_bWillDirectlyTrans; }	// ���ڑJ�ڃt���O�擾
	bool IsFlinchAction() { return m_bFinchAction; }		// ���ݍs���t���O

protected:

	// �A���U���ݒ�
	void SetChainAttack(CEnemyAttack* pAttack)
	{
		if (m_pChainAttackPtr != nullptr)
		{
			delete m_pChainAttackPtr;
		}

		m_bChainAttack = true;
		m_pChainAttackPtr = pAttack;
	}

	// �A���U���̍s���֐ؑ�
	void ChangeChainAttack(CEnemy* boss)
	{
		if (m_pChainAttackPtr == nullptr)
		{
			return;
		}
		boss->ChangeATKState(m_pChainAttackPtr);
		m_pChainAttackPtr->ChangeMotionIdx(boss);

		// ���E���E�ǂ���������
		boss->ToggleCatchUp(true);
		boss->ToggleInSight(true);
	}

	bool m_bWillDirectlyTrans;		// ���ڑJ�ڃt���O
	bool m_bSetAngleBeforeAttack;	// �U���O�������킹�t���O
	bool m_bSetAngleNotAttacking;	// �U������O�������킹�t���O
	bool m_bChainAttack;			// �A���U���t���O
	CEnemyAttack* m_pChainAttackPtr;// �A���U���̃|�C���^
};

// �ߐڍU��
class CEnemyProximity : public CEnemyAttack
{
public:

	CEnemyProximity() : m_fAttackLength(200.0f) {}
	CEnemyProximity(float len) : m_fAttackLength(len) {}

	virtual void Action(CEnemy* boss) override;	// �s��
	virtual void ChangeMotionIdx(CEnemy* boss) override = 0;	// ���[�V�����C���f�b�N�X�؂�ւ�

protected:
	float m_fAttackLength;	// �U���\�Ȋԍ���

};

// �������U��
class CEnemyRemote : public CEnemyAttack
{
public:
	CEnemyRemote() {}

	virtual void Action(CEnemy* boss) override;	// �s��
	virtual void ChangeMotionIdx(CEnemy* boss) override = 0;	// ���[�V�����C���f�b�N�X�؂�ւ�
};


//=============================
// �ߐڌQ
//=============================
// �ʏ�U��
class CEnemyNormalAttack : public CEnemyProximity
{
public:
	CEnemyNormalAttack() { m_bWillDirectlyTrans = true; }

	// ���[�V�����C���f�b�N�X�؂�ւ�
	virtual void ChangeMotionIdx(CEnemy* boss) override
	{
		m_nIdxMotion = CEnemy::MOTION_ATTACK_NORMAL;
		CEnemyAttack::ChangeMotionIdx(boss);

		// ���ڑJ�ڂ���
		m_bWillDirectlyTrans = true;
	}
};

// ���U��
class CEnemyStrongAttack : public CEnemyProximity
{
public:
	CEnemyStrongAttack() { m_bWillDirectlyTrans = true; }
	CEnemyStrongAttack(float len) : CEnemyProximity(len) {}

	// ���[�V�����C���f�b�N�X�؂�ւ�
	virtual void ChangeMotionIdx(CEnemy* boss) override
	{
		m_nIdxMotion = CEnemy::MOTION_ATTACK_STRONG;
		CEnemyAttack::ChangeMotionIdx(boss);

		// ���U����Ԃɐݒ�
		boss->SetState(CEnemy::STATE_STRONGATK);
	}

	// �J�ڑO����
	virtual void BeforeTransitionProcess(CEnemy* boss) override
	{
		// ���ލs����ݒ�
		boss->ChangeATKState(DEBUG_NEW CEnemyBeforeAction());
		boss->GetATKState()->ChangeMotionIdx(boss);
	}

};

//=============================
// �������Q
//=============================



#endif