#include "ColorDisplay.h"
#include <QPainter>

ColorDisplay::ColorDisplay(QWidget* parent) : QWidget(parent)
{
	default_ = QColor(0, 0, 0);
	colors_.push_back(QColor(255, 0, 0));
	colors_.push_back(QColor(0, 255, 0));
	value_ = -1;
	shape_ = Shape::Ellipse;

	setMinimumHeight(16);
}

ColorDisplay::ColorDisplay(QWidget* parent, QColor def, const std::vector<QColor>& colors)
{
	default_ = def;
	colors_ = colors;
	value_ = -1;
	shape_ = Shape::Ellipse;

	setMinimumHeight(16);
}

ColorDisplay::~ColorDisplay()
{
}

void ColorDisplay::mousePressEvent(QMouseEvent* ev)
{
	int shapeno = static_cast<int>(shape_);
	shapeno++;
	Shape sh = static_cast<Shape>(shapeno);
	if (sh == Shape::Invalid)
		sh = Shape::Ellipse;

	shape_ = sh;
	repaint();
}

void ColorDisplay::setValue(std::shared_ptr<nt::Value> value)
{
	if (value->type() == NT_BOOLEAN)
	{
		value_ = value->GetBoolean();
	}
	else if (value->type() == NT_DOUBLE)
	{
		value_ = static_cast<int>(value->GetDouble());
	}
	else
	{
		assert(0 == "only boolean or double type is allowed");
	}
	repaint();
}

void ColorDisplay::paintEvent(QPaintEvent *ev)
{
	QPainter painter(this);
	QColor c;

	if (value_ < 0 || value_ >= colors_.size())
	{
		c = default_;
	}
	else
	{
		c = colors_[value_];
	}
	QBrush brush(c);
	QPen pen(c);
	painter.setBrush(brush);
	painter.setPen(pen);

	if (shape_ == Shape::Ellipse)
	{
		painter.drawEllipse(0, 0, width(), height());
	}
	else
	{
		painter.drawRect(0, 0, width(), height());
	}
}