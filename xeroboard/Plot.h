#pragma once

#include "SingleDataSource.h"
#include "PlotNode.h"
#include <memory>
#include <string>

class Plot : public QObject
{
	Q_OBJECT

public:
	Plot(const std::string& key);
	virtual ~Plot();

	const std::string &key() {
		return key_;
	}

signals:
	void nodeCountChanged();
	void completeStateChanged();

private:
	void populate();
	void countChanged();
	void completeChanged();

	void emitCountChanged();
	void emitCompleteStateChanged();

	void getAllData();
	void createNodes(const std::vector<std::string>& columns);

private:
	std::string key_;
	std::vector<std::shared_ptr<PlotNode>> nodes_;
	std::shared_ptr<SingleDataSource> node_count_;
	std::shared_ptr<SingleDataSource> complete_;

	int node_count_value_;
	bool complete_value_;

	QMetaObject::Connection node_count_conn_;
	QMetaObject::Connection complete_conn_;
};
