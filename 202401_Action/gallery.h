//=============================================================================
// 
//  �ϏO�w�b�_�[ [gallery.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _GALLERY_H_
#define _GALLERY_H_	// ��d�C���N���[�h�h�~

#include "object.h"
#include "listmanager.h"

//==========================================================================
// �O���錾
//==========================================================================
class CObjectX;

//==========================================================================
// �N���X��`
//==========================================================================
// �ϏO�N���X��`
class CGallery : public CObject
{
public:
	//=============================
	// �񋓌^��`
	//=============================
	// ���
	enum STATE
	{
		STATE_NONE = 0,	// �Ȃɂ��Ȃ�
		STATE_CLEARHEAT,		// �N���A����オ��
		STATE_COUNTERHEAT,		// �J�E���^�[����オ��
		STATE_MAX
	};

private:

	//=============================
	// �񋓌^��`
	//=============================
	// �F�̎��
	enum COLORTYPE
	{
		COLORTYPE_RED = 0,
		COLORTYPE_BLUE,
		COLORTYPE_GREEN,
		COLORTYPE_MAX
	};

	// �|�[�Y
	enum POSE
	{
		POSE_NONE = 0,
		POSE_RIGHT,
		POSE_LEFT,
		POSE_MAX
	};


	//=============================
	// �\���̒�`
	//=============================
	struct sGalleryInfo
	{
		CObjectX* objX;			// �I�u�W�F�N�gX�̃|�C���^
		COLORTYPE colorType;	// �F�̎��
		POSE poseType;			// �|�[�Y�̎��

		// �R���X�g���N�^
		sGalleryInfo() : objX(nullptr), colorType(COLORTYPE_RED), poseType(POSE_RIGHT) {}
	};

public:

	CGallery(int nPriority = mylib_const::PRIORITY_DEFAULT);
	~CGallery();

	// �I�[�o�[���C�h���ꂽ�֐�
	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;

	void Kill(void);	// �폜
	void SetState(STATE state);	// ��Ԑݒ�
	static CGallery* Create(const MyLib::Vector3& pos, const MyLib::Vector3& rot);	// ����
	static CListManager<CGallery> GetList() { return m_List; }
	static void SetGallery(void);
private:

	//=============================
	// �֐����X�g
	//=============================
	typedef void(CGallery::* STATE_FUNC)(void);
	static STATE_FUNC m_StateFunc[];

	//=============================
	// �����o�֐�
	//=============================
	// ��Ԋ֐�
	void StateNone() {}
	void StateClearHeat();
	void StateCounterHeat();

	// ���̑�
	void Jump();

	//=============================
	// �����o�ϐ�
	//=============================
	STATE m_State;	// ���
	float m_fStateTime;	// ��ԃJ�E���^�[
	std::vector<sGalleryInfo> m_GalleryInfo;	// �ϏO���
	static std::vector<int> m_nModelIdx;		// ���f���C���f�b�N�X�ԍ�
	static CListManager<CGallery> m_List;		// ���X�g
};


#endif