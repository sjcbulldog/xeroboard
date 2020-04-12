#include "XeroBoardMainWindow.h"
#include "NetworkTableMonitor.h"
#include <QDebug>
#include <QMenu>
#include <QMenuBar>
#include <QTabWidget>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFileDialog>
#include <QMessageBox>

XeroBoardMainWindow::XeroBoardMainWindow(QWidget *parent) : QMainWindow(parent)
{
	image_mgr_.init();
	count_ = 0;

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

	timer_ = new QTimer(this);
	(void)connect(timer_, &QTimer::timeout, this, &XeroBoardMainWindow::timerProc);
	timer_->start(100);

	connected_ = false;
}

void XeroBoardMainWindow::closeEvent(QCloseEvent* event)
{
	settings_.setValue(GeometrySettings, saveGeometry());
	settings_.setValue(WindowStateSettings, saveState());

	QList<QVariant> stored;
	for (int size : left_right_splitter_->sizes())
		stored.push_back(QVariant(size));
	settings_.setValue(LeftSplitterSettings, stored);
}

void XeroBoardMainWindow::createWindows()
{
	left_right_splitter_ = new QSplitter();
	left_right_splitter_->setOrientation(Qt::Orientation::Horizontal);

	select_tab_ = new QTabWidget(left_right_splitter_);
	select_tab_->setTabsClosable(false);
	left_right_splitter_->addWidget(select_tab_);

	nt_tree_ = new XeroNTTreeWidget(select_tab_);
	nt_tree_->setColumnCount(2);
	nt_tree_->setDragEnabled(true);
	nt_tree_->setDragDropMode(QAbstractItemView::DragOnly);
	nt_tree_->setSelectionMode(QAbstractItemView::SingleSelection);
	nt_tree_->setHeaderLabels({ "Name", "Value" });
	select_tab_->addTab(nt_tree_, "Table");

	plot_tree_ = new XeroPlotTreeWidget(select_tab_);
	plot_tree_->setColumnCount(1);
	plot_tree_->setDragEnabled(true);
	plot_tree_->setDragDropMode(QAbstractItemView::DragOnly);
	plot_tree_->setSelectionMode(QAbstractItemView::SingleSelection);
	plot_tree_->setHeaderLabels({ "Plot" });
	select_tab_->addTab(plot_tree_, "Plots");

	board_tab_ = new QTabWidget(left_right_splitter_);
	board_tab_->setTabsClosable(true);

	(void)connect(board_tab_, &QTabWidget::tabCloseRequested, this, &XeroBoardMainWindow::closeTab);
	(void)connect(board_tab_->tabBar(), &QTabBar::tabBarDoubleClicked, this, &XeroBoardMainWindow::editTab);

	newTab();

	setCentralWidget(left_right_splitter_);
	
	status_text_ = new QLabel("Ready");
	statusBar()->insertWidget(0, status_text_);
}

