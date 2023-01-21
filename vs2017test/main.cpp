#include "glut.h"
#include <time.h>
#include <vector>
#include <iostream>
#include "Cell.h"
#include <math.h>
#include <queue>
#include "Params.h"
#include "CompareCells.h"
#include "Ghost.h"
#include "Pacman.h"

using namespace std;

const int MSZ = 100;

int coinsNum = 40;
int maze[MSZ][MSZ];
bool startAStar = false, gameOver = true,startBFS=true, startEscape=false;

Pacman* pacman;
Ghost* ghost1;
Ghost* ghost2;
Ghost* ghost3;
Ghost* ghost4;


void InitMaze();

void init()
{
	glClearColor(0.8, 0.8, 0.8, 0);// color of window background
	//           RED GREEN BLUE

	glOrtho(0, MSZ, 0, MSZ, -1, 1);
	//     left right bottom top near, far

	srand(time(0)); // initialize random seed

	InitMaze();

}


void InitMaze() 
{
	int i, j;
	// setup borders
	for (i = 0; i < MSZ; i++)
	{
		maze[0][i] = WALL;
		maze[MSZ-1][i] = WALL;
		maze[i][0] = WALL;
		maze[i][MSZ - 1] = WALL;
	}
	// setup inner space of maze
	for(i=1;i<MSZ-1;i++)
		for (j = 1; j < MSZ - 1; j++)
		{
			if (i % 2 == 0) // mostly WALLs
			{
				if (rand() % 100 < 50)
					maze[i][j] = WALL; // 65%
				else
					maze[i][j] = SPACE; // 35%
			}
			else // mostly SPACEs
			{
				if (rand() % 100 < 85)
					maze[i][j] = SPACE;
				else
					maze[i][j] = WALL;
			}
		}

	//create pacman in the middle
	Cell* pcp = new Cell(MSZ / 2, MSZ / 2, nullptr);
	pacman = new Pacman(pcp);
	pacman->getPq().push(pcp);
	pacman->getBFS().push_back(pcp);
	maze[pacman->getCell()->getRow()][pacman->getCell()->getCol()] = PACMAN;

	
	//create ghost 1
	Cell* pcg1 = new Cell(rand() % (MSZ - 1) + 1, rand() % (MSZ - 1) + 1, nullptr);
	ghost1 = new Ghost(pcg1);
	ghost1->getPq().push(pcg1);
	maze[ghost1->getCell()->getRow()][ghost1->getCell()->getCol()] = GHOST;

	//create ghost 2
	Cell* pcg2 = new Cell(rand() % (MSZ - 1) + 1, rand() % (MSZ - 1) + 1, nullptr);
	ghost2 = new Ghost(pcg2);
	ghost2->getPq().push(pcg2);
	maze[ghost2->getCell()->getRow()][ghost2->getCell()->getCol()] = GHOST;

	//create ghost 3
	Cell* pcg3 = new Cell(rand() % (MSZ - 1) + 1, rand() % (MSZ - 1) + 1, nullptr);
	ghost3 = new Ghost(pcg3);
	ghost3->getPq().push(pcg3);
	maze[ghost3->getCell()->getRow()][ghost3->getCell()->getCol()] = GHOST;

	//create ghost 4
	Cell* pcg4 = new Cell(rand() % (MSZ - 1) + 1, rand() % (MSZ - 1) + 1, nullptr);
	ghost4 = new Ghost(pcg4);
	ghost4->getPq().push(pcg4);
	maze[ghost4->getCell()->getRow()][ghost4->getCell()->getCol()] = GHOST;
 
	//create N coins in the maze
	// Coin distribution
	for (i = 1; i < MSZ - 2; i++)
		for (j = 1; j < MSZ - 2; j++)
		{
			if (rand() % 100 > 97 && maze[i][j]!=GHOST && maze[i][j] != WALL && maze[i][j] != PACMAN)
				maze[i][j] = COIN; // 60% coins
			}
}

