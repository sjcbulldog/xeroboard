#include "ColorListDisplay.h"
#include <QPainter>

ColorListDisplay::ColorListDisplay(QWidget* parent) : QWidget(parent)
{
	setMinimumHeight(16);
	height_ = 16;
}

ColorListDisplay::~ColorListDisplay()
{
}

void ColorListDisplay::setValue(std::shared_ptr<nt::Value> value)
{
	if (value->type() == NT_BOOLEAN_ARRAY)
	{
		auto a = value->GetBooleanArray();
		values_.clear();
		for (int i = 0; i < a.size(); i++) 
		{
			if (a[i] == 0)
				values_.push_back(false);
			else
				values_.push_back(true) ;
		}
	}
	repaint();
}

void ColorListDisplay::paintEvent(QPaintEvent* ev)
{
	QPainter painter(this);
	QColor c, ctrue, cfalse;

	ctrue = QColor(0, 255, 0, 255);
	cfalse = QColor(255, 0, 0, 255);

	QFontMetrics fm(painter.font());

	for (int i = 0; i < values_.size(); i++)
	{
		QRect r(0, i * height_, width(), height_);
		if (values_[i])
			c = ctrue;
		else
			c = cfalse;

		QBrush brush(c);
		QPen pen(QColor(0, 0, 0, 255));
		painter.setBrush(brush);
		painter.setPen(pen);
		painter.drawRect(r);

		QBrush tbrush(QColor(255, 255, 255, 255));
		QString str = QString::number(i);
		QPoint p(r.center().x() - fm.horizontalAdvance(str) / 2, r.center().y() + fm.height() / 2);
		painter.drawText(p, str);
	}
}
