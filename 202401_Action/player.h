//=============================================================================
// 
//  �v���C���[�w�b�_�[ [player.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _PLAYER_H_
#define _PLAYER_H_	// ��d�C���N���[�h�h�~

#include "gamemanager.h"
#include "objectChara.h"
#include "listmanager.h"

//==========================================================================
// �O���錾
//==========================================================================
class CShadow;
class CHP_GaugePlayer;
class CStaminaGauge_Player;
class CSkillPoint;
class CEnemy;
class CEndCounterSetting;

class CPlayerControlAttack;	// �U��
class CPlayerControlDefence;	// �h��
class CPlayerControlAvoid;	// ���
class CPlayerGuard;	// �K�[�h

//==========================================================================
// �N���X��`
//==========================================================================
// �v���C���[�N���X��`
class CPlayer : public CObjectChara
{
public:
	// �񋓌^��`
	enum MOTION
	{
		MOTION_DEF = 0,		// �j���[�g�������[�V����
		MOTION_WALK,		// �ړ�
		MOTION_DASH,		// �_�b�V��
		MOTION_AVOID,		// ���
		MOTION_ATK,			// �U��
		MOTION_ATK2,		// �U��(�h��1)
		MOTION_ATK3,		// �U��(�h��2)
		MOTION_JUMP,		// �W�����v
		MOTION_FALL,		// ������
		MOTION_KNOCKBACK,	// �m�b�N�o�b�N
		MOTION_DEAD,		// ���S
		MOTION_RESPAWN,		// ����
		MOTION_PRAYER,		// �F��
		MOTION_GUARD,		// �K�[�h
		MOTION_GUARD_DMG,	// �K�[�h�_���[�W
		MOTION_COUNTER_ACCEPT,		// �����󂯕t��
		MOTION_COUNTER_TURN,		// �����󂯗���
		MOTION_COUNTER_ATTACK,		// ����
		MOTION_DASHATK,			// �_�b�V���U��
		MOTION_MAX
	};

	// ��Ԓ�`
	enum STATE
	{
		STATE_NONE = 0,		// �Ȃɂ��Ȃ�
		STATE_INVINCIBLE,	// ���G
		STATE_DMG,			// �_���[�W
		STATE_KNOCKBACK,	// �m�b�N�o�b�N
		STATE_DEAD,			// ��
		STATE_DEADWAIT,		// ���S�ҋ@
		STATE_FADEOUT,		// �t�F�[�h�A�E�g
		STATE_RESPAWN,		// ����
		STATE_COUNTER,		// ����
		STATE_AVOID,		// ���
		STATE_MAX
	};

	// ���[�V�����̔���
	struct SMotionFrag
	{
		bool bJump;			// �W�����v��
		bool bATK;			// �U����
		bool bGuard;		// �K�[�h
		bool bCounter;		// �J�E���^�[��
		bool bKnockBack;	// �m�b�N�o�b�N��
		bool bDead;			// ���S��
		bool bMove;			// �ړ���
	};

	CPlayer(int nPriority = 2);
	~CPlayer();

	// �I�[�o�[���C�h���ꂽ�֐�
	virtual HRESULT Init() override;
	virtual void Uninit() override;
	virtual void Update() override;
	virtual void Draw() override;

	MyLib::HitResult_Character Hit(const int nValue, CGameManager::AttackType atkType = CGameManager::ATTACK_NORMAL);	// �q�b�g����
	MyLib::HitResult_Character Hit(const int nValue, CEnemy* pEnemy, CGameManager::AttackType atkType = CGameManager::ATTACK_NORMAL);	// �q�b�g����
	MyLib::HitResult_Character ProcessHit(const int nValue);

	STATE GetState();		// ��Ԏ擾
	void SetState(STATE state, int nCntState = 0);	// ��Ԑݒ�
	virtual void Kill();			// ���S����
	void SwitchRockOnTarget();		// ���b�N�Ώې؂�ւ�

	// ���[�V����
	void SetMotion(int motionIdx);	// ���[�V�����̐ݒ�
	bool IsJump() { return m_bJump; }	// �W�����v����
	float GetDashTime() { return m_fDashTime; }		// �_�b�V������
	void SetComboStage(int stage) { m_nComboStage = stage;; }	// �R���{�̒i�K�ݒ�
	int GetComboStage() { return m_nComboStage; }	// �R���{�̒i�K�擾
	bool IsReadyDashAtk() { return m_bReadyDashAtk; }
	void SetMotionFrag(SMotionFrag frag) { m_sMotionFrag = frag; }
	SMotionFrag GetMotionFrag() { return m_sMotionFrag; }

