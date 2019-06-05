#pragma once

#include <vector>

class Memento
{
private:
	std::vector<int> t;
	friend class Table;

public:
	Memento() = default;
	Memento(std::vector<int>);
	~Memento() = default;
};

