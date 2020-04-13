#pragma once

#include <string>
#include <vector>

class PlotNode
{
public:
	PlotNode(const std::string& name) {
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

	void setAt(size_t offset, double v) {
		if (offset == values_.size())
			values_.push_back(v);
		else if (offset < values_.size())
			values_[offset] = v;
	}

private:
	std::string name_;
	std::vector<double> values_;
};

