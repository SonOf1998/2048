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

bool Table::isGameOver() const noexcept
{
	if (flipAllowed(Direction::DOWN) || flipAllowed(Direction::UP) || flipAllowed(Direction::LEFT) || flipAllowed(Direction::RIGHT))
	{
		return false;
	}

	return true;
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
		
		for (int i = n * (n - 1); i >= 0; i -= n)	// az utols� sor elej�r�l akarunk indulni (n a n�gyzetalap� p�lya oldalhossza)
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
						if (t[i + j] == t[i + j - k])  // megtal�ltuk a f�l�tt�nk l�v� els� egyez� elemet
						{
							t[i + j] *= 2;			// a lentit megdupl�zzuk
							t[i + j - k] = 0;		// a f�ntit elt�netj�k

							if (!gapes[j].empty())
							{
								t[*gapes[j].cbegin()] = t[i + j];
								gapes[j].erase(gapes[j].begin());
								t[i + j] = 0;
							}

							foundEq = true;
							break;
						}

						if (t[i + j - k] != 0)	// f�lfel� menet n�zz�k a null�kat, ha kapunk egy olyat cell�t ami nem nulla �s nem is a keresett, akkor kil�p�nk
						{
							break;
						}

						k += n;
					}

					if (!foundEq)	// ha nem tal�ltunk egyez�s�get f�l�tt�nk
					{
						if (!gapes[j].empty())					// Ha van lejjebb �res cella
						{
							t[*gapes[j].cbegin()] = t[i + j];	// Kiv�lasztjuk a legm�lyebben l�v�t (a lista legutols�j�t), majd ide mozgatjuk az adott elemet
							gapes[j].erase(gapes[j].begin());	// A list�b�l kit�r�lj�k, mert a legm�lyebb index m�r foglalt
							t[i + j] = 0;						// Az adott elem r�gi helye most m�r �res
							gapes[j].push_back(i + j);			// Itt musz�j foglalkoznunk az �jonnan felszabadult mez� list�ba ad�s�hoz, nem b�zhatjuk r� egy f�l�tt�nk l�v� sorra
						}
					}
				}
			}
		}

		break;

	case Direction::UP: 

		for (int i = 0; i <= n * (n - 1); i += n)	// az utols� sor elej�r�l akarunk indulni (n a n�gyzetalap� p�lya oldalhossza)
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
					while (i + j + k < n * n)		// megn�zz�k, hogy az adott elem alatt van-e egyez�s�g, �res cell�kat kihagyva
					{
						if (t[i + j] == t[i + j + k])  // megtal�ltuk a alattunk l�v� els� egyez� elemet
						{
							t[i + j] *= 2;			// a f�ntit megdupl�zzuk
							t[i + j + k] = 0;		// a lentit elt�netj�k

							if (!gapes[j].empty())
							{
								t[*gapes[j].cbegin()] = t[i + j];
								gapes[j].erase(gapes[j].begin());
								t[i + j] = 0;
							}

							foundEq = true;
							break;
						}

						if (t[i + j + k] != 0)	// lefel� menet n�zz�k a null�kat, ha kapunk egy olyat cell�t ami nem nulla �s nem is a keresett, akkor kil�p�nk
						{
							break;
						}

						k += n;
					}

					if (!foundEq)	// ha nem tal�ltunk egyez�s�get alattunk
					{
						if (!gapes[j].empty())					// Ha van feljebb �res cella
						{
							t[*gapes[j].cbegin()] = t[i + j];	// Kiv�lasztjuk a legm�lyebben l�v�t (a lista legutols�j�t), majd ide mozgatjuk az adott elemet
							gapes[j].erase(gapes[j].begin());	// A list�b�l kit�r�lj�k, mert a legm�lyebb index m�r foglalt
							t[i + j] = 0;						// Az adott elem r�gi helye most m�r �res
							gapes[j].push_back(i + j);			// Itt musz�j foglalkoznunk az �jonnan felszabadult mez� list�ba ad�s�hoz, nem b�zhatjuk r� egy alattunk l�v� sorra
						}
					}
				}
			}
		}
		break;

	case Direction::LEFT: 
		
		for (int i = 0; i < n; ++i)							// az els� oszlop elej�r�l akarunk indulni (n a n�gyzetalap� p�lya oldalhossza)
		{
			for (int j = 0; j <= n * (n - 1); j += n)		// elindulunk az oszlop legel�j�r�l
			{
				if (t[i + j] == 0)				// ha �res (=0) a cella
				{
					gapes[j / n].push_back(i + j);
				}
				else							// ha nem akkor...
				{
					int k = 1;
					bool foundEq = false;
					while (i + j + k < ((j / n) + 1) * n)		// megn�zz�k, hogy az adott elemt�l jobbra van-e egyez�s�g, �res cell�kat kihagyva
					{
						if (t[i + j] == t[i + j + k])  // megtal�ltuk a jobbra l�v� els� egyez� elemet
						{
							t[i + j] *= 2;			// a bal oldalit megdupl�zzuk
							t[i + j + k] = 0;		// a jobb oldalit elt�netj�k

							if (!gapes[j / n].empty())
							{
								t[*gapes[j / n].cbegin()] = t[i + j];
								gapes[j / n].erase(gapes[j / n].begin());
								t[i + j] = 0;
							}

							foundEq = true;
							break;
						}

						if (t[i + j + k] != 0)	// jobbramenet n�zz�k a null�kat, ha kapunk egy olyat cell�t ami nem nulla �s nem is a keresett, akkor kil�p�nk
						{
							break;
						}

						++k;
					}

					if (!foundEq)	// ha nem tal�ltunk egyez�s�get alattunk
					{
						if (!gapes[j / n].empty())					// Ha van balra �res cella
						{
							t[*gapes[j / n].cbegin()] = t[i + j];	// Kiv�lasztjuk a legm�lyebben l�v�t (a lista legutols�j�t), majd ide mozgatjuk az adott elemet
							gapes[j / n].erase(gapes[j / n].begin());	// A list�b�l kit�r�lj�k, mert a legm�lyebb index m�r foglalt
							t[i + j] = 0;							// Az adott elem r�gi helye most m�r �res
							gapes[j / n].push_back(i + j);			// Itt musz�j foglalkoznunk az �jonnan felszabadult mez� list�ba ad�s�hoz, nem b�zhatjuk r� egy jobbra l�v� sorra
						}
					}
				}
			}
		}
		break;
	
	case Direction::RIGHT:

		for (int i = n - 1; i >= 0; --i)					// az utols� oszlop elej�r�l akarunk indulni (n a n�gyzetalap� p�lya oldalhossza)
		{
			for (int j = 0; j <= n * (n - 1); j += n)		// elindulunk az oszlop legel�j�r�l
			{
				if (t[i + j] == 0)				// ha �res (=0) a cella
				{
					gapes[j / n].push_back(i + j);
				}
				else							// ha nem akkor...
				{
					int k = 1;
					bool foundEq = false;
					while (i + j - k >= j)		// megn�zz�k, hogy az adott elemt�l balra van-e egyez�s�g, �res cell�kat kihagyva
					{
						if (t[i + j] == t[i + j - k])  // megtal�ltuk a balra l�v� els� egyez� elemet
						{
							t[i + j] *= 2;			// a jobb oldalit megdupl�zzuk
							t[i + j - k] = 0;		// a bal oldalit elt�netj�k

							if (!gapes[j / n].empty())
							{
								t[*gapes[j / n].cbegin()] = t[i + j];
								gapes[j / n].erase(gapes[j / n].begin());
								t[i + j] = 0;
							}

							foundEq = true;
							break;
						}

						if (t[i + j - k] != 0)	// balramenet n�zz�k a null�kat, ha kapunk egy olyat cell�t ami nem nulla �s nem is a keresett, akkor kil�p�nk
						{
							break;
						}

						++k;
					}

					if (!foundEq)	// ha nem tal�ltunk egyez�s�get alattunk
					{
						if (!gapes[j / n].empty())					// Ha van jobbra �res cella
						{
							t[*gapes[j / n].cbegin()] = t[i + j];		// Kiv�lasztjuk a legm�lyebben l�v�t (a lista legutols�j�t), majd ide mozgatjuk az adott elemet
							gapes[j / n].erase(gapes[j / n].begin());	// A list�b�l kit�r�lj�k, mert a legm�lyebb index m�r foglalt
							t[i + j] = 0;							// Az adott elem r�gi helye most m�r �res
							gapes[j / n].push_back(i + j);			// Itt musz�j foglalkoznunk az �jonnan felszabadult mez� list�ba ad�s�hoz, nem b�zhatjuk r� egy balra l�v� sorra
						}
					}
				}
			}
		}
		break;
	}

	t[randomEmptyCellIndex()] = randomNewTile();

	return isGameOver();
}

bool Table::flipAllowed(Direction dir) const noexcept
{
	switch (dir)
	{
	case Direction::UP:
		for (int i = n; i < n * n; ++i)
		{
			if (t[i] != 0)
			{
				if (t[i - n] == t[i] || t[i - n] == 0)
				{
					return true;
				}
			}
		}
		break;

	case Direction::DOWN:
		for (int i = 0; i < n * (n - 1); ++i)
		{
			if (t[i] != 0)
			{
				if (t[i + n] == t[i] || t[i + n] == 0)
				{
					return true;
				}
			}
		}
		break;

	case Direction::LEFT:
		for (int i = 1; i < n; ++i)
		{
			for (int j = 0; j < n * n; j += n)
			{
				if (t[i + j] != 0)
				{
					if (t[i + j - 1] == t[i + j] || t[i + j - 1] == 0)
					{
						return true;
					}
				}
			}
		}
		break;

	case Direction::RIGHT:
		for (int i = 0; i < n - 1; ++i)
		{
			for (int j = 0; j < n * n; j += n)
			{
				if (t[i + j] != 0)
				{
					if (t[i + j + 1] == t[i + j] || t[i + j + 1] == 0)
					{
						return true;
					}
				}
			}
		}
		break;
	}

	return false;
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
