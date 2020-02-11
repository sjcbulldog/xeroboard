#pragma once

#include "IDataDisplay.h"
#include <qtreewidget.h>

class TextListDisplay : public QTreeWidget, public IDataDisplay
{
public:
	TextListDisplay(QWidget* parent);
	virtual ~TextListDisplay();

	void setValue(std::shared_ptr<nt::Value> value) override;

private:
};

