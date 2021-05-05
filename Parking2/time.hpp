#pragma once
#include "Model.hpp"
#include <ctime>
#include <chrono>
#include "ReadableTime.hpp"
struct Time: Model<std::time_t> {
	template<class type> void addTime(const int);
	tm to_gmtm();
	void from_gmtm(tm& gmtm);
	void fromReadable(ReadableTime& readable);
	void toReadable(ReadableTime& readable);
};

template<class type>
inline void Time::addTime(const int value)
{
	this->v = std::chrono::system_clock::to_time_t(std::chrono::system_clock::from_time_t(this->v) + type(value));
}


inline tm Time::to_gmtm() {
	tm toReturn;
	gmtime_s(&toReturn, &this->v);
	return toReturn;
}

inline void Time::from_gmtm(tm& gmtm) {
	this->v = _mkgmtime(&gmtm);
}

inline void Time::fromReadable(ReadableTime& readable)
{
	tm time_object;
	readable.toTm(time_object);
	this->from_gmtm(time_object);
}

inline void Time::toReadable(ReadableTime& readable)
{
	tm time_object = this->to_gmtm();
	readable.fromTm(time_object);
}
