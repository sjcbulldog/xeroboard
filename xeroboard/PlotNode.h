#pragma once

#include <string>
#include <vector>

class PlotNode
{
public:
	PlotNode(const std::string& name) {
		name_ = name;
	}

	const std::string& name() {
		return name_;
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

	double get(size_t i) {
		return values_[i];
	}

	void setAt(size_t offset, double v) {
		if (offset == values_.size())
			values_.push_back(v);
		else if (offset < values_.size())
			values_[offset] = v;
	}

	void getMinMax(bool init, double& minv, double& maxv) {
		if (init) {
			minv = std::numeric_limits<double>::max();
			maxv = std::numeric_limits<double>::min();
		}

		for (double v : values_) {
			if (v > maxv)
				maxv = v;
			if (v < minv)
				minv = v;
		}
	}

private:
	std::string name_;
	std::vector<double> values_;
};

