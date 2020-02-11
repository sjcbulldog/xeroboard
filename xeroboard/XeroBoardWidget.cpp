#include "XeroBoardWidget.h"
#include "XeroSingleItemWidget.h"
#include "XeroMultiItemWidget.h"
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QPainter>
#include <algorithm>
#include <cassert>

XeroBoardWidget::XeroBoardWidget(QWidget *parent) : QWidget(parent)
{
	setAcceptDrops(true);
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

void XeroBoardWidget::dropEvent(QDropEvent* ev)
{
	QString text;
	static const char* textMimeType = "application/x-text";

	const QMimeData* data = ev->mimeData();
	if (data->hasFormat(textMimeType))
	{
		QString node = QString::fromUtf8(data->data(textMimeType));

		for (auto w : display_widgets_)
		{
			QRect r = w->geometry();
			if (r.contains(ev->pos()))
			{
				//
				// Replace a single widget with a multi widget
				//
				replaceSingleWithMulti(w, node.toStdString());
				return;
			}
		}

		XeroSingleItemWidget* item = new XeroSingleItemWidget(node.toStdString(), ev->pos(), this);
		display_widgets_.push_back(item);
		item->setVisible(true);
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