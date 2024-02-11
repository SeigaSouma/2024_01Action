//=============================================================================
// 
//  �����w�b�_�[ [number.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _NUMBER_H_
#define _NUMBER_H_	// ��d�C���N���[�h�h�~

#include "main.h"
#include "object2D.h"
#include "object3D.h"

//==========================================================================
// �O���錾
//==========================================================================
class CObject2D;
class CObjectBillboard;

//==========================================================================
// �N���X��`
//==========================================================================
// �����N���X��`
class CNumber
{
public:

	// �񋓌^��`
	enum EObjectType
	{
		OBJECTTYPE_2D = 0,		// 2D
		OBJECTTYPE_3D,			// 3D
		OBJECTTYPE_BILLBOARD,	// �r���{�[�h
		OBJECTTYPE_MAX
	};

	CNumber(int nPriority = 6);
	~CNumber();

	static CNumber *Create(EObjectType objtype, int nPriority);

	// �����o�֐�
	virtual HRESULT Init(int nPriority) = 0;
	virtual void Uninit() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual void Release() = 0;	// �J������


	virtual void SetPosition(const MyLib::Vector3 pos);	// �ʒu�ݒ�
	virtual MyLib::Vector3 GetPosition() const;		// �ʒu�擾
	virtual void SetMove(const MyLib::Vector3 move);		// �ړ��ʐݒ�
	virtual MyLib::Vector3 GetMove() const;			// �ړ��ʎ擾
	virtual void SetRotation(const MyLib::Vector3 rot);	// �����ݒ�
	virtual MyLib::Vector3 GetRotation() const;		// �����擾

	virtual void SetColor(const D3DXCOLOR col);			// �F�ݒ�
	virtual D3DXCOLOR GetColor() const;				// �F�擾
	virtual void SetSize(const D3DXVECTOR2 size);		// �T�C�Y�̐ݒ�
	virtual D3DXVECTOR2 GetSize() const;			// �T�C�Y�̎擾
	virtual void SetSizeOrigin(const D3DXVECTOR2 size);	// ���̃T�C�Y�̐ݒ�
	virtual D3DXVECTOR2 GetSizeOrigin() const;		// ���̃T�C�Y�̎擾
	virtual void SetSize3D(const MyLib::Vector3 size);		// �T�C�Y�̐ݒ�
	virtual MyLib::Vector3 GetSize3D() const;			// �T�C�Y�̎擾
	virtual void SetTex(D3DXVECTOR2 *tex);				// �e�N�X�`�����W�̐ݒ�
	virtual D3DXVECTOR2 *GetTex();					// �e�N�X�`�����W�̎擾

	virtual void SetVtx() = 0;
	virtual void BindTexture(int nIdx) = 0;
	virtual void SetType(const CObject::TYPE type) = 0;

	virtual CObject2D *GetObject2D();
	virtual CObject3D *GetObject3D();
	virtual CObjectBillboard *GetObjectBillboard();

private:
	EObjectType m_objType;				// �I�u�W�F�N�g�̎��
};

#endif