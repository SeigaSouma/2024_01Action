//=============================================================================
// 
//  ���b�N�I���}�[�J�[�w�b�_�[ [rockon_marker.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _ROCKON_MARKER_H_
#define _ROCKON_MARKER_H_	// ��d�C���N���[�h�h�~

#include "objectBillboard.h"

//==========================================================================
// �N���X��`
//==========================================================================
// ���b�N�I���}�[�J�[�N���X
class CRockOnMarker : public CObject
{
public:

	CRockOnMarker(int nPriority = 7);
	~CRockOnMarker();


	// �I�[�o�[���C�h���ꂽ�֐�
	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;

	void Kill(void);

	static CRockOnMarker* Create(const MyLib::Vector3& pos);

private:

	//=============================
	// �񋓌^��`
	//=============================
	enum VTXTYPE
	{
		VTXTYPE_RIGHTUP = 0,	// �E��
		VTXTYPE_RIGHTDW,		// �E��
		VTXTYPE_LEFTDW,		// ����
		VTXTYPE_LEFTUP,		// ����
		VTXTYPE_MAX
	};

	// �\���̒�`
	struct SRockOnMarker
	{
		CObjectBillboard *pBillboard;	// �r���{�[�h�̃I�u�W�F�N�g
		float distance;		// �Ԋu
		int Angle;			// ����
		int HeightAngle;	// �����̌���
		float distancetimer;	// ��������

		SRockOnMarker() : pBillboard(nullptr), distance(0.0f), Angle(0), HeightAngle(0), distancetimer(0.0f) {}
	};

	//=============================
	// �����o�֐�
	//=============================

	//=============================
	// �����o�ϐ�
	//=============================
	SRockOnMarker m_RockOnInfo[VTXTYPE_MAX];	// �}�[�J�[���
};


#endif