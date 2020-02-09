#pragma once
#include <QTreeWidget>

class XeroNTTreeWidget : public QTreeWidget
{
public:
	XeroNTTreeWidget(QWidget* parent = nullptr);

	virtual ~XeroNTTreeWidget();

	virtual QMimeData *mimeData(const QList<QTreeWidgetItem*> items) const;

private:
	void expanded(QTreeWidgetItem* item);
};

