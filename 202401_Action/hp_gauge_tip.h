//=============================================================================
// 
//  HP�Q�[�W�̐�[�w�b�_�[ [hp_gauge_tip.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _HPGAUGE_TIP_H_
#define _HPGAUGE_TIP_H_	// ��d�C���N���[�h�h�~

#include "object2d.h"

//==========================================================================
// �N���X��`
//==========================================================================
// HP�Q�[�W�̐�[�N���X
class CHPGaugeTip : public CObject
{
public:

	CHPGaugeTip(int nPriority = 7);
	~CHPGaugeTip();


	// �I�[�o�[���C�h���ꂽ�֐�
	HRESULT Init() override { return S_OK; }
	HRESULT Init(const MyLib::Vector3& leftpos, const MyLib::Vector3& rightpos);
	void Uninit() override;
	void Update() override;
	void Draw() override {}

	void Kill();

	void SetLeftPosition(const MyLib::Vector3& pos);
	void SetRightPosition(const MyLib::Vector3& pos);
	static CHPGaugeTip* Create(const MyLib::Vector3& leftpos, const MyLib::Vector3& rightpos);

private:

	//=============================
	// �񋓌^��`
	//=============================
	enum VTXTYPE
	{
		VTXTYPE_LEFT = 0,	// ��
		VTXTYPE_RIGHT,		// �E
		VTXTYPE_MAX
	};

	//=============================
	// �����o�ϐ�
	//=============================
	CObject2D* m_pObj2D[VTXTYPE_MAX];	// �I�u�W�F�N�g2D�̃|�C���^
};


#endif