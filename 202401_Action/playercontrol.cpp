//=============================================================================
// 
//  プレイヤーコントロール処理 [playercontrol.cpp]
//  Author : 相馬靜雅
// 
//=============================================================================
#include "playercontrol.h"
#include "manager.h"
#include "calculation.h"
#include "input.h"
#include "camera.h"

//==========================================================================
// 通常攻撃
//==========================================================================
void CPlayerControlAttack::Attack(CPlayer* player)
{
	// インプット情報取得
	CInputKeyboard* pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();
	CInputGamepad* pInputGamepad = CManager::GetInstance()->GetInputGamepad();

	// 現在の種類取得
	CMotion* pMotion = player->GetMotion();
	int nMotionType = pMotion->GetType();

	// 目標の向き取得
	float fRotDest = player->GetRotDest();

	// カメラの向き取得
	CCamera* pCamera = CManager::GetInstance()->GetCamera();
	MyLib::Vector3 Camerarot = pCamera->GetRotation();

	// 攻撃
	if ((pMotion->IsGetCombiable() || pMotion->IsGetCancelable()) &&
		!player->IsJump() &&
		!pInputGamepad->GetPress(CInputGamepad::BUTTON_RB, player->GetMyPlayerIdx()) &&
		pInputGamepad->GetTrigger(CInputGamepad::BUTTON_X, player->GetMyPlayerIdx()))
	{

		if (pInputGamepad->IsTipStick())
		{// 左スティックが倒れてる場合
			fRotDest = D3DX_PI + pInputGamepad->GetStickRotL(player->GetMyPlayerIdx()) + Camerarot.y;
		}

		int combostage = player->GetComboStage();
		if (player->IsReadyDashAtk() && combostage == 0)
		{// ダッシュ中の初撃は2からスタート
			combostage++;
			player->SetComboStage(combostage);

			pMotion->Set(CPlayer::MOTION_DASHATK, true);
		}
		else
		{
			// コンボ段階分考慮
			int nSetType = CPlayer::MOTION_ATK + combostage;
			pMotion->Set(nSetType, true);
		}
		player->SetComboStage(combostage);

		// 段階別リセット処理
		StageByReset(player);
	}

	// 目標の向き設定
	player->SetRotDest(fRotDest);
}

//==========================================================================
// 防御
//==========================================================================
// 基底
void CPlayerControlDefence::Defence(CPlayer* player)
{
	// インプット情報取得
	CInputKeyboard* pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();
	CInputGamepad* pInputGamepad = CManager::GetInstance()->GetInputGamepad();

	// 現在の種類取得
	CMotion* pMotion = player->GetMotion();

	// 目標の向き取得
	float fRotDest = player->GetRotDest();

	// モーションフラグ取得
	CPlayer::SMotionFrag motionFrag = player->GetMotionFrag();

	// カメラの向き取得
	CCamera* pCamera = CManager::GetInstance()->GetCamera();
	MyLib::Vector3 Camerarot = pCamera->GetRotation();

	// ガード
	if ((pMotion->IsGetCombiable() || pMotion->IsGetCancelable()) &&
		!player->IsJump() &&
		pInputGamepad->GetPress(CInputGamepad::BUTTON_RB, player->GetMyPlayerIdx()))
	{
		pMotion->Set(CPlayer::MOTION_DEF);

		if (pInputGamepad->IsTipStick())
		{// 左スティックが倒れてる場合
			fRotDest = D3DX_PI + pInputGamepad->GetStickRotL(player->GetMyPlayerIdx()) + Camerarot.y;
		}

		pMotion->Set(CPlayer::MOTION_GUARD);
		motionFrag.bGuard = true;	// ガードON
	}

	// 目標の向き設定
	player->SetRotDest(fRotDest);

	// モーションフラグ設定
	player->SetMotionFrag(motionFrag);
}

// レベル1
void CPlayerControlDefence_Level1::Defence(CPlayer* player)
{
	// 防御
	CPlayerControlDefence::Defence(player);


	// インプット情報取得
	CInputKeyboard* pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();
	CInputGamepad* pInputGamepad = CManager::GetInstance()->GetInputGamepad();

	// 現在の種類取得
	CMotion* pMotion = player->GetMotion();

	// モーションフラグ取得
	CPlayer::SMotionFrag motionFrag = player->GetMotionFrag();

	// ガード中に攻撃でカウンター
	if (pMotion->GetType() == CPlayer::MOTION_GUARD &&
		pInputGamepad->GetTrigger(CInputGamepad::BUTTON_X, player->GetMyPlayerIdx()))
	{
		pMotion->Set(CPlayer::MOTION_COUNTER_ACCEPT);
		motionFrag.bCounter = true;		// 攻撃判定ON

		// スタミナ減算
		if (player->GetStaminaGauge() != nullptr)
		{
			player->GetStaminaGauge()->SubValue(player->GetCounterSubValue());
		}
	}

	// モーションフラグ設定
	player->SetMotionFrag(motionFrag);
}

//==========================================================================
// 回避
//==========================================================================
void CPlayerControlAvoid::Avoid(CPlayer* player)
{
	// インプット情報取得
	CInputKeyboard* pInputKeyboard = CManager::GetInstance()->GetInputKeyboard();
	CInputGamepad* pInputGamepad = CManager::GetInstance()->GetInputGamepad();

	// 現在の種類取得
	CMotion* pMotion = player->GetMotion();

	// 目標の向き取得
	float fRotDest = player->GetRotDest();

	// モーションフラグ取得
	CPlayer::SMotionFrag motionFrag = player->GetMotionFrag();

	// カメラの向き取得
	CCamera* pCamera = CManager::GetInstance()->GetCamera();
	MyLib::Vector3 Camerarot = pCamera->GetRotation();

	// 回避
	if (!player->IsJump() &&
		(pMotion->IsGetCombiable() || pMotion->IsGetCancelable()) &&
		pInputGamepad->GetTrigger(CInputGamepad::BUTTON_B, player->GetMyPlayerIdx()))
	{
		pMotion->Set(CPlayer::MOTION_DEF);
		pMotion->Set(CPlayer::MOTION_AVOID);

		if (pInputGamepad->IsTipStick())
		{// 左スティックが倒れてる場合

			fRotDest = D3DX_PI + pInputGamepad->GetStickRotL(player->GetMyPlayerIdx()) + Camerarot.y;
		}
		player->SetState(CPlayer::STATE_AVOID);

		// スタミナ減算
		if (player->GetStaminaGauge() != nullptr)
		{
			player->GetStaminaGauge()->SubValue(40.0f);
		}
	}

	// 目標の向き設定
	player->SetRotDest(fRotDest);

	// モーションフラグ設定
	player->SetMotionFrag(motionFrag);
}




//==========================================================================
// ガード
//==========================================================================
// 受けた時の処理
void CPlayerGuard::HitProcess(CPlayer* player, MyLib::Vector3 enemypos)
{
	// スタミナ減算
	if (player->GetStaminaGauge() != nullptr)
	{
		player->GetStaminaGauge()->SubValue(player->GetGuardSubValue());
	}

	// 位置取得
	MyLib::Vector3 pos = player->GetPosition();
	MyLib::Vector3 rot = player->GetRotation();

	// 目標の角度設定
	float fRotDest = atan2f((pos.x - enemypos.x), (pos.z - enemypos.z));
	player->SetRotation(MyLib::Vector3(rot.x, fRotDest, rot.z));
	player->SetRotDest(fRotDest);

	player->SetMove(MyLib::Vector3(
		sinf(fRotDest) * 40.0f,
		0.0f,
		cosf(fRotDest) * 40.0f));
}