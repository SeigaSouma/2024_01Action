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

	CSkillPoint(int nPriority = 9);
	~CSkillPoint();

	HRESULT Init();
	void Uninit();
	void Update();
	void Draw();

	void Kill();

	void SetPoint(int nValue);	// �|�C���g�ݒ�
	int GetPoint() const { return m_nPoint; }		// �|�C���g�擾
	void AddPoint();		// �|�C���g�ǉ�
	void AddPoint(int nValue);	// �|�C���g�ǉ�
	void SubPoint();		// �|�C���g�폜
	void SubPoint(int nValue);	// �|�C���g�폜
	static CSkillPoint* Create();

private:

	// �����o�ϐ�
	int m_nPoint;	// �|�C���g
	CMultiNumber* m_apNumber;	// �����̃I�u�W�F�N�g
};



#endif