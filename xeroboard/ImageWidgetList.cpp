#include "ImageWidgetList.h"
#include "CustomImageMgr.h"
#include "CustomImage.h"
#include <QMimeData>

ImageWidgetList::ImageWidgetList(CustomImageMgr& mgr, QWidget* parent) : QListWidget(parent), mgr_(mgr)
{
	QStringList names = mgr_.imageNames();
	for (const QString& name : names) {
		auto image = mgr_.get(name);
		QListWidgetItem* item = new QListWidgetItem(name);
		QPixmap pmap = QPixmap::fromImage(image->image());
		QIcon icon(pmap);
		item->setIcon(icon);

		addItem(item);
	}
}

ImageWidgetList::~ImageWidgetList()
{
}

QMimeData* ImageWidgetList::mimeData(const QList<QListWidgetItem *> items) const
{
	if (items.size() != 1)
		return nullptr;

	QMimeData* data = QListWidget::mimeData(items);

	QListWidgetItem* item = items[0];
	QString text = item->text();
	text = "image:" + text;
	QByteArray dbytes = text.toUtf8();
	data->setData("application/x-text", dbytes);
	return data;
}