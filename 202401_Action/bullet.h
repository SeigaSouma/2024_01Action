//=============================================================================
// 
//  �e�w�b�_�[ [bullet.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _BULLET_H_
#define _BULLET_H_	// ��d�C���N���[�h�h�~

#include "listmanager.h"
#include "meshsphere.h"

class CThunderRing;
class CObjectBillboard;

//==========================================================================
// �N���X��`
//==========================================================================
// �e�N���X��`
class CBullet : public CMeshSphere
{
public:

	// �񋓌^��`
	enum TYPE
	{
		TYPE_PLAYER = 0,	// �v���C���[�̒e
		TYPE_ENEMY,		// �G�̒e
		TYPE_MAX
	};

	enum MOVETYPE
	{
		MOVETYPE_NORMAL = 0,	// �ʏ�
		MOVETYPE_PARABOLA,		// ������
		MOVETYPE_MAX
	};

	enum STATE
	{
		STATE_NONE = 0,	// �Ȃɂ��Ȃ�
		STATE_DMG,		// ���˕Ԃ�
		STATE_MAX
	};

	CBullet(int nPriority = 6);
	~CBullet();


	// �I�[�o�[���C�h���ꂽ�֐�
	virtual HRESULT Init();
	virtual void Uninit();
	virtual void Update();
	virtual void Draw();
	void SetVtx();

	void SetType(TYPE type);	// ��ސݒ�
	TYPE GetType();			// ��ގ擾
	void SetState(STATE state, int nCntState);	// ��Ԑݒ�
	STATE GetState();	// ��Ԏ擾
	void SetTargetPosition(MyLib::Vector3 pos);	// �ڕW�̈ʒu
	void SetParabolaHeight(float fHeight);		// �������̍ő卂��

	void SetReverseAutoDeath() { m_bAutoDeath = m_bAutoDeath ?  false : true; }	// �����폜�̔���폜
	bool IsFinish() { return m_bFinish; }					// �I���̔���

	static int GetNumAll();
	static CBullet *Create(TYPE type, MOVETYPE movetype, const MyLib::Vector3 pos, const MyLib::Vector3 rot, const MyLib::Vector3 move, const float fSize);
	static CListManager<CBullet> GetListObj() { return m_List; }	// ���X�g�擾

private:
	void UpdatePos();		// �ړ�
	void UpdateTypePlayer();	// �v���C���[�e�̍X�V
	void CollisionPlayer();		// �v���C���[�Ƃ̔���
	void CollisionEnemy();		// �G�Ƃ̔���
	void StateNone();		// �����Ȃ����
	void StateDamage();		// �_���[�W��ԏ���

	typedef void(CBullet::* STATE_FUNC)();
	typedef void(CBullet::* COLLISION_FUNC)();
	static STATE_FUNC m_FuncList[];
	static COLLISION_FUNC m_CollisionFuncList[];	// �����蔻��̃��X�g

	TYPE m_type;				// �e�̎��
	STATE m_state;				// ���
	MOVETYPE m_MoveType;		// �ړ��̎��
	MyLib::Vector3 m_OriginPosition;	// ���̈ʒu
	MyLib::Vector3 m_TargetPosition;	// �ڕW�̈ʒu
	int m_nCntState;			// ��ԑJ�ڃJ�E���^�[
	int m_nLife;				// ����
	int m_nLifeMax;				// �����̍ő�l
	int m_nTexIdx;				// �e�N�X�`���̃C���f�b�N�X�ԍ�
	int m_nCntEmission;			// �������̃J�E���^�[
	float m_fMaxParabolaHeight;	// �������̍ő卂��
	bool m_bAutoDeath;		// �����폜�̃t���O
	bool m_bFinish;			// �I���������
	static int m_nNumAll;		// �e�̑���
	static const char *m_apTextureFile[TYPE_MAX];	// �e�N�X�`���̃t�@�C��
	static CListManager<CBullet> m_List;	// ���X�g
};


#endif