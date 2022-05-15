#pragma once
#include <dsound.h>
#include <string>
#include <vector>
#include <wrl.h>

#pragma comment(lib,"dxguid.lib")
#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "winmm.lib")

namespace rdr
{
	using Microsoft::WRL::ComPtr;

	class Audio
	{
	public:
		Audio();
		~Audio();

	public:
		void LoadMusic();
		void Play(uint32_t index);
		void Stop();

	private:
		void LoadMusicFromFile(const std::wstring& file);

	private:
		int32_t CurrentMusicIndex;
		std::vector<std::pair<ComPtr<IDirectSound>, ComPtr<IDirectSoundBuffer>>> AudioData;
	};
}
