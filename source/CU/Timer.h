#pragma once
#include <chrono>

namespace CU
{
	class Timer
	{
	public:
		Timer() : myDT(0), myTT(0), myLastFrame(0), myStartTime(std::chrono::high_resolution_clock::now()) {}

		void Update()
		{
			myTT = std::chrono::duration_cast<std::chrono::duration<double>>(std::chrono::high_resolution_clock::now() - myStartTime);
			myDT = myTT - myLastFrame;
			myLastFrame = myTT;
		}

		inline float GetDeltaTime()  const { return myDT.count(); }
		inline double GetTotalTime() const { return myTT.count(); }

	private:
		std::chrono::duration<float> myDT;
		std::chrono::duration<double> myLastFrame;
		std::chrono::duration<double> myTT;
		std::chrono::high_resolution_clock::time_point myStartTime;
	};
}