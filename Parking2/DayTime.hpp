#pragma once
#include "Model.hpp"
#include <tuple>
#include <ctime>
#include "Hour.hpp"
#include "Minute.hpp"
#include "time.hpp"

struct DayTime : Model<std::tuple<Hour, Minute>> {
	void increase(Minute&& minutes) {
		std::get<Minute>(this->v).v += minutes.v;
		std::get<Hour>(this->v).v += std::get<Minute>(this->v).v / 60;
		std::get<Minute>(this->v).v %= 60;
		std::get<Hour>(this->v).v %= 24;
	}
	void fromTime(Time&& time) {
		std::tm tmStruct = time.to_gmtm();
		this->v = { {tmStruct.tm_hour}, {tmStruct.tm_min} };
	}
};