void ShowMaze() 
{
	int i, j;
	for(i=0;i<MSZ;i++)
		for(j=0;j<MSZ;j++)
		{
			switch (maze[i][j]) 
			{
			case WALL: 
				glColor3d(0, 0, 0);// set color black
				break;
			case SPACE: 
				glColor3d(1, 1, 1);// set color white
				break;
			case START:
				glColor3d(0.6, 0.8, 1);// set color blue
				break;
			case TARGET:
				glColor3d(1, 0, 0);// set color red
				break;
			//case BLACK:
			//	glColor3d(0.5, 0.7, 1);// set color light blue
			//	break;
			//case GRAY:
			//	glColor3d(1, 0.7, 0);// set color orange
			//	break;
			case PATH:
				glColor3d(0, 1, 0);// green
				break;
			case PACMAN:
				glColor3d(1, 1, 0);// Yellow
				break;
			case GHOST:
				glColor3d(0, 0, 1);// Blue
				break;
			case GHOST2:
				glColor3d(1, 0, 1);// Purple
				break;
			case GHOST3:
				glColor3d(0, 0.5, 0);// Dark Green
				break;
			case COIN:
				glColor3d(0.8,0.498039,0.196078);// Gold
				break;


			} // switch
			// now show the cell of maze
			glBegin(GL_POLYGON);
			glVertex2d(j,i); // left-bottom vertex
			glVertex2d(j,i+1); // left-top vertex
			glVertex2d(j+1,i+1); // right-top vertex
			glVertex2d(j+1,i); // right-bottom vertex
			glEnd();
		}
}
//will check the distance if A*
int ManhattanDistance(int row, int col,int rowT,int colT)
{
	return abs(row - rowT) + abs(col - colT);
}

void cleanMaze(Ghost* ghost)
{
	
	while (!ghost->getVisited().empty())
	{
		Cell* pCurrent;
		pCurrent = ghost->getVisited().back();
		ghost->getVisited().pop_back();
		if(maze[pCurrent->getRow()][pCurrent->getCol()]!=GHOST)
			maze[pCurrent->getRow()][pCurrent->getCol()] = SPACE;
	}
	if (ghost->getCurrentCoin() != NULL)
	{
		maze[ghost->getCurrentCoin()->getRow()][ghost->getCurrentCoin()->getCol()] = COIN;
	}
}

void RestorePath(Cell* pc,Ghost* ghost)
{
	while (pc->getParent()->getParent() != nullptr)
	{
		pc = pc->getParent();
	}
	Cell* pCurrent = new Cell(pc->getRow(), pc->getCol(), nullptr);
	maze[pc->getRow()][pc->getCol()] = GHOST;
	maze[pc->getParent()->getRow()][pc->getParent()->getCol()] = SPACE;
	ghost->setCell(pCurrent);
	ghost->getPq().push(pCurrent);
}




void cleanAStar(Ghost* ghost)
{		
	while (!ghost->getPq().empty()) 
		ghost->getPq().pop();
	ghost->getVisited().clear();
	ghost->setFinished(false);
	Cell* pCurrent = new Cell(ghost->getCell()->getRow(), ghost->getCell()->getCol(),nullptr);
	ghost->getPq().push(pCurrent);
	ghost->setCell(pCurrent);
	while (!ghost->getCoins().empty())
	{
		Cell* pCoin;
		pCoin = ghost->getCoins().back();
		ghost->getCoins().pop_back();
		if (maze[pCoin->getRow()][pCoin->getCol()] != GHOST)
		{
			maze[pCoin->getRow()][pCoin->getCol()] = COIN;
		}	
		else
		{
			ghost->setCurrentCoin(pCoin);
		}	
	}
}

void CheckNeighbor(Cell* pCurrent, int row, int col, Pacman* pac, Ghost* ghost)
{
	if (maze[row][col] == PACMAN) {
		RestorePath(pCurrent,ghost);
		cleanMaze(ghost);
		ghost->setFinished(true);
		startAStar = false;
		Cell* pCurrent = new Cell(ghost->getCell()->getRow(), ghost->getCell()->getCol(), nullptr);
		return;
	}
	else
	{
		
		Cell* pc = new Cell(row, col, pCurrent);
		if (maze[row][col] == COIN)
		{
			ghost->getCoins().push_back(pc);
		}
		maze[row][col] = GRAY;
		pc->setG(pCurrent->getG() + 1);
		pc->setH(ManhattanDistance(row,col,pac->getCell()->getRow(), pac->getCell()->getCol()));
		pc->setF();
		ghost->getPq().push(pc);
		ghost->getVisited().push_back(pc);
	}
}

