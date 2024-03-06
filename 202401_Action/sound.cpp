//==========================================================================
//
// サウンド処理 [sound.cpp]
// Author : 相馬靜雅
//
//==========================================================================
#include "sound.h"
#include "calculation.h"

//==========================================================================
// 静的メンバ変数宣言
//==========================================================================
CSound::SOUNDINFO CSound::m_aSoundInfo[LABEL_MAX] = 
{
	{ "data/BGM/title.wav", -1 },			// タイトル
	{ "data/BGM/game.wav", -1 },			// ゲーム中
	{ "data/BGM/gameclear.wav", -1 },		// ゲームクリア
	{ "data/BGM/enhance.wav", -1 },			// 強化
	{ "data/BGM/enhance_wind.wav", -1 },	// 強化の風
	{ "data/BGM/boss.wav", -1 },			// ボス
	{ "data/BGM/result.wav", -1 },			// タイトル
	{ "data/BGM/galleryClear.wav", -1 },	// 観客
	{ "data/SE/walk01.wav", 0 },			// 歩行1
	{ "data/SE/walk02.wav", 0 },			// 歩行2
	{ "data/SE/dash01.wav", 0 },			// ダッシュ1
	{ "data/SE/dash02.wav", 0 },			// ダッシュ2
	{ "data/SE/jump.wav", 0 },				// ジャンプ
	{ "data/SE/avoid.wav", 0 },				// 回避
	{ "data/SE/knockback_passive.wav", 0 },	// 受け身
	{ "data/SE/atkswing_01.wav", 0 },		// 通常攻撃スイング1
	{ "data/SE/atkswing_02.wav", 0 },		// 通常攻撃スイング2
	{ "data/SE/atkswing_03.wav", 0 },		// 通常攻撃スイング3
	{ "data/SE/atkswing_02_dash.wav", 0 },	// ダッシュ攻撃スイング2
	{ "data/SE/atkhit_01.wav", 0 },			// 通常攻撃ヒット1
	{ "data/SE/atkhit_02.wav", 0 },			// 通常攻撃ヒット2
	{ "data/SE/atkhit_03.wav", 0 },			// 通常攻撃ヒット3
	{ "data/SE/gallery01.wav", 0 },			// 観客1
	{ "data/SE/gallery02.wav", 0 },			// 観客2
	{ "data/SE/Inenhance.wav", 0 },			// 強化入場
	{ "data/SE/battlestart.wav", 0 },			// 戦闘開始
	{ "data/SE/battlestart_strart.wav", 0 },	// 戦闘開始開始
	{ "data/SE/battlestart_charge.wav", 0 },	// 戦闘開始溜め
	{ "data/SE/skilltreeStart.wav", 0 },		// スキルツリー起動
	{ "data/SE/skillget.wav", 0 },				// スキル獲得
	{ "data/SE/skilltree_wait.wav", -1 },		// スキルツリー起動待ち
	{ "data/SE/gameclear.wav", 0 },				// クリア
	{ "data/SE/battleresult.wav", 0 },			// 戦果表示
	{ "data/SE/battleresult_end.wav", 0 },		// 戦果表示閉じ
	{ "data/SE/battleresult_rank.wav", 0 },		// 戦果ランク
	{ "data/SE/strongATK.wav", 0 },				// 強攻撃発生
	{ "data/SE/charge_start.wav", 0 },			// チャージ攻撃開始
	{ "data/SE/charge_complete.wav", 0 },		// チャージ攻撃完了
	{ "data/SE/charge_endress.wav", -1 },		// チャージ続け
	{ "data/SE/charge_move.wav", 0 },			// チャージ攻撃移動
	{ "data/SE/chargeatk_hit.wav", 0 },			// チャージ攻撃ヒット
	{ "data/SE/cursor_move.wav", 0 },			// カーソル移動
	{ "data/SE/surcor_end.wav", 0 },			// カーソル閉じ
	{ "data/SE/cancel01.wav", 0 },				// キャンセル1
	{ "data/SE/cancel02.wav", 0 },				// キャンセル2
	{ "data/SE/counter_normal.wav", 0 },		// カウンター通常攻撃
	{ "data/SE/counter_strong.wav", 0 },		// カウンター強攻撃
	{ "data/SE/counter_hit4.wav", 0 },			// カウンター攻撃
	{ "data/SE/playerdmg_01.wav", 0 },			// 通常攻撃被弾
	{ "data/SE/playerdmg_big.wav", 0 },			// 強攻撃被弾
	{ "data/SE/guard.wav", 0 },					// ガード被弾
	{ "data/SE/guardbreak.wav", 0 },			// ガードブレイク
	{ "data/SE/enemyField_strong.wav", 0 },		// 敵地面(強)
	{ "data/SE/enemyField_short.wav", 0 },		// 敵地面(短)
	{ "data/SE/enemyswing_01.wav", 0 },			// 敵攻撃スイング(遅)
	{ "data/SE/enemyswing_02.wav", 0 },			// 敵攻撃スイング(早)

};	// サウンドの情報

