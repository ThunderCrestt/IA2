#include "Variable.h"
#include "Constraint.h"

Variable::Variable(int index)
{
	this->index = index;
	SetupDomain();
}

void Variable::SetupDomain()
{
	for (int i = 1; i < 10; i++)
	{
		domain.push_back(i);
	}
}

void Variable::ResetDomain()
{
	domain.clear();
	SetupDomain();
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
	}
}
