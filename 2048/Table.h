#pragma once

#include <vector>
#include "Direction.h"

class Table
{
	std::vector<int> t;


public:
	Table();
	~Table();

	bool flip(Direction d);
};

