#include "Audio.h"
#include "RendererHelper.h"
#include "Game.h"
#include "mmsystem.h"
#include "ConstValue.h"
#include <filesystem>

namespace rdr
{
	Audio::Audio() : CurrentMusicIndex(-1)
	{
		LoadMusic();
	}

	Audio::~Audio()
	{
		
	}

	void Audio::LoadMusic()
	{
		const auto dirIt = std::filesystem::directory_iterator(AUDIO_PATH);
		for(const auto& entry : dirIt)
		{
			std::wstring audioPath = entry.path();
			LoadMusicFromFile(audioPath);
		}
	}

	void Audio::Play(uint32_t index)
	{
		if (index >= AudioData.size()) DX_THROW("Index Out Of Range")
		if (CurrentMusicIndex >= 0) CHECK_RESULT(AudioData[CurrentMusicIndex].second->Stop());
		CHECK_RESULT(AudioData[index].second->SetCurrentPosition(0));
		CHECK_RESULT(AudioData[index].second->Play(0, 0, 1));
		CurrentMusicIndex = index;
	}

	void Audio::Stop()
	{
		CHECK_RESULT(AudioData[CurrentMusicIndex].second->Stop());
	}

	void Audio::LoadMusicFromFile(const std::wstring& file)
	{
		ComPtr<IDirectSound> ptrAudio = nullptr;
		ComPtr<IDirectSoundBuffer> ptrBuffer = nullptr;
		CHECK_RESULT(DirectSoundCreate(nullptr, &ptrAudio, nullptr));
		CHECK_RESULT(ptrAudio->SetCooperativeLevel(Game::Instance()->GetMainWinHandle(), DSSCL_PRIORITY));
		if (HMMIO audio = mmioOpen(const_cast<LPWSTR>(file.c_str()), nullptr, MMIO_ALLOCBUF | MMIO_READ))
		{
			MMCKINFO fileInfo;
			fileInfo.fccType = mmioFOURCC('W', 'A', 'V', 'E');
			if (mmioDescend(audio, &fileInfo, nullptr, MMIO_FINDRIFF) == MMSYSERR_NOERROR)
			{
				MMCKINFO fmtInfo;
				fmtInfo.ckid = mmioFOURCC('f', 'm', 't', ' ');
				if (mmioDescend(audio, &fmtInfo, &fileInfo, MMIO_FINDCHUNK) == MMSYSERR_NOERROR)
				{
					WAVEFORMATEX waveFormat;
					if (mmioRead(audio, reinterpret_cast<HPSTR>(&waveFormat), sizeof(waveFormat)) != -1)
					{
						mmioAscend(audio, &fmtInfo, 0);
						fmtInfo.ckid = mmioFOURCC('d', 'a', 't', 'a');
						if (mmioDescend(audio, &fmtInfo, &fileInfo, MMIO_FINDCHUNK) == MMSYSERR_NOERROR)
						{
							DSBUFFERDESC desc = {};
							desc.dwSize = sizeof(desc);
							desc.lpwfxFormat = &waveFormat;
							desc.dwFlags = DSBCAPS_STATIC;
							desc.dwBufferBytes = fmtInfo.cksize;
							CHECK_RESULT(ptrAudio->CreateSoundBuffer(&desc, &ptrBuffer, nullptr));

							LPVOID tPtrAudio;
							DWORD BytesAudio;
							ptrBuffer->Lock(0, fmtInfo.cksize, &tPtrAudio, &BytesAudio, nullptr, nullptr, 0);
							if (mmioRead(audio, static_cast<HPSTR>(tPtrAudio), BytesAudio) != -1)
							{
								ptrBuffer->Unlock(&tPtrAudio, BytesAudio, nullptr, 0);
								mmioClose(audio, 0);
							}
						}
					}
				}
			}
		}
		else
		{
			DX_THROW("Can Not Open Audio File")
		}

		if (ptrBuffer == nullptr)
			DX_THROW("Buffer Is Null")

		std::pair tSoundPair = { ptrAudio, ptrBuffer };
		AudioData.push_back(tSoundPair);
	}
}
