#include <iostream>
#include <algorithm>
#include <map>
#include <vector>
#include <deque>
#include "Constraint.h"
#include "Variable.h"
#include "Sudoku.h"
#include <time.h>
#include <cstdlib>
#include <locale>

bool AC3();
void AddConstraint(Constraint* newConstraint);
void AssignValue(Variable* variable, int value, bool withPrint = true);
void UnassignValue(Variable* variable);
std::vector<int> LeastConstrainingValue(Variable* targetVariable);
std::string recoverSudoku();
std::map<Variable*, int> backTrackingSearch();
std::map<Variable*, int> recursiveBackTrackingSearch(std::map<Variable*, int>& assignment);
bool assignmentIsComplete(const std::map<Variable*, int>& assignment);
Variable* SelectUnassignedVariable(std::map<Variable*, int>& assignment);
void setCaseFromIndex(int index, int val);
void setupAssignement(const Sudoku mySudoku);
std::map<Variable*, int> assignment;

std::vector<Constraint*> constraints;
std::vector<Variable*> variables;
std::map<Variable*, int> failure;
Sudoku* mySudoku;

int main()
{
	//recuperation du sudoku
	std::string sudoku = recoverSudoku();
	mySudoku = new Sudoku(sudoku);
	mySudoku->printSudoku();

	failure.emplace(nullptr, -1);
	for(int i = 0; i < 81; i++)
	{
		variables.push_back(new Variable(i, assignment));
	}

	for(Variable* variable : variables)
	{
		variable->SetupNeighbours(variables);
		std::vector<Variable*> neighbours = variable->GetNeighbours();
		for(Variable* neighbour : neighbours)
		{
			AddConstraint(new Constraint(variable, neighbour));
		}
	}
	setupAssignement(*mySudoku);
	clock_t tStart = clock();
	backTrackingSearch();
	std::cout << (double)(clock() - tStart) / CLOCKS_PER_SEC << std::endl;
	return EXIT_SUCCESS;
}

std::map<Variable*, int> backTrackingSearch()
{
	//std::cout << assignment[0];
	return recursiveBackTrackingSearch(assignment);
}

std::map<Variable*, int> recursiveBackTrackingSearch(std::map<Variable*, int>& assignment)
{
	if (assignmentIsComplete(assignment)) { return assignment; }
	Variable* var = SelectUnassignedVariable(assignment);
	if(var)
	{
		std::vector<int> orderedValues = LeastConstrainingValue(var);
		for (int val : orderedValues)
		{
			bool consistantValue = true;
			for (Constraint* constraint : var->getConstraints())
			{
				if (!constraint->IsAssignmentValid(val, var, assignment))
				{
					consistantValue = false;
				}
			}
			if (consistantValue)
			{
				AssignValue(var, val);
				if (AC3())
				{
					std::map<Variable*, int> result = recursiveBackTrackingSearch(assignment);
					if (result != failure)
					{
						return result;
					}
				}
				UnassignValue(var);
			}
		}
	}
	return failure;
}

bool assignmentIsComplete(const std::map<Variable*, int> &assignment)
{
	//std::cout << assignment.size();
	return assignment.size()==81; //toutes les cases ont un chiffres
}

void AssignValue(Variable* variable, int value,bool withPrint)
{
	assignment.emplace(variable, value);
	variable->RemoveAssignedValueFromNeighbours(value);
	variable->SetAssigned(true);
	if (withPrint)
	{
		setCaseFromIndex(variable->GetIndex(), value);
		mySudoku->printSudoku();
	}

}

void UnassignValue(Variable* variable)
{
	assignment.erase(variable);
	variable->SetAssigned(false);
	for(auto variable : variables)
	{
		variable->ResetDomain(assignment);
	}
	setCaseFromIndex(variable->GetIndex(), 0);
	mySudoku->printSudoku();
}

void AddConstraint(Constraint* newConstraint)
{
	for(Constraint* currentConstraint : constraints)
	{
		if(*currentConstraint == *newConstraint)
		{
			delete newConstraint;
			return;
		}
	}

	constraints.push_back(newConstraint);
	newConstraint->First()->AddConstraint(newConstraint);
	newConstraint->Second()->AddConstraint(newConstraint);
}

std::vector<Variable*> MinimumRemainingValues()
{
	int currentMinimum = INT_MAX, duplicateCount = 0;
	std::vector<Variable*> minimumVariables;

	for(Variable* currentVariable : variables)
	{
		if(!currentVariable->GetAssigned())
		{
			int variableValue = currentVariable->GetAmountOfLegalValues();
			if (variableValue < currentMinimum)
			{
				minimumVariables.clear();
				minimumVariables.push_back(currentVariable);
				currentMinimum = variableValue;
			}
			else if (variableValue == currentMinimum)
			{
				minimumVariables.push_back(currentVariable);
			}
		}
	}

	return minimumVariables;
}

