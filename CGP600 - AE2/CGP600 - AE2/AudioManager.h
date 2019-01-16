#pragma once
#include <d3d11.h>
#include <dxgi.h>
#include <d3dx11.h>
#include <Windows.h>
#include <dxerr.h>
#define _XM_NO_INTRINSICS_
#define _XM_NO_ALIGNMENT
#include <xnamath.h>
#include <Xinput.h>
#include<WinUser.h>
#include<windowsx.h>
#include <mmsystem.h>
#include <dsound.h>
#include <XAudio2.h>
#include "atlstr.h"
#include <map>
#include <vector>

#ifdef _XBOX //Big-Endian
#define fourccRIFF 'RIFF'
#define fourccDATA 'data'
#define fourccFMT 'fmt '
#define fourccWAVE 'WAVE'
#define fourccXWMA 'XWMA'
#define fourccDPDS 'dpds'
#endif

#ifndef _XBOX //Little-Endian
#define fourccRIFF 'FFIR'
#define fourccDATA 'atad'
#define fourccFMT ' tmf'
#define fourccWAVE 'EVAW'
#define fourccXWMA 'AMWX'
#define fourccDPDS 'sdpd'
#endif
using namespace std;

#define laserFilePath "Assets/Sounds/Laser.wav"
#define explosionFilePath "Assets/Sounds/Explosion.wav"
#define spaceFilePath "Assets/Sounds/Space.wav"

class AudioManager
{
public:
	//***************************************************
	//METHODS
	//***************************************************
	AudioManager();
	~AudioManager();
	HRESULT Init();
	HRESULT PlaySoundEffect(string fileName);

private:
	struct VOICE_BUFFER
	{
		XAUDIO2_BUFFER buffer = { 0 };
		IXAudio2SourceVoice* pSourceVoice;
	};

	//***************************************************
	//METHODS
	//***************************************************
	HRESULT FindChunk(HANDLE hFile, DWORD fourcc, DWORD & dwChunkSize, DWORD & dwChunkDataPosition);
	HRESULT ReadChunkData(HANDLE hFile, void * buffer, DWORD buffersize, DWORD bufferoffset);
	HRESULT LoadFromFile(LPCSTR soundFilePath, VOICE_BUFFER * VB);
	HRESULT CreateVoice(IXAudio2SourceVoice** const& pSourceVoice, XAUDIO2_BUFFER * buffer);

private:
	//***************************************************
	//POINTERS
	//***************************************************
	IXAudio2 * pXAudio2;
	IXAudio2MasteringVoice* pMasterVoice;
	IXAudio2SourceVoice* pSourceVoice;
	IXAudio2SourceVoice* pSourceVoice2;
	XAUDIO2_VOICE_STATE* m_voiceState;

	//***************************************************
	//VARIABLES
	//***************************************************
	WAVEFORMATEXTENSIBLE wfx = { 0 };
	XAUDIO2_BUFFER buffer = { 0 };
	map<string, int> m_soundForBuffer;
	vector<VOICE_BUFFER> m_soundEffects;
};

