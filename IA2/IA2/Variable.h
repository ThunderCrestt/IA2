#pragma once

#include <vector>
#include <map>

class Constraint;

class Variable
{
private:

	int index = 0;
	std::vector<int> domain;
	std::vector<Constraint*> constraints;


public:

	Variable(int index);

	void SetupDomain();
	void ResetDomain();

	int GetAmountActiveConstraints(const std::map<Variable*, int>& assignment);
	int GetAmountOfLegalValues() { return domain.size(); }
	bool HasLegalValue(int value);
	void RemoveLegalValue(int value);
	std::vector<int>& GetDomain() { return domain; }
	void SetContraints(std::vector<Constraint*> value) { constraints = value; }
};