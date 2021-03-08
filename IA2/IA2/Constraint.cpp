#include "Constraint.h"
#include "Variable.h"

Constraint::Constraint(Variable* firstVariable, Variable* secondVariable)
{
	this->firstVariable = firstVariable;
	this->secondVariable = secondVariable;
}

bool Constraint::IsAssignmentValid(int valueToAssign, Variable* targetVariable, const std::map<Variable*, int>& assignment)
{
	Variable* otherVariable = nullptr;
	if(targetVariable == firstVariable)
	{
		otherVariable = secondVariable;
	}
	else if(targetVariable == secondVariable)
	{
		otherVariable = firstVariable;
	}
	else
	{
		return false;
	}
	auto otherVariableIt = assignment.find(otherVariable);
	if (otherVariableIt == assignment.end()) return true;
	return valueToAssign != otherVariableIt->second;
}

Variable* Constraint::getOtherVariable(Variable* currentVariable)
{
	if (currentVariable == firstVariable) return secondVariable;
	if (currentVariable == secondVariable) return firstVariable;
	return nullptr;
}
