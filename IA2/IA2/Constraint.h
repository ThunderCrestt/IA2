#pragma once
#include <map>
class Variable;

class Constraint
{
private:
	Variable* firstVariable = nullptr;
	Variable* secondVariable = nullptr;

public:

	bool IsAssignmentValid(int valueToAssign, Variable* targetVariable, const std::map<Variable*, int>& assignment);
	Variable* getOtherVariable(Variable* currentVariable);

	bool operator==(const Constraint& other)
	{
		return this->firstVariable == other.firstVariable && this->secondVariable && other.secondVariable;
	}
};

