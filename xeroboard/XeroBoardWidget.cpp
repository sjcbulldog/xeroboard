#include "XeroBoardWidget.h"
#include "XeroSingleItemWidget.h"
#include "XeroMultiItemWidget.h"
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QPainter>
#include <QDebug>
#include <algorithm>
#include <cassert>

XeroBoardWidget::XeroBoardWidget(QWidget *parent) : QWidget(parent)
{
	setAcceptDrops(true);
	setFocusPolicy(Qt::ClickFocus);
}

XeroBoardWidget::~XeroBoardWidget()
{
}

void XeroBoardWidget::dragEnterEvent(QDragEnterEvent* ev)
{
	setBackgroundRole(QPalette::Highlight);
	ev->setDropAction(Qt::DropAction::CopyAction);
	ev->acceptProposedAction();
}

void XeroBoardWidget::dragMoveEvent(QDragMoveEvent* ev)
{
	ev->acceptProposedAction();
}

void XeroBoardWidget::dragLeaveEvent(QDragLeaveEvent* ev)
{
	setBackgroundRole(QPalette::Background);
}

void XeroBoardWidget::dropVariable(QString node, QPoint pt)
{
	for (auto w : display_widgets_)
	{
		QRect r = w->geometry();
		if (r.contains(pt))
		{
			//
			// Replace a single widget with a multi widget
			//
			replaceSingleWithMulti(w, node.toStdString());
			return;
		}
	}

	SingleDataSource* src = new SingleDataSource(node.toStdString());
	if (src->isSubtable())
	{
		delete src;
	}
	else
	{
		XeroSingleItemWidget* item = new XeroSingleItemWidget(node.toStdString(), pt, this);
		display_widgets_.push_back(item);
		item->setVisible(true);
	}
}

void XeroBoardWidget::dropPlot(QString plot, QPoint pt)
{
}

void XeroBoardWidget::dropEvent(QDropEvent* ev)
{
	QString text;
	static const char* textMimeType = "application/x-text";

	const QMimeData* data = ev->mimeData();
	if (data->hasFormat(textMimeType))
	{
		QString node = QString::fromUtf8(data->data(textMimeType));

		if (node.length() > 3 && node.mid(0, 4) == "var:")
			dropVariable(node.mid(4), ev->pos());
		else if (node.length() > 4 && node.mid(0, 5) == "plot:")
			dropPlot(node.mid(5), ev->pos());


	}
	setBackgroundRole(QPalette::Background);
}

void XeroBoardWidget::removeChild(XeroDisplayWidget *widget)
{
	auto it = std::find(display_widgets_.begin(), display_widgets_.end(), static_cast<QWidget*>(widget));
	assert(it != display_widgets_.end());

	display_widgets_.erase(it);
}

void XeroBoardWidget::replaceSingleWithMulti(XeroDisplayWidget* w, const std::string& newnode)
{
	auto multi = dynamic_cast<XeroMultiItemWidget *>(w);
	if (multi != nullptr)
	{
		multi->addSource(newnode);
		return;
	}

	auto single = dynamic_cast<XeroSingleItemWidget*>(w);
	if (single != nullptr)
	{
		QRect r = w->geometry();
		XeroMultiItemWidget* neww = new XeroMultiItemWidget(r, this);
		neww->addSource(single->takeSource());
		single->close();

		neww->addSource(newnode);
		display_widgets_.push_back(neww);
		neww->setVisible(true);
		return;
	}
}

void XeroBoardWidget::selectWidget(XeroDisplayWidget* w, bool replace)
{
	if (replace)
	{
		for (auto* wd : selected_widgets_)
			wd->setSelected(false);
		selected_widgets_.clear();
	}

	w->setSelected(true);

	auto it = std::find(selected_widgets_.begin(), selected_widgets_.end(), w);
	if (it == selected_widgets_.end())
		selected_widgets_.push_back(w);
}

void XeroBoardWidget::mousePressEvent(QMouseEvent* ev)
{
	for (auto* wd : selected_widgets_)
		wd->setSelected(false);
	selected_widgets_.clear();
}

void XeroBoardWidget::startDragging(const QPointF& start)
{
	drag_start_ = start;
}

void XeroBoardWidget::continueDragging(const QPointF &point)
{
	double dx = point.x() - drag_start_.x();
	double dy = point.y() - drag_start_.y();

	for (auto w : selected_widgets_)
	{
		QRect geom = w->geometry();
		QRect newgeom(geom.left() + dx, geom.top() + dy, geom.width(), geom.height());
		w->setGeometry(newgeom);
	}

	drag_start_ = point;
}

void XeroBoardWidget::endDragging()
{
}

