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

// �O���錾
class CHP_Gauge;
class CShadow;
class CEffect3D;
class CEnemyFixedMoveManager;
class CRockOnMarker;

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
		TYPE_BOSS = 0,	// �{�X
		TYPE_COOKIE,	// �N�b�L�[
		TYPE_ORAFU,		// �I���t
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
		STATE_PLAYERCHASE,	// �v���C���[�ǂ��|��
		STATE_PARENTCHASE,	// �e�ǂ��|��
		STATE_ATTACK,		// �U��
		STATE_WAIT,			// �ҋ@
		STATE_DOWN,			// �_�E��
		STATE_MAX
	};

	// �s����
	enum ACTION
	{
		ACTION_DEF = 0,	// �ҋ@�s��
		ACTION_ATTACK,	// �U���s��
		ACTION_SEARCH,	// ���G�s��
		ACTION_MAX
	};

	CEnemy(int nPriority = mylib_const::ENEMY_PRIORITY);
	virtual ~CEnemy();


	// �I�[�o�[���C�h���ꂽ�֐�
	virtual HRESULT Init() override;
	virtual void Uninit() override;
	virtual void Update() override;
	virtual void Draw() override;

	void SetState(STATE state);		// ��Ԑݒ�
	STATE GetState() { return m_state; }
	virtual bool Hit(const int nValue, CGameManager::AttackType atkType = CGameManager::ATTACK_NORMAL);
	virtual void NormalHitResponse();	// �q�b�g���̔���
	virtual void CounterHitResponse();	// �q�b�g���̔���

	void SetSpawnPosition(MyLib::Vector3 pos);	// �X�|�[���n�_�ݒ�
	MyLib::Vector3 GetSpawnPosition();	// �X�|�[���n�_�擾
	void SetTargetPosition(MyLib::Vector3 pos) { m_TargetPosition = pos; }	// �ڕW�̈ʒu�ݒ�
	void SetEnableRockOn(bool bSet) { m_bRockOnAccepting = bSet; }
	bool IsRockOnAccept() { return m_bRockOnAccepting; }


	// �G�t�F�N�V�A
	Effekseer::Handle GetEfkHandle() { return m_pWeaponHandle; }
	void SetEfkHandle(Effekseer::Handle handle) { m_pWeaponHandle = handle; }

	// ���[�V����
	void SetMotion(int motionIdx);	// ���[�V�����̐ݒ�

	HRESULT RoadText(const char *pFileName);
	virtual void Kill();	// �폜
	void SetParent(CEnemy *pParent);		// �e�̃|�C���^�ݒ�
	void SetOriginRotation(MyLib::Vector3 rot);	// ���̌���
	CEnemy *GetEnemy();
	TYPE GetType() { return m_type; }	// ��ގ擾
	static CListManager<CEnemy> GetListObj() { return m_List; }	// ���X�g�擾
	static CEnemy* Create(const char* pFileName, MyLib::Vector3 pos, TYPE type = TYPE_BOSS);

