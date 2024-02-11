//=============================================================================
// 
//  �G�f�B�b�g�w�b�_�[ [edit.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _EDIT_H_
#define _EDIT_H_	// ��d�C���N���[�h�h�~

#include "main.h"
#include "object.h"

class CObjectX;

//==========================================================================
// �N���X��`
//==========================================================================
// �G�f�B�b�g�N���X��`
class CEdit : public CObject
{
public:

	CEdit(int nPriority = 0);
	~CEdit();

	// �I�[�o�[���C�h���ꂽ�֐�
	HRESULT Init();
	HRESULT Init(const char *pFileName);
	void Uninit();
	void Update();
	void Draw();

	static CEdit *Create();
	void Release();
	static int GetNumAll();
protected:

private:
	HRESULT ReadText();
	void Control(CObjectX *pObjX);
	void ChangeType();
	void GrabModel();
	void DeleteModel();

	static CObjectX *m_pObjX;
	MyLib::Vector3 m_posOld;	// �O��̈ʒu
	static int m_nNumAll;	// ����
	static int m_nType;		// �^�C�v
	static bool m_bShadow;	// �e���g�����ǂ���
};



#endif