void XeroBoardWidget::alignLeftEdge()
{
	if (selected_widgets_.size() < 2)
		return;

	int left = selected_widgets_[0]->geometry().left();
	for (size_t i = 1; i < selected_widgets_.size(); i++)
	{
		QRect geom = selected_widgets_[i]->geometry();
		QRect newgeom(left, geom.top(), geom.width(), geom.height());
		selected_widgets_[i]->setGeometry(newgeom);
	}
}

void XeroBoardWidget::alignRightEdge()
{
	if (selected_widgets_.size() < 2)
		return;

	int right = selected_widgets_[0]->geometry().right();
	for (size_t i = 1; i < selected_widgets_.size(); i++)
	{
		QRect geom = selected_widgets_[i]->geometry();
		QRect newgeom(right - geom.width(), geom.top(), geom.width(), geom.height());
		selected_widgets_[i]->setGeometry(newgeom);
	}
}

void XeroBoardWidget::alignTopEdge()
{
	if (selected_widgets_.size() < 2)
		return;

	int top = selected_widgets_[0]->geometry().top();
	for (size_t i = 1; i < selected_widgets_.size(); i++)
	{
		QRect geom = selected_widgets_[i]->geometry();
		QRect newgeom(geom.left(), top, geom.width(), geom.height());
		selected_widgets_[i]->setGeometry(newgeom);
	}
}

void XeroBoardWidget::alignBottomEdge()
{
	if (selected_widgets_.size() < 2)
		return;

	int bottom = selected_widgets_[0]->geometry().bottom();
	for (size_t i = 1; i < selected_widgets_.size(); i++)
	{
		QRect geom = selected_widgets_[i]->geometry();
		QRect newgeom(geom.left(), bottom - geom.height(), geom.width(), geom.height());
		selected_widgets_[i]->setGeometry(newgeom);
	}
}

void XeroBoardWidget::alignHorizontalCenter()
{
	if (selected_widgets_.size() < 2)
		return;

	int center = selected_widgets_[0]->geometry().center().y();
	for (size_t i = 1; i < selected_widgets_.size(); i++)
	{
		QRect geom = selected_widgets_[i]->geometry();
		QRect newgeom(geom.left(), center - geom.height() / 2, geom.width(), geom.height());
		selected_widgets_[i]->setGeometry(newgeom);
	}
}

void XeroBoardWidget::alignVerticalCenter()
{
	if (selected_widgets_.size() < 2)
		return;

	int center = selected_widgets_[0]->geometry().center().x();
	for (size_t i = 1; i < selected_widgets_.size(); i++)
	{
		QRect geom = selected_widgets_[i]->geometry();
		QRect newgeom(center - geom.width() / 2, geom.top(),  geom.width(), geom.height());
		selected_widgets_[i]->setGeometry(newgeom);
	}
}

void XeroBoardWidget::makeSameSize()
{
	if (selected_widgets_.size() < 2)
		return;

	int width = selected_widgets_[0]->geometry().width();
	int height = selected_widgets_[0]->geometry().height();
	for (size_t i = 1; i < selected_widgets_.size(); i++)
	{
		QRect geom = selected_widgets_[i]->geometry();
		QRect newgeom(geom.left(), geom.top(), width, height);
		selected_widgets_[i]->setGeometry(newgeom);
	}
}

void XeroBoardWidget::keyPressEvent(QKeyEvent* ev)
{
	bool shift = (ev->modifiers() == Qt::KeyboardModifier::ShiftModifier);
	int delta = shift ? 1 : 10;

	switch (ev->key())
	{
		case Qt::Key::Key_Delete:
			for (auto w : selected_widgets_)
				w->close();
			break;

		case Qt::Key::Key_Left:
			for (auto w : selected_widgets_)
			{
				QRect geom = w->geometry();
				QRect newgeom(geom.left() - delta, geom.top(), geom.width(), geom.height());
				w->setGeometry(newgeom);
			}
			break;
		case Qt::Key::Key_Right:
			for (auto w : selected_widgets_)
			{
				QRect geom = w->geometry();
				QRect newgeom(geom.left() + delta, geom.top(), geom.width(), geom.height());
				w->setGeometry(newgeom);
			}
			break;
		case Qt::Key::Key_Up:
			for (auto w : selected_widgets_)
			{
				QRect geom = w->geometry();
				QRect newgeom(geom.left(), geom.top() - delta, geom.width(), geom.height());
				w->setGeometry(newgeom);
			}
			break;
		case Qt::Key::Key_Down:
			for (auto w : selected_widgets_)
			{
				QRect geom = w->geometry();
				QRect newgeom(geom.left(), geom.top() + delta, geom.width(), geom.height());
				w->setGeometry(newgeom);
			}
			break;
	}
}