#include "AudioManager.h"



AudioManager::AudioManager()
{
}


AudioManager::~AudioManager()
{
	if(pXAudio2) pXAudio2->StopEngine();
}

HRESULT AudioManager::Init()
{
	 pXAudio2 = NULL;
	HRESULT hr;
	if (FAILED(hr = XAudio2Create(&pXAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR)))
		return hr;

	 pMasterVoice = NULL;
	if (FAILED(hr = pXAudio2->CreateMasteringVoice(&pMasterVoice)))
		return hr;

	VOICE_BUFFER * voice_buffer = new VOICE_BUFFER();
	if (FAILED(hr = LoadFromFile(laserFilePath, voice_buffer)))
		return hr;

	if (FAILED(hr = CreateVoice(&voice_buffer->pSourceVoice, &voice_buffer->buffer)))
		return hr;
	m_soundForBuffer["Laser"] = 0;
	voice_buffer->pSourceVoice->SetVolume(0.5f);
	m_soundEffects.push_back(*voice_buffer);

	if (FAILED(hr = LoadFromFile(explosionFilePath, voice_buffer)))
		return hr;

	if (FAILED(hr = CreateVoice(&voice_buffer->pSourceVoice, &voice_buffer->buffer)))
		return hr;
	m_soundForBuffer["Explosion"] = 1;
	voice_buffer->pSourceVoice->SetVolume(0.4f);
	m_soundEffects.push_back(*voice_buffer);

	if (FAILED(hr = LoadFromFile(spaceFilePath, voice_buffer)))
		return hr;

	if (FAILED(hr = CreateVoice(&voice_buffer->pSourceVoice, &voice_buffer->buffer)))
		return hr;
	m_soundForBuffer["Space"] = 2;
	voice_buffer->buffer.LoopCount = XAUDIO2_LOOP_INFINITE;
	m_soundEffects.push_back(*voice_buffer);

	return S_OK;
}

HRESULT AudioManager::PlaySoundEffect(string fileName)
{
	HRESULT hr = S_OK;

	if (FAILED(hr = m_soundEffects[m_soundForBuffer[fileName]].pSourceVoice->Stop()))
		return hr;

	if (FAILED(hr = m_soundEffects[m_soundForBuffer[fileName]].pSourceVoice->FlushSourceBuffers()))
		return hr;

	if (FAILED(hr = m_soundEffects[m_soundForBuffer[fileName]].pSourceVoice->SubmitSourceBuffer(&m_soundEffects[m_soundForBuffer[fileName]].buffer)))
		return hr;

	if (FAILED(hr = m_soundEffects[m_soundForBuffer[fileName]].pSourceVoice->Start(0)))
		return hr;

	return S_OK;
}

HRESULT AudioManager::FindChunk(HANDLE hFile, DWORD fourcc, DWORD & dwChunkSize, DWORD & dwChunkDataPosition)
{
	HRESULT hr = S_OK;
	if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, 0, NULL, FILE_BEGIN))
		return HRESULT_FROM_WIN32(GetLastError());

	DWORD dwChunkType;
	DWORD dwChunkDataSize;
	DWORD dwRIFFDataSize = 0;
	DWORD dwFileType;
	DWORD bytesRead = 0;
	DWORD dwOffset = 0;

	while (hr == S_OK)
	{
		DWORD dwRead;
		if (0 == ReadFile(hFile, &dwChunkType, sizeof(DWORD), &dwRead, NULL))
			hr = HRESULT_FROM_WIN32(GetLastError());

		if (0 == ReadFile(hFile, &dwChunkDataSize, sizeof(DWORD), &dwRead, NULL))
			hr = HRESULT_FROM_WIN32(GetLastError());

		switch (dwChunkType)
		{
		case fourccRIFF:
			dwRIFFDataSize = dwChunkDataSize;
			dwChunkDataSize = 4;
			if (0 == ReadFile(hFile, &dwFileType, sizeof(DWORD), &dwRead, NULL))
				hr = HRESULT_FROM_WIN32(GetLastError());
			break;

		default:
			if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, dwChunkDataSize, NULL, FILE_CURRENT))
				return HRESULT_FROM_WIN32(GetLastError());
		}

		dwOffset += sizeof(DWORD) * 2;

		if (dwChunkType == fourcc)
		{
			dwChunkSize = dwChunkDataSize;
			dwChunkDataPosition = dwOffset;
			return S_OK;
		}

		dwOffset += dwChunkDataSize;

		if (bytesRead >= dwRIFFDataSize) return S_FALSE;

	}

	return S_OK;
}

HRESULT AudioManager::ReadChunkData(HANDLE hFile, void * buffer, DWORD buffersize, DWORD bufferoffset)
{
	HRESULT hr = S_OK;
	if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, bufferoffset, NULL, FILE_BEGIN))
		return HRESULT_FROM_WIN32(GetLastError());
	DWORD dwRead;
	if (0 == ReadFile(hFile, buffer, buffersize, &dwRead, NULL))
		hr = HRESULT_FROM_WIN32(GetLastError());
	return hr;

	return S_OK;
}

HRESULT AudioManager::LoadFromFile(LPCSTR soundFilePath, VOICE_BUFFER *  VB)
{
	// Open the file
	HANDLE hFile = CreateFile(
		soundFilePath,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		0,
		NULL);

	if (INVALID_HANDLE_VALUE == hFile)
		return HRESULT_FROM_WIN32(GetLastError());

	if (INVALID_SET_FILE_POINTER == SetFilePointer(hFile, 0, NULL, FILE_BEGIN))
		return HRESULT_FROM_WIN32(GetLastError());

	DWORD dwChunkSize;
	DWORD dwChunkPosition;
	//check the file type, should be fourccWAVE or 'XWMA'
	FindChunk(hFile, fourccRIFF, dwChunkSize, dwChunkPosition);
	DWORD filetype;
	ReadChunkData(hFile, &filetype, sizeof(DWORD), dwChunkPosition);
	if (filetype != fourccWAVE)
		return S_FALSE;

	//Locate the 'fmt ' chunk, and copy its contents into a WAVEFORMATEXTENSIBLE structure.
	FindChunk(hFile, fourccFMT, dwChunkSize, dwChunkPosition);
	ReadChunkData(hFile, &wfx, dwChunkSize, dwChunkPosition);

	//fill out the audio data buffer with the contents of the fourccDATA chunk
	FindChunk(hFile, fourccDATA, dwChunkSize, dwChunkPosition);
	BYTE * pDataBuffer = new BYTE[dwChunkSize];
	ReadChunkData(hFile, pDataBuffer, dwChunkSize, dwChunkPosition);

	// Populate an XAUDIO2_BUFFER structure.
	VB->buffer.AudioBytes = dwChunkSize;  //buffer containing audio data
	VB->buffer.pAudioData = pDataBuffer;  //size of the audio buffer in bytes
	VB->buffer.Flags = XAUDIO2_END_OF_STREAM; // tell the source voice not to expect any data after this buffer

	return S_OK;
}

HRESULT AudioManager::CreateVoice(IXAudio2SourceVoice** const& pSourceVoice, XAUDIO2_BUFFER *  buffer)
{
	HRESULT hr = S_OK;
	
	if (FAILED(hr = pXAudio2->CreateSourceVoice(&*pSourceVoice, (WAVEFORMATEX*)&wfx))) return hr;


	if (FAILED(hr = ((*pSourceVoice)->SubmitSourceBuffer((&*buffer)))))
		return hr;

	return S_OK;
}
