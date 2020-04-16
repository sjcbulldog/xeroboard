#include "SingleValueDisplay.h"
#include <QPainter>

SingleValueDisplay::SingleValueDisplay(std::shared_ptr<DataSource> src, QWidget* parent) : ValueDisplay(src, parent)
{
	display_ = DisplayType::Default;
	font_set_ = false;
	colors_.push_back(QColor(255, 0, 0, 255));
	colors_.push_back(QColor(0, 255, 0, 255));
}

SingleValueDisplay::~SingleValueDisplay()
{
}

void SingleValueDisplay::paintEvent(QPaintEvent* ev)
{
	assert(source()->count() == 1);

	if (!font_set_) {
		font_ = font();
		font_set_ = true;
	}

	QPainter p(this);

	switch (source()->getType(0)) {
	case DataSource::DataType::Boolean:
		if (display_ == DisplayType::Color || display_ == DisplayType::Default) {
			int v = static_cast<int>(source()->getBoolean(0));
			paintColor(p, v);
		}
		else {
			QString txt = source()->getBoolean(0) ? "TRUE" : "FALSE";
			paintText(p, txt);
		}
		break;
	case DataSource::DataType::String:
		paintText(p, source()->getString(0));
		break;
	case DataSource::DataType::Double:
		paintText(p, QString::number(source()->getDouble(0)));
		break;
	}
}

void SingleValueDisplay::paintColor(QPainter& p, int v)
{
	QColor c;

	if (v >= colors_.size())
		c = QColor(128, 128, 128, 255);
	else
		c = colors_[v];

	p.save();
	p.setBrush(QBrush(c));
	p.drawRect(0, 0, width(), height());
	p.restore();
}

void SingleValueDisplay::paintText(QPainter& p, const QString& txt)
{
	QRect r(0, 0, width(), height());
	p.save();
	p.setFont(font_);
	p.drawText(r, Qt::AlignHCenter | Qt::AlignVCenter, txt);
	p.restore();
}

void SingleValueDisplay::resizeEvent(QResizeEvent* ev)
{
}
