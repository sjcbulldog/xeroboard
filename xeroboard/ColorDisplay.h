#pragma once
#include "IDataDisplay.h"
#include <QWidget>
#include <QColor>
#include <vector>

class ColorDisplay : public QWidget, public IDataDisplay
{
public:
	enum class Shape
	{
		Ellipse,
		Rectangle,
		Invalid,
	};

public:
	ColorDisplay(QWidget* parent);
	ColorDisplay(QWidget* parent, QColor, const std::vector<QColor>& colors);
	virtual ~ColorDisplay();

	void setValue(std::shared_ptr<nt::Value> value) override;

protected:
	virtual void paintEvent(QPaintEvent* ev);
	virtual void mousePressEvent(QMouseEvent* ev);

private:
	int value_;
	QColor default_;
	std::vector<QColor> colors_;
	Shape shape_;
};

