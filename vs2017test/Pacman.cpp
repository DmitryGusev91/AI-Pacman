#include "Pacman.h"

Pacman::Pacman(Cell* cell)
{
	c = cell;
	finished = false;
	coinNum = 0;
}

Pacman::~Pacman()
{
}
