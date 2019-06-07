#pragma once

#include <memory>
#include <vector>

#include "Direction.h"
#include "Memento.h"

class Table
{
private:

	int n;
	std::vector<int> t;
	int randomEmptyCellIndex() const noexcept;
	int randomNewTile() const noexcept;
	bool isGameOver() const noexcept;

public:
	Table() = default;
	Table(int);
	~Table() = default;
	Table& operator=(Table&&);

	bool flip(Direction) noexcept;
	bool flipAllowed(Direction) const noexcept;
	std::unique_ptr<Memento> createMemento() const;
	void restore(Memento&);
	int getPoints() const noexcept;
	void print() const noexcept;
};