void AStarIteration(Ghost* ghost)
{
	Cell* pCurrent;

	//no pacman found
	if (ghost->getPq().empty())
	{
		startAStar = false;
		ghost->setFinished(true);
		gameOver = true;
		return;
	}
	else
	{
		//pacman cought
		if (maze[ghost->getCell()->getRow() + 1][ghost->getCell()->getCol()] == PACMAN ||
			maze[ghost->getCell()->getRow()][ghost->getCell()->getCol() + 1] == PACMAN ||
			maze[ghost->getCell()->getRow() - 1][ghost->getCell()->getCol()] == PACMAN ||
			maze[ghost->getCell()->getRow()][ghost->getCell()->getCol() - 1] == PACMAN)
		{
			cout << "Pacman cought!\n";
			cout << pacman->getCoinNum();
			gameOver = true;
			ghost->setFinished(true);
			startAStar = false;
			return;
		}
		
		pCurrent = ghost->getPq().top();
		ghost->getPq().pop();
		ghost->getVisited().push_back(pCurrent);

		int row, col;
		row = pCurrent->getRow();
		col = pCurrent->getCol();
		maze[row][col] = BLACK;

		if (maze[row + 1][col] == SPACE || maze[row + 1][col] == PACMAN|| maze[row + 1][col] == COIN)
			CheckNeighbor(pCurrent, row + 1, col,pacman,ghost);

		if(startAStar)
			if(maze[row - 1][col] == SPACE || maze[row - 1][col] == PACMAN || maze[row + 1][col] == COIN)
				CheckNeighbor(pCurrent, row - 1, col,pacman,ghost);

		if (startAStar)
			if (maze[row][col+1] == SPACE || maze[row][col+1] == PACMAN || maze[row + 1][col] == COIN)
				CheckNeighbor(pCurrent, row, col+1,pacman,ghost);

		if (startAStar)
			if (maze[row ][col-1] == SPACE || maze[row ][col-1] == PACMAN || maze[row + 1][col] == COIN)
				CheckNeighbor(pCurrent, row , col-1,pacman,ghost);
	}
}

void cleanBFS()
{
	pacman->getBFS().clear();
	pacman->getVisited().clear();
	pacman->setFinished(false);
	Cell* pCurrent = new Cell(pacman->getCell()->getRow(), pacman->getCell()->getCol(), nullptr);
	pacman->getBFS().push_back(pCurrent);
	pacman->setCell(pCurrent);
	while (!pacman->getPq().empty())
		pacman->getPq().pop();
	pacman->getPq().push(pCurrent);
}

//clean the maze after path signs in BFS
void cleanBFSMaze()
{
	while (!pacman->getVisited().empty())
	{
		Cell* pCurrent;
		pCurrent = pacman->getVisited().back();
		pacman->getVisited().pop_back();
		if(maze[pCurrent->getRow()][pCurrent->getCol()]!=PACMAN)
		maze[pCurrent->getRow()][pCurrent->getCol()] = SPACE;
	}

}

void RestoreBFSPath(Cell* pc)
{
	while (pc->getParent()->getParent() != nullptr)
	{
		pc = pc->getParent();
	}
	maze[pc->getRow()][pc->getCol()] = PACMAN;
	maze[pc->getParent()->getRow()][pc->getParent()->getCol()] = SPACE;
	Cell* pCurrent = new Cell(pc->getRow(), pc->getCol(), nullptr);
	if (pacman->getCoinRow() != NULL && pacman->getCoinCol() != NULL && pacman->getCell()->getRow() == pacman->getCoinRow() && pacman->getCell()->getCol() == pacman->getCoinCol())
	{
		pacman->addCoin();
	}
	pacman->setCell(pCurrent);
	pacman->getBFS().push_back(pCurrent);
	pacman->setFinished(true);
	
}

//BFS neighbors check
void CheckBFSNeighbor(Cell* pCurrent, int row, int col)
{
	vector<Cell*>::iterator visitIter;
	if (maze[row][col] == COIN) // The algorithm is over
	{
		Cell* pc = new Cell(row, col, pCurrent);
		pacman->setCoinRow(row);
		pacman->setCoinCol(col);
		startBFS = false;
		RestoreBFSPath(pc);
		cleanBFSMaze();
		return;
	}
	else
	{
		Cell* pc = new Cell(row, col, pCurrent);
		for (visitIter = pacman->getBFS().begin();
			visitIter < pacman->getBFS().end(); ++visitIter)
			if (pc->compareCells(*visitIter))
				break;
		if (visitIter == pacman->getBFS().end())
			pacman->getBFS().push_back(pc);
	}
}

