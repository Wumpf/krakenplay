#include "../Stopwatch.h"

namespace Krakenplay
{
	Stopwatch::Stopwatch()
	{
		m_LastCheckpoint = Time::Now();

		StopAndReset();
		Resume();
	}

	void Stopwatch::StopAndReset()
	{
		m_TotalDuration.SetZero();
		m_bRunning = false;
	}

	void Stopwatch::Resume()
	{
		if (m_bRunning)
			return;

		m_bRunning = true;
		m_LastUpdate = Time::Now();
	}

	void Stopwatch::Pause()
	{
		if (!m_bRunning)
			return;

		m_bRunning = false;

		m_TotalDuration += Time::Now() - m_LastUpdate;
	}

	Time Stopwatch::GetRunningTotal()
	{
		if (m_bRunning)
		{
			const Time tNow = Time::Now();

			m_TotalDuration += tNow - m_LastUpdate;
			m_LastUpdate = tNow;
		}

		return m_TotalDuration;
	}

	Time Stopwatch::Checkpoint()
	{
		const Time tNow = Time::Now();

		const Time tDiff = tNow - m_LastCheckpoint;
		m_LastCheckpoint = tNow;

		return tDiff;
	}
}