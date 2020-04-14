#include "XeroNTTreeWidget.h"
#include "NTValueWrapper.h"
#include <QMimeData>
#include <QDebug>

XeroNTTreeWidget::XeroNTTreeWidget(nt::NetworkTableInstance &inst, QWidget* parent) : XeroTreeWidget(parent), inst_(inst)
{
	(void)connect(this, &QTreeWidget::itemExpanded, this, &XeroNTTreeWidget::expanded);

	unsigned int flags = NT_NOTIFY_IMMEDIATE | NT_NOTIFY_NEW | NT_NOTIFY_DELETE | NT_NOTIFY_UPDATE | NT_NOTIFY_FLAGS;
	std::function<void(const nt::EntryNotification& ev)> cb = std::bind(&XeroNTTreeWidget::tableChangedEvent, this, std::placeholders::_1);
	listener_ = inst_.AddEntryListener("/", cb, flags);
}

XeroNTTreeWidget::~XeroNTTreeWidget()
{
	inst_.RemoveEntryListener(listener_);
}

void XeroNTTreeWidget::tableChangedEvent(const nt::EntryNotification& ev)
{
	if ((ev.flags & NT_NOTIFY_DELETE) != 0) {
		deleteItem(ev.name.c_str());
	}
	else {
		QString name = ev.name.c_str();
		if (!name.startsWith("/XeroPlot")) {
			QTreeWidgetItem* item = findOrCreate(ev.name.c_str());
			if (item != nullptr)
			{
				NTValueWrapper v(ev.value);
				item->setText(1, v.toString());
			}
		}
	}
}

QMimeData* XeroNTTreeWidget::mimeData(const QList<QTreeWidgetItem*> items) const
{
	if (items.size() != 1)
		return nullptr;

	QMimeData* data = QTreeWidget::mimeData(items);

	QTreeWidgetItem* item = items[0];
	QString text;
	text += item->text(0);
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

	text = "var:/" + text;
	QByteArray dbytes = text.toUtf8();
	data->setData("application/x-text", dbytes);
	return data;
}

void XeroNTTreeWidget::expanded(QTreeWidgetItem* item)
{
	resizeColumnToContents(0);
	resizeColumnToContents(1);
}