//BFS
void BFSIteration()
{
	Cell* pCurrent;
	// 1. check if grays [queue] is not empty
	if (pacman->getBFS().empty())
	{
		startBFS = false;
		return;
	}
	else // grays is not empty
	{
		// 2. extract the FIRST element from grays and paint it BLACK
		pCurrent = pacman->getBFS().front();
		// 2.1 paint it BLACK
		int row, col;
		// 2.2 and remove it from grays
		pacman->getBFS().erase(pacman->getBFS().begin());
		pacman->getVisited().push_back(pCurrent);
		row = pCurrent->getRow();
		col = pCurrent->getCol();
		maze[row][col] = BLACK;
		// 3 check the neighbors of pCurrent and pick the white one and add them to the end of grays
		// UP
		if (maze[row + 1][col] == SPACE || maze[row + 1][col] == COIN )
			CheckBFSNeighbor(pCurrent, row + 1, col);
		// DOWN
		if (startBFS)
			if (maze[row - 1][col] == SPACE || maze[row - 1][col] == COIN)
				CheckBFSNeighbor(pCurrent, row - 1, col);
		// right		
		if (startBFS)
			if (maze[row][col + 1] == SPACE || maze[row][col + 1] == COIN )
				CheckBFSNeighbor(pCurrent, row, col + 1);
		// left		
		if (startBFS)
			if (maze[row][col - 1] == SPACE || maze[row][col - 1] == COIN )
				CheckBFSNeighbor(pCurrent, row, col - 1);
	}
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT); // clean frame buffer
	ShowMaze();
	glutSwapBuffers(); // show all
}

void initGhost(Ghost* ghost)
{
	while (!ghost->getFinished())
		AStarIteration(ghost);
	if (ghost->getFinished())
	{
		cleanAStar(ghost);
		startAStar = true;
	}
}

int distanceFromGhost(int row,int col, Ghost* ghost)
{
	return sqrt(pow(row-ghost->getCell()->getRow(),2)+pow(col - ghost->getCell()->getCol(),2));
}

Ghost* checkDistanceFromGhosts()
{
	Ghost* ghost = ghost1;
	double minDistance = 0, tempDistance = 0;
	minDistance = distanceFromGhost(pacman->getCell()->getRow(), pacman->getCell()->getCol(), ghost1);
	tempDistance = distanceFromGhost(pacman->getCell()->getRow(), pacman->getCell()->getCol(), ghost2);
	if (tempDistance < minDistance)
	{
		minDistance = tempDistance;
		ghost = ghost2;
	}
	tempDistance = distanceFromGhost(pacman->getCell()->getRow(), pacman->getCell()->getCol(), ghost3);
	if (tempDistance < minDistance)
	{
		minDistance = tempDistance;
		ghost = ghost3;
	}
	tempDistance = distanceFromGhost(pacman->getCell()->getRow(), pacman->getCell()->getCol(), ghost4);
	if (tempDistance < minDistance)
	{
		minDistance = tempDistance;
		ghost = ghost4;
	}


	if (minDistance < pacman->getMinGhostDistance())
		return ghost;
	return nullptr;
}


void cleanPacmanAStar()
{
	while (!pacman->getPq().empty())
		pacman->getPq().pop();
	pacman->getVisited().clear();
	pacman->setFinished(false);
	Cell* pCurrent = new Cell(pacman->getCell()->getRow(), pacman->getCell()->getCol(), nullptr);
	pacman->getPq().push(pCurrent);
	pacman->setCell(pCurrent);
}

void cleanPacmanMaze()
{

	while (!pacman->getVisited().empty())
	{
		Cell* pCurrent;
		pCurrent = pacman->getVisited().back();
		pacman->getVisited().pop_back();
		if (maze[pCurrent->getRow()][pCurrent->getCol()] != PACMAN)
			maze[pCurrent->getRow()][pCurrent->getCol()] = SPACE;
	}
}

void RestorePacmanPath(Cell* pc)
{
	while (pc->getParent()->getParent() != nullptr)
	{
		pc = pc->getParent();
	}
	Cell* pCurrent = new Cell(pc->getRow(), pc->getCol(), nullptr);
	maze[pc->getRow()][pc->getCol()] = PACMAN;
	maze[pc->getParent()->getRow()][pc->getParent()->getCol()] = SPACE;
	pacman->setCell(pCurrent);
	pacman->getPq().push(pCurrent);
}

