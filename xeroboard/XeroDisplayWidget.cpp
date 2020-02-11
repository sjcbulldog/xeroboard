#include "XeroDisplayWidget.h"
#include "XeroBoardWidget.h"
#include <QPainter>
#include <QFontMetricsF>
#include <QMouseEvent>
#include <QDebug>
#include <cassert>

XeroDisplayWidget::XeroDisplayWidget(QWidget* parent) : QWidget(parent, Qt::Widget)
{
	setMouseTracking(true);
	dragging_ = false;
}

XeroDisplayWidget::~XeroDisplayWidget()
{
}



void XeroDisplayWidget::closeEvent(QCloseEvent* ev)
{
	XeroBoardWidget* board = dynamic_cast<XeroBoardWidget*>(parentWidget());
	assert(board != nullptr);

	board->removeChild(this);
}

void XeroDisplayWidget::mousePressEvent(QMouseEvent* ev)
{
	if (getCloseRectArea().contains(ev->pos()))
	{
		this->close();
	}
	else if (getTitleBarArea().contains(ev->pos()))
	{
		dragging_ = true;
		start_drag_point_ = ev->screenPos();
		start_widget_point_ = geometry().topLeft();
		grabMouse();
	}
}

void XeroDisplayWidget::mouseMoveEvent(QMouseEvent* ev)
{
	if (dragging_)
	{
		double dx = ev->screenPos().x() - start_drag_point_.x();
		double dy = ev->screenPos().y() - start_drag_point_.y();
		int x = start_widget_point_.x() + dx;
		int y = start_widget_point_.y() + dy;

		QWidget* p = dynamic_cast<QWidget*>(parent());

		if (x < 0)
			x = 0;
		else if (x + width() > p->width())
			x = p->width() - width();
		if (y < 0)
			y = 0;
		else if (y + height() > p->height())
			y = p->height() - height();
		QRect next(x, y, width(), height());
		setGeometry(next);
	}
	else
	{
	}
}

void XeroDisplayWidget::mouseReleaseEvent(QMouseEvent* ev)
{
	dragging_ = false;
	releaseMouse();
}

QRect XeroDisplayWidget::getCloseRectArea()
{
	QRect r(width() - TitleHeight, CloseXBorder, TitleHeight - 2 * CloseXBorder, TitleHeight - 2 * CloseXBorder);
	return r;
}

QRect XeroDisplayWidget::getTitleBarArea()
{
	QRect r(0, 0, width(), TitleHeight);
	return r;
}

QRect XeroDisplayWidget::getClientArea()
{
	QRect r(0, TitleHeight, width(), height() - TitleHeight);
	return r;
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
	QRectF rect(LeftRightBorder, 0, width() - 2ll * LeftRightBorder - TitleHeight, TitleHeight);
	p.drawText(rect, 0, title_);

	r = getCloseRectArea();
	p.setPen(QPen(QColor(0, 0, 0, 255)));
	p.drawLine(r.topLeft(), r.bottomRight());
	p.drawLine(r.topRight(), r.bottomLeft());
}
