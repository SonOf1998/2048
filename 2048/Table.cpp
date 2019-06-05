#include "pch.h"
#include "Table.h"

#include <list>
#include <numeric>
#include <random>

int Table::randomEmptyCellIndex() const noexcept
{
	std::random_device rd;
	std::mt19937 rng(rd());
	std::uniform_int_distribution<int> uid(0, 16 - 1);

	int rand = uid(rng);

	while (t[rand] >= 2)
	{
		rand = uid(rng);
	}

	return rand;
}

int Table::randomNewTile() const noexcept
{
	if (getPoints() >= 100)
	{
		std::random_device rd;
		std::mt19937 rng(rd());
		std::uniform_int_distribution<int> uid(1, 2);

		return 2 * uid(rng);
	}

	return 2;
}

Table::Table(int n = 0) : n(n)
{
	t[randomEmptyCellIndex()] = 2;
	t[randomEmptyCellIndex()] = 2;
}

bool Table::flip(Direction dir) noexcept
{
	std::vector<std::list<int>> gapes;


	switch (dir)
	{
	case Direction::DOWN: 
		
		for(int i = n * (n - 1); i >= 0; i -= n)	// az utols� sor elej�r�l akarunk indulni (n a n�gyzetalap� p�lya oldalhossza)
		{
			for (int j = 0; j < n; ++j)			// elindulunk a sor legel�j�r�l
			{
				if (t[i + j] == 0)				// ha �res (=0) a cella
				{
					gapes[j].push_back(i + j);
				} 
				else							// ha nem akkor...
				{
					int k = n;
					bool foundEq = false;
					while (i + j - k >= 0)		// megn�zz�k, hogy az adott elem f�l�tt van-e egyez�s�g, �res cell�kat kihagyva
					{
						if (t[i + j - k] != 0 && t[i + j] == t[i + j + k])  // megtal�ltuk a f�l�tt�nk l�v� els� egyez� elemet
						{
							t[i + j] *= 2;			// a lentit megdupl�zzuk
							t[i + j - k] = 0;		// a f�ntit elt�netj�k

							if (!gapes[j].empty())
							{
								t[gapes[j].back()] = t[i + j];
								gapes[j].remove(gapes[j].back());
								t[i + j] = 0;
							}

							foundEq = true;
							break;
						}

						k += n;
					}

					if (!foundEq)	// ha nem tal�ltunk egyez�s�get f�l�tt�nk
					{
						if (!gapes[j].empty())					// Ha van lejjebb �res cella
						{
							t[gapes[j].back()] = t[i + j];		// Kiv�lasztjuk a legm�lyebben l�v�t (a lista legutols�j�t), majd ide mozgatjuk az adott elemet
							gapes[j].remove(gapes[j].back());	// A list�b�l kit�r�lj�k, mert a legm�lyebb index m�r foglalt
							t[i + j] = 0;						// Az adott elem r�gi helye most m�r �res
						}
					}
				}
			}
		}		

		break;

	case Direction::UP: 
		
		break;

	case Direction::LEFT: 
		
		break;
	
	case Direction::RIGHT:
		
		break;
	}
}

std::unique_ptr<Memento> Table::createMemento() const
{
	return std::unique_ptr<Memento>(new Memento(t));
}

void Table::restore(Memento& memento)
{
	t = std::move(memento.t);
}

int Table::getPoints() const noexcept
{
	return std::accumulate(t.cbegin(), t.cend(), 0);
}
