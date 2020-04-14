#include "XeroPlotTreeWidget.h"
#include <QMimeData>
#include <QDebug>

XeroPlotTreeWidget::XeroPlotTreeWidget(nt::NetworkTableInstance &inst, QWidget* parent) : XeroTreeWidget(parent), inst_(inst)
{
	unsigned int flags = NT_NOTIFY_IMMEDIATE | NT_NOTIFY_NEW | NT_NOTIFY_DELETE | NT_NOTIFY_UPDATE | NT_NOTIFY_FLAGS;
	std::function<void(const nt::EntryNotification& ev)> cb = std::bind(&XeroPlotTreeWidget::tableChangedEvent, this, std::placeholders::_1);
	inst_.AddEntryListener("/XeroPlot", cb, flags);
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

void XeroPlotTreeWidget::tableChangedEvent(const nt::EntryNotification& ev)
{
	QRegExp exp("\\/XeroPlot/(.*)/columns");
	if (exp.exactMatch(ev.name.c_str()) && ev.value->IsStringArray()) {
		QString plotname = exp.cap(1);
		QTreeWidgetItem* item = findItem(ev.name.c_str());
		if (item == nullptr) {
			QTreeWidgetItem* item = new QTreeWidgetItem(this);
			item->setText(0, plotname);
			addTopLevelItem(item);

			auto cols = ev.value->GetStringArray();
			for (auto col : cols) {
				QTreeWidgetItem* colitem = new QTreeWidgetItem(item);
				colitem->setText(0, col.c_str());
				item->addChild(colitem);
			}
		}
	}
}