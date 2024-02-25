//=============================================================================
//
// サウンド処理 [sound.h]
// Author : 相馬靜雅
//
//=============================================================================
#ifndef _SOUND_H_
#define _SOUND_H_

#include "main.h"

//==========================================================================
// クラス定義
//==========================================================================
// サウンドクラス
class CSound
{
public:

	// 列挙型定義
	enum LABEL
	{
		LABEL_BGM_TITLE = 0,		// タイトル
		LABEL_BGM_GAME,				// ゲーム
		LABEL_BGM_GAMECLEAR,		// クリア
		LABEL_BGM_ENHANCE,			// 強化
		LABEL_BGM_ENHANCE_WIND,		// 強化の風
		LABEL_BGM_BOSS,				// ボス
		LABEL_BGM_RESULT,			// リザルト
		LABEL_BGM_GALLERY,			// 観客
		LABEL_SE_WALK1,				// 歩行1
		LABEL_SE_WALK2,				// 歩行2
		LABEL_SE_DASH1,				// ダッシュ1
		LABEL_SE_DASH2,				// ダッシュ2
		LABEL_SE_JUMP,				// ジャンプ
		LABEL_SE_NORMALATK_SWING1,	// 通常攻撃スイング1
		LABEL_SE_NORMALATK_SWING2,	// 通常攻撃スイング2
		LABEL_SE_NORMALATK_SWING3,	// 通常攻撃スイング3
		LABEL_SE_DASHATK_SWING2,	// ダッシュ攻撃スイング2
		LABEL_SE_NORMALATK_HIT1,	// 通常攻撃ヒット1
		LABEL_SE_NORMALATK_HIT2,	// 通常攻撃ヒット2
		LABEL_SE_NORMALATK_HIT3,	// 通常攻撃ヒット3
		LABEL_SE_GALLERY1,			// 観客1
		LABEL_SE_GALLERY2,			// 観客2
		LABEL_SE_IN_ENHANCE,		// 強化入場
		LABEL_SE_BATTLESTART,		// 戦闘開始
		LABEL_SE_BATTLESTART_START,	// 戦闘開始開始
		LABEL_SE_BATTLESTART_CHARGE,	// 戦闘開始溜め
		LABEL_SE_SKILLTREE_START,		// スキルツリー起動
		LABEL_SE_SKILLTREE_GET,			// スキル獲得
		LABEL_SE_SKILLTREE_WAIT,		// スキル起動待ち
		LABEL_SE_BATTLERESULT_CLEAR,	// クリア
		LABEL_SE_BATTLERESULT,			// 戦果表示
		LABEL_SE_BATTLERESULT_END,		// 戦果表示閉じ
		LABEL_SE_BATTLERESULT_RANK,		// 戦果ランク
		LABEL_MAX,
	};

	typedef struct
	{
		const char *pFilename;	// ファイル名
		int nCntLoop;		// ループカウント
	} SOUNDINFO;

	CSound();
	~CSound();

	HRESULT Init(HWND hWnd);
	void Uninit();
	HRESULT PlaySound(LABEL label);
	void StopSound();
	void StopSound(LABEL label);
	void VolumeChange(LABEL label, float volume);	// 音量設定
	void VolumeChange(float fVolume);				// マスターボリューム設定
	int GetVolume();
	void SetFrequency(LABEL label, float fValue);	// 周波数設定

private:
	HRESULT CheckChunk(HANDLE hFile, DWORD format, DWORD *pChunkSize, DWORD *pChunkDataPosition);
	HRESULT ReadChunkData(HANDLE hFile, void *pBuffer, DWORD dwBuffersize, DWORD dwBufferoffset);

	IXAudio2 *m_pXAudio2;								// XAudio2オブジェクトへのインターフェイス
	IXAudio2MasteringVoice *m_pMasteringVoice;			// マスターボイス
	IXAudio2SourceVoice *m_apSourceVoice[LABEL_MAX];	// ソースボイス
	BYTE *m_apDataAudio[LABEL_MAX];						// オーディオデータ
	DWORD m_aSizeAudio[LABEL_MAX];						// オーディオデータサイズ
	DSBUFFERDESC SetVolume;								// 音量調整
	float m_fVolume;									// 音量
	static SOUNDINFO m_aSoundInfo[LABEL_MAX];			// サウンドの情報
	WAVEFORMATEXTENSIBLE m_wfx[LABEL_MAX];
};


#endif
