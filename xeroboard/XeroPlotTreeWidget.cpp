#include "XeroPlotTreeWidget.h"
#include <QMimeData>
#include <QDebug>

XeroPlotTreeWidget::XeroPlotTreeWidget(QWidget* parent) : QTreeWidget(parent)
{
}

XeroPlotTreeWidget::~XeroPlotTreeWidget()
{
}

QMimeData* XeroPlotTreeWidget::mimeData(const QList<QTreeWidgetItem*> items) const
{
	if (items.size() != 1)
		return nullptr;

	QMimeData* data = QTreeWidget::mimeData(items);

	QTreeWidgetItem* item = items[0];
	QString text("plot:");
	text += item->text(0);
	QByteArray dbytes = text.toUtf8();
	data->setData("application/x-text", dbytes);
	return data;
}
