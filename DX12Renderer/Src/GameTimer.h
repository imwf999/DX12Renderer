#ifndef RDR_GAMETIMER_H
#define RDR_GAMETIMER_H

#include <string>

namespace rdr
{
	class GameTimer
	{
	public:
		GameTimer();

		float TotalTime()const; // in seconds
		float DeltaTime()const; // in seconds

		void Reset(); // Call before message loop.
		void Start(); // Call when unpaused.
		void Stop();  // Call when paused.
		void Tick();  // Call every frame.

		void ShowFrameTime(HWND hwnd, const std::wstring winName);

	private:
		double mSecondsPerCount;
		double mDeltaTime;

		__int64 mBaseTime;
		__int64 mPausedTime;
		__int64 mStopTime;
		__int64 mPrevTime;
		__int64 mCurrTime;

		bool mStopped;
	};
}

#endif
