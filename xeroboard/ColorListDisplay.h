#pragma once

#include "IDataDisplay.h"
#include <QWidget>

class ColorListDisplay : public QWidget, public IDataDisplay
{
public:
	ColorListDisplay(QWidget* parent);
	virtual ~ColorListDisplay();

	void setValue(std::shared_ptr<nt::Value> value) override;

protected:
	virtual void paintEvent(QPaintEvent* ev);

private:
	std::vector<bool> values_;
	int height_;
};
