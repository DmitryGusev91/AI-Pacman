#pragma once

using namespace std;



class Cell
{
private:
	int row, col;
	Cell* parent;
	int g, f, h;
public:
	Cell();
	Cell(int r, int c, Cell* p);
	~Cell();
	int getRow() { return row; }
	int getCol() { return col; }
	int getG() { return g; }
	int getF() { return f; }
	int getH() { return h; }
	void setG(int value) { g = value; }
	void setH(int value) { h = value; }
	void setF() { f = g + h; }
	Cell* getParent() { return parent; }
	void setParent(Cell* c) { parent = c; }
	bool compareCells(Cell* other);
	void setRow(int r) { row = r; }
	void setCol(int c) { col = c; }
};