void XeroBoardMainWindow::createMenus()
{
	QAction *act;

	file_ = new QMenu(tr("&File"));
	menuBar()->addMenu(file_);
	act = file_->addAction(tr("New Layout"));
	(void)connect(act, &QAction::triggered, this, &XeroBoardMainWindow::newLayout);

	act = file_->addAction(tr("Load Layout ..."));
	(void)connect(act, &QAction::triggered, this, &XeroBoardMainWindow::loadLayout);

	act = file_->addAction(tr("Save Layout"));
	(void)connect(act, &QAction::triggered, this, &XeroBoardMainWindow::saveLayout);

	act = file_->addAction(tr("Save Layout As .."));
	(void)connect(act, &QAction::triggered, this, &XeroBoardMainWindow::saveAsLayout);

	file_->addSeparator();
	act = file_->addAction(tr("Exit"));
	(void)connect(act, &QAction::triggered, this, &XeroBoardMainWindow::exit);

	edit_ = new QMenu(tr("&Edit"));
	menuBar()->addMenu(edit_);
	act = edit_->addAction(tr("Delete"));
	(void)connect(act, &QAction::triggered, this, &XeroBoardMainWindow::editDelete);
	
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
	(void)connect(act, &QAction::triggered, this, &XeroBoardMainWindow::newTab);

	align_ = new QMenu(tr("&Align"));
	menuBar()->addMenu(align_);
	act = align_->addAction(tr("Left Edge"));
	(void)connect(act, &QAction::triggered, this, &XeroBoardMainWindow::alignLeftEdge);
	act = align_->addAction(tr("Right Edge"));
	(void)connect(act, &QAction::triggered, this, &XeroBoardMainWindow::alignRightEdge);
	act = align_->addAction(tr("Top Edge"));
	(void)connect(act, &QAction::triggered, this, &XeroBoardMainWindow::alignTopEdge);
	act = align_->addAction(tr("Bottom Edge"));
	(void)connect(act, &QAction::triggered, this, &XeroBoardMainWindow::alignBottomEdge);
	act = align_->addAction(tr("Horizontal Center"));
	(void)connect(act, &QAction::triggered, this, &XeroBoardMainWindow::alignHorizontalCenter);
	act = align_->addAction(tr("Vertical Center"));
	(void)connect(act, &QAction::triggered, this, &XeroBoardMainWindow::alignVerticalCenter);
	act = align_->addAction(tr("Make Same Size"));
	(void)connect(act, &QAction::triggered, this, &XeroBoardMainWindow::makeSameSize);
}

void XeroBoardMainWindow::createStatusBar()
{
}

void XeroBoardMainWindow::editDelete()
{
}

void XeroBoardMainWindow::newTab()
{
	QString title = "Display";
	newTabWithName(title);
}

XeroBoardWidget* XeroBoardMainWindow::newTabWithName(const QString &title)
{
	XeroBoardWidget* cnt = new XeroBoardWidget();
	cnt->setTitle(title);
	int index = board_tab_->addTab(cnt, title);
	QWidget* widget = board_tab_->widget(index);
	widget->setProperty(ContainerPropName, QVariant(count_));
	containers_[count_++] = cnt;

	return cnt;
}

void XeroBoardMainWindow::timerProc()
{
	if (!monitor_->isConnected()) {
		if (connected_) {
			status_text_->setText("Not Connected");
			connected_ = false;
			for (auto& pair : containers_)
				pair.second->setEnabled(false);
		}
	}
	else 
	{
		if (!connected_) {
			status_text_->setText("Connected");
			connected_ = true;
			for (auto& pair : containers_)
				pair.second->setEnabled(true);
		}
		if (monitor_->isDirty())
		{
			top_ = monitor_->getCopy();
			syncDisplay(nt_tree_, top_);
			syncPlots(plot_tree_, top_);
		}
	}
}

bool XeroBoardMainWindow::treeHasTopLevelItem(QTreeWidget* tree, const QString& label)
{
	for (int i = 0; i < tree->topLevelItemCount(); i++)
	{
		if (tree->topLevelItem(i)->text(0) == label)
			return true;
	}

	return false;
}

void XeroBoardMainWindow::syncPlots(QTreeWidget* tree, std::shared_ptr<NTEntryTracker> data)
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

void XeroBoardMainWindow::syncDisplay(QTreeWidget* tree, std::shared_ptr<NTEntryTracker> data)
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

void XeroBoardMainWindow::syncDisplay(QTreeWidgetItem* item, std::shared_ptr<NTEntryTracker> data)
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

void XeroBoardMainWindow::closeTab(int which)
{
	QWidget* widget = board_tab_->widget(which);
	QVariant v = widget->property(ContainerPropName);
	int tag = v.toInt();
	board_tab_->removeTab(which);

	auto it = containers_.find(tag);
	if (it != containers_.end())
	{
		XeroBoardWidget* cnt = containers_[tag];
		delete cnt;
		containers_.erase(it);
	}
}

