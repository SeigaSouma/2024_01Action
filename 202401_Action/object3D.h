//=============================================================================
// 
//  �I�u�W�F�N�g3D�w�b�_�[ [object3D.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _OBJECT3D_H_
#define _OBJECT3D_H_	// ��d�C���N���[�h�h�~

#include "main.h"
#include "object.h"

//==========================================================================
// �N���X��`
//==========================================================================
// �I�u�W�F�N�g3D�N���X��`
class CObject3D : public CObject
{
public:

	CObject3D(int nPriority = 5);
	virtual ~CObject3D();

	// �I�[�o�[���C�h���ꂽ�֐�
	virtual HRESULT Init();
	virtual void Uninit();
	virtual void Update();
	virtual void Draw();
	virtual void SetVtx();
	virtual void BindTexture(int nIdx);

	void SetWorldMtx(const D3DXMATRIX mtx);		// �}�g���b�N�X�ݒ�
	D3DXMATRIX GetWorldMtx() const;			// �}�g���b�N�X�擾
	void SetOriginPosition(const MyLib::Vector3 pos);	//	���̈ʒu�ݒ�
	MyLib::Vector3 GetOriginPosition() const;		//	���̈ʒu�擾
	void SetOriginRotation(const MyLib::Vector3 rot);	// ���̌����ݒ�
	MyLib::Vector3 GetOriginRotation() const;		// ���̌����擾
	void SetColor(const D3DXCOLOR col);			// �F�ݒ�
	D3DXCOLOR GetColor() const;				// �F�擾
	void SetSize(const MyLib::Vector3 size);		// �T�C�Y�̐ݒ�
	MyLib::Vector3 GetSize() const;			// �T�C�Y�̎擾
	virtual void SetTex(D3DXVECTOR2 *tex);				// �e�N�X�`�����W�̐ݒ�
	virtual D3DXVECTOR2 *GetTex();					// �e�N�X�`�����W�̎擾

	static CObject3D *Create(int nPriority);
	static CObject3D *Create(MyLib::Vector3 pos, MyLib::Vector3 rot);
	CObject3D *GetObject3D();
	virtual CObject3DMesh *GetObject3DMesh();

protected:

	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;	// ���_�o�b�t�@

private:
	D3DXMATRIX	m_mtxWorld;		// ���[���h�}�g���b�N�X
	MyLib::Vector3 m_posOrigin;	// ���̈ʒu
	MyLib::Vector3 m_rotOrigin;	// ���̌���
	D3DXVECTOR2 m_fTex[32];		// �e�N�X�`�����W
	D3DXCOLOR m_col;			// �F
	MyLib::Vector3 m_fSize;		// �T�C�Y
	int m_nTexIdx;					// �e�N�X�`���̃C���f�b�N�X�ԍ�
};



#endif