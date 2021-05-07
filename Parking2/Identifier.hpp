#pragma once
#include "Model.hpp"
long long ID;
struct Identifier:Model<long long> {
	Identifier() {
		this->v = ID;
		ID++;
	}
	Identifier(const long long &id) {
		this->v = id;
	}
};