#pragma once
#include <XeroTreeWidget.h>
#include <networktables/NetworkTableInstance.h>
#include <networktables/NetworkTable.h>

class XeroPlotTreeWidget : public XeroTreeWidget
{
public:
	XeroPlotTreeWidget(nt::NetworkTableInstance &inst, QWidget* parent = nullptr);
	virtual ~XeroPlotTreeWidget();
	virtual QMimeData* mimeData(const QList<QTreeWidgetItem*> items) const;

private:
	void tableChangedEvent(const nt::EntryNotification& ev);

private:
	nt::NetworkTableInstance& inst_;
};

