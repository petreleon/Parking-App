#pragma once
#include <map>
#include <tuple>
#include "Money.hpp"
#include "Zone.hpp"
#include "DayTime.hpp"
struct PricingTable {
	std::map<std::tuple<Zone, DayTime>, Money> table;
	void addPrice(Zone zone, Money price){
		price.approximate();
		DayTime first = { { { {0}, {0} } } }, iterator;
		iterator = first;
		do {
			this->table[{zone, iterator}] = price;
			iterator.increase({ 15 });
		} while (iterator != first);
	}
	void addPrice(Zone zone, Money price, DayTime begin, DayTime end) {
		price.approximate();
		
		for (DayTime iterator = begin; iterator != end; iterator.increase({ 15 })) {
			this->table[{zone, iterator}] = price;
		}
	}
};