	// �]�ڃr�[�R��
	void SetEnableTouchBeacon(bool bTouch) { m_bTouchBeacon = bTouch; }	// �r�[�R���ɐG��Ă锻��ݒ�
	bool IsTouchBeacon() { return m_bTouchBeacon; }	// �r�[�R���ɐG��Ă锻��擾

	// �X�L���|�C���g
	CSkillPoint* GetSkillPoint() { return m_pSkillPoint; }

	// �X�^�~�i
	CStaminaGauge_Player* GetStaminaGauge() { return m_pStaminaGauge; }

	// �X�L���c���[�p�֐�
	void ResetEnhance();	// �������Z�b�g
	void UpgradeLife(int addvalue);	// �̗̓A�b�v�O���[�h
	void UpgradeMaxStamina(int addvalue);		// �X�^�~�i�ő�l�A�b�v�O���[�h
	void UpgradeAutoHealStamina(float ratio);	// �X�^�~�i�����񕜃A�b�v�O���[�h
	void ChangeAtkControl(CPlayerControlAttack* control);	// �U���̑���ύX
	void ChangeDefenceControl(CPlayerControlDefence* control);	// �h��̑���ύX
	void ChangeAvoidControl(CPlayerControlAvoid* control);	// ����̑���ύX
	void ChangeGuardGrade(CPlayerGuard* guard);	// �K�[�h���\�ύX

	// ���X�|�[��
	void RespawnSetting();	// ���X�|�[�����̐ݒ�
	int GetRespawnPercent() { return m_nRespawnPercent; }	// ���X�|�[���m���擾
	void SetRespawnPercent(int value);	// ���X�|�[���m���ݒ�

	// ����
	bool IsLockOnAtStart() { return m_bLockOnAtStart; }	// �����J�n���Ƀ��b�N�I�����Ă�����
	void SetCounterSubValue(float value) { m_fCounterStaminaSubValue = value; }	// �J�E���^�[���̏���X�^�~�i�ݒ�
	float GetCounterSubValue() { return m_fCounterStaminaSubValue; }	// �J�E���^�[���̏���X�^�~�i�擾
	void SetCounterHealValue(float value) { m_fCounterStaminaHealValue = value; }	// �J�E���^�[���̃X�^�~�i�񕜐ݒ�
	float GetCounterHealValue() { return m_fCounterStaminaHealValue; }	// �J�E���^�[���̃X�^�~�i�񕜐ݎ擾

	// �K�[�h
	void SetGuardSubValue(float value) { m_fGuardStaminaSubValue = value; }	// �K�[�h���̏���X�^�~�i�ݒ�
	float GetGuardSubValue() { return m_fGuardStaminaSubValue; }	// �K�[�h���̏���X�^�~�i�擾

	// �q�b�g�n
	void DeadSetting(MyLib::HitResult_Character* result);

	// ���̑�
	int GetMyPlayerIdx() { return m_nMyPlayerIdx; }

	static CPlayer* Create(int nIdx);	// ����
	static CListManager<CPlayer> GetListObj() { return m_List; }	// ���X�g�擾

protected:

	bool Collision(MyLib::Vector3 &pos, MyLib::Vector3 &move);	// �����蔻��
	void MotionSet();	// ���[�V�����̐ݒ�

	bool m_bJump;				// �W�����v�����ǂ���
	bool m_bLandOld;			// �ߋ��̒��n���
	bool m_bHitStage;			// �X�e�[�W�̓����蔻��
	bool m_bLandField;			// �t�B�[���h�̒��n����
	bool m_bHitWall;			// �ǂ̓����蔻��
	int m_nMyPlayerIdx;			// �v���C���[�C���f�b�N�X�ԍ�
	int m_nCntWalk;				// ���s�J�E���^�[
	STATE m_state;				// ���
	CShadow *m_pShadow;			// �e�̏��
	SMotionFrag m_sMotionFrag;	// ���[�V�����̃t���O
private:

	//=============================
	// �֐����X�g
	//=============================
	typedef void(CPlayer::* STATE_FUNC)();
	static STATE_FUNC m_StateFunc[];

	//=============================
	// �����o�֐�
	//=============================
	// ��Ԋ֐�
	void StateNone();		// �Ȃ�
	void StateInvincible();	// ���G
	void StateDamage();		// �_���[�W
	void StateKnockBack();	// �m�b�N�o�b�N
	void StateDead();		// ���S
	void StateDeadWait();	// ���S�ҋ@
	void StateFadeOut();	// �t�F�[�h�A�E�g
	void StateRespawn();	// ���X�|�[��
	void StateCounter();	// �J�E���^�[��
	void StateAvoid();		// ���

	// ���̑��֐�
	virtual void Controll();	// ����
	void LimitPos();			// �ʒu����
	void MotionBySetState();	// ���[�V�����ʂ̏�Ԑݒ�
	void ResetFrag();			// �t���O���Z�b�g
	void RockOn();				// ���b�N�I��

	// ���[�V�����n�֐�
	void AttackAction(CMotion::AttackInfo ATKInfo, int nCntATK) override;	// �U��������
	void AttackInDicision(CMotion::AttackInfo* pATKInfo, int nCntATK) override;			// �U�����蒆����

	//=============================
	// �����o�ϐ�
	//=============================
	STATE m_Oldstate;				// �O��̏��
	D3DXCOLOR m_mMatcol;			// �}�e���A���̐F
	MyLib::Vector3 m_posKnokBack;	// �m�b�N�o�b�N�̈ʒu
	MyLib::Vector3 m_KnokBackMove;	// �m�b�N�o�b�N�̈ړ���
	int m_nCntState;				// ��ԑJ�ڃJ�E���^�[
	int m_nComboStage;				// �R���{�̒i�K
	int m_nIdxRockOn;				// ���b�N�I���Ώۂ̃C���f�b�N�X�ԍ�
	bool m_bLockOnAtStart;			// �J�E���^�[�J�n���Ƀ��b�N�I�����Ă�����
	bool m_bReadyDashAtk;				// �_�b�V���A�^�b�N�̃t���O
	bool m_bAttacking;				// �U����
	bool m_bCounterAccepting;		// �J�E���^�[��t��
	bool m_bDash;					// �_�b�V������
	float m_fDashTime;				// �_�b�V������
	int m_nRespawnPercent;			// ���X�|�[���m��
	bool m_bTouchBeacon;			// �r�[�R���ɐG��Ă锻��
	float m_fGuardStaminaSubValue;	// �K�[�h���̃X�^�~�i������
	float m_fCounterStaminaSubValue;	// �J�E���^�[���̃X�^�~�i������
	float m_fCounterStaminaHealValue;	// �J�E���^�[���̃X�^�~�i�񕜗�
	CSkillPoint* m_pSkillPoint;		// �X�L���|�C���g�̃I�u�W�F�N�g
	CHP_GaugePlayer* m_pHPGauge;		// HP�Q�[�W�̃|�C���^
	CStaminaGauge_Player* m_pStaminaGauge;	// �X�^�~�i�Q�[�W�̃|�C���^
	
	// �p�^�[���p�C���X�^���X
	CEndCounterSetting* m_pEndCounterSetting;	// �J�E���^�[�I�����̐ݒ�
	CPlayerControlAttack* m_pControlAtk;		// �U������
	CPlayerControlDefence* m_pControlDefence;	// �h�䑀��
	CPlayerControlAvoid* m_pControlAvoid;		// ��𑀍�
	CPlayerGuard* m_pGuard;						// �K�[�h

	Effekseer::Handle *m_pWeaponHandle;		// �G�t�F�N�g�̕���n���h��
	static CListManager<CPlayer> m_List;	// ���X�g
};


//==========================================================================
// �J�E���^�[�I�����̐ݒ�N���X
//==========================================================================
class CEndCounterSetting
{
public:
	CEndCounterSetting() {}

	virtual void EndSetting(CPlayer* player);	// �I�����̐ݒ�
};

class CEndTurn : public CEndCounterSetting
{
	//void EndSetting(CPlayer* player) override {}
};

class CEndAttack : public CEndCounterSetting
{
	void EndSetting(CPlayer* player) override;
};



#endif