#pragma once

#include <vector>
#include <map>

class Constraint;

class Variable
{
private:

	int index = 0;
	bool bAssigned;
	std::vector<int> domain;
	std::vector<Constraint*> constraints;
	std::vector<Variable*> neighbours;

public:

	Variable(int index, const std::map<Variable*, int>& assignment);

	int GetIndex() { return index; }

	bool GetAssigned() { return bAssigned; }
	void SetAssigned(bool value) { bAssigned = value; }

	void SetupNeighbours(std::vector<Variable*> variables);
	std::vector<Variable*>& GetNeighbours();

	void SetupDomain(const std::map<Variable*, int>& assignment);
	void ResetDomain(const std::map<Variable*, int>& assignment);

	int GetAmountActiveConstraints(const std::map<Variable*, int>& assignment);
	int GetAmountOfLegalValues() { return domain.size(); }
	bool HasLegalValue(int value);
	void RemoveLegalValue(int value);
	std::vector<int>& GetDomain() { return domain; }
	void AddConstraint(Constraint* value) { constraints.push_back(value); }

	void RemoveAssignedValueFromNeighbours(int assignedValue);
};