#include <iostream>
#include <fstream>
#include <string>


class Sudoku
{
public:
	Sudoku(std::string const sudoku);
	~Sudoku();

	void printSudoku();
	


private:
	int m_sudoku[9][9] = { 0 };




};

