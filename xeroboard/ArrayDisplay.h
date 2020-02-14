#pragma once

#include "IDataDisplay.h"
#include <QTreeWidget>

class ArrayDisplay : public QTreeWidget, public IDataDisplay
{
public:
	ArrayDisplay(QWidget* parent);
	virtual ~ArrayDisplay();

	void setValue(std::shared_ptr<nt::Value> value) override;

private:
};

