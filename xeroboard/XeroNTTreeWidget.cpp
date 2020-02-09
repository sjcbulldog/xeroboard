#include "XeroNTTreeWidget.h"
#include <QMimeData>
#include <QDebug>

XeroNTTreeWidget::XeroNTTreeWidget(QWidget* parent) : QTreeWidget(parent)
{
	(void)connect(this, &QTreeWidget::itemExpanded, this, &XeroNTTreeWidget::expanded);
}

XeroNTTreeWidget::~XeroNTTreeWidget()
{
}

QMimeData* XeroNTTreeWidget::mimeData(const QList<QTreeWidgetItem*> items) const
{
	if (items.size() != 1)
		return nullptr;

	QMimeData* data = QTreeWidget::mimeData(items);

	QTreeWidgetItem* item = items[0];
	QString text = item->text(0);
	item = item->parent();

	while (item != nullptr)
	{
		QString ptext = item->text(0);
		if (ptext == "/")
			text = "/" + text;
		else
			text = ptext + "/" + text;
		item = item->parent();
	}

	QByteArray dbytes = text.toUtf8();
	data->setData("application/x-text", dbytes);
	return data;
}

void XeroNTTreeWidget::expanded(QTreeWidgetItem* item)
{
	resizeColumnToContents(0);
	resizeColumnToContents(1);
}