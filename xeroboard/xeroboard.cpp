#include "xeroboard.h"
#include "NetworkTableMonitor.h"
#include <QDebug>
#include <QMenu>
#include <QMenuBar>
#include <QTabWidget>

xeroboard::xeroboard(QWidget *parent) : QMainWindow(parent)
{
	image_mgr_.init();

	monitor_ = new NetworkTableMonitor();
	createWindows();
	createMenus();
	createStatusBar();

	if (settings_.contains(GeometrySettings))
		restoreGeometry(settings_.value(GeometrySettings).toByteArray());

	if (settings_.contains(WindowStateSettings))
		restoreState(settings_.value(WindowStateSettings).toByteArray());

	if (settings_.contains(LeftSplitterSettings))
	{
		QList<QVariant> stored = settings_.value(LeftSplitterSettings).toList();
		QList<int> sizes;
		for (const QVariant& v : stored)
			sizes.push_back(v.toInt());
		left_right_splitter_->setSizes(sizes);
	}

	if (settings_.contains(TopSplitterSettings))
	{
		QList<QVariant> stored = settings_.value(TopSplitterSettings).toList();
		QList<int> sizes;
		for (const QVariant& v : stored)
			sizes.push_back(v.toInt());
		top_bottom_splitter_->setSizes(sizes);
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
	settings_.setValue(LeftSplitterSettings, stored);

	stored.clear();
	for (int size : top_bottom_splitter_->sizes())
		stored.push_back(QVariant(size));
	settings_.setValue(TopSplitterSettings, stored);
}

void xeroboard::createWindows()
{
	left_right_splitter_ = new QSplitter();
	left_right_splitter_->setOrientation(Qt::Orientation::Horizontal);

	top_bottom_splitter_ = new QSplitter(left_right_splitter_);
	top_bottom_splitter_->setOrientation(Qt::Orientation::Vertical);

	nt_tree_ = new XeroNTTreeWidget(top_bottom_splitter_);
	nt_tree_->setColumnCount(2);
	nt_tree_->setDragEnabled(true);
	nt_tree_->setDragDropMode(QAbstractItemView::DragOnly);
	nt_tree_->setSelectionMode(QAbstractItemView::SingleSelection);
	nt_tree_->setHeaderLabels({ "Name", "Value" });

	plot_tree_ = new XeroPlotTreeWidget(top_bottom_splitter_);
	plot_tree_->setColumnCount(1);
	plot_tree_->setDragEnabled(true);
	plot_tree_->setDragDropMode(QAbstractItemView::DragOnly);
	plot_tree_->setSelectionMode(QAbstractItemView::SingleSelection);
	plot_tree_->setHeaderLabels({ "Plot" });

	tab_ = new QTabWidget(left_right_splitter_);
	tab_->setTabsClosable(true);

	(void)connect(tab_, &QTabWidget::tabCloseRequested, this, &xeroboard::closeTab);
	(void)connect(tab_->tabBar(), &QTabBar::tabBarDoubleClicked, this, &xeroboard::editTab);

	newTab();

	setCentralWidget(left_right_splitter_);
}

void xeroboard::createMenus()
{
	QAction *act;

	file_ = new QMenu(tr("&File"));
	menuBar()->addMenu(file_);
	act = file_->addAction(tr("New Layout"));
	(void)connect(act, &QAction::triggered, this, &xeroboard::newLayout);

	file_->addAction(tr("Load Layout ..."));
	file_->addAction(tr("Save Layout"));
	file_->addAction(tr("Save Layout As .."));
	file_->addSeparator();
	file_->addAction(tr("Exit"));
	(void)connect(act, &QAction::triggered, this, &xeroboard::exit);

	edit_ = new QMenu(tr("&Edit"));
	menuBar()->addMenu(edit_);
	act = edit_->addAction(tr("Delete"));
	(void)connect(act, &QAction::triggered, this, &xeroboard::editDelete);
	
	if (image_mgr_.count() > 0)
	{
		QMenu *menu = edit_->addMenu("&Create");
		for (const QString& im : image_mgr_.imageNames())
		{
			menu->addAction(im);
		}
	}

	view_ = new QMenu(tr("&View"));
	menuBar()->addMenu(view_);
	act = view_->addAction(tr("New Tab"));
	(void)connect(act, &QAction::triggered, this, &xeroboard::newTab);

	align_ = new QMenu(tr("&Align"));
	menuBar()->addMenu(align_);
	act = align_->addAction(tr("Left Edge"));
	(void)connect(act, &QAction::triggered, this, &xeroboard::alignLeftEdge);
	act = align_->addAction(tr("Right Edge"));
	(void)connect(act, &QAction::triggered, this, &xeroboard::alignRightEdge);
	act = align_->addAction(tr("Top Edge"));
	(void)connect(act, &QAction::triggered, this, &xeroboard::alignTopEdge);
	act = align_->addAction(tr("Bottom Edge"));
	(void)connect(act, &QAction::triggered, this, &xeroboard::alignBottomEdge);
	act = align_->addAction(tr("Horizontal Center"));
	(void)connect(act, &QAction::triggered, this, &xeroboard::alignHorizontalCenter);
	act = align_->addAction(tr("Vertical Center"));
	(void)connect(act, &QAction::triggered, this, &xeroboard::alignVerticalCenter);
	act = align_->addAction(tr("Make Same Size"));
	(void)connect(act, &QAction::triggered, this, &xeroboard::makeSameSize);
}

void xeroboard::createStatusBar()
{
}

void xeroboard::editDelete()
{

}

void xeroboard::newTab()
{
	QString title = "Plots (" + QString::number(count_) + ")";
	newTabWithName(title);
}

XeroBoardWidget* xeroboard::newTabWithName(const QString &title)
{
	XeroBoardWidget* cnt = new XeroBoardWidget();
	int index = tab_->addTab(cnt, title);
	QWidget* widget = tab_->widget(index);
	widget->setProperty(ContainerPropName, QVariant(count_));
	containers_[count_++] = cnt;

	return cnt;
}

void xeroboard::timerProc()
{
	if (monitor_->isDirty())
	{
		top_ = monitor_->getCopy();
		syncDisplay(nt_tree_, top_);
		syncPlots(plot_tree_, top_);
	}
}

bool xeroboard::treeHasTopLevelItem(QTreeWidget* tree, const QString& label)
{
	for (int i = 0; i < tree->topLevelItemCount(); i++)
	{
		if (tree->topLevelItem(i)->text(0) == label)
			return true;
	}

	return false;
}

void xeroboard::syncPlots(QTreeWidget* tree, std::shared_ptr<NTEntryTracker> data)
{
	auto plotkey = data->getChild("XeroPlot");
	if (plotkey != nullptr && plotkey->isSubTable())
	{
		for (const auto& entry : plotkey->getChildMap())
		{
			if (!treeHasTopLevelItem(tree, entry.first.c_str()))
			{
				QTreeWidgetItem* item = new QTreeWidgetItem();
				item->setText(0, entry.first.c_str());
				tree->addTopLevelItem(item);
			}
		}
	}
}

void xeroboard::syncDisplay(QTreeWidget* tree, std::shared_ptr<NTEntryTracker> data)
{
	QTreeWidgetItem* topitem = nullptr;

	if (tree == nullptr)
		return;

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
		item->setText(1, NetworkTableMonitor::toString(*data->getValue()).c_str());
	}
}