protected:

	// ���_�ʒu�̎��
	enum eBaseType
	{
		BASETYPE_MAP = 0,	// �}�b�v�ɉ����Ă���
		BASETYPE_ORIGIN,	// �o���ʒu
		BASETYPE_MAX
	};

	// ���[�V�����̔���
	struct SMotionFrag
	{
		bool bJump;			// �W�����v�����ǂ���
		bool bATK;			// �U�������ǂ���
		bool bKnockback;	// �m�b�N�o�b�N�����ǂ���
		bool bMove;			// �ړ������ǂ���
		bool bCharge;		// �`���[�W�����ǂ���
	};

	// ����̍\���̒�`
	struct SFormationInfo
	{
		MyLib::Vector3 pos;	// ����̈ʒu
		float fAngle;		// ����
		float fLength;		// ����
	};

	virtual void ProcessLanding();	// ���n������
	virtual void AttackAction(CMotion::AttackInfo ATKInfo, int nCntATK) override;		// �U��������
	virtual void AttackInDicision(CMotion::AttackInfo* pATKInfo, int nCntATK) override;	// �U�����蒆����
	virtual void ChaseMove(float fMove);	// �ǂ��|���ړ�
	virtual void RotationTarget();		// �^�[�Q�b�g�̕�������
	virtual bool CalcLenPlayer(float fLen); // �v���C���[�Ƃ̋�������
	virtual void MoveRotation();		// �ړ�����������
	virtual void Move();				// �ړ�

	virtual void UpdateAction();		// �s���X�V
	virtual void ActionSet() = 0;		// �s���̐ݒ�

	// ��ԍX�V�n
	virtual void StateNone();			// �����Ȃ����
	virtual void SpawnWait();			// �X�|�[���ҋ@
	virtual void Spawn();				// �X�|�[��
	virtual void Damage();				// �_���[�W
	virtual void Dead();				// ���S
	virtual void FadeOut();				// �t�F�[�h�A�E�g
	virtual void PlayerChase();			// �v���C���[�Ǐ]
	virtual void ParentChase();			// �e�Ǐ]
	virtual void StateAttack();			// �U������
	virtual void TriggerChasePlayer();	// �v���C���[�Ǐ]ON�ɂ���g���K�[
	virtual void ChangeToAttackState();	// �U����Ԉڍs����
	virtual void StateWait();			// �ҋ@����
	virtual void StateDown();			// �_�E�����

	STATE m_state;							// ���
	STATE m_Oldstate;						// �O��̏��
	float m_fStateTime;						// ��ԃJ�E���^�[
	int m_nTargetPlayerIndex;				// �ǂ��|����v���C���[�̃C���f�b�N�X�ԍ�
	float m_fActCounter;					// �ړ��J�E���^�[
	bool m_bActionable;						// �s���\��
	float m_fDownTime;						// �_�E���J�E���^�[
	float m_fRockOnDistance;				// ���b�N�I���̋���
	MyLib::Vector3 m_posOrigin;				// �ŏ��̈ʒu
	MyLib::Vector3 m_posKnokBack;			// �m�b�N�o�b�N�̈ʒu
	SMotionFrag m_sMotionFrag;				// ���[�V�����̃t���O
	CHP_Gauge *m_pHPGauge;					// HP�Q�[�W�̏��
	CEnemy *m_pParent;						// �e�̃|�C���^
	D3DXCOLOR m_mMatcol;					// �}�e���A���̐F
	MyLib::Vector3 m_TargetPosition;		// �ڕW�̈ʒu
	Effekseer::Handle m_pWeaponHandle;		// �G�t�F�N�g�̕���n���h��
private:

	enum MOTION
	{
		MOTION_DEF = 0,		// �j���[�g�������[�V����
		MOTION_WALK,		// �ړ����[�V����
		MOTION_ATK,			// �U��
		MOTION_DMG,			// �_���[�W
		MOTION_DOWN,		// �_�E��
		MOTION_KNOCKBACK,	// ���ꃂ�[�V����
		MOTION_FADEOUT,		// �y�A��
		MOTION_MAX
	};
	
	void UpdateState();					// ��ԍX�V����
	virtual void UpdateStateByType();	// ��ޕʏ�ԍX�V����
	virtual void UpdateByType();		// ��ޕʍX�V����
	void Collision();					// �����蔻��
	virtual void MotionSet() = 0;		// ���[�V�����̐ݒ�
	void RegistrChild(CEnemy *pChild);
	void ResetChild(CEnemy *pChild);
	void LimitArea(); // ��l�̕ǔ���

	//=============================
	// �����o�ϐ�
	//=============================
	TYPE m_type;			// ���
	SFormationInfo m_sFormationInfo;	// ����̏��
	MyLib::Vector3 m_rotOrigin;	// �ŏ��̌���
	int m_nTexIdx;				// �e�N�X�`���̃C���f�b�N�X�ԍ�
	int m_nNumChild;			// �q�̐�
	bool m_bAddScore;			// �X�R�A���Z���邩�̔���
	bool m_bRockOnAccepting;	// ���b�N�I����t
	int m_nBallastEmission;		// ���I�̔����J�E���^�[
	CEnemy *m_pChild[mylib_const::MAX_ENEMY];	// �q�̃|�C���^
	CShadow *m_pShadow;			// �e�̏��
	CRockOnMarker* m_pRockOnMarker;		// ���b�N�I���}�[�J�[
	static CListManager<CEnemy> m_List;	// ���X�g
};



#endif