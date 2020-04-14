#pragma once
#include <XeroTreeWidget.h>
#include <networktables/NetworkTableInstance.h>
#include <networktables/NetworkTable.h>

class XeroNTTreeWidget : public XeroTreeWidget
{
public:
	XeroNTTreeWidget(nt::NetworkTableInstance &inst, QWidget* parent = nullptr);

	virtual ~XeroNTTreeWidget();

	virtual QMimeData *mimeData(const QList<QTreeWidgetItem*> items) const;

private:
	void expanded(QTreeWidgetItem* item);
	void tableChangedEvent(const nt::EntryNotification& ev);

private:
	nt::NetworkTableInstance& inst_;
	NT_EntryListener listener_;
};

