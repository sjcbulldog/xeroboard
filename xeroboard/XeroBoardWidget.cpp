#include "XeroBoardWidget.h"
#include "XeroSingleItemWidget.h"
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