//==========================================================================
// マクロ定義
//==========================================================================
CSound::CSound()
{
	
}

//==========================================================================
// マクロ定義
//==========================================================================
CSound::~CSound()
{

}

//==========================================================================
// 初期化処理
//==========================================================================
HRESULT CSound::Init(HWND hWnd)
{
	HRESULT hr;

	// COMライブラリの初期化
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	// XAudio2オブジェクトの作成
	hr = XAudio2Create(&m_pXAudio2, 0);
	if(FAILED(hr))
	{
		MessageBox(hWnd, "XAudio2オブジェクトの作成に失敗！", "警告！", MB_ICONWARNING);

		// COMライブラリの終了処理
		CoUninitialize();

		return E_FAIL;
	}
	
	// マスターボイスの生成
	hr = m_pXAudio2->CreateMasteringVoice(&m_pMasteringVoice);
	if(FAILED(hr))
	{
		MessageBox(hWnd, "マスターボイスの生成に失敗！", "警告！", MB_ICONWARNING);

		if(m_pXAudio2 != nullptr)
		{
			// XAudio2オブジェクトの開放
			m_pXAudio2->Release();
			m_pXAudio2 = nullptr;
		}

		// COMライブラリの終了処理
		CoUninitialize();

		return E_FAIL;
	}

	// サウンドデータの初期化
	for(int nCntSound = 0; nCntSound < LABEL_MAX; nCntSound++)
	{
		HANDLE hFile;
		DWORD dwChunkSize = 0;
		DWORD dwChunkPosition = 0;
		DWORD dwFiletype;
		WAVEFORMATEXTENSIBLE wfx;
		XAUDIO2_BUFFER buffer;

		// バッファのクリア
		memset(&wfx, 0, sizeof(WAVEFORMATEXTENSIBLE));
		memset(&buffer, 0, sizeof(XAUDIO2_BUFFER));

		// サウンドデータファイルの生成
		hFile = CreateFile(m_aSoundInfo[nCntSound].pFilename, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
		if(hFile == INVALID_HANDLE_VALUE)
		{
			MessageBox(hWnd, "サウンドデータファイルの生成に失敗！(1)", "警告！", MB_ICONWARNING);
			return HRESULT_FROM_WIN32(GetLastError());
		}
		if(SetFilePointer(hFile, 0, nullptr, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
		{// ファイルポインタを先頭に移動
			MessageBox(hWnd, "サウンドデータファイルの生成に失敗！(2)", "警告！", MB_ICONWARNING);
			return HRESULT_FROM_WIN32(GetLastError());
		}
	
		// WAVEファイルのチェック
		hr = CheckChunk(hFile, 'FFIR', &dwChunkSize, &dwChunkPosition);
		if(FAILED(hr))
		{
			MessageBox(hWnd, "WAVEファイルのチェックに失敗！(1)", "警告！", MB_ICONWARNING);
			return S_FALSE;
		}
		hr = ReadChunkData(hFile, &dwFiletype, sizeof(DWORD), dwChunkPosition);
		if(FAILED(hr))
		{
			MessageBox(hWnd, "WAVEファイルのチェックに失敗！(2)", "警告！", MB_ICONWARNING);
			return S_FALSE;
		}
		if(dwFiletype != 'EVAW')
		{
			MessageBox(hWnd, "WAVEファイルのチェックに失敗！(3)", "警告！", MB_ICONWARNING);
			return S_FALSE;
		}
	
		// フォーマットチェック
		hr = CheckChunk(hFile, ' tmf', &dwChunkSize, &dwChunkPosition);
		if(FAILED(hr))
		{
			MessageBox(hWnd, "フォーマットチェックに失敗！(1)", "警告！", MB_ICONWARNING);
			return S_FALSE;
		}
		hr = ReadChunkData(hFile, &wfx, dwChunkSize, dwChunkPosition);
		if(FAILED(hr))
		{
			MessageBox(hWnd, "フォーマットチェックに失敗！(2)", "警告！", MB_ICONWARNING);
			return S_FALSE;
		}

		// オーディオデータ読み込み
		hr = CheckChunk(hFile, 'atad', &m_aSizeAudio[nCntSound], &dwChunkPosition);
		if(FAILED(hr))
		{
			MessageBox(hWnd, "オーディオデータ読み込みに失敗！(1)", "警告！", MB_ICONWARNING);
			return S_FALSE;
		}
		m_apDataAudio[nCntSound] = (BYTE*)malloc(m_aSizeAudio[nCntSound]);
		hr = ReadChunkData(hFile, m_apDataAudio[nCntSound], m_aSizeAudio[nCntSound], dwChunkPosition);
		if(FAILED(hr))
		{
			MessageBox(hWnd, "オーディオデータ読み込みに失敗！(2)", "警告！", MB_ICONWARNING);
			return S_FALSE;
		}
	
		// ソースボイスの生成
		hr = m_pXAudio2->CreateSourceVoice(&m_apSourceVoice[nCntSound], &(wfx.Format));
		if(FAILED(hr))
		{
			MessageBox(hWnd, "ソースボイスの生成に失敗！", "警告！", MB_ICONWARNING);
			return S_FALSE;
		}

		// バッファの値設定
		memset(&buffer, 0, sizeof(XAUDIO2_BUFFER));
		buffer.AudioBytes = m_aSizeAudio[nCntSound];
		buffer.pAudioData = m_apDataAudio[nCntSound];
		buffer.Flags      = XAUDIO2_END_OF_STREAM;
		buffer.LoopCount  = m_aSoundInfo[nCntSound].nCntLoop;
		SetVolume.dwFlags = DSBCAPS_CTRLVOLUME;		//音量調整のフラグ

		m_fVolume = 1.0f;

		//音量をセットする
		m_pMasteringVoice->SetVolume(1.0f);

		// オーディオバッファの登録
		m_apSourceVoice[nCntSound]->SubmitSourceBuffer(&buffer);

		m_wfx[nCntSound] = wfx;

		// ファイルをクローズ
		CloseHandle(hFile);
	}

	return S_OK;
}

//==========================================================================
// 終了処理
//==========================================================================
void CSound::Uninit()
{
	// 一時停止
	for(int nCntSound = 0; nCntSound < LABEL_MAX; nCntSound++)
	{
		if(m_apSourceVoice[nCntSound] != nullptr)
		{
			// 一時停止
			m_apSourceVoice[nCntSound]->Stop(0);
	
			// ソースボイスの破棄
			m_apSourceVoice[nCntSound]->DestroyVoice();
			m_apSourceVoice[nCntSound] = nullptr;
	
			// オーディオデータの開放
			free(m_apDataAudio[nCntSound]);
			m_apDataAudio[nCntSound] = nullptr;
		}
	}
	
	// マスターボイスの破棄
	m_pMasteringVoice->DestroyVoice();
	m_pMasteringVoice = nullptr;
	
	if(m_pXAudio2 != nullptr)
	{
		// XAudio2オブジェクトの開放
		m_pXAudio2->Release();
		m_pXAudio2 = nullptr;
	}
	
	// COMライブラリの終了処理
	CoUninitialize();
}

//==========================================================================
// セグメント再生(再生中なら停止)
//==========================================================================
HRESULT CSound::PlaySound(LABEL label, bool stop)
{
#if 0
	XAUDIO2_VOICE_STATE xa2state;
	XAUDIO2_BUFFER buffer;

	// バッファの値設定
	memset(&buffer, 0, sizeof(XAUDIO2_BUFFER));
	buffer.AudioBytes = m_aSizeAudio[label];
	buffer.pAudioData = m_apDataAudio[label];
	buffer.Flags      = XAUDIO2_END_OF_STREAM;
	buffer.LoopCount  = m_aSoundInfo[label].nCntLoop;

	// 状態取得
	m_apSourceVoice[label]->GetState(&xa2state);
	if(xa2state.BuffersQueued != 0)
	{// 再生中
		// 一時停止
		m_apSourceVoice[label]->Stop(0);

		// オーディオバッファの削除
		m_apSourceVoice[label]->FlushSourceBuffers();
	}

	// オーディオバッファの登録
	m_apSourceVoice[label]->SubmitSourceBuffer(&buffer);

	// 再生
	m_apSourceVoice[label]->Start(0);

	// 周波数リセット
	m_apSourceVoice[label]->SetFrequencyRatio(1.0f);

#else
	XAUDIO2_BUFFER buffer;

	// バッファの値設定
	memset(&buffer, 0, sizeof(XAUDIO2_BUFFER));
	buffer.AudioBytes = m_aSizeAudio[label];
	buffer.pAudioData = m_apDataAudio[label];
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	buffer.LoopCount = m_aSoundInfo[label].nCntLoop;

	if (m_apSourceVoice[label] != nullptr)
	{
		// 状態取得
		XAUDIO2_VOICE_STATE xa2state;
		m_apSourceVoice[label]->GetState(&xa2state);
		if (xa2state.BuffersQueued != 0 &&
			(buffer.LoopCount == -1 || stop))
		{// 再生中
			// 一時停止
			m_apSourceVoice[label]->Stop(0);

			// オーディオバッファの削除
			m_apSourceVoice[label]->FlushSourceBuffers();
		}
	}

	// ソースボイスを新しく作成して再生
	HRESULT hr = m_pXAudio2->CreateSourceVoice(&m_apSourceVoice[label], &(m_wfx[label].Format));
	if (FAILED(hr))
	{
		MessageBox(GetWnd(), "ソースボイスの生成に失敗！", "警告！", MB_ICONWARNING);
		return S_FALSE;
	}

	// オーディオバッファの登録
	m_apSourceVoice[label]->SubmitSourceBuffer(&buffer);

	// 再生
	m_apSourceVoice[label]->Start(0);

	// 周波数リセット
	m_apSourceVoice[label]->SetFrequencyRatio(1.0f);

	// 音量リセット
	m_apSourceVoice[label]->SetVolume(1.0f);

#endif
	return S_OK;

}

//==========================================================================
// セグメント停止(ラベル指定)
//==========================================================================
void CSound::StopSound(LABEL label)
{
	XAUDIO2_VOICE_STATE xa2state;

	if (m_apSourceVoice[label] == nullptr)
	{
		return;
	}

	// 状態取得
	m_apSourceVoice[label]->GetState(&xa2state);

	if(xa2state.BuffersQueued != 0)
	{// 再生中
		// 一時停止
		m_apSourceVoice[label]->Stop(0);

		// オーディオバッファの削除
		m_apSourceVoice[label]->FlushSourceBuffers();

		// 周波数リセット
		m_apSourceVoice[label]->SetFrequencyRatio(1.0f);
	}

	if (m_apSourceVoice[label] != nullptr)
	{
		m_apSourceVoice[label]->Stop(0);
		m_apSourceVoice[label]->FlushSourceBuffers();
		m_apSourceVoice[label]->DestroyVoice();
		m_apSourceVoice[label] = nullptr;
	}
}

//==========================================================================
// セグメント停止(全て)
//==========================================================================
void CSound::StopSound()
{
	// 一時停止
	for(int nCntSound = 0; nCntSound < LABEL_MAX; nCntSound++)
	{
		if(m_apSourceVoice[nCntSound] != nullptr)
		{
			// 一時停止
			m_apSourceVoice[nCntSound]->Stop(0);

			// 周波数リセット
			m_apSourceVoice[nCntSound]->SetFrequencyRatio(1.0f);
		}

		if (m_apSourceVoice[nCntSound] != nullptr)
		{
			m_apSourceVoice[nCntSound]->Stop(0);
			m_apSourceVoice[nCntSound]->FlushSourceBuffers();
			m_apSourceVoice[nCntSound]->DestroyVoice();
			m_apSourceVoice[nCntSound] = nullptr;
		}
	}
}

//==========================================================================
// チャンクのチェック
//==========================================================================
HRESULT CSound::CheckChunk(HANDLE hFile, DWORD format, DWORD *pChunkSize, DWORD *pChunkDataPosition)
{
	HRESULT hr = S_OK;
	DWORD dwRead;
	DWORD dwChunkType;
	DWORD dwChunkDataSize;
	DWORD dwRIFFDataSize = 0;
	DWORD dwFileType;
	DWORD dwBytesRead = 0;
	DWORD dwOffset = 0;
	
	if(SetFilePointer(hFile, 0, nullptr, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{// ファイルポインタを先頭に移動
		return HRESULT_FROM_WIN32(GetLastError());
	}
	
	while(hr == S_OK)
	{
		if(ReadFile(hFile, &dwChunkType, sizeof(DWORD), &dwRead, nullptr) == 0)
		{// チャンクの読み込み
			hr = HRESULT_FROM_WIN32(GetLastError());
		}

		if(ReadFile(hFile, &dwChunkDataSize, sizeof(DWORD), &dwRead, nullptr) == 0)
		{// チャンクデータの読み込み
			hr = HRESULT_FROM_WIN32(GetLastError());
		}

		switch(dwChunkType)
		{
		case 'FFIR':
			dwRIFFDataSize  = dwChunkDataSize;
			dwChunkDataSize = 4;
			if(ReadFile(hFile, &dwFileType, sizeof(DWORD), &dwRead, nullptr) == 0)
			{// ファイルタイプの読み込み
				hr = HRESULT_FROM_WIN32(GetLastError());
			}
			break;

		default:
			if(SetFilePointer(hFile, dwChunkDataSize, nullptr, FILE_CURRENT) == INVALID_SET_FILE_POINTER)
			{// ファイルポインタをチャンクデータ分移動
				return HRESULT_FROM_WIN32(GetLastError());
			}
		}

		dwOffset += sizeof(DWORD) * 2;
		if(dwChunkType == format)
		{
			*pChunkSize         = dwChunkDataSize;
			*pChunkDataPosition = dwOffset;
			return S_OK;
		}

		dwOffset += dwChunkDataSize;
		if(dwBytesRead >= dwRIFFDataSize)
		{
			return S_FALSE;
		}
	}
	
	return S_OK;
}

//==========================================================================
// チャンクデータの読み込み
//==========================================================================
HRESULT CSound::ReadChunkData(HANDLE hFile, void *pBuffer, DWORD dwBuffersize, DWORD dwBufferoffset)
{
	DWORD dwRead;
	
	if(SetFilePointer(hFile, dwBufferoffset, nullptr, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{// ファイルポインタを指定位置まで移動
		return HRESULT_FROM_WIN32(GetLastError());
	}

	if(ReadFile(hFile, pBuffer, dwBuffersize, &dwRead, nullptr) == 0)
	{// データの読み込み
		return HRESULT_FROM_WIN32(GetLastError());
	}
	
	return S_OK;
}

//==========================================================================
// 音量設定
//==========================================================================
void CSound::VolumeChange(LABEL label, float volume)
{
	if (m_apSourceVoice[label] == nullptr)
	{
		return;
	}
	UtilFunc::Transformation::ValueNormalize(volume, 2.0f, 0.0f);

	// 音量設定
	m_apSourceVoice[label]->SetVolume(volume);
}

//==========================================================================
// 音量調整(マスターボリューム設定)
//==========================================================================
void CSound::VolumeChange(float fVolume)
{
	m_fVolume += fVolume;
	UtilFunc::Transformation::ValueNormalize(m_fVolume, 2.0f, 0.0f);

	//音量をセットする
	m_pMasteringVoice->SetVolume(m_fVolume);
}

//==========================================================================
// 音量取得(3桁表示)
//==========================================================================
int CSound::GetVolume()
{
	return (int)((m_fVolume + 0.009) * 100);
}

//==========================================================================
// 周波数設定
//==========================================================================
void CSound::SetFrequency(LABEL label, float fValue)
{
	XAUDIO2_VOICE_STATE state;

	if (m_apSourceVoice[label] == nullptr){
		return;
	}

	// 状態取得
	m_apSourceVoice[label]->GetState(&state);

	if (state.BuffersQueued != 0)
	{// 再生中
		m_apSourceVoice[label]->SetFrequencyRatio(fValue);
	}
}
