#include "XeroTreeWidget.h"
#include <QDebug>
#include <algorithm>
#include <cassert>

void XeroTreeWidget::deleteItem(const QString& text)
{
	int cnt = std::count(text.begin(), text.end(), '/');

	if (cnt == 0)
	{
		//
		// Malformed, expected at least one slash
		//
		qDebug() << "Malformed node " << text << "'passed to XeroTreeWidget::deleteItem";
	}
	else if (cnt == 1) {
		//
		// This is a top level item
		//
		int index = -1;

		for (int i = 0; i < topLevelItemCount(); i++) {
			QTreeWidgetItem* item = topLevelItem(i);
			if (item->text(0) == text) {
				index = i;
				break;
			}
		}

		if (index != -1) {
			QTreeWidgetItem* item = takeTopLevelItem(index);
			delete item;
		}
	}
	else {
		int index = text.lastIndexOf("/");
		QString parent = text.mid(0, index);
		QString itemtext = text.mid(index + 1);

		QTreeWidgetItem* pitem = findItem(parent);
		if (pitem != nullptr) {
			int index = -1;
			for (int i = 0; i < pitem->childCount(); i++) {
				if (pitem->child(i)->text(0) == itemtext) {
					index = i;
					break;
				}
			}

			if (index != -1) {
				QTreeWidgetItem* item = pitem->takeChild(index);
				delete item;
			}
		}
	}
}

QTreeWidgetItem* XeroTreeWidget::findTopLevelItem(const QString& text)
{
	for (int i = 0; i < topLevelItemCount(); i++) {
		QTreeWidgetItem* item = topLevelItem(i);
		if (item->text(0) == text)
			return item;
	}

	return nullptr;
}

QTreeWidgetItem* XeroTreeWidget::findChildItem(QTreeWidgetItem *parent, const QString& text)
{
	for (int i = 0; i < parent->childCount(); i++) {
		QTreeWidgetItem* item = parent->child(i);
		if (item->text(0) == text)
			return item;
	}

	return nullptr;
}

QTreeWidgetItem *XeroTreeWidget::findItem(const QString& text)
{
	QTreeWidgetItem* item = nullptr;
	QStringList path = text.split('/', QString::SplitBehavior::SkipEmptyParts);

	while (path.size() > 0) {
		if (item == nullptr)
		{
			item = findTopLevelItem(path.front());
		}
		else
		{
			item = findChildItem(item, path.front());
		}

		if (item == nullptr)
			break;

		path.pop_front();
	}

	return item;
}

QTreeWidgetItem* XeroTreeWidget::findOrCreate(const QString& text)
{
	QTreeWidgetItem* item = nullptr;

	QStringList path = text.split('/', QString::SplitBehavior::SkipEmptyParts);

	while (path.size() > 0) {
		QTreeWidgetItem* newitem = nullptr;

		if (item == nullptr) {
			newitem = findTopLevelItem(path.front());
			if (newitem == nullptr) {
				newitem = new QTreeWidgetItem(this);
				newitem->setText(0, path.front());
				addTopLevelItem(newitem);
			}
		}
		else {
			newitem = findChildItem(item, path.front());
			if (newitem == nullptr) {
				newitem = new QTreeWidgetItem(item);
				newitem->setText(0, path.front());
				item->addChild(newitem);
			}
		}

		item = newitem;
		path.pop_front();
	}

	return item;
}