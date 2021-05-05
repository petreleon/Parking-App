#pragma once
#include <set>
#include <vector>
#include "PricingTable.hpp"
#include "EventCar.hpp"
#include "ParkingPlace.hpp"
#include "Zone.hpp"
#include "EventCar.hpp"
#include "time.hpp"
#include "UnnamedEvent.hpp"
#include "Money.hpp"
#include "message.hpp"
#include "Identifier.hpp"

struct ParkingManager {
	PricingTable pricingTable;
	Time actualTime;
	Money wallet{0};
	std::map<ParkingPlace, Zone> placeZone;
	std::map<Time, std::set<EventCar>> carTimer;
	std::map<Identifier, Message> messageEvent;

	std::map<Time, std::vector<UnnamedEvent>> EventVector;
	std::map<std::tuple<Time, ParkingPlace>, std::set<Car>> parkingSimulator;
	void safeAddToParkingSimulator(Time& time, ParkingPlace& parkingPlace, Car& car) {
		if (!parkingSimulator.count({time, parkingPlace})) {
			parkingSimulator[{time, parkingPlace}] = {};
			Time timeToDelete = time;
			timeToDelete.addTime<std::chrono::minutes>(5);
			EventVector[timeToDelete].push_back({ [&, time, parkingPlace]() {
				parkingSimulator.erase({time, parkingPlace});
			} });
		}
		parkingSimulator[{time, parkingPlace}].insert(car);
	}
	bool possibleCarParking(ParkingPlace place, Time timeBegin, Time timeEnd) {
		if (timeBegin < actualTime) {
			return false;
		}
		if (timeEnd <= timeBegin) {
			return false;
		}
		if (not place.parkingType.validPeriod(timeBegin, timeEnd)) {
			return false;
		}
		if (place.maxCapacity.v == 0) {
			return false;
		}
		for (Time time = timeBegin; time < timeEnd; time.addTime<std::chrono::minutes>(15)) {
			if (parkingSimulator.count({ time, place })) {
				if (parkingSimulator[{ time, place }].size() == place.maxCapacity.v) {
					return false;
				}
			}
		}
		return true;
	}
	void safeParkingCar(ParkingPlace place, Car car, Time timeBegin, Time timeEnd){
		if(possibleCarParking(place, timeBegin, timeEnd)){
			for (Time timeIterator = timeBegin; timeIterator < timeEnd; timeIterator.addTime<std::chrono::minutes>(15))
			{
				safeAddToParkingSimulator(timeIterator, place, car);
			}
		}

	}
	void timeForward() {
		// make all events happens
		for (auto& iterator : messageEvent) {
			iterator.second.checkIfIgnored(actualTime);
		}
		for (auto& carEvent : carTimer[actualTime]) {
			carEvent.execution();
		}
		carTimer.erase(actualTime);
		for (auto& unnamedEvent : EventVector[actualTime]) {
			unnamedEvent.v();
		}
		actualTime.addTime<std::chrono::minutes>(5);
	}

};
