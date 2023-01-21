#include "Ghost.h"

Ghost::Ghost(Cell* cell)
{
	c = cell;
	finished = false;
	currentCoin = NULL;
}

Ghost::~Ghost()
{
}
