#include "Cell.h"



Cell::Cell()
{
	parent = nullptr;
	g = 0;
	h = 0;
	f = 0;
}

Cell::Cell(int r, int c, Cell * p)
{
	row = r;
	col = c;
	parent = p;
	g = 0;
	h = 0;
	f = 0;
}


Cell::~Cell()
{
}


bool Cell::compareCells(Cell* other)
{
	if (row == other->getRow() && col == other->getCol())
		return true;
	return false;
}