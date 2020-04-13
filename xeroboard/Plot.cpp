#include "Plot.h"
#include "SingleDataSource.h"

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
	emit nodeCountChanged;
}

void Plot::emitCompleteStateChanged()
{
	emit completeStateChanged;
}
