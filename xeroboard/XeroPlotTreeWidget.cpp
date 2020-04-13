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
	QString text;

	if (items.size() != 1)
		return nullptr;

	QMimeData* data = QTreeWidget::mimeData(items);

	QTreeWidgetItem* item = items[0];
	if (item->parent() == nullptr) {
		text = "plot:";
		text += item->text(0);
	}
	else {
		text = "plotvar:";
		text += "{" + item->parent()->text(0) + "}";
		text += ",";
		text += "{" + item->text(0) + "}";
	}

	if (text.length() > 0) {
		QByteArray dbytes = text.toUtf8();
		data->setData("application/x-text", dbytes);
	}

	return data;
}