Variable* DegreeHeuristic(std::vector<Variable*> variables, std::map<Variable*, int>& assignment)
{
	int currentMaximum = 0;
	Variable* chosenVariable = nullptr;
	for (Variable* currentVariable : variables)
	{
		if (!currentVariable->GetAssigned())
		{
			int variableValue = currentVariable->GetAmountActiveConstraints(assignment);
			if (variableValue > currentMaximum)
			{
				chosenVariable = currentVariable;
				currentMaximum = variableValue;
			}
		}
	}

	return chosenVariable;
}

Variable* SelectUnassignedVariable(std::map<Variable*, int>& assignment)
{
	Variable* chosenVariable = nullptr;
	std::vector<Variable*> minimumVariables = MinimumRemainingValues();
	if (minimumVariables.size() == 1)
	{
		chosenVariable = minimumVariables[0];
	}
	else
	{
		chosenVariable = DegreeHeuristic(minimumVariables, assignment);
	}
	if (chosenVariable == nullptr) chosenVariable = minimumVariables[0];

	return chosenVariable;
}

std::vector<int> LeastConstrainingValue(Variable* targetVariable)
{
	int currentMax = -1;
	int chosenValue = -1;
	std::vector<std::pair<int, int>> countVector;
	std::vector<Variable*> neighbours = targetVariable->GetNeighbours();
	for(int value : targetVariable->GetDomain())
	{
		int removedValues = 0;
		for(Variable* neighbour : neighbours)
		{
			if (neighbour->HasLegalValue(value)) removedValues++;
		}
		countVector.push_back(std::pair<int, int>(value, removedValues));
	}
	std::sort(countVector.begin(), countVector.end(), [](std::pair<int, int> left, std::pair<int, int> right)
		{
			return left.second < right.second;
		});

	std::vector<int> orderedValues;
	for(auto currentValue : countVector)
	{
		orderedValues.push_back(currentValue.first);
	}

	return orderedValues;
}

int RemoveInconsistentValues(Constraint& constraint)
{
	int removed = 0;

	std::vector<int> firstDomain = constraint.First()->GetDomain(), secondDomain = constraint.Second()->GetDomain();

	for (auto currentFirstIt = firstDomain.begin(); currentFirstIt != firstDomain.end();)
	{
		bool noneSatisfies = true;
		for (auto currentSecondValue : secondDomain)
		{
			if (*currentFirstIt != currentSecondValue)
			{
				noneSatisfies = false;
				break;
			}
		}
		if (noneSatisfies)
		{
			constraint.First()->RemoveLegalValue(*currentFirstIt);
			removed = 1;
		}
		currentFirstIt++;
	}
	if(constraint.First()->GetAmountOfLegalValues() == 0)
	{
		removed = -1;
	}

	return removed;
}

bool AC3()
{
	std::deque<Constraint> constraintsQueue;
	for(auto constraint : constraints)
	{
		constraintsQueue.push_back(*constraint);
		constraintsQueue.push_back(Constraint(constraint->Second(), constraint->First()));
	}

	while(constraintsQueue.size() != 0)
	{
		Constraint currentConstraint = constraintsQueue.front();
		constraintsQueue.pop_front();
		
		int result = RemoveInconsistentValues(currentConstraint);
		if(result == 1)
		{
			std::vector<Variable*> neighbours = currentConstraint.First()->GetNeighbours();
			for(auto neighbour : neighbours)
			{
				constraintsQueue.push_back(Constraint(neighbour, currentConstraint.First()));
			}
		}
		else if(result == -1)
		{
			return false;
		}
	}
	return true;
}

std::string recoverSudoku() 
{
	std::locale::global(std::locale{ "" });
	std::cout << "Entrez un chiffre entre 1 et 6 ou le nom du nouveau sudoku importé :\n";
	std::string i;
	std::cin >> i;
	std::ifstream importSudoku{ "Sudoku/" + i + ".ss" };
	while (!importSudoku.good())
	{
		importSudoku.close();
		std::cout << "Aucun fichier correspondant, veuillez reessayer : \n";
		std::cin >> i;
		importSudoku.open("Sudoku/" + i + ".ss");
		if (importSudoku.good())
		{
			break;
		}
	}
	std::string sudoku;
	while (importSudoku.good()) {
		sudoku = sudoku + (char)importSudoku.get();
	}
	return sudoku;
}

void setCaseFromIndex(int index,int val)
{
	int j= index%9;
	int i = index/9;
	mySudoku->setCase(i, j,val);
}

void setupAssignement(const Sudoku mySudoku)
{
	for (int i = 0; i < 9; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			if (mySudoku.m_sudoku[i][j] != 0)
			{
				AssignValue(variables[i * 9 + j], mySudoku.m_sudoku[i][j],false);
			}
		}
	}
}