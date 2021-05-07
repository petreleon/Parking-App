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
	void safeAddToParkingSimulator(const Time& time, const ParkingPlace& parkingPlace, const Car& car) {
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
		// reserve the future
		if (timeBegin < actualTime) {
			return false;
		}

		// timeEnd always after timeBegin
		if (timeEnd <= timeBegin) {
			return false;
		}

		// time restricted by parkinng period
		if (not place.parkingType.validPeriod(timeBegin, timeEnd)) {
			return false;
		}

		// maxCapacity not arrived
		for (Time time = timeBegin; time < timeEnd; time.addTime<std::chrono::minutes>(15)) {
			if (parkingSimulator.count({ time, place })) {
				if (parkingSimulator[{ time, place }].size() == place.maxCapacity.v) {
					return false;
				}
			}
		}
		return true;
	}

	Money priceSimulation(Zone zone, Time timeBegin, Time timeEnd) {
		Money money{ 0 };
		for (Time timeIterator = timeBegin; timeIterator < timeEnd; timeIterator.addTime<std::chrono::minutes>(15)) {
			tm tmIterator = timeIterator.to_gmtm();
			DayTime dayTime = { { { {tmIterator.tm_hour}, {tmIterator.tm_min} } } };
			money.v += pricingTable.table[{zone,dayTime}].v;
		}
		money.v /= 4;
		money.approximate();
		return money;
	}

	void addEvents(Car car, Time timeBegin, Time timeEnd, ParkingPlace place) {
		Time timeMessage = timeEnd;
		timeMessage.addTime<std::chrono::minutes>(-10-5); // -10 minus step
		// insert pricint to wallet
		carTimer[timeEnd].insert({ car.v , [&, place, timeBegin, timeEnd]() {
			Zone zone = placeZone[place];
			wallet.v += priceSimulation(zone, timeBegin, timeEnd).v;
		} });
		// create event who create event message
		EventVector[timeMessage].push_back({ [&, car, timeBegin, timeEnd, place]() {
			Identifier id; // for self destroying
			messageEvent[id] = {
				// delete
				[&, id]() {
					EventVector[actualTime].push_back({
						[&, id]() {
							messageEvent.erase(id);
						}
					});
				},
				// actionate
				[&, car, timeBegin, timeEnd, place]() {
					Time newTimeEnd = timeEnd;
					newTimeEnd.addTime<std::chrono::minutes>(15);
					if (possibleCarParking(place, timeEnd, newTimeEnd)) {
						carTimer[timeEnd].erase({car.v});
						safeAddToParkingSimulator(timeEnd, place, car);
						// recursion
						addEvents(car, timeBegin, newTimeEnd, place);
					}
				},
				// time to compare expiration
				timeEnd
			};
		} });
	}

	void safeParkingCar(ParkingPlace place, Car car, Time timeBegin, Time timeEnd){
		if(possibleCarParking(place, timeBegin, timeEnd)){
			for (Time timeIterator = timeBegin; timeIterator < timeEnd; timeIterator.addTime<std::chrono::minutes>(15))
			{
				safeAddToParkingSimulator(timeIterator, place, car);
			}
			addEvents(car, timeBegin, timeEnd, place);
		}
	}
	void timeForward() {
		// make all events happens
		for (auto& iterator : messageEvent) {
			iterator.second.checkIfIgnored(actualTime);
		}
		if (carTimer.count(actualTime)) {
			for (auto& carEvent : carTimer[actualTime]) {
				carEvent.execution();
			}
			carTimer.erase(actualTime);

		}
		if (EventVector.count(actualTime)) {
			for (auto& unnamedEvent : EventVector[actualTime]) {
				unnamedEvent.v();
			}
			EventVector.erase(actualTime);

		}
		actualTime.addTime<std::chrono::minutes>(5);
	}
	void timeForwardUntil(Time timeUntil) {
		while (actualTime < timeUntil) {
			timeForward();
		}
	}
};
