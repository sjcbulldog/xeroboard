#include "XeroDisplayWidget.h"
#include "XeroBoardWidget.h"
#include <QPainter>
#include <QFontMetricsF>
#include <QMouseEvent>
#include <QDebug>
#include <QGuiApplication>
#include <cassert>

XeroDisplayWidget::XeroDisplayWidget(QWidget* parent) : QWidget(parent, Qt::Widget)
{
	setMouseTracking(true);
	mouse_mode_ = MouseMode::None;
	mouse_cursor_ = MouseCursor::Arrow;
	selected_ = false;
	enabled_ = false;
	title_editor_ = nullptr;
}

XeroDisplayWidget::~XeroDisplayWidget()
{
}

XeroBoardWidget* XeroDisplayWidget::parentBoard()
{
	auto p = dynamic_cast<XeroBoardWidget*>(parent());
	assert(p != nullptr);
	return p;
}

void XeroDisplayWidget::resizeEvent(QResizeEvent* ev)
{
	if (child_ != nullptr)
	{
		QRect geom(LeftRightBorder, TitleHeight + TopBottomBorder, width() - 2 * LeftRightBorder, height() - TitleHeight - 2 * TopBottomBorder);
		child_->setGeometry(geom);
	}
}

void XeroDisplayWidget::setChild(std::shared_ptr<QWidget> child)
{
	child_ = child;
	resizeEvent(nullptr);
}

void XeroDisplayWidget::closeEvent(QCloseEvent* ev)
{
	XeroBoardWidget* board = dynamic_cast<XeroBoardWidget*>(parentWidget());
	assert(board != nullptr);

	board->removeChild(this);
}

void XeroDisplayWidget::mouseDoubleClickEvent(QMouseEvent* ev)
{
	if (title_editor_ == nullptr)
		title_editor_ = new TabEditName(this);

	QRect r = getTitleBarArea();
	title_editor_->setGeometry(r);
	title_editor_->setText(title_);
	title_editor_->show();
	title_editor_->setFocus();

	(void)connect(title_editor_, &TabEditName::returnPressed, this, &XeroDisplayWidget::editTabDone);
	(void)connect(title_editor_, &TabEditName::escapePressed, this, &XeroDisplayWidget::editTabAborted);
}

void XeroDisplayWidget::editTabDone()
{
	title_ = title_editor_->text();
	repaint();

	title_editor_->hide();
	title_editor_->setDisabled(true);
}

void XeroDisplayWidget::editTabAborted()
{
	delete title_editor_;
	title_editor_ = nullptr;
}

void XeroDisplayWidget::mousePressEvent(QMouseEvent* ev)
{
	if (getCloseRectArea().contains(ev->pos()))
	{
		this->close();
		return;
	}

	if (getTitleBarArea().contains(ev->pos()))
	{
		bool replace = true;

		if ((QGuiApplication::queryKeyboardModifiers()) & Qt::ShiftModifier)
			replace = false ;

		XeroBoardWidget* board = parentBoard();
		board->selectWidget(this, replace);

		grabMouse();
		board->startDragging(ev->screenPos());
		mouse_mode_ = MouseMode::Dragging;
	}
	else
	{
		switch (mouse_cursor_)
		{
		case MouseCursor::ResizeLeft:
			grabMouse();
			mouse_mode_ = MouseMode::ResizeLeft;
			start_drag_point_ = ev->screenPos();
			start_widget_geom_ = geometry();
			break;
		case MouseCursor::ResizeRight:
			grabMouse();
			mouse_mode_ = MouseMode::ResizeRight;
			start_drag_point_ = ev->screenPos();
			start_widget_geom_ = geometry();
			break;
		case MouseCursor::ResizeLLCorner:
			grabMouse();
			mouse_mode_ = MouseMode::ResizeLLCorner;
			start_drag_point_ = ev->screenPos();
			start_widget_geom_ = geometry();
			break;
		case MouseCursor::ResizeLRCorner:
			grabMouse();
			mouse_mode_ = MouseMode::ResizeLRCorner;
			start_drag_point_ = ev->screenPos();
			start_widget_geom_ = geometry();
			break;
		case MouseCursor::ResizeBottom:
			grabMouse();
			mouse_mode_ = MouseMode::ResizeBottom;
			start_drag_point_ = ev->screenPos();
			start_widget_geom_ = geometry();
			break;
		}
	}
}

