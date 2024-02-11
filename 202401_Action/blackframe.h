//=============================================================================
// 
//  ���t���[���w�b�_�[ [blackframe.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _BLACKFRAME_H_
#define _BLACKFRAME_H_	// ��d�C���N���[�h�h�~

#include "main.h"
#include "object.h"

//==========================================================================
// �O���錾
//==========================================================================
class CObject2D;

//==========================================================================
// �}�N����`
//==========================================================================

//==========================================================================
// �N���X��`
//==========================================================================
// ���t���[���N���X��`
class CBlackFrame
{
public:

	// �񋓌^��`
	enum STATE
	{
		STATE_NONE = 0,
		STATE_IN,				// �N��
		STATE_OUT,				// �J����
		STATE_INCOMPLETION,		// �N������
		STATE_OUTCOMPLETION,	// �J���銮��
		STATE_MAX
	};

	CBlackFrame(int nPriority = 8);
	~CBlackFrame();

	static CBlackFrame *Create();

	// �I�[�o�[���C�h���ꂽ�֐�
	HRESULT Init();
	void Uninit();
	void Update();
	void Draw();

	void SetState(STATE state);	// ��Ԑݒ�
	STATE GetState() { return m_state; }	// ��Ԑݒ�
	void Reset();			// ���Z�b�g
private:
	// �񋓌^��`
	enum VTX
	{
		VTX_UP = 0,		// ��
		VTX_DOWN,		// ��
		VTX_MAX
	};

	// �����o�֐�
	void UpdateState(int nCntVtx);	// ��ԍX�V����
	void StateIn(int nCntVtx);	// �N��
	void StateOut(int nCntVtx);	// �J��

	// �����o�ϐ�
	STATE m_state;	// ���
	int m_nCntMove;	// �ړ��J�E���g
	CObject2D *m_pObj3D[VTX_MAX];	// �I�u�W�F�N�g2D�̃I�u�W�F�N�g
	static const MyLib::Vector3 m_DestPosition[VTX_MAX];	// �ڕW�̈ʒu
};



#endif