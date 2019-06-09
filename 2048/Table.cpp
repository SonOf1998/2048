#include "pch.h"
#include "Table.h"

#include <iostream>
#include <list>
#include <numeric>
#include <random>

#define POSX0 -300.0f
#define POSY0 300.0f
#define LENGTH 600.0f
#define GAP 2.0f

int Table::randomEmptyCellIndex() const noexcept
{
	std::random_device rd;
	std::mt19937 rng(rd());
	std::uniform_int_distribution<int> uid(0, n * n - 1);

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
		std::uniform_int_distribution<int> uid(1, 3);

		if (uid(rng) == 3)	// 100pont felett 1/3 es�llyel m�r 4-es spawnol, k�l�nben csak kettes
		{
			return 4;
		}
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

Table::Table(int n) : n(n)
{
	if (n < 4)
	{
		n = 4;
	}

	if (n > 6)
	{
		n = 6;
	}
	
	t.reserve(n * n);
	t = std::move(std::vector<int>(n * n, 0));
	t[randomEmptyCellIndex()] = 2;
	t[randomEmptyCellIndex()] = 2;
}

Table & Table::operator=(Table&& rhs)
{
	if (this != &rhs)
	{
		t = std::move(rhs.t);
		n = rhs.n;
	}

	return *this;
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

Memento* Table::createMemento() const
{
	return new Memento(t);
}

void Table::restore(Memento* memento)
{
	t = std::move(memento->t);
}

int Table::getPoints() const noexcept
{
	return std::accumulate(t.cbegin(), t.cend(), 0);
}

bool Table::detect2048() const noexcept
{
	return std::any_of(t.cbegin(), t.cend(), [](int i) {return i == 2048; });
}

void Table::print() const noexcept
{
	for (int i = 0; i < n * n; i += n)
	{
		for (int j = 0; j < n; ++j)
		{
			std::cout << t[i + j] << " ";
		}
		std::cout << std::endl;
	}
}

std::set<int> Table::getUniqueElements() const noexcept
{
	std::set<int> result(t.cbegin(), t.cend());
	result.erase(0);

	return result;
}

std::vector<float> Table::getVertexData(int x) const noexcept
{
	std::vector<float> result;

	for (int i = 0; i < n; ++i)
	{
		for (int j = 0; j < n; ++j)
		{
			if (t[i * n + j] == x)
			{
				result.push_back(GAP + POSX0 + j * LENGTH / n); result.push_back(- GAP + POSY0 - i * LENGTH / n); result.push_back(-0.5f);	// bal fels� cs�cs x,y,z-je a poz�ci� f�ggv�ny�ben
				result.push_back(0.0f); result.push_back(1.0f);																	// bal fels� cs�cs text�ra koordin�t�i
				result.push_back(POSX0 + (j + 1) * LENGTH / n); result.push_back(- GAP + POSY0 - i * LENGTH / n); result.push_back(-0.5f);	// jobb fels� cs�cs x,y,z-je a poz�ci� f�ggv�ny�ben
				result.push_back(1.0f); result.push_back(1.0f);																		// jobb fels� cs�cs text�ra koordin�t�i
				result.push_back(GAP + POSX0 + j * LENGTH / n); result.push_back(POSY0 - (i + 1) * LENGTH / n); result.push_back(-0.5f);	// bal als� cs�cs x,y,z-je a poz�ci� f�ggv�ny�ben
				result.push_back(0.0f); result.push_back(0.0f);																		// bal als� cs�cs text�ra koordin�t�i
				result.push_back(POSX0 + (j + 1) * LENGTH / n); result.push_back(POSY0 - (i + 1) * LENGTH / n); result.push_back(-0.5f);	// jobb als� cs�cs x,y,z-je a poz�ci� f�ggv�ny�ben
				result.push_back(1.0f); result.push_back(0.0f);																				// jobb als� cs�cs text�ra koordin�t�i
			}
		}
	}

	return result;
}

std::vector<unsigned int> Table::getIndexData(int x) const noexcept
{
	std::vector<unsigned int> result;

	int cnt = 0;

	for (int i = 0; i < n * n; ++i)
	{
		if (t[i] == x)
		{
			std::vector<int> tmp = { 0 + cnt, 1 + cnt, 2 + cnt, 2 + cnt, 1 + cnt, 3 + cnt };
			result.insert(result.cend(), tmp.cbegin(), tmp.cend());
			cnt += 4;
		}
	}

	return result;
}
