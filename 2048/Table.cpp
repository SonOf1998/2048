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
	for (int i = 0; i < n; i++)
	{
		gapes.emplace_back();
	}

	switch (dir)
	{
	case Direction::DOWN: 
		
		for (int i = n * (n - 1); i >= 0; i -= n)	// az utolsó sor elejérõl akarunk indulni (n a négyzetalapú pálya oldalhossza)
		{
			for (int j = 0; j < n; ++j)			// elindulunk a sor legeléjérõl
			{
				if (t[i + j] == 0)				// ha üres (=0) a cella
				{
					gapes[j].push_back(i + j);
				}
				else							// ha nem akkor...
				{
					int k = n;
					bool foundEq = false;
					while (i + j - k >= 0)		// megnézzük, hogy az adott elem fölött van-e egyezõség, üres cellákat kihagyva
					{
						if (t[i + j] == t[i + j - k])  // megtaláltuk a fölöttünk lévõ elsõ egyezõ elemet
						{
							t[i + j] *= 2;			// a lentit megduplázzuk
							t[i + j - k] = 0;		// a föntit eltünetjük

							if (!gapes[j].empty())
							{
								t[*gapes[j].cbegin()] = t[i + j];
								gapes[j].erase(gapes[j].begin());
								t[i + j] = 0;
							}

							foundEq = true;
							break;
						}

						if (t[i + j - k] != 0)	// fölfelé menet nézzük a nullákat, ha kapunk egy olyat cellát ami nem nulla és nem is a keresett, akkor kilépünk
						{
							break;
						}

						k += n;
					}

					if (!foundEq)	// ha nem találtunk egyezõséget fölöttünk
					{
						if (!gapes[j].empty())					// Ha van lejjebb üres cella
						{
							t[*gapes[j].cbegin()] = t[i + j];	// Kiválasztjuk a legmélyebben lévõt (a lista legutolsóját), majd ide mozgatjuk az adott elemet
							gapes[j].erase(gapes[j].begin());	// A listából kitöröljük, mert a legmélyebb index már foglalt
							t[i + j] = 0;						// Az adott elem régi helye most már üres
							gapes[j].push_back(i + j);			// Itt muszáj foglalkoznunk az újonnan felszabadult mezõ listába adásához, nem bízhatjuk rá egy fölöttünk lévõ sorra
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
