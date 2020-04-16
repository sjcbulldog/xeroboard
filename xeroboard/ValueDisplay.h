#pragma once
#include "DataSource.h"
#include <QWidget>

class ValueDisplay : public QWidget
{
public:
	ValueDisplay(std::shared_ptr<DataSource> src, QWidget* parent) : QWidget(parent)
	{
		src_ = src;
	}

	std::shared_ptr<DataSource> source() {
		return src_;
	}

private:
	std::shared_ptr<DataSource> src_;
};
