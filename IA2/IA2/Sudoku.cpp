#include "Sudoku.h"
#include <iostream>
#include <fstream>
#include <string>

Sudoku::Sudoku(std::string const sudoku)
{
	int i = 0;
	int j = 0;
	int k = 0;
	int max = sudoku.max_size();
	while (j < 9)
	{
		i = 0;
	    while (i < 9)
		{
			char test = sudoku[k];

			if (test == '.')
			{
				m_sudoku[j][i] = 0;
				i++;
				k++;
			}
			else if (test == '1' || test == '2' || test == '3' || test == '4' || test == '5' || test == '6' || test == '7' || test == '8' || test == '9')
			{
				m_sudoku[j][i] = test - '0';
				i++;
				k++;

			}
			else
			{
				k++;
			}
		}
		j++;
	}
}

Sudoku::~Sudoku()
{

}

void Sudoku::printSudoku()
{
	std::cout << "\n";
	for (int j = 0; j < 9; j++)
	{
		std::cout << "  ";
		for (int i = 0; i < 9; i++)
		{
			if (m_sudoku[j][i] == 0)
			{
				std::cout << "  ";

			}
			else
			{
				std::cout << m_sudoku[j][i] << " ";
			}
			if (i == 2 || i == 5)
			{
				std::cout << " | ";
			}
		}
		std::cout << "\n";
		if (j == 2 || j == 5)
		{
			std::cout << "  ------   ------   ------ \n";
		}

	}
	std::cout << "\n";
}