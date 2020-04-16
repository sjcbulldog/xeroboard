#include "XeroSingleItemWidget.h"
#include <QLabel>
#include <QJsonObject>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QMimeData>
#include <memory>

XeroSingleItemWidget::XeroSingleItemWidget(std::shared_ptr<DataSource> src, QWidget* parent) : XeroDisplayWidget(parent)
{
	size_t index = src->getName(0).lastIndexOf('/');
	if (index == -1)
		setTitle(src->getName(0));
	else
		setTitle(src->getName(0).mid(index + 1));

	source_ = src;
	valueChanged();

	conn_ = connect(source_.get(), &DataSource::valueChanged, this, &XeroSingleItemWidget::valueChanged);

	if (src->count() == 1)
		display_ = new SingleValueDisplay(src, this);

	setAcceptDrops(true);
}

XeroSingleItemWidget::~XeroSingleItemWidget()
{
	disconnect(conn_);
}

void XeroSingleItemWidget::resizeEvent(QResizeEvent* ev)
{
	QRect r = getClientArea();
	r.adjust(4, 4, -4, -4);
	display_->setGeometry(r);
}

void XeroSingleItemWidget::valueChanged()
{
}

void XeroSingleItemWidget::createJSON(QJsonObject& obj)
{
	obj["title"] = title();
	// obj["source"] = source_->name();
}

void XeroSingleItemWidget::dragEnterEvent(QDragEnterEvent* ev)
{
	setBackgroundRole(QPalette::Highlight);
	ev->setDropAction(Qt::DropAction::CopyAction);
	ev->acceptProposedAction();
	repaint();
}

void XeroSingleItemWidget::dragMoveEvent(QDragMoveEvent* ev)
{
	ev->acceptProposedAction();
}

void XeroSingleItemWidget::dragLeaveEvent(QDragLeaveEvent* ev)
{
	setBackgroundRole(QPalette::Background);
	repaint();
}

void XeroSingleItemWidget::dropEvent(QDropEvent* ev)
{
	QString text;
	static const char* textMimeType = "application/x-text";

	const QMimeData* data = ev->mimeData();
	if (data->hasFormat(textMimeType))
	{
	}
}