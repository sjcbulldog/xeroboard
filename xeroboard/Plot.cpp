#include "Plot.h"
#include "SingleDataSource.h"
#include <networktables/NetworkTableInstance.h>
#include <networktables/NetworkTable.h>
#include <QDebug>


Plot::Plot(const std::string& key)
{
	node_count_value_ = -1;
	complete_value_ = false;

	key_ = key;
	populate();
}

Plot::~Plot()
{
	disconnect(node_count_conn_);
	disconnect(complete_conn_);
}

void Plot::populate()
{
	node_count_ = std::make_shared<SingleDataSource>("/XeroPlot/" + key_ + "/count");
	node_count_conn_ = connect(node_count_.get(), &SingleDataSource::valueChanged, this, &Plot::countChanged);

	complete_ = std::make_shared<SingleDataSource>("/XeroPlot/" + key_ + "/complete");
	complete_conn_ = connect(complete_.get(), &SingleDataSource::valueChanged, this, &Plot::completeChanged);

	getAllData();
}

void Plot::countChanged()
{
	emitCountChanged();
}

void Plot::completeChanged()
{
	emitCompleteStateChanged();
}

void Plot::emitCountChanged()
{
	emit nodeCountChanged();
}

void Plot::emitCompleteStateChanged()
{
	emit completeStateChanged();
}

void Plot::getAllData()
{
	nt::NetworkTableInstance inst = nt::NetworkTableInstance::GetDefault();
	std::string name = "/XeroPlot/" + key_ + "/data/";
	std::string cntname = "/XeroPlot/" + key_ + "/points";
	std::string colname = "/XeroPlot/" + key_ + "/columns";

	nt::NetworkTableEntry entry = inst.GetEntry(cntname);
	if (entry.GetType() != nt::NetworkTableType::kDouble)
		return;
	int count = static_cast<int>(entry.GetDouble(0.0) + 0.5);


	entry = inst.GetEntry(colname);
	if (entry.GetType() != nt::NetworkTableType::kStringArray)
		return;
	std::vector<std::string> columns = entry.GetStringArray({});

	if (node_count_value_ != count)
		createNodes(columns);

	for (int i = 0; i < count; i++) {
		std::string dname = name + std::to_string(i);
		nt::NetworkTableEntry entry = inst.GetEntry(dname);

		if (entry.GetType() == nt::NetworkTableType::kDoubleArray) {
			std::vector<double> row = entry.GetDoubleArray({});
			for (int j = 0; j < row.size(); j++) {
				nodes_[j]->setAt(i, row[j]);
			}
		}
	}
}

void Plot::createNodes(const std::vector<std::string> &columns)
{
	nodes_.clear();

	for (const std::string& name : columns) {
		auto node = std::make_shared<PlotNode>(name);
		nodes_.push_back(node);
	}
}
