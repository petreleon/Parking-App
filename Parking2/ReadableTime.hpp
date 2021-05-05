#pragma once
#include "Model.hpp"
#include <string>
#include <ctime>
#include <iomanip>
struct ReadableTime: Model<std::string>{
	void fromTm(const tm& time) {
		std::stringstream timeStream;
		timeStream << std::put_time(&time, "%d %m %Y %H:%M:%S");
		this->v = timeStream.str();
	}
	void toTm(tm& time) {
		std::stringstream timeStream(this->v);
		timeStream >> std::get_time(&time, "%d %m %Y %H:%M:%S");
	}
};
