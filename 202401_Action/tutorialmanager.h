//=============================================================================
// 
//  �`���[�g���A���}�l�[�W���w�b�_�[ [tutorialmanager.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _TUTORIALMANAGER_H_
#define _TUTORIALMANAGER_H_	// ��d�C���N���[�h�h�~

#include "gamemanager.h"

//==========================================================================
// �N���X��`
//==========================================================================
// �Q�[���}�l�[�W���N���X��`
class CTutorialManager : public CGameManager
{
public:

	CTutorialManager();
	~CTutorialManager();

	HRESULT Init() override;
	void Uninit() override;
	void Update() override;

private:

	// �����o�֐�
	void SceneTransition() override;	// �J�ڒ�
	void SceneEnhance() override;
	void SceneReaspawn() override;	// ����
	void SetEnemy() override;

};



#endif