void XeroBoardMainWindow::editTab(int which)
{
	which_tab_ = which;

	QTabBar* bar = board_tab_->tabBar();

	QRect r = bar->tabRect(which);

	if (editor_ == nullptr)
	{
		editor_ = new TabEditName(bar);
		(void)connect(editor_, &TabEditName::returnPressed, this, &XeroBoardMainWindow::editTabDone);
		(void)connect(editor_, &TabEditName::escapePressed, this, &XeroBoardMainWindow::editTabAborted);
	}

	editor_->setGeometry(r);
	editor_->setFocus(Qt::FocusReason::OtherFocusReason);
	editor_->setVisible(true);
	editor_->setText(bar->tabText(which));
	editor_->selectAll();
}

void XeroBoardMainWindow::editTabDone()
{
	QTabBar* bar = board_tab_->tabBar();
	QString txt = editor_->text();

	editor_->setVisible(false);
	bar->setTabText(which_tab_, txt);
}

void XeroBoardMainWindow::editTabAborted()
{
	editor_->setVisible(false);
}

void XeroBoardMainWindow::exit()
{
	close();
}

void XeroBoardMainWindow::newLayout()
{
}

void XeroBoardMainWindow::saveLayout()
{
	if (filename_.length() == 0)
		saveAsLayout();
	else
		save();
}

void XeroBoardMainWindow::saveAsLayout()
{
	QFileDialog dialog;
	QString filename = QFileDialog::getSaveFileName(this, tr("Save Layout"), "", tr("Layout File (*.lay);;All Files (*)"));
	if (filename.length() == 0)
		return;
	
	filename_ = filename;
	save();
}

void XeroBoardMainWindow::save()
{
	QJsonArray all;
	QJsonObject obj;

	createJSONForBoards(all);
	obj["version"] = "1";
	obj["boards"] = all;
	QJsonDocument doc(obj);
	QFile file(filename_);

	if (!file.open(QIODevice::OpenModeFlag::Truncate | QIODevice::OpenModeFlag::WriteOnly)) {
		QString msg;
		msg += "could not save layout file '";
		msg += filename_;
		msg += "'";
		QMessageBox::warning(this, "Save Error", msg);
	}
	else {
		file.write(doc.toJson());
		file.close();
		dirty_ = false;
	}
}

void XeroBoardMainWindow::loadLayout()
{
}

void XeroBoardMainWindow::alignLeftEdge()
{
	XeroBoardWidget* board = dynamic_cast<XeroBoardWidget*>(board_tab_->currentWidget());
	assert(board != nullptr);
	board->alignLeftEdge();
}

void XeroBoardMainWindow::alignRightEdge()
{
	XeroBoardWidget* board = dynamic_cast<XeroBoardWidget*>(board_tab_->currentWidget());
	assert(board != nullptr);
	board->alignRightEdge();
}

void XeroBoardMainWindow::alignTopEdge()
{
	XeroBoardWidget* board = dynamic_cast<XeroBoardWidget*>(board_tab_->currentWidget());
	assert(board != nullptr);
	board->alignTopEdge();
}

void XeroBoardMainWindow::alignBottomEdge()
{
	XeroBoardWidget* board = dynamic_cast<XeroBoardWidget*>(board_tab_->currentWidget());
	assert(board != nullptr);
	board->alignBottomEdge();
}

void XeroBoardMainWindow::alignHorizontalCenter()
{
	XeroBoardWidget* board = dynamic_cast<XeroBoardWidget*>(board_tab_->currentWidget());
	assert(board != nullptr);
	board->alignHorizontalCenter();
}

void XeroBoardMainWindow::alignVerticalCenter()
{
	XeroBoardWidget* board = dynamic_cast<XeroBoardWidget*>(board_tab_->currentWidget());
	assert(board != nullptr);
	board->alignVerticalCenter();
}

void XeroBoardMainWindow::makeSameSize()
{
	XeroBoardWidget* board = dynamic_cast<XeroBoardWidget*>(board_tab_->currentWidget());
	assert(board != nullptr);
	board->makeSameSize();
}

void XeroBoardMainWindow::createJSONForBoards(QJsonArray& doc)
{
	for (auto& pair : containers_)
	{
		QJsonObject obj;
		pair.second->createJSON(obj);
		doc.push_back(obj);
	}
}