//=============================================================================
// 
//  �I�u�W�F�N�g2D�w�b�_�[ [object2D_Multi.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _OBJECT2D_MULTI_H_
#define _OBJECT2D_MULTI_H_	// ��d�C���N���[�h�h�~

#include "main.h"
#include "object.h"

class CEnemy;

//==========================================================================
// �N���X��`
//==========================================================================
// �I�u�W�F�N�g2D�N���X��`
class CObject2DMulti : public CObject
{
public:

	CObject2DMulti(int nPriority = mylib_const::PRIORITY_DEF2D);
	~CObject2DMulti();

	// �I�[�o�[���C�h���ꂽ�֐�
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void Draw(int nNumVertex);
	void Draw(LPDIRECT3DTEXTURE9 mutitex);
	virtual void SetVtx(void);
	void BindTexture(int nIdx);
	int GetIdxTexture(void) { return m_nTexIdx; }

	virtual void SetColor(const D3DXCOLOR col);			// �F�ݒ�
	virtual D3DXCOLOR GetColor(void) const;				// �F�擾
	virtual void SetSize(const D3DXVECTOR2 size);		// �T�C�Y�̐ݒ�
	virtual D3DXVECTOR2 GetSize(void) const;			// �T�C�Y�̎擾
	virtual void SetSizeOrigin(const D3DXVECTOR2 size);	// ���̃T�C�Y�̐ݒ�
	virtual D3DXVECTOR2 GetSizeOrigin(void) const;		// ���̃T�C�Y�̎擾
	virtual void SetTex(D3DXVECTOR2 *tex);				// �e�N�X�`�����W�̐ݒ�
	virtual D3DXVECTOR2 *GetTex(void);					// �e�N�X�`�����W�̎擾
	virtual MyLib::Vector3 *GetVtxPos(void);				// ���_���W�擾
	virtual void SetVtxPos(MyLib::Vector3 *pos);			// ���_���W�擾
	

	static CObject2DMulti *Create();
	static CObject2DMulti *Create(int nPriority);
	CObject2DMulti * GetObject2DMulti(void);
	LPDIRECT3DVERTEXBUFFER9 GetVtxBuff(void) const;
protected:

private:

	void SetLength(const float fLength);		// �Ίp���̒����ݒ�
	float GetLength(void) const;				// �Ίp���̒����擾
	void SetAngle(const float fAngle);			// �Ίp���̌����ݒ�
	float GetAngle(void) const;					// �Ίp���̌����擾

	D3DXCOLOR m_col;			// �F
	D3DXVECTOR2 m_size;			// �T�C�Y
	D3DXVECTOR2 m_sizeOrigin;	// ���̃T�C�Y
	D3DXVECTOR2 m_fTex[32];		// �e�N�X�`�����W
	float m_fLength;			// �Ίp���̒���
	float m_fAngle;				// �Ίp���̌���
	MyLib::Vector3 m_VtxPos[32];		// ���_���W
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;	// ���_�o�b�t�@
	int m_nTexIdx;					// �e�N�X�`���̃C���f�b�N�X�ԍ�
};

#endif