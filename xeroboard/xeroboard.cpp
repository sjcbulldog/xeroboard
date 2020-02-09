#include "xeroboard.h"
#include "NetworkTableMonitor.h"
#include <QDebug>

xeroboard::xeroboard(QWidget *parent) : QMainWindow(parent)
{
	monitor_ = new NetworkTableMonitor();
	createWindows();
	createMenus();
	createStatusBar();

	if (settings_.contains(GeometrySettings))
		restoreGeometry(settings_.value(GeometrySettings).toByteArray());

	if (settings_.contains(WindowStateSettings))
		restoreState(settings_.value(WindowStateSettings).toByteArray());

	if (settings_.contains(TopSplitterSettings))
	{
		QList<QVariant> stored = settings_.value(TopSplitterSettings).toList();
		QList<int> sizes;
		for (const QVariant& v : stored)
			sizes.push_back(v.toInt());
		left_right_splitter_->setSizes(sizes);
	}

	timer_ = new QTimer(this);
	(void)connect(timer_, &QTimer::timeout, this, &xeroboard::timerProc);
	timer_->start(100);
}

void xeroboard::closeEvent(QCloseEvent* event)
{
	settings_.setValue(GeometrySettings, saveGeometry());
	settings_.setValue(WindowStateSettings, saveState());

	QList<QVariant> stored;
	for (int size : left_right_splitter_->sizes())
		stored.push_back(QVariant(size));
	settings_.setValue(TopSplitterSettings, stored);
}

void xeroboard::createWindows()
{
	left_right_splitter_ = new QSplitter();
	left_right_splitter_->setOrientation(Qt::Orientation::Horizontal);
	setCentralWidget(left_right_splitter_);

	nt_tree_ = new XeroNTTreeWidget(left_right_splitter_);
	nt_tree_->setColumnCount(1);
	nt_tree_->setDragEnabled(true);
	nt_tree_->setDragDropMode(QAbstractItemView::DragOnly);
	nt_tree_->setSelectionMode(QAbstractItemView::SingleSelection);
	nt_tree_->setHeaderLabels({ "Name", "Value" });

	board_ = new XeroBoardWidget(left_right_splitter_);
}

void xeroboard::createMenus()
{
}

void xeroboard::createStatusBar()
{
}

void xeroboard::timerProc()
{
	if (monitor_->isDirty())
	{
		top_ = monitor_->getCopy();
		syncDisplay(nt_tree_, top_);
	}
}

void xeroboard::syncDisplay(QTreeWidget* tree, std::shared_ptr<NTEntryTracker> data)
{
	QTreeWidgetItem* topitem = nullptr;

	if (tree->topLevelItemCount() == 0)
	{
		topitem = new QTreeWidgetItem(tree);
		topitem->setText(0, "/");
		tree->insertTopLevelItem(0, topitem);
	}
	else
	{
		topitem = tree->topLevelItem(0);
	}

	syncDisplay(topitem, data);

}

void xeroboard::syncDisplay(QTreeWidgetItem* item, std::shared_ptr<NTEntryTracker> data)
{
	std::list<QTreeWidgetItem*> todel;

	if (data->isSubTable())
	{
		for (int i = 0; i < item->childCount(); i++)
		{
			QTreeWidgetItem* child = item->child(i);
			if (data->getChild(child->text(0).toStdString()) == nullptr)
			{
				todel.push_back(child);
			}
		}

		for (QTreeWidgetItem* child : todel)
			item->removeChild(child);

		for (auto pair : data->getChildMap())
		{
			QTreeWidgetItem* child = nullptr;
			for (int i = 0; i < item->childCount(); i++)
			{
				if (item->child(i)->text(0).toStdString() == pair.first)
				{
					child = item->child(i);
					break;
				}
			}

			if (child == nullptr)
			{
				child = new QTreeWidgetItem(item);
				child->setText(0, pair.first.c_str());
			}

			syncDisplay(child, pair.second);
		}
	}
	else
	{
		if (item->text(0) == "dbleft")
			qDebug() << "Found it";
		item->setText(1, NetworkTableMonitor::toString(*data->getValue()).c_str());
	}
}
