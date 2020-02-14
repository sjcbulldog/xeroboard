#pragma once
#include <QTreeWidget>

class XeroPlotTreeWidget : public QTreeWidget
{
public:
	XeroPlotTreeWidget(QWidget* parent = nullptr);
	virtual ~XeroPlotTreeWidget();
	virtual QMimeData* mimeData(const QList<QTreeWidgetItem*> items) const;

private:
};