void CheckPacmanNeighbor(Cell* pCurrent, int row, int col, Pacman* pac, Ghost* ghost)
{

	if (maze[row][col]==GHOST) {
		RestorePacmanPath(pCurrent);
		cleanPacmanMaze();
		pacman->setFinished(true);
		startAStar = false;
		return;
	}
	else if(maze[row][col] == SPACE || maze[row][col] == COIN)
	{
		
		Cell* pc = new Cell(row, col, pCurrent);
		pc->setG(pCurrent->getG() + 1);
		pc->setH(ManhattanDistance(row, col, ghost->getCell()->getRow(), ghost->getCell()->getCol()));
		pc->setF();

		maze[row][col] = GRAY;
		pacman->getPq().push(pc);
		pacman->getVisited().push_back(pc);
	}

}

void pacmanAStarIteration(Ghost* ghost)
{
	Cell* pCurrent;

	if (pacman->getPq().empty())
	{
		startAStar = false;
		pacman->setFinished(true);
		gameOver = true;
		cout << "There is no solution" << endl;
		return;
	}
	else
	{
		//pacman cought
		if (maze[pacman->getCell()->getRow() + 1][pacman->getCell()->getCol()] == GHOST ||
			maze[pacman->getCell()->getRow()][pacman->getCell()->getCol() + 1] == GHOST ||
			maze[pacman->getCell()->getRow() - 1][pacman->getCell()->getCol()] == GHOST ||
			maze[pacman->getCell()->getRow()][pacman->getCell()->getCol() - 1] == GHOST)
		{
			cout << "Pacman cought!\n";
			gameOver = true;
			pacman->setFinished(true);
			startAStar = false;
			return;
		}

		pCurrent = pacman->getPq().top();
		pacman->getPq().pop();
		pacman->getVisited().push_back(pCurrent);

		int row, col;
		row = pCurrent->getRow();
		col = pCurrent->getCol();

		maze[row][col] = BLACK;
		if (maze[row + 1][col] == SPACE|| maze[row + 1][col] == COIN || maze[row + 1][col] == GHOST)
			CheckPacmanNeighbor(pCurrent, row + 1, col, pacman, ghost);

		if (startAStar)
			if (maze[row - 1][col] == SPACE  || maze[row + 1][col] == COIN || maze[row + 1][col] == GHOST)
				CheckPacmanNeighbor(pCurrent, row - 1, col, pacman, ghost);

		if (startAStar)
			if (maze[row][col + 1] == SPACE  || maze[row + 1][col] == COIN || maze[row + 1][col] == GHOST)
				CheckPacmanNeighbor(pCurrent, row, col + 1, pacman, ghost);

		if (startAStar)
			if (maze[row][col - 1] == SPACE || maze[row + 1][col] == COIN || maze[row + 1][col] == GHOST)
				CheckPacmanNeighbor(pCurrent, row, col - 1, pacman, ghost);
	}
}

void initPacman()
{
	Ghost* ghost = checkDistanceFromGhosts();
	if (ghost==nullptr)
	{
		while (!pacman->getFinished())
			BFSIteration();
		if (pacman->getFinished())
		{
			cleanBFS();
			startBFS = true;
		}
	}
	else
	{
		while (!pacman->getFinished())
			{
				pacmanAStarIteration(ghost);
			}
		if (pacman->getFinished())
		{		
			cleanPacmanAStar();
			startAStar = true;
		}
	}
}


// runs all the time in the background
void idle()
{
	if (!gameOver && startAStar)
	{
		initPacman();
		initGhost(ghost1);
		initGhost(ghost2);
		initGhost(ghost3);
		initGhost(ghost4);
		glutPostRedisplay(); // indirect call to display
	}
}

void menu(int choice)
{
	switch (choice) 
	{
	case 1: 
		startAStar = true;
		gameOver = false;
		break;
	}
}

void main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(800, 800);
	glutInitWindowPosition(200, 100);
	glutCreateWindow("PacMan");

	glutDisplayFunc(display);
	glutIdleFunc(idle);
	// add menu
	glutCreateMenu(menu); // defines function menu as "menu" function
	glutAddMenuEntry("Start", 1);
	glutAttachMenu(GLUT_RIGHT_BUTTON); // attach to right mouse button

	init();

	glutMainLoop();
}