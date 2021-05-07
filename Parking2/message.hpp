#pragma once
//selfdeleting message
#include <functional>
#include "time.hpp"

struct Message {
	std::function< void()> delete_;
	std::function< void()> actionate;
	Time until;

	void accept() {
		actionate();
		delete_();
	}
	void reject() {
		delete_();
	}
	void checkIfIgnored(Time actual) {
		if (actual < until) {
			return;
		}
		else {
			delete_();
		}
	}
};
