#pragma once
#include "Cell.h"

class ComparePacman
{
public:
	ComparePacman();
	~ComparePacman();
	bool operator() (Cell* pc1, Cell* pc2) {
		return pc1->getF() < pc2->getF();
	}
};


