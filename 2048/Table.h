#pragma once

#include <memory>
#include <set>
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
	void print() const noexcept;	// for debug

	std::set<int> getUniqueElements() const noexcept;		// megadja egy halmazban, hogy milyen értékek vannak a táblában.. Ha 0 is van azt nem, mivel nem textúrázzuk
	std::vector<float> getVertexData(int) const noexcept;	// visszaadja a paraméterként megadott négyzetek koordinátáit (POSITION + TEXTURE U,V)
	std::vector<int> getIndexData(int) const noexcept;		// visszaadja a sorrendiséget, hogy a getVertexData-ban lévõ háromszögeket milyen sorrendbe rajzoljuk ki
};

