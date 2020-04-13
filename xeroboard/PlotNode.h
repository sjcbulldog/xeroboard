#pragma once

#include <string>
#include <vector>

class PlotNode
{
public:
	PlotNode(std::string& name) {
		name_ = name;
	}

	void add(double d) {
		values_.push_back(d);
	}

	size_t size() {
		return values_.size();
	}

	double operator[](size_t i) {
		return values_[i];
	}

private:
	std::string name_;
	std::vector<double> values_;
};

