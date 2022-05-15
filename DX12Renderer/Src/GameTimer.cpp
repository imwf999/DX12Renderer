#include <Windows.h>
#include "GameTimer.h"

namespace rdr
{
	GameTimer::GameTimer()
		: mStopTime(0), mSecondsPerCount(0.0), mDeltaTime(-1.0), mBaseTime(0),
		mPausedTime(0), mPrevTime(0), mCurrTime(0), mStopped(false), FPSText(L"帧率: 0   每帧用时: 0")
	{
		__int64 countsPerSec;
		QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
		mSecondsPerCount = 1.0 / static_cast<double>(countsPerSec);
	}

	float GameTimer::TotalTime()const
	{
		if (mStopped)
		{
			return (float)(((mStopTime - mPausedTime) - mBaseTime) * mSecondsPerCount);
		}
		else
		{
			return (float)(((mCurrTime - mPausedTime) - mBaseTime) * mSecondsPerCount);
		}
	}

	float GameTimer::DeltaTime()const
	{
		return (float)mDeltaTime;
	}

	void GameTimer::Reset()
	{
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

		mBaseTime = currTime;
		mPrevTime = currTime;
		mStopTime = 0;
		mStopped = false;
	}

	void GameTimer::Start()
	{
		__int64 startTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&startTime);

		if (mStopped)
		{
			mPausedTime += (startTime - mStopTime);

			mPrevTime = startTime;
			mStopTime = 0;
			mStopped = false;
		}
	}

	void GameTimer::Stop()
	{
		if (!mStopped)
		{
			__int64 currTime;
			QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

			mStopTime = currTime;
			mStopped = true;
		}
	}

	void GameTimer::Tick()
	{
		if (mStopped)
		{
			mDeltaTime = 0.0;
			return;
		}

		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
		mCurrTime = currTime;

		mDeltaTime = (mCurrTime - mPrevTime) * mSecondsPerCount;

		mPrevTime = mCurrTime;

		if (mDeltaTime < 0.0)
		{
			mDeltaTime = 0.0;
		}
	}

	std::wstring GameTimer::GetFrameTimeText()
	{
		static int frameCnt = 0;
		static float timeElapsed = 0.0f;

		frameCnt++;

		if (TotalTime() - timeElapsed >= 1.0f)
		{
			int fps = frameCnt; // fps = frameCnt / 1
			float mspf = 1000.0f / fps;

			std::wstring fpsStr = std::to_wstring(fps);
			std::wstring mspfStr = std::to_wstring(mspf);

			 FPSText = L"帧率: " + fpsStr +
				L"   每帧用时: " + mspfStr +L"毫秒";

			// Reset for next average.
			frameCnt = 0;
			timeElapsed += 1.0f;
		}

		return FPSText;
	}
}