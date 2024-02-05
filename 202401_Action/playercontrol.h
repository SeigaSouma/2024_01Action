//=============================================================================
// 
//  �v���C���[�R���g���[���w�b�_�[ [playercontrol.h]
//  Author : ���n�Ή�
// 
//=============================================================================

#ifndef _PLAYERCONTROL_H_
#define _PLAYERCONTROL_H_	// ��d�C���N���[�h�h�~

#include "player.h"

//==========================================================================
// �v���C���[�R���g���[���N���X��`
//==========================================================================
// �U��
class CPlayerControlAttack
{
public:
	virtual void Attack(CPlayer* player);	// �ʏ�U��

protected:
	virtual void StageByReset(CPlayer* player)	// �i�K���̃��Z�b�g����
	{
		// �i�K�擾
		int combostage = player->GetComboStage();

		// �R���{�̒i�K���Z
		combostage++;
		if (combostage > CPlayer::MOTION_ATK3 - CPlayer::MOTION_ATK)
		{
			combostage = 0;
		}
		player->SetComboStage(combostage);
	}
};

class CPlayerControlAttack_Level1 : public CPlayerControlAttack
{
protected:

	// �i�K���̃��Z�b�g����
	virtual void StageByReset(CPlayer* player) override
	{
		// �i�K�擾
		int combostage = player->GetComboStage();

		// �R���{�̒i�K���Z
		combostage++;
		//if (combostage > CPlayer::MOTION_ATK4 - CPlayer::MOTION_ATK)
		{
			combostage = 0;
		}
		player->SetComboStage(combostage);
	}
};


// �h��
class CPlayerControlDefence
{
public:
	virtual void Defence(CPlayer* player);	// �h��
};

// �h�䃌�x��1
class CPlayerControlDefence_Level1 : public CPlayerControlDefence
{
public:
	virtual void Defence(CPlayer* player) override;	// �h��
};



// ���
class CPlayerControlAvoid
{
public:
	virtual void Avoid(CPlayer* player);	// ���
};

#endif