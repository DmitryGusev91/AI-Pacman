#pragma once
#include "Cell.h"

class CompareCells
{
public:
	CompareCells();
	~CompareCells();
	bool operator() (Cell* pc1, Cell* pc2) {
		return pc1->getF() > pc2->getF();
	}
};


