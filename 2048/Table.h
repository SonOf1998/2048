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

	std::set<int> getUniqueElements() const noexcept;		// megadja egy halmazban, hogy milyen �rt�kek vannak a t�bl�ban.. Ha 0 is van azt nem, mivel nem text�r�zzuk
	std::vector<float> getVertexData(int) const noexcept;	// visszaadja a param�terk�nt megadott n�gyzetek koordin�t�it (POSITION + TEXTURE U,V)
	std::vector<int> getIndexData(int) const noexcept;		// visszaadja a sorrendis�get, hogy a getVertexData-ban l�v� h�romsz�geket milyen sorrendbe rajzoljuk ki
};