void XeroDisplayWidget::mouseMoveEvent(QMouseEvent* ev)
{
	if (mouse_mode_ == MouseMode::Dragging)
	{
		auto board = parentBoard();
		board->continueDragging(ev->screenPos());
	}
	else if (mouse_mode_ == MouseMode::ResizeLeft)
	{
		double dx = ev->screenPos().x() - start_drag_point_.x();
		int x = start_widget_geom_.left() + dx;
		int width = start_widget_geom_.right() - x;

		if (x < 0)
		{
			x = 0;
			width = start_widget_geom_.left() + start_widget_geom_.width();
		}

		QRect next(x, start_widget_geom_.y(), width, start_widget_geom_.height());
		setGeometry(next);
	}
	else if (mouse_mode_ == MouseMode::ResizeRight)
	{
		double dx = ev->screenPos().x() - start_drag_point_.x();
		int width = start_widget_geom_.width() + dx;

		QRect next(start_widget_geom_.x(), start_widget_geom_.y(), width, start_widget_geom_.height());
		setGeometry(next);
	}
	else if (mouse_mode_ == MouseMode::ResizeBottom)
	{
		double dy = ev->screenPos().y() - start_drag_point_.y();
		int height = start_widget_geom_.height() + dy ;

		QRect next(start_widget_geom_.x(), start_widget_geom_.y(), start_widget_geom_.width(), height);
		setGeometry(next);
	}
	else if (mouse_mode_ == MouseMode::ResizeLLCorner)
	{
		double dx = ev->screenPos().x() - start_drag_point_.x();
		double dy = ev->screenPos().y() - start_drag_point_.y();
		int x = start_widget_geom_.left() + dx;
		int width = start_widget_geom_.right() - x;
		int height = start_widget_geom_.height() + dy;

		if (x < 0)
		{
			x = 0;
			width = start_widget_geom_.left() + start_widget_geom_.width();
		}

		QRect next(x, start_widget_geom_.y(), width, height);
		setGeometry(next);
	}
	else if (mouse_mode_ == MouseMode::ResizeLRCorner)
	{
		double dx = ev->screenPos().x() - start_drag_point_.x();
		int width = start_widget_geom_.width() + dx;
		double dy = ev->screenPos().y() - start_drag_point_.y();
		int height = start_widget_geom_.height() + dy;

		QRect next(start_widget_geom_.x(), start_widget_geom_.y(), width, height);
		setGeometry(next);
	}
	else
	{
		QRect r(0, height() - CornerCursorBoundary, CornerCursorBoundary, CornerCursorBoundary);
		if (r.contains(ev->pos()) && mouse_cursor_ != MouseCursor::ResizeLLCorner)
		{
			setCursor(Qt::SizeBDiagCursor);
			mouse_cursor_ = MouseCursor::ResizeLLCorner;
			return;
		}

		r = QRect(width() - CornerCursorBoundary, height() - CornerCursorBoundary, CornerCursorBoundary, CornerCursorBoundary);
		if (r.contains(ev->pos()) && mouse_cursor_ != MouseCursor::ResizeLRCorner)
		{
			setCursor(Qt::SizeFDiagCursor);
			mouse_cursor_ = MouseCursor::ResizeLRCorner;
			return;
		}

		r = QRect(width() - EdgeCursorBoundary, TitleHeight, EdgeCursorBoundary, height() - TitleHeight);
		if (r.contains(ev->pos()) && mouse_cursor_ != MouseCursor::ResizeRight)
		{
			setCursor(Qt::SizeHorCursor);
			mouse_cursor_ = MouseCursor::ResizeRight;
			return;
		}

		r = QRect(0, TitleHeight, EdgeCursorBoundary, height() - TitleHeight);
		if (r.contains(ev->pos()) && mouse_cursor_ != MouseCursor::ResizeLeft)
		{
			setCursor(Qt::SizeHorCursor);
			mouse_cursor_ = MouseCursor::ResizeLeft;
			return;
		}

		r = QRect(0, height() - EdgeCursorBoundary, width(), EdgeCursorBoundary);
		if (r.contains(ev->pos()) && mouse_cursor_ != MouseCursor::ResizeBottom)
		{
			setCursor(Qt::SizeVerCursor);
			mouse_cursor_ = MouseCursor::ResizeBottom;
			return;
		}

		if (mouse_cursor_ != MouseCursor::Arrow)
		{
			setCursor(Qt::ArrowCursor);
			mouse_cursor_ = MouseCursor::Arrow;
		}
	}
}

void XeroDisplayWidget::mouseReleaseEvent(QMouseEvent* ev)
{
	if (mouse_mode_ == MouseMode::Dragging)
	{
		auto board = parentBoard();
		board->endDragging();
	}
	mouse_mode_ = MouseMode::None;
	releaseMouse();
	setCursor(Qt::ArrowCursor);
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

	if (selected_)
	{
		QRectF r(0, 0, width(), height());
		p.setPen(QPen(QColor(0, 0, 255, 255), 2.0));
		p.setBrush(Qt::BrushStyle::NoBrush);
		p.drawRect(r);
	}
}
