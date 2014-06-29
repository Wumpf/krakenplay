#pragma once

namespace Krakenplay
{
	/// \brief The time class encapsulates a double value storing the time in seconds.
	///
	/// It offers convenient functions to get the time in other units.
	/// ezTime is a high-precision time using the OS specific high-precision timing functions
	/// and may thus be used for profiling as well as simulation code.
	class Time
	{
	public:

		/// \brief Gets the current time
		static Time Now();

		/// \brief Creates an instance of ezTime that was initialized from nanoseconds.
		static Time Nanoseconds(double fNanoseconds)    { return Time(fNanoseconds * 0.000000001); }

		/// \brief Creates an instance of ezTime that was initialized from microseconds.
		static Time Microseconds(double fMicroseconds)  { return Time(fMicroseconds * 0.000001); }

		/// \brief Creates an instance of ezTime that was initialized from milliseconds.
		static Time Milliseconds(double fMilliseconds)  { return Time(fMilliseconds * 0.001); }

		/// \brief Creates an instance of ezTime that was initialized from seconds.
		static Time Seconds(double fSeconds)            { return Time(fSeconds); }


		Time() { m_fTime = 0.0; }

		/// \brief Sets the time value to zero.
		void SetZero();

		/// \brief Returns the time as a float value (in seconds).
		///
		/// Useful for simulation time steps etc.
		/// Please note that it is not recommended to use the float value for long running
		/// time calculations since the precision can deteriorate quickly. (Only use for delta times is recommended)
		float AsFloat() const;

		/// \brief Returns the nanoseconds value
		double GetNanoseconds() const;

		/// \brief Returns the microseconds value
		double GetMicroseconds() const;

		/// \brief Returns the milliseconds value
		double GetMilliseconds() const;

		/// \brief Returns the seconds value.
		double GetSeconds() const;

		/// \brief Subtracts the time value of "other" from this instances value.
		void operator -= (const Time& other);

		/// \brief Adds the time value of "other" to this instances value.
		void operator += (const Time& other);

		/// \brief Returns the difference: "this instance - other"
		Time operator - (const Time& other) const;

		/// \brief Returns the sum: "this instance + other"
		Time operator + (const Time& other) const;

		bool operator< (const Time& rhs) const { return m_fTime <  rhs.m_fTime; }
		bool operator<=(const Time& rhs) const { return m_fTime <= rhs.m_fTime; }
		bool operator>(const Time& rhs) const { return m_fTime >  rhs.m_fTime; }
		bool operator>=(const Time& rhs) const { return m_fTime >= rhs.m_fTime; }
		bool operator==(const Time& rhs) const { return m_fTime == rhs.m_fTime; }
		bool operator!=(const Time& rhs) const { return m_fTime != rhs.m_fTime; }

	private:

		/// \brief For internal use only.
		explicit Time(double fTime);

		/// \brief The time is stored in seconds
		double m_fTime;

	public: // private:

		/// Initializes timer. This happens automatically!
		static void Initialize();
	};

	Time operator* (Time t, double f);
	Time operator* (double f, Time t);

	Time operator/ (Time t, double f);
	Time operator/ (double f, Time t);
}

#include "implementation/time_inl.h"
