#pragma once
#include "Model.hpp"
#include "Car.hpp"
#include <functional>
struct EventCar : Car {
	std::function< void() > execution;
};