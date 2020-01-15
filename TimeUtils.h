#ifndef TIMEUTILS_H
#define TIMEUTILS_H

#include <chrono>
#include <ctime>

namespace utils
{
	class Time {
	public:

		enum Format {
			TIME = 1,
			DATE = 2,
			MS = 4,
			MCS = 8,
			TIME_MS = TIME | MS,
			DATETIME = TIME | DATE,
			DATETIME_MS = DATETIME | MS,
			ALL = DATETIME_MS | MCS
		};

		typedef long long time_mcs;
		typedef time_mcs  time_ms;
		typedef time_mcs  time_s;

		Time() = default;
		explicit Time(const time_mcs microseconds) : _mcs(microseconds) {}

		time_mcs GetMCS() const {return _mcs;}
		time_mcs getSeconds() const {

			const time_mcs seconds = _mcs / 1000 / 1000;
			return seconds;
		}

		std::string toString(const Format format = DATETIME, const bool local = false) const {
			const time_mcs mcsAll = _mcs;
			const time_ms msAll = _mcs / 1000;
			const time_s sAll = msAll / 1000;

			const auto seconds_int64 = (std::time_t) sAll;

			std::tm timeInfo;

			const decltype (&gmtime_s) gmTimeFunc = (local ? &localtime_s : &gmtime_s);
			gmTimeFunc(&timeInfo, &seconds_int64);

			constexpr unsigned int bufferL = 256;
			char strBuf[bufferL];

			std::string dateStr;
			std::string timeStr;
			std::string msStr;
			std::string mcsStr;

			if (format & DATE) {

				const int year = timeInfo.tm_year + 1900;
				const int mon = timeInfo.tm_mon + 1;

				sprintf_s(strBuf, "[%i/%i/%i]", timeInfo.tm_mday, mon, year);   //" %.0f %.0f %.0f", x, y, z);
				dateStr = strBuf + std::string(format > DATE ? " " : "");
			}

			if (format & TIME) {

				sprintf_s(strBuf, "%i:%i:%i", timeInfo.tm_hour, timeInfo.tm_min, timeInfo.tm_sec);
				timeStr = strBuf;
			}

			if (format & MS) {
				const time_ms pureMS = msAll - sAll * 1000;
				sprintf_s(strBuf, ".%lld", pureMS);
				msStr = strBuf;
			}

			if (format & MCS) {
				const time_mcs pureMCS = mcsAll - msAll * 1000;
				sprintf_s(strBuf, "[%lld]", pureMCS);
				mcsStr = strBuf;
			}

			sprintf_s(strBuf, "%s%s%s%s", dateStr.c_str(), timeStr.c_str(), msStr.c_str(), mcsStr.c_str());

			return strBuf;
		}

		float getDuration() const {

			const float seconds = _mcs / 1000.f / 1000.f;
			return seconds;
		}

		static Time CurrentTime() {

			const auto now = std::chrono::system_clock::now();
			const auto duration = now.time_since_epoch();
			const auto mcs = std::chrono::duration_cast<std::chrono::microseconds>(duration).count();

			const Time t(mcs);
			return t;
		}

		Time operator-(const Time& t2) const {

			const Time& tm1 = *this;

			const auto& mcs1 = tm1.GetMCS();
			const auto& mcs2 = t2.GetMCS();
			const auto dt = mcs1 - mcs2;

			const Time dtTime(dt);
			return dtTime;
		}

		static Time getDuration(const Time& first, const Time& second) {

			const Time& duration = second - first;
			return duration;
		}

		static float dt(const Time& first, const Time& second) {

			const Time& duration = getDuration(first, second);
			const float dt = duration.getDuration();
			return dt;
		}

		bool isNull() const {

			const bool eq = _mcs == 0;
			return eq;
		}
		bool operator==(const int other) const {

			const bool eq = _mcs == other;
			return eq;
		}

		bool operator==(const float other) const {

			const auto& duration = getDuration();
			const bool eq = (duration == other);
			return eq;
		}

	private:
		time_mcs _mcs {0};
	};

	inline bool operator< (const Time& t1, const Time& t2) {

		const auto& mcs1 = t1.GetMCS();
		const auto& mcs2 = t2.GetMCS();

		const bool less = mcs1 < mcs2;
		return less;
	}

	inline bool operator==(const Time& t1, const Time& t2) {

		const auto& mcs1 = t1.GetMCS();
		const auto& mcs2 = t2.GetMCS();

		const bool eq = mcs1 == mcs2;
		return eq;
	}

}

#endif