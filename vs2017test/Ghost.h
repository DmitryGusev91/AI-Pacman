#pragma once
#include "Cell.h"
#include <queue>
#include <vector>
#include "CompareCells.h"

class Ghost
{
private:
	Cell* c;
	bool finished ;
	priority_queue <Cell*, vector<Cell*>, CompareCells > pq;
	vector<Cell*> visited;
	vector<Cell*> coins;
	Cell* currentCoin;
public:
	Ghost(Cell* cell);
	~Ghost();
	Cell* getCell() { return c; }
	void setCell(Cell* cell) { c = cell; }
	bool getFinished() { return finished; }
	void setFinished(bool state) { finished = state; }
	priority_queue <Cell*, vector<Cell*>, CompareCells >& getPq() { return pq; }
	vector<Cell*>& getVisited() { return visited; }
	vector<Cell*>& getCoins() { return coins; }
	void setCurrentCoin(Cell* coin) { currentCoin = coin; }
	Cell* getCurrentCoin() { return currentCoin; }
};

