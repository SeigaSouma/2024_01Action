//=============================================================================
// 
//  �`���[�g���A���w�b�_�[ [tutorial.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _TUTORIAL_H_
#define _TUTORIAL_H_	// ��d�C���N���[�h�h�~

#include "scene.h"
//#include "tutorialmanager.h"

//==========================================================================
// �O���錾
//==========================================================================
class CPlayer;
class CEnemyManager;
class CLimitArea;
class CPlayer;
class CEnemyBase;

//==========================================================================
// �N���X��`
//==========================================================================
// �Q�[���N���X��`
class CTutorial : public CScene
{
public:

	// �G�f�B�b�g�^�C�v
	enum EEditType
	{
		EDITTYPE_OFF = 0,			// �I�t
		EDITTYPE_ENEMYBASE,			// �G���_
		EDITTYPE_MAX
	};

	CTutorial();
	~CTutorial();

	// �I�[�o�[���C�h���ꂽ�֐�
	HRESULT Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;

	void ResetBeforeBoss();
	//CTutorialManager *GetGameManager();
	void SetEnableClear(bool bClear) { m_clear = bClear; }		// �N���A�̃t���O
	bool IsClearFrag() { return m_clear; }						// �N���A�t���O�̎擾
	CEnemyManager* GetEnemyManager();							// �G�}�l�[�W��
	CEnemyBase* GetEnemyBase();									// �G�̋��_
	bool IsEdit() { return m_bEdit; }							// �G�f�B�b�g�̔���
	float GetRockOnDistance() { return m_fMaxRokOnDistance; }	// ���b�N�I���̍ő勗���擾

	// �ÓI�֐�
	static CTutorial* GetInstance();	// �C���X�^���X�擾
	static CTutorial* Create();		// ��������

private:

	//=============================
	// �����o�֐�
	//=============================
	void EditReset();

	//=============================
	// �����o�ϐ�
	//=============================
	CLimitArea *m_pLimitArea;				// �G���A�����̃I�u�W�F�N�g
	//CTutorialManager *m_pGameManager;			// �Q�[���}�l�[�W���̃I�u�W�F�N�g
	CEnemyManager *m_pEnemyManager;			// �G�}�l�[�W���̃I�u�W�F�N�g
	CEnemyBase *m_pEnemyBase;				// �G�̋��_
	bool m_bEdit;				// �G�f�B�b�g�̔���
	bool m_clear; // �N���A����
	float m_fMaxRokOnDistance;			// ���b�N�I���̍ő勗��

	static CTutorial* m_pThisPtr;	// ���g�̃|�C���^
};



#endif