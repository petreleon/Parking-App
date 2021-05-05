#pragma once
#include "Model.hpp"
#include <iomanip>
#include <sstream>
struct Money : Model<double>
{
	void approximate() {
		// only first two decimals
		std::stringstream ss;
		ss << std::fixed << std::setprecision(2) << this->v;
		ss >> this->v;
	}
};