#pragma once
#include "Cell.h"
#include <queue>
#include <vector>
#include "ComparePacman.h"

class Pacman
{
private:
	Cell* c;
	bool finished;
	int coinNum;
	int minGhostDistance = 15;
	priority_queue <Cell*, vector<Cell*>, ComparePacman > pq;
	vector<Cell*> visited;
	vector<Cell*> bfs;
	int currentCoinRow, currentCoinCol;
public:
	Pacman(Cell* cell);
	~Pacman();
	Cell* getCell() { return c; }
	void setCell(Cell* cell) { c = cell; }
	priority_queue <Cell*, vector<Cell*>, ComparePacman >& getPq() { return pq; }
	bool getFinished() { return finished; }
	void setFinished(bool state) { finished = state; }
	void setCoinRow(int row) { currentCoinRow = row; }
	void setCoinCol(int col) { currentCoinRow = col; }
	int getCoinRow() { return currentCoinRow; }
	int getCoinCol() { return currentCoinCol; }
	int getCoinNum() { return coinNum; }
	int getMinGhostDistance() { return minGhostDistance; }
	void addCoin() { coinNum += 1; }
	vector<Cell*>& getVisited() { return visited; }
	vector<Cell*>& getBFS() { return bfs; }
};

