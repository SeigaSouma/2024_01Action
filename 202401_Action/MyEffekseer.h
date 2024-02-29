//=============================================================================
// 
//  �G�t�F�N�V�A�w�b�_�[ [MyEffekseer.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _MYEFFEKSEER_H_
#define _MYEFFEKSEER_H_	// ��d�C���N���[�h�h�~

#include <EffekseerRendererDX9.h>
#include <XAudio2.h>
#include <d3d9.h>
#include <functional>
#include <wrl/client.h>
#include "listmanager.h"

//==========================================================================
// �N���X��`
//==========================================================================
// �G�t�F�N�V�A�N���X��`
class CMyEffekseer
{
public:
	
	struct MyEffekseerInfo
	{

		MyLib::Vector3 pos;
		MyLib::Vector3 rot;
		float scale;
		MyLib::Vector3 move;		// �ړ���
		Effekseer::EffectRef efcRef;



		Effekseer::Handle handle;	// �G�t�F�N�g�̃I�u�W�F�N�g
		bool bAutoDeath;			// �����폜�̃t���O
	};

	enum EFKLABEL
	{
		EFKLABEL_SAMPLE_LASER = 0,	// �T���v���̃��[�U�[
		EFKLABEL_COUNTERLINE,		// �J�E���^�[�̐�
		EFKLABEL_COUNTERLINE2,		// �J�E���^�[�̐�2
		EFKLABEL_COUNTER_KRKR,		// �J�E���^�[�̃L���L��
		EFKLABEL_COUNTER_BREAK,		// �J�E���^�[�u���C�N
		EFKLABEL_BGFIRE,			// �w�i�̉�
		EFKLABEL_HITMARK_RED,		// �q�b�g�}�[�N[��]
		EFKLABEL_STRONGATK_SIGN,		// ���U���̃T�C��
		EFKLABEL_BOSS_ROLLING,		// �{�X�̃��[�����O
		EFKLABEL_STONEBASE_LIGHT,		// �Δ̌�
		EFKLABEL_STONEBASE_BEGIN,		// �ΔN��
		EFKLABEL_TRANSFER,				// �]��
		EFKLABEL_CHARGEFINISH,			// �`���[�W����
		EFKLABEL_CHARGE,			// �`���[�W������
		EFKLABEL_TITLEBLUR,			// �^�C�g���̌�
		EFKLABEL_ENEMYATK_SAND,			// �G�U���̉�
		EFKLABEL_NORMALATK_LEFT,		// �ʏ�U�����U��
		EFKLABEL_NORMALATK_RIGHT,		// �ʏ�U���E�U��
		EFKLABEL_NORMALATK,				// �ʏ�U��
		EFKLABEL_CHARGEATK,				// �`���[�W�U��
		EFKLABEL_RESPAWN_START,			// �����J�n
		EFKLABEL_RESPAWN_WIND,			// �����C���p�N�g
		EFKLABEL_MAX
	};

	template <typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	CMyEffekseer();
	~CMyEffekseer();

	void Uninit();
	void Update();

	/**
	@brief	�G�t�F�N�g�̐ݒ�
	@details �p�X�̊g���q��[.efkefc]�ł�
	@param	efkpath		[in]	�ǂݍ��ރG�t�F�N�g�̃p�X
	@param	pos			[in]	�ʒu
	@param	rot			[in]	����
	@param	move		[in]	�ړ���
	@param	scale		[in]	�g�嗦
	@param	bAutoDeath	[in]	�����폜
	@return	Effekseer::Handle
	*/
	Effekseer::Handle SetEffect(EFKLABEL label, MyLib::Vector3 pos, MyLib::Vector3 rot, MyLib::Vector3 move, float scale, bool bAutoDeath = true);
	Effekseer::Handle SetEffect(Effekseer::Handle **pHandle, EFKLABEL label, MyLib::Vector3 pos, MyLib::Vector3 rot, MyLib::Vector3 move, float scale, bool bAutoDeath = true);
	void SetPosition(Effekseer::Handle handle, MyLib::Vector3 pos);	// �ʒu�ݒ�
	void SetRotation(Effekseer::Handle handle, MyLib::Vector3 rot);	// �����ݒ�
	void SetMatrix(Effekseer::Handle handle, D3DXMATRIX mtx);	// �}�g���b�N�X�ݒ�
	void SetScale(Effekseer::Handle handle, float scale);	// �X�P�[���ݒ�
	Effekseer::Matrix43 GetMatrix(Effekseer::Handle handle);	// �}�g���b�N�X�擾
	void SetTransform(Effekseer::Handle handle, MyLib::Vector3 pos, MyLib::Vector3 rot);	// �}�g���b�N�X�ݒ�
	bool IsDeath(Effekseer::Handle handle);	// ���S�t���O
	void StopAll();	// �S�Ē�~
	void Stop(Effekseer::Handle handle);	// ��~
	void SetTrigger(Effekseer::Handle handle, int idx);

	static CMyEffekseer* GetInstance() { return m_pMyEffekseer; }	// �C���X�^���X�擾
	static CMyEffekseer* Create();								// �C���X�^���X����

private:

	// �����o�֐�
	HRESULT Init();
	void Draw();
	void SetupEffekseerModules(::Effekseer::ManagerRef efkManager);	// ���W���[���̃Z�b�g�A�b�v
	::EffekseerRendererDX9::RendererRef GetEffekseerRenderer() { return efkRenderer; }	// �G�t�F�N�V�A�̃����_���[�擾
	Effekseer::EffectRef LoadEffect(std::string efkpath);
	
	// �����o�ϐ�
	int32_t time = 0;
	Effekseer::Handle efkHandle = 0;
	EffekseerRendererDX9::RendererRef efkRenderer;
	Effekseer::ManagerRef efkManager;
	Effekseer::Vector3D viewerPosition;
	Effekseer::Matrix44 projectionMatrix;
	Effekseer::Matrix44 cameraMatrix;
	std::function<void()> onLostDevice;
	std::function<void()> onResetDevice;

	// ����ϐ�
	Effekseer::EffectRef m_LoadEffect;				// �ǂݍ��񂾃G�t�F�N�g�̃X�}�[�g�|�C���^
	std::vector<MyEffekseerInfo> m_EffectObj;		// �G�t�F�N�g�̃I�u�W�F�N�g
	std::vector<Effekseer::Handle> m_Handle;		// �G�t�F�N�g�̃n���h��
	static std::string m_EffectName[EFKLABEL_MAX];	// �G�t�F�N�g�̃t�@�C����
	static CMyEffekseer* m_pMyEffekseer;			// ���g�̃|�C���^
};



#endif