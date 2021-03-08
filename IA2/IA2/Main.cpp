#include <iostream>
#include <map>
#include <vector>
#include "Constraint.h"
#include "Variable.h"

void AddConstraint(Constraint* newConstraint);
void AssignValue(Variable* variable, int value);
int LeastRestrainingValue(Variable* targetVariable);
Variable* SelectUnassignedVariable();

std::map<Variable*, int> assignment;

std::vector<Constraint*> constraints;
std::vector<Variable*> variables;



int main()
{
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

	AssignValue(variables[0], 1);
	AssignValue(variables[1], 2);
	AssignValue(variables[9], 4);
	AssignValue(variables[10], 5);
	AssignValue(variables[11], 6);
	AssignValue(variables[19], 8);
	AssignValue(variables[20], 9);

	AssignValue(variables[36], 2);
	AssignValue(variables[37], 1);
	AssignValue(variables[39], 4);
	AssignValue(variables[40], 5);
	AssignValue(variables[41], 6);
	AssignValue(variables[42], 3);
	AssignValue(variables[43], 8);
	AssignValue(variables[44], 9);

	//SelectUnassignedVariable();
	std::cout << LeastRestrainingValue(variables[2]) << std::endl;

	return EXIT_SUCCESS;
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