void xeroboard::closeTab(int which)
{
	QWidget* widget = tab_->widget(which);
	QVariant v = widget->property(ContainerPropName);
	int tag = v.toInt();
	tab_->removeTab(which);

	auto it = containers_.find(tag);
	if (it != containers_.end())
	{
		XeroBoardWidget* cnt = containers_[tag];
		delete cnt;
		containers_.erase(it);
	}
}

void xeroboard::editTab(int which)
{
	which_tab_ = which;

	QTabBar* bar = tab_->tabBar();

	QRect r = bar->tabRect(which);

	if (editor_ == nullptr)
	{
		editor_ = new TabEditName(bar);
		(void)connect(editor_, &TabEditName::returnPressed, this, &xeroboard::editTabDone);
		(void)connect(editor_, &TabEditName::escapePressed, this, &xeroboard::editTabAborted);
	}

	editor_->setGeometry(r);
	editor_->setFocus(Qt::FocusReason::OtherFocusReason);
	editor_->setVisible(true);
	editor_->setText(bar->tabText(which));
	editor_->selectAll();
}

void xeroboard::editTabDone()
{
	QTabBar* bar = tab_->tabBar();
	QString txt = editor_->text();

	editor_->setVisible(false);
	bar->setTabText(which_tab_, txt);
}

void xeroboard::editTabAborted()
{
	editor_->setVisible(false);
}

void xeroboard::exit()
{
	close();
}

void xeroboard::newLayout()
{

}

void xeroboard::saveLayout()
{
}

void xeroboard::saveAsLayout()
{

}

void xeroboard::loadLayout()
{

}

void xeroboard::alignLeftEdge()
{
	XeroBoardWidget* board = dynamic_cast<XeroBoardWidget*>(tab_->currentWidget());
	assert(board != nullptr);
	board->alignLeftEdge();
}

void xeroboard::alignRightEdge()
{
	XeroBoardWidget* board = dynamic_cast<XeroBoardWidget*>(tab_->currentWidget());
	assert(board != nullptr);
	board->alignRightEdge();
}

void xeroboard::alignTopEdge()
{
	XeroBoardWidget* board = dynamic_cast<XeroBoardWidget*>(tab_->currentWidget());
	assert(board != nullptr);
	board->alignTopEdge();
}

void xeroboard::alignBottomEdge()
{
	XeroBoardWidget* board = dynamic_cast<XeroBoardWidget*>(tab_->currentWidget());
	assert(board != nullptr);
	board->alignBottomEdge();
}

void xeroboard::alignHorizontalCenter()
{
	XeroBoardWidget* board = dynamic_cast<XeroBoardWidget*>(tab_->currentWidget());
	assert(board != nullptr);
	board->alignHorizontalCenter();
}

void xeroboard::alignVerticalCenter()
{
	XeroBoardWidget* board = dynamic_cast<XeroBoardWidget*>(tab_->currentWidget());
	assert(board != nullptr);
	board->alignVerticalCenter();
}

void xeroboard::makeSameSize()
{
	XeroBoardWidget* board = dynamic_cast<XeroBoardWidget*>(tab_->currentWidget());
	assert(board != nullptr);
	board->makeSameSize();
}