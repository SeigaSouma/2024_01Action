//=============================================================================
// 
//  �X�L���|�C���g�w�b�_�[ [skillpoint.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _SKILLPOINT_H_
#define _SKILLPOINT_H_	// ��d�C���N���[�h�h�~

class CMultiNumber;

#include "object2D.h"
//#include "object2D_Multi.h"

//==========================================================================
// �N���X��`
//==========================================================================
// �X�L���|�C���g�N���X��`
class CSkillPoint : public CObject2D
{
public:

	CSkillPoint(int nPriority = 8);
	~CSkillPoint();

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	void SetPoint(int nValue);	// �|�C���g�ݒ�
	void AddPoint(void);		// �|�C���g�ǉ�
	void AddPoint(int nValue);	// �|�C���g�ǉ�
	void SubPoint(void);		// �|�C���g�폜
	int GetPoint(void) const { return m_nPoint; }		// �|�C���g�擾
	static CSkillPoint* Create(void);

private:

	// �����o�ϐ�
	int m_nPoint;	// �|�C���g
	CMultiNumber* m_apNumber;	// �����̃I�u�W�F�N�g
	//LPDIRECT3DTEXTURE9 multi;
};



#endif