#pragma once
#include <string>
#include "Model.hpp"
#include "ParkingType.hpp"
#include "MaxCapacity.hpp"
struct ParkingPlace : Model<std::string> {
	ParkingType parkingType;
	MaxCapacity maxCapacity;
};