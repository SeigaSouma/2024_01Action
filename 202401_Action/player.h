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
		MOTION_DEF = 0,				// �j���[�g�������[�V����
		MOTION_WALK,				// �ړ�
		MOTION_DASH,				// �_�b�V��
		MOTION_AVOID,				// ���
		MOTION_ATK,					// �U��
		MOTION_ATK2,				// �U��(�h��1)
		MOTION_ATK3,				// �U��(�h��2)
		MOTION_ATK4,				// �U��(�h��3)
		MOTION_JUMP,				// �W�����v
		MOTION_FALL,				// ������
		MOTION_DMG,					// �_���[�W
		MOTION_KNOCKBACK,			// �m�b�N�o�b�N
		MOTION_KNOCKBACK_PASSIVE,	// �m�b�N�o�b�N�󂯐g
		MOTION_DOWN,				// �_�E��
		MOTION_DEAD,				// ���S
		MOTION_RESPAWN,				// ����
		MOTION_PRAYER,				// �F��
		MOTION_PRAYERLOOP,			// �F�胋�[�v
		MOTION_GUARD,				// �K�[�h
		MOTION_GUARD_DMG,			// �K�[�h�_���[�W
		MOTION_COUNTER_ACCEPT,		// �����󂯕t��
		MOTION_COUNTER_TURN,		// �����󂯗���
		MOTION_COUNTER_ATTACK,		// ����
		MOTION_DASHATK,				// �_�b�V���U��
		MOTION_ATK4_FINISH,			// 4�i�K�ڍU���̃`���[�W
		MOTION_MAX
	};

	// ��Ԓ�`
	enum STATE
	{
		STATE_NONE = 0,		// �Ȃɂ��Ȃ�
		STATE_INVINCIBLE,	// ���G
		STATE_DMG,			// �_���[�W
		STATE_KNOCKBACK,	// �m�b�N�o�b�N
		STATE_DOWN,			// �_�E��
		STATE_DEAD,			// ��
		STATE_DEADWAIT,		// ���S�ҋ@
		STATE_FADEOUT,		// �t�F�[�h�A�E�g
		STATE_RESPAWN,		// ����
		STATE_COUNTER,		// ����
		STATE_AVOID,		// ���
		STATE_PRAYER,		// �F��
		STATE_CHARGE,		// �`���[�W
		STATE_MAX
	};

	//=============================
	// �\����
	//=============================
	// �v���C���[�X�e�[�^�X
	struct sPlayerStatus
	{
		float respawnHeal;				// ���X�|�[�����̉񕜗�
		float guardStaminaSubValue;		// �K�[�h���̃X�^�~�i������
		float counterStaminaSubValue;	// �J�E���^�[���̃X�^�~�i������
		float counterStaminaHealValue;	// �J�E���^�[���̃X�^�~�i�񕜗�
		float attackMultiply;			// �U���̔{��
		float chargeTime;				// �`���[�W����
		float counterExtensionFrame;	// �J�E���^�[�P�\�t���[��
		float damageMitigation;			// �_���[�W�y����
		float addDownTime;				// �_�E�����ԕt�^
		bool bChargeFlinch;				// ���݃t���O

		sPlayerStatus() : respawnHeal(0.0f), guardStaminaSubValue(0.0f), counterStaminaSubValue(0.0f),
			counterStaminaHealValue(0.0f), attackMultiply(0.0f), chargeTime(0.0f),
			counterExtensionFrame(0.0f), damageMitigation(0.0f), addDownTime(0.0f), bChargeFlinch(false) {}

		sPlayerStatus(
			float resheal, float guardvalue, float countervalue, 
			float counterheal, float attackmultiply, float chargetime, 
			float countertime, float damagemitigation, float downtime, bool bFlinsh) : 
			respawnHeal(resheal), guardStaminaSubValue(guardvalue), counterStaminaSubValue(countervalue),
			counterStaminaHealValue(counterheal), attackMultiply(attackmultiply), chargeTime(chargetime),
			counterExtensionFrame(countertime), damageMitigation(damagemitigation), addDownTime(downtime), bChargeFlinch(bFlinsh) {}
	};

	// �_���[�W���
	struct sDamageInfo
	{
		bool bActiveSuperArmor;	// �X�[�p�[�A�[�}�[
		bool bReceived;			// �_���[�W�󂯕t������
		float reciveTime;		// �_���[�W��t����

		sDamageInfo() : bActiveSuperArmor(false), bReceived(false), reciveTime(0.0f) {}
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
	MyLib::HitResult_Character ProcessHit(const int nValue, const MyLib::Vector3& hitpos);

	STATE GetState();		// ��Ԏ擾
	void SetState(STATE state, int nCntState = 0);	// ��Ԑݒ�
	virtual void Kill();			// ���S����
	void SwitchRockOnTarget();		// ���b�N�Ώې؂�ւ�

	// ���[�V����
	void SetMotion(int motionIdx);								// ���[�V�����̐ݒ�
	bool IsJump() { return m_bJump; }							// �W�����v����
	bool IsAttacking() { return m_bAttacking; }					// �U�����t���O�擾
	float GetDashTime() { return m_fDashTime; }					// �_�b�V������
	void SetComboStage(int stage) { m_nComboStage = stage;; }	// �R���{�̒i�K�ݒ�
	int GetComboStage() { return m_nComboStage; }				// �R���{�̒i�K�擾
	bool IsReadyDashAtk() { return m_bReadyDashAtk; }
	void SetMotionFrag(SMotionFrag frag) { m_sMotionFrag = frag; }
	SMotionFrag GetMotionFrag() { return m_sMotionFrag; }
	void SetDamageInfo(sDamageInfo info) { m_sDamageInfo = info; }	// �_���[�W���ݒ�
	sDamageInfo GetDamageInfo() { return m_sDamageInfo; }			// �_���[�W���擾

	// �]�ڃr�[�R��
	void SetEnableTouchBeacon(bool bTouch) { m_bTouchBeacon = bTouch; }	// �r�[�R���ɐG��Ă锻��ݒ�
	bool IsTouchBeacon() { return m_bTouchBeacon; }						// �r�[�R���ɐG��Ă锻��擾

	// �X�L���|�C���g
	CSkillPoint* GetSkillPoint() { return m_pSkillPoint; }

	// �X�^�~�i
	CStaminaGauge_Player* GetStaminaGauge() { return m_pStaminaGauge; }

	//=============================
	// �X�L���c���[�p�֐�
	//=============================
	void ResetEnhance();										// �������Z�b�g
	void UpgradeLife(int addvalue);								// �̗̓A�b�v�O���[�h
	void UpgradeMaxStamina(int addvalue);						// �X�^�~�i�ő�l�A�b�v�O���[�h
	void UpgradeAutoHealStamina(float ratio);					// �X�^�~�i�����񕜃A�b�v�O���[�h
	void ChangeAtkControl(CPlayerControlAttack* control);		// �U���̑���ύX
	void ChangeDefenceControl(CPlayerControlDefence* control);	// �h��̑���ύX
	void ChangeAvoidControl(CPlayerControlAvoid* control);		// ����̑���ύX
	void ChangeGuardGrade(CPlayerGuard* guard);					// �K�[�h���\�ύX

	// ���X�|�[��
	void RespawnSetting();	// ���X�|�[�����̐ݒ�
	int GetRespawnPercent() { return m_nRespawnPercent; }						// ���X�|�[���m���擾
	void SetRespawnPercent(int value);											// ���X�|�[���m���ݒ�
	void AddRespawnPercent(int value);											// ���X�|�[���m�����Z
	void SetRespawnHeal(float value) { m_PlayerStatus.respawnHeal = value; }	// ���X�|�[�����̉񕜗ʐݒ�
	float GetRespawnHeal() { return m_PlayerStatus.respawnHeal; }				// ���X�|�[�����̉񕜗ʎ擾

	// ����
	bool IsLockOnAtStart() { return m_bLockOnAtStart; }												// �����J�n���Ƀ��b�N�I�����Ă�����
	void SetCounterSubValue(float value) { m_PlayerStatus.counterStaminaSubValue = value; }			// �J�E���^�[���̏���X�^�~�i�ݒ�
	float GetCounterSubValue() { return m_PlayerStatus.counterStaminaSubValue; }					// �J�E���^�[���̏���X�^�~�i�擾
	void SetCounterHealValue(float value) { m_PlayerStatus.counterStaminaHealValue = value; }		// �J�E���^�[���̃X�^�~�i�񕜐ݒ�
	float GetCounterHealValue() { return m_PlayerStatus.counterStaminaHealValue; }					// �J�E���^�[���̃X�^�~�i�񕜐ݎ擾
	void SetCounterExtensionFrame(float value) { m_PlayerStatus.counterExtensionFrame = value; }	// �J�E���^�[�P�\�t���[���ݒ�
	float GetCounterExtensionFrame() { return m_PlayerStatus.counterExtensionFrame; }				// �J�E���^�[�P�\�t���[���擾

	// �K�[�h
	void SetGuardSubValue(float value) { m_PlayerStatus.guardStaminaSubValue = value; }	// �K�[�h���̏���X�^�~�i�ݒ�
	float GetGuardSubValue() { return m_PlayerStatus.guardStaminaSubValue; }			// �K�[�h���̏���X�^�~�i�擾
	void SetGuardMitigation(float value) { m_PlayerStatus.damageMitigation = value; }	// �K�[�h���̌y�����ݒ�
	float GetGuardMitigation() { return m_PlayerStatus.damageMitigation; }				// �K�[�h���̌y�����擾

	// �U��
	void SetAttackMultiply(float value) { m_PlayerStatus.attackMultiply = value; }	// ��b�U���̔{���ݒ�
	float GetAttackMultiply() { return m_PlayerStatus.attackMultiply; }				// ��b�U���̔{���擾
	void SetToggleFlinch(bool bFlinch) { m_PlayerStatus.bChargeFlinch = bFlinch; }	// ���݃t���O�؂�ւ�
	bool IsFlinch() { return m_PlayerStatus.bChargeFlinch; }						// ���݃t���O�擾
	void SetAddDownTime(float time) { m_PlayerStatus.addDownTime = time; }			// �_�E���t�^���Ԑݒ�
	float GetAddDownTime() { return m_PlayerStatus.addDownTime; }					// �_�E���t�^���Ԏ擾
	void SetChargeTime(float time) { m_PlayerStatus.chargeTime = time; }			// �`���[�W���Ԑݒ�
	float GetChargeTime() { return m_PlayerStatus.chargeTime; }						// �`���[�W���Ԏ擾


	//=============================
	// ���̑�
	//=============================
	int GetMyPlayerIdx() { return m_nMyPlayerIdx; }

	// �_�E��
	void DownSetting(const MyLib::Vector3& hitpos);

	// �q�b�g�n
	void DeadSetting(MyLib::HitResult_Character* result);

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
	void UpdateState();		// ��ԍX�V
	void StateNone();		// �Ȃ�
	void StateInvincible();	// ���G
	void StateDamage();		// �_���[�W
	void StateDown();		// �_�E��
	void StateKnockBack();	// �m�b�N�o�b�N
	void StateDead();		// ���S
	void StateDeadWait();	// ���S�ҋ@
	void StateFadeOut();	// �t�F�[�h�A�E�g
	void StateRespawn();	// ���X�|�[��
	void StateCounter();	// �J�E���^�[��
	void StateAvoid();		// ���
	void StatePrayer();		// �F��
	void StateCharge();		// �`���[�W

	// ���̑��֐�
	virtual void Controll();	// ����
	void LimitPos();			// �ʒu����
	void MotionBySetState();	// ���[�V�����ʂ̏�Ԑݒ�
	void ResetFrag();			// �t���O���Z�b�g
	void RockOn();				// ���b�N�I��
	void UpdateDamageReciveTimer();	// �_���[�W��t���ԍX�V

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
	int m_nCntPowerEmission;		// �p���[�A�b�v�̔������J�E���^�[
	int m_nComboStage;				// �R���{�̒i�K
	int m_nIdxRockOn;				// ���b�N�I���Ώۂ̃C���f�b�N�X�ԍ�
	bool m_bLockOnAtStart;			// �J�E���^�[�J�n���Ƀ��b�N�I�����Ă�����
	bool m_bReadyDashAtk;			// �_�b�V���A�^�b�N�̃t���O
	bool m_bAttacking;				// �U����
	bool m_bCounterAccepting;		// �J�E���^�[��t��
	bool m_bDash;					// �_�b�V������
	float m_fDashTime;				// �_�b�V������
	float m_fChargeTime;			// �`���[�W����
	bool m_bChargeCompletion;		// �`���[�W�����t���O
	int m_nRespawnPercent;			// ���X�|�[���m��
	bool m_bTouchBeacon;			// �r�[�R���ɐG��Ă锻��
	bool m_bMotionAutoSet;			// ���[�V�����̎����ݒ�
	Effekseer::Handle m_WeaponHandle;

	sPlayerStatus m_PlayerStatus;	// �v���C���[�X�e�[�^�X
	sDamageInfo m_sDamageInfo;		// �_���[�W���

	CSkillPoint* m_pSkillPoint;		// �X�L���|�C���g�̃I�u�W�F�N�g
	CHP_GaugePlayer* m_pHPGauge;		// HP�Q�[�W�̃|�C���^
	CStaminaGauge_Player* m_pStaminaGauge;	// �X�^�~�i�Q�[�W�̃|�C���^
	
	// �p�^�[���p�C���X�^���X
	CEndCounterSetting* m_pEndCounterSetting;	// �J�E���^�[�I�����̐ݒ�
	CPlayerControlAttack* m_pControlAtk;		// �U������
	CPlayerControlDefence* m_pControlDefence;	// �h�䑀��
	CPlayerControlAvoid* m_pControlAvoid;		// ��𑀍�
	CPlayerGuard* m_pGuard;						// �K�[�h

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