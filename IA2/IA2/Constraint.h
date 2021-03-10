#pragma once
#include <map>
class Variable;

class Constraint
{
private:
	Variable* firstVariable = nullptr;
	Variable* secondVariable = nullptr;

public:

	Constraint(Variable* firstVariable, Variable* secondVariable);

	Variable* First() { return firstVariable; }
	Variable* Second() { return secondVariable; }

	bool IsAssignmentValid(int valueToAssign, Variable* targetVariable, const std::map<Variable*, int>& assignment);
	Variable* getOtherVariable(Variable* currentVariable);

	bool operator==(const Constraint& other)
	{
		return this->firstVariable == other.firstVariable && this->secondVariable == other.secondVariable 
			|| this->secondVariable == other.firstVariable && this->firstVariable == other.secondVariable;
	}

};

