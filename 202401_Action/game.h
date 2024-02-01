//=============================================================================
// 
//  �Q�[���w�b�_�[ [game.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _GAME_H_
#define _GAME_H_	// ��d�C���N���[�h�h�~

#include "main.h"
#include "scene.h"
#include "gamemanager.h"

//==========================================================================
// �O���錾
//==========================================================================
class CPlayer;
class CScore;
class CTimer;
class CPowerGauge;
class CEditEnemyBase;
class CStage;
class CHitScore;
class CEnemyManager;
class CLimitArea;
class CPlayer;
class CEnemyBase;

//==========================================================================
// �N���X��`
//==========================================================================
// �Q�[���N���X��`
class CGame : public CScene
{
public:

	// �G�f�B�b�g�^�C�v
	enum EEditType
	{
		EDITTYPE_OFF = 0,			// �I�t
		EDITTYPE_ENEMYBASE,			// �G���_
		EDITTYPE_MAX
	};

	CGame();
	~CGame();

	// �I�[�o�[���C�h���ꂽ�֐�
	HRESULT Init(void) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;

	void ResetBeforeBoss(void);
	EEditType GetEditType(void) { return m_EditType; }
	CScore *GetScore(void);
	CTimer *GetTimer(void) { return m_pTimer; }	// �^�C�}�[�擾
	CStage *GetStage(void);
	CGameManager *GetGameManager(void);
	void SetEnableClear(bool bClear) { m_clear = bClear; }	// �N���A�̃t���O
	bool IsClearFrag(void) { return m_clear; } // �N���A�t���O�̎擾
	CEnemyManager* GetEnemyManager(void);
	CEnemyBase* GetEnemyBase(void);		// �G�̋��_
	bool IsEdit(void) { return m_bEdit; }	// �G�f�B�b�g�̔���
	float GetRockOnDistance() { return m_fMaxRokOnDistance; }	// ���b�N�I���̍ő勗���擾

	// �ÓI�֐�
	static CGame* GetInstance(void);	// �C���X�^���X�擾
	static CGame* Create(void);		// ��������

private:

	// �����o�֐�
	void EditReset(void);

	// �����o�ϐ�
	CScore *m_pScore;						// �X�R�A�̃I�u�W�F�N�g
	CTimer *m_pTimer;						// �^�C�}�[�̃I�u�W�F�N�g
	CLimitArea *m_pLimitArea;				// �G���A�����̃I�u�W�F�N�g
	CEditEnemyBase *m_pEditEnemyBase;		// �G�̋��_�G�f�B�^�[
	CStage *m_pStage;						// �X�e�[�W�̃I�u�W�F�N�g
	CGameManager *m_pGameManager;			// �Q�[���}�l�[�W���̃I�u�W�F�N�g
	EEditType m_EditType;					// �G�f�B�b�g�̎��
	CEnemyManager *m_pEnemyManager;			// �G�}�l�[�W���̃I�u�W�F�N�g
	CEnemyBase *m_pEnemyBase;				// �G�̋��_
	bool m_bEdit;				// �G�f�B�b�g�̔���
	bool m_clear; // �N���A����
	float m_fMaxRokOnDistance;			// ���b�N�I���̍ő勗��

	static CGame* m_pThisPtr;	// ���g�̃|�C���^
};



#endif