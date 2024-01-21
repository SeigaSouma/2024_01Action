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
class CSkillPoint;

//==========================================================================
// �N���X��`
//==========================================================================
// �v���C���[�N���X��`
class CPlayer : public CObjectChara
{
protected:
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
		MOTION_COUNTER_ACCEPT,		// �����󂯕t��
		MOTION_COUNTER_TURN,		// �����󂯗���
		MOTION_COUNTER_ATTACK,		// ����
		MOTION_MAX
	};

	// ���[�V�����̔���
	struct SMotionFrag
	{
		bool bJump;			// �W�����v��
		bool bATK;			// �U����
		bool bCounter;		// �J�E���^�[��
		bool bKnockBack;	// �m�b�N�o�b�N��
		bool bDead;			// ���S��
		bool bMove;			// �ړ���
	};

public:

	// ��Ԓ�`
	enum STATE
	{
		STATE_NONE = 0,		// �Ȃɂ��Ȃ�
		STATE_INVINCIBLE,	// ���G
		STATE_DMG,			// �_���[�W
		STATE_KNOCKBACK,	// �m�b�N�o�b�N
		STATE_DEAD,			// ��
		STATE_FADEOUT,		// �t�F�[�h�A�E�g
		STATE_COUNTER,		// ����
		STATE_AVOID,		// ���
		STATE_MAX
	};

	CPlayer(int nPriority = mylib_const::PRIORITY_DEF2D);
	~CPlayer();

	// �I�[�o�[���C�h���ꂽ�֐�
	virtual HRESULT Init(void) override;
	virtual void Uninit(void) override;
	virtual void Update(void) override;
	virtual void Draw(void) override;

	bool Hit(const int nValue, CGameManager::AttackType atkType = CGameManager::ATTACK_NORMAL);	// �q�b�g����
	STATE GetState(void);		// ��Ԏ擾
	void SetState(STATE state, int nCntState = 0);	// ��Ԑݒ�
	virtual void Kill(void);			// ���S����
	void SwitchRockOnTarget(void);		// ���b�N�Ώې؂�ւ�

	// �]�ڃr�[�R��
	void SetEnableTouchBeacon(bool bTouch) { m_bTouchBeacon = bTouch; }	// �r�[�R���ɐG��Ă锻��ݒ�
	bool IsTouchBeacon(void) { return m_bTouchBeacon; }	// �r�[�R���ɐG��Ă锻��擾

	// �X�L���|�C���g
	CSkillPoint* GetSkillPoint(void) { return m_pSkillPoint; }

	static CPlayer* Create(int nIdx);	// ����
	static CListManager<CPlayer> GetListObj(void) { return m_List; }	// ���X�g�擾

protected:

	bool Collision(MyLib::Vector3 &pos, MyLib::Vector3 &move);	// �����蔻��
	void MotionSet(void);	// ���[�V�����̐ݒ�

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

	// �֐����X�g
	typedef void(CPlayer::* STATE_FUNC)(void);
	static STATE_FUNC m_StateFunc[];

	// �����o�֐�
	void StateNone(void);		// �Ȃ�
	void StateInvincible(void);	// ���G
	void StateDamage(void);		// �_���[�W
	void StateKnockBack(void);	// �m�b�N�o�b�N
	void StateDead(void);		// ���S
	void StateFadeOut(void);	// �t�F�[�h�A�E�g
	void StateCounter(void);	// �J�E���^�[��
	void StateAvoid(void);		// ���

	virtual void Controll(void);	// ����
	void LimitPos(void);			// �ʒu����
	void MotionBySetState(void);	// ���[�V�����ʂ̏�Ԑݒ�
	void ResetFrag(void);			// �t���O���Z�b�g
	void RockOn(void);				// ���b�N�I��

	void AttackAction(CMotion::AttackInfo ATKInfo, int nCntATK) override;	// �U��������
	void AttackInDicision(CMotion::AttackInfo ATKInfo, int nCntATK) override;			// �U�����蒆����

	STATE m_Oldstate;				// �O��̏��
	D3DXCOLOR m_mMatcol;			// �}�e���A���̐F
	MyLib::Vector3 m_posKnokBack;	// �m�b�N�o�b�N�̈ʒu
	MyLib::Vector3 m_KnokBackMove;	// �m�b�N�o�b�N�̈ړ���
	int m_nCntState;				// ��ԑJ�ڃJ�E���^�[
	int m_nComboStage;				// �R���{�̒i�K
	int m_nIdxRockOn;				// ���b�N�I���Ώۂ̃C���f�b�N�X�ԍ�
	bool m_bAttacking;				// �U����
	bool m_bCounterAccepting;		// �J�E���^�[��t��
	bool m_bDash;					// �_�b�V������
	float m_fDashTime;				// �_�b�V������
	bool m_bTouchBeacon;			// �r�[�R���ɐG��Ă锻��
	CSkillPoint* m_pSkillPoint;		// �X�L���|�C���g�̃I�u�W�F�N�g
	CHP_GaugePlayer* m_pHPGauge;		// HP�Q�[�W�̃|�C���^
	Effekseer::Handle *m_pWeaponHandle;	// �G�t�F�N�g�̕���n���h��
	static CListManager<CPlayer> m_List;	// ���X�g
};



#endif