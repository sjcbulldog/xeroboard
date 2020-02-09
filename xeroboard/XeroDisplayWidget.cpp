#include "XeroDisplayWidget.h"
#include "XeroBoardWidget.h"
#include <QPainter>
#include <QFontMetricsF>
#include <cassert>

XeroDisplayWidget::XeroDisplayWidget(QWidget* parent) : QWidget(parent, Qt::Widget)
{
}

XeroDisplayWidget::~XeroDisplayWidget()
{
}

QRect XeroDisplayWidget::getClientArea()
{
	QRect r(0, TitleHeight, width(), height() - TitleHeight);
	return r;
}

void XeroDisplayWidget::closeEvent(QCloseEvent* ev)
{
	XeroBoardWidget* board = dynamic_cast<XeroBoardWidget*>(parentWidget());
	assert(board != nullptr);

	board->removeChild(this);
}

void XeroDisplayWidget::paintEvent(QPaintEvent* ev)
{
	QPainter p(this);

	QRectF r(0, 0, width(), height());
	p.setPen(QPen(QColor(0, 0, 0, 255)));
	p.setBrush(QBrush(QColor(255, 255, 255, 255)));
	p.drawRect(r);

	r = QRectF(0, 0, width(), TitleHeight);
	p.setBrush(QBrush(QColor(192, 192, 192, 255)));
	p.drawRect(r);

	QFontMetricsF f(p.font());

	QPoint pt(0, f.lineSpacing());
	QRectF rect(LeftRightBorder, 0, width() - 2 * LeftRightBorder, TitleHeight);
	p.drawText(rect, 0, title_);
}
