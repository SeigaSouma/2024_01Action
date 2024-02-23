//=============================================================================
// 
//  �퓬�J�n�w�b�_�[ [battlestart.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _BATTLESTART_H_
#define _BATTLESTART_H_	// ��d�C���N���[�h�h�~

#include "object2d.h"

//==========================================================================
// �N���X��`
//==========================================================================
// �퓬�J�n�N���X
class CBattleStart : public CObject2D
{
public:

	CBattleStart(int nPriority = 7);
	~CBattleStart();


	// �I�[�o�[���C�h���ꂽ�֐�
	HRESULT Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;

	void Kill();

	static CBattleStart* Create(const MyLib::Vector3& pos);

private:

	//=============================
	// �񋓌^��`
	//=============================
	enum VTXTYPE
	{
		VTXTYPE_RIGHTUP = 0,	// �E��
		VTXTYPE_LEFTDW,		// ����
		VTXTYPE_MAX
	};

	enum STATE
	{
		STATE_GETTOGETHER = 0,	// �W�܂�
		STATE_CHARGE,	// ����
		STATE_STINGS,	// �h����
		STATE_SCALE,	// �g��
		STATE_FADEOUT,
		STATE_MAX
	};

	// �\���̒�`
	struct SRockOnMarker
	{
		CObject2D* p2D;	// 2D�I�u�W�F�̃|�C���^
		float distance;		// �Ԋu
		int Angle;			// ����
		float distancetimer;	// ��������

		SRockOnMarker() : p2D(nullptr), distance(0.0f), Angle(0), distancetimer(0.0f) {}
	};

	//=============================
	// �֐����X�g
	//=============================
	typedef void(CBattleStart::*STATE_FUNC)();
	static STATE_FUNC m_StateFuncList[];	// ��Ԋ֐��̃��X�g

	//=============================
	// �����o�֐�
	//=============================
	// ��Ԋ֐�
	void StateGetTogether();	// �W�܂�
	void StateCharge();			// ����
	void StateStings();	// �h����
	void StateScale();	// �g�k
	void StateFadeOut();	// �t�F�[�h�A�E�g

	//=============================
	// �����o�ϐ�
	//=============================
	STATE m_state;		// ���
	float m_fStateTime;	// ��ԃ^�C�}�[
	SRockOnMarker m_RockOnInfo[VTXTYPE_MAX];	// �}�[�J�[���
};


#endif