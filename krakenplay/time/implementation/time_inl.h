#pragma once

namespace Krakenplay
{
	inline Time::Time(double fTime) : m_fTime(fTime)
	{
	}

	inline void Time::SetZero()
	{
		m_fTime = 0.0;
	}

	inline float Time::AsFloat() const
	{
		return static_cast<float>(m_fTime);
	}

	inline double Time::GetNanoseconds() const
	{
		return m_fTime * 1000000000.0;
	}

	inline double Time::GetMicroseconds() const
	{
		return m_fTime * 1000000.0;
	}

	inline double Time::GetMilliseconds() const
	{
		return m_fTime * 1000.0;
	}

	inline double Time::GetSeconds() const
	{
		return m_fTime;
	}

	inline void Time::operator -= (const Time& other)
	{
		m_fTime -= other.m_fTime;
	}

	inline void Time::operator += (const Time& other)
	{
		m_fTime += other.m_fTime;
	}

	inline Time Time::operator - (const Time& other) const
	{
		return Time(m_fTime - other.m_fTime);
	}

	inline Time Time::operator + (const Time& other) const
	{
		return Time(m_fTime + other.m_fTime);
	}

	inline Time operator* (Time t, double f)
	{
		return Time::Seconds(t.GetSeconds() * f);
	}

	inline Time operator* (double f, Time t)
	{
		return Time::Seconds(t.GetSeconds() * f);
	}

	inline Time operator/ (Time t, double f)
	{
		return Time::Seconds(t.GetSeconds() / f);
	}

	inline Time operator/ (double f, Time t)
	{
		return Time::Seconds(t.GetSeconds() / f);
	}
}