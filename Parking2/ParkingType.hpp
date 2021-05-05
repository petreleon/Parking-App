#pragma once
#include "Model.hpp"
#include "time.hpp"
#include <chrono>
#include <string>
struct ParkingType : Model<std::string>
{
	bool validPeriod(Time &timeBegin, Time &timeEnd) {
		Time maxTime = timeBegin;
		if (this->v == "ORANGE") {
			maxTime.addTime<std::chrono::hours>(1);
		}
		if (this->v == "PURPLE") {
			maxTime.addTime<std::chrono::hours>(3);
		}
		if (this->v == "BLUE") {
			maxTime.addTime<std::chrono::hours>(1);
		}
		return timeEnd <= maxTime and timeBegin < timeEnd;
	}
};