#pragma once

#include <QTreeWidget>

class XeroTreeWidget : public QTreeWidget
{
public:
	XeroTreeWidget(QWidget* parent = nullptr) : QTreeWidget(parent) {
	}

	virtual ~XeroTreeWidget() {
	}

protected:
	void deleteItem(const QString& item);
	QTreeWidgetItem *findItem(const QString& item);
	QTreeWidgetItem* findOrCreate(const QString& item);

	QTreeWidgetItem* findTopLevelItem(const QString& item);
	QTreeWidgetItem* findChildItem(QTreeWidgetItem *parent, const QString& item);
};
