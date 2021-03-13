#include <iostream>
#include <map>
#include <vector>
#include <deque>
#include "Constraint.h"
#include "Variable.h"
#include "Sudoku.h"
#include <cstdlib>
#include <locale>

void AC3();
void AddConstraint(Constraint* newConstraint);
void AssignValue(Variable* variable, int value);
void UnassignValue(Variable* variable);
int LeastRestrainingValue(Variable* targetVariable);
std::string recoverSudoku();
std::map<Variable*, int> backTrackingSearch();
std::map<Variable*, int> recursiveBackTrackingSearch(std::map<Variable*, int>& assignment);
bool assignmentIsComplete(const std::map<Variable*, int>& assignment);
Variable* SelectUnassignedVariable();

std::map<Variable*, int> assignment;

std::vector<Constraint*> constraints;
std::vector<Variable*> variables;
std::map<Variable*, int> failure;


int main()
{
	//recuperation du sudoku
	std::string sudoku = recoverSudoku();
	Sudoku* mySudoku = new Sudoku(sudoku);
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
	backTrackingSearch();
	return EXIT_SUCCESS;
}

std::map<Variable*, int> backTrackingSearch()
{
	std::cout << assignment[0];
	return recursiveBackTrackingSearch(assignment);
}

std::map<Variable*, int> recursiveBackTrackingSearch(std::map<Variable*, int>& assignment)
{
	if (assignmentIsComplete(assignment)) { return assignment; }
	Variable* var = SelectUnassignedVariable();
	for (int val : var->GetDomain())
	{
		bool consistantValue = true;
		for (Constraint* constaint : var->getConstraints())
		{
			if (!constaint->IsAssignmentValid(val, var, assignment))
			{
				consistantValue = false;
			}
		}
		if (consistantValue)
		{
			AssignValue(var, val);
			std::map<Variable*, int> result = recursiveBackTrackingSearch(assignment);
			if (result != failure)
			{
				return result;
			}
			UnassignValue(var);
		}
	}
	return failure;
}

bool assignmentIsComplete(const std::map<Variable*, int> &assignment)
{
	std::cout << assignment.size();
	return assignment.size()==81; //toutes les cases ont un chiffres
}

void AssignValue(Variable* variable, int value)
{
	assignment.emplace(variable, value);
	variable->RemoveAssignedValueFromNeighbours(value);
	variable->SetAssigned(true);
}

void UnassignValue(Variable* variable)
{
	assignment.erase(variable);
	variable->SetAssigned(false);
	std::vector<Variable*> neighbours = variable->GetNeighbours();
	for(auto neighbour : neighbours)
	{
		neighbour->ResetDomain(assignment);
	}
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

Variable* MinimumRemainingValues()
{
	int currentMinimum = INT_MAX, duplicateCount = 0;
	Variable* chosenVariable = nullptr;

	for(Variable* currentVariable : variables)
	{
		if(!currentVariable->GetAssigned())
		{
			int variableValue = currentVariable->GetAmountOfLegalValues();
			if (variableValue < currentMinimum)
			{
				chosenVariable = currentVariable;
				currentMinimum = variableValue;
				duplicateCount = 0;
			}
			else if (variableValue == currentMinimum)
			{
				duplicateCount++;
			}
		}
	}

	if (duplicateCount > 0) return nullptr;
	return chosenVariable;
}

Variable* DegreeHeuristic()
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

Variable* SelectUnassignedVariable()
{
	Variable* chosenVariable = MinimumRemainingValues();
	if (chosenVariable == nullptr)
	{
		chosenVariable = DegreeHeuristic();
	}
	return chosenVariable;
}

int LeastRestrainingValue(Variable* targetVariable)
{
	int currentMax = -1;
	int chosenValue = -1;
	std::vector<Variable*> neighbours = targetVariable->GetNeighbours();
	for(int value : targetVariable->GetDomain())
	{
		int sum = 0;
		for(Variable* neighbour : neighbours)
		{
			sum += neighbour->GetAmountOfLegalValues();
			if (neighbour->HasLegalValue(value)) sum--;
		}

		if(sum > currentMax)
		{
			currentMax = sum;
			chosenValue = value;
		}
	}

	return chosenValue;
}

bool RemoveInconsistentValues(Constraint& constraint)
{
	bool removed = false;

	int secondIndex = constraint.Second()->GetIndex();
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
		if (noneSatisfies) constraint.First()->RemoveLegalValue(*currentFirstIt);
		currentFirstIt++;
	}

	return removed;
}

void AC3()
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
		if(RemoveInconsistentValues(currentConstraint))
		{
			std::vector<Variable*> neighbours = currentConstraint.First()->GetNeighbours();
			for(auto neighbour : neighbours)
			{
				constraintsQueue.push_back(Constraint(neighbour, currentConstraint.First()));
			}
		}
	}
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