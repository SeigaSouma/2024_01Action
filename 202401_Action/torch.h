//=============================================================================
// 
//  �����w�b�_�[ [torch.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _TORCH_H_
#define _TORCH_H_	// ��d�C���N���[�h�h�~

#include "objectX.h"
#include "listmanager.h"

//==========================================================================
// �N���X��`
//==========================================================================
// �����N���X
class CTorch : public CObjectX
{
public:

	enum TYPE
	{
		TYPE_WALL = 0,	// �Ǌ|��
		TYPE_STAND,		// �u���^
		TYPE_MAX
	};

	CTorch(int nPriority = 3);
	~CTorch();

	// �I�[�o�[���C�h���ꂽ�֐�
	virtual HRESULT Init(void) override;
	virtual void Uninit(void) override;
	virtual void Update(void) override {}

	void Kill(void);	// �폜

	// �ÓI�֐�
	static CTorch *Create(TYPE type, const MyLib::Vector3& pos, const MyLib::Vector3& rot);
	static void SetTorch(void);

	static CListManager<CTorch> GetList() { return m_List; }

protected:
	int m_nCntFire;

private:

	//=============================
	// �����o�֐�
	//=============================
	static HRESULT ReadXFile(const char* pTextFile);	// ���f���ǂݍ��ݏ���

	//=============================
	// �����o�ϐ�
	//=============================
	static std::vector<std::string> ModelFile;		// ���f���t�@�C����
	static CListManager<CTorch> m_List;		// ���X�g

};

class CTorchStand : public CTorch
{
public:
	CTorchStand() {}

	// �I�[�o�[���C�h���ꂽ�֐�
	virtual HRESULT Init(void) override;
	virtual void Update(void) override;
};

class CTorchWall : public CTorch
{
public:
	CTorchWall() {}

	// �I�[�o�[���C�h���ꂽ�֐�
	virtual HRESULT Init(void) override;
	virtual void Update(void) override;
};

#endif