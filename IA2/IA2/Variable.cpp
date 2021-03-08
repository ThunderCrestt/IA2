#include "Variable.h"
#include "Constraint.h"

Variable::Variable(int index, const std::map<Variable*, int>& assignment)
{
	this->index = index;
	SetupDomain(assignment);
}

void Variable::SetupNeighbours(std::vector<Variable*> variables)
{
	int rowNum = index / 9;
	int colNum = index % 9;
	for (int i = 0; i < 9; i++)
	{
		if (rowNum * 9 + i != index)
		{
			neighbours.push_back(variables[rowNum * 9 + i]);
		}
		if (i * 9 + colNum != index)
		{
			neighbours.push_back(variables[i * 9 + colNum]);
		}
	}
	int startRow = rowNum / 3 * 3;
	int endRow = startRow + 3;
	int startCol = colNum / 3 * 3;
	int endCol = startCol + 3;

	for (int i = startRow; i < endRow; i++)
	{
		for (int j = startCol; j < endCol; j++)
		{
			if (i * 9 + j != index && std::find(neighbours.begin(), neighbours.end(), variables[i * 9 + j]) == neighbours.end())
			{
				neighbours.push_back(variables[i * 9 + j]);
			}
		}
	}
}

std::vector<Variable*>& Variable::GetNeighbours()
{
	return neighbours;
}

void Variable::SetupDomain(const std::map<Variable*, int>& assignment)
{
	for (int i = 1; i < 10; i++)
	{
		domain.push_back(i);
	}

	for (Constraint* constraint : constraints)
	{
		Variable* otherVariable = constraint->getOtherVariable(this);
		if (otherVariable && assignment.find(otherVariable) != assignment.end())
		{
			RemoveLegalValue(assignment.at(otherVariable));
		}
	}
}

void Variable::ResetDomain(const std::map<Variable*, int>& assignment)
{
	domain.clear();
	SetupDomain(assignment);
}

int Variable::GetAmountActiveConstraints(const std::map<Variable*, int>& assignment)
{
	int amount = 0;

	for(Constraint* constraint : constraints)
	{
		Variable* currentVariable = constraint->getOtherVariable(this);
		if(currentVariable)
		{
			if(assignment.find(currentVariable) == assignment.end())
			{
				amount++;
			}
		}
	}

	return amount;
}

bool Variable::HasLegalValue(int value)
{
	for(int legalValue : domain)
	{
		if (value == legalValue) return true;
	}
	return false;
}

void Variable::RemoveLegalValue(int value)
{
	for(auto it = domain.begin(); it != domain.end();)
	{
		if(*it == value)
		{
			domain.erase(it);
			break;
		}
		it++;
	}
}

void Variable::RemoveAssignedValueFromNeighbours(int assignedValue)
{
	for (Constraint* constraint : constraints)
	{
		Variable* otherVariable = constraint->getOtherVariable(this);
		if (otherVariable != nullptr)
		{
			otherVariable->RemoveLegalValue(assignedValue);
		}
	}
}
