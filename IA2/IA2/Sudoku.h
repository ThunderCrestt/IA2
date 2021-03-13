#include <iostream>
#include <fstream>
#include <string>


class Sudoku
{
public:
	Sudoku(std::string const sudoku);
	~Sudoku();

	void printSudoku();
	int getCase(int i, int j);
	void setCase(int i, int j, int val);
	


private:
	int m_sudoku[9][9] = { 0 };




};

