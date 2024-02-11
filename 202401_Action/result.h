//=============================================================================
// 
//  ���U���g�w�b�_�[ [result.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _RESULT_H_
#define _RESULT_H_	// ��d�C���N���[�h�h�~

#include "main.h"
#include "manager.h"

//==========================================================================
// �O���錾
//==========================================================================
class CResultScore;

//==========================================================================
// �N���X��`
//==========================================================================
// ���U���g�N���X��`
class CResult : public CScene
{
public:

	CResult();
	~CResult();

	// �I�[�o�[���C�h���ꂽ�֐�
	HRESULT Init();
	void Uninit();
	void Update();
	void Draw();

	static CResultScore *GetResultScore();
	static void SetEnableArrival();

private:

	// �����o�ϐ�
	D3DXCOLOR m_col; // �t�H�O�̐F
	float m_fLength; // �t�H�O�̋���
	bool m_clear; // �N���A����

	static bool m_bAllArrival;		// �S�ē�����������
	static CResultScore *m_pResultScore;
};



#endif