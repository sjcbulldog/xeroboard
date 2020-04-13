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
	editor_ = nullptr;
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

	dirty_ = false;
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
	XeroBoardWidget* cnt = new XeroBoardWidget(this);
	cnt->setTitle(title);
	int index = board_tab_->addTab(cnt, title);
	QWidget* widget = board_tab_->widget(index);
	widget->setProperty(ContainerPropName, QVariant(count_));
	containers_[count_++] = cnt;

	dirty_ = true;

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

QTreeWidgetItem *XeroBoardMainWindow::getTopLevelItem(QTreeWidget* tree, const QString& label)
{
	for (int i = 0; i < tree->topLevelItemCount(); i++)
	{
		QTreeWidgetItem* item = tree->topLevelItem(i);
		if (item->text(0) == label)
			return item;
	}

	return nullptr;
}

QTreeWidgetItem* XeroBoardMainWindow::getItem(QTreeWidgetItem* item, const QString& label)
{
	for (int i = 0; i < item->childCount(); i++) {
		QTreeWidgetItem* child = item->child(i);
		if (child->text(0) == label)
			return child;
	}

	return nullptr;
}

void XeroBoardMainWindow::syncPlots(QTreeWidget* tree, std::shared_ptr<NTEntryTracker> data)
{
	auto plotkey = data->getChild("XeroPlot");
	if (plotkey != nullptr && plotkey->isSubTable())
	{
		for (const auto& entry : plotkey->getChildMap())
		{
			QTreeWidgetItem* item = getTopLevelItem(tree, entry.first.c_str());
			if (item == nullptr)
			{
				item = new QTreeWidgetItem();
				item->setText(0, entry.first.c_str());
				tree->addTopLevelItem(item);
			}

			auto key = entry.second;
			auto cols = key->getChild("columns");
			if (cols->getValue()->IsStringArray()) {
				auto colarr = cols->getValue()->GetStringArray();
				for (auto col : colarr) {
					if (getItem(item, col.c_str()) == nullptr) {
						QTreeWidgetItem* colitem = new QTreeWidgetItem();
						colitem->setText(0, col.c_str());
						item->addChild(colitem);
					}
				}
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

	XeroBoardWidget* widget = dynamic_cast<XeroBoardWidget*>(board_tab_->currentWidget());
	assert(widget != nullptr);

	widget->setTitle(txt);
	dirty_ = true;
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
	QJsonArray tablist;
	createJSONForBoards(all);
	obj["version"] = "1";
	obj["boards"] = all;
	obj["current_tab"] = board_tab_->currentIndex();

	for (int i = 0; i < board_tab_->count(); i++) {
		QString txt = board_tab_->tabText(i);
		tablist.push_back(txt);
	}
	obj["tabs"] = tablist;

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

void XeroBoardMainWindow::load(QString& filename)
{
	filename_ = filename;

	QFile file(filename);
	load(file);
}

void XeroBoardMainWindow::load(QFile& file)
{
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		std::string msg = "cannot open file '";
		msg += file.fileName().toStdString();
		msg += "' for reading";
		std::runtime_error err(msg.c_str());
		throw err;
	}

	QString txt = file.readAll();
	QJsonParseError err;
	QJsonDocument doc = QJsonDocument::fromJson(txt.toUtf8(), &err);

	if (doc.isNull()) {
		std::string msg = "cannot parse file '";
		msg += file.fileName().toStdString();
		msg += "' as JSON";
		std::runtime_error rerr(msg.c_str());
		throw rerr;
	}

	if (!doc.isObject()) {
		std::string msg = "file '";
		msg += file.fileName().toStdString();
		msg += "' is not a JSON object";
		std::runtime_error rerr(msg.c_str());
		throw rerr;
	}

	load(doc.object());
}

void XeroBoardMainWindow::clear()
{
	for (auto pair : containers_) {
		pair.second->close();
		delete pair.second;
	}
	containers_.clear();
}

void XeroBoardMainWindow::load(const QJsonObject &obj)
{
	if (dirty_) {
		std::runtime_error err("cannot load layout file, existing layout has been modified");
		throw err;
	}

	clear();

	if (!obj.contains("tabs") || !obj["tabs"].isArray())
	{
		std::runtime_error err("invalid layout file, missing top level 'tabs' array");
		throw err;
	}

	if (!obj.contains("boards") || !obj["boards"].isArray())
	{
		std::runtime_error err("invalid layout file, missing top level 'boards' array");
		throw err;
	}

	QJsonArray boards = obj["boards"].toArray();

	QJsonArray tabs = obj["tabs"].toArray();
	for (int i = 0; i < tabs.size(); i++) {
		if (tabs[i].isString()) {
			QString tabname = tabs[i].toString();
			QJsonObject obj = findBoard(boards, tabname);
			if (!obj.isEmpty()) {
				XeroBoardWidget* widget = newTabWithName(tabname);
				widget->init(obj);
			}
		}
	}

	if (obj.contains("current_tab") && obj["current_tab"].isDouble())
	{
		int tab = static_cast<int>(obj["current_tab"].toDouble() + 0.5);
		board_tab_->setCurrentIndex(tab);
	}
}

QJsonObject XeroBoardMainWindow::findBoard(QJsonArray boards, const QString& name)
{
	QJsonObject obj;

	for (int i = 0; i < boards.size(); i++) {
		QJsonValue v = boards[i];
		if (v.isObject()) {
			QJsonObject bobj = v.toObject();
			if (bobj.contains("title") && bobj["title"].isString() && bobj["title"].toString() == name) {
				obj = bobj;
				break;
			}
		}
	}

	return obj;
}


void XeroBoardMainWindow::loadLayout()
{
	if (dirty_)
	{
		QMessageBox question;
		question.setText("The existing layout has been modified.");
		question.setInformativeText("Do you want to save your changes?");
		question.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
		question.setDefaultButton(QMessageBox::Save);
		int ret = question.exec();
		if (ret == QMessageBox::Cancel)
			return;

		if (ret == QMessageBox::Save)
		{
			saveLayout();
		}
	}

	QString filename = QFileDialog::getOpenFileName(this, tr("Load Layout File"), "", tr("Layout File (*.lay);;All Files (*)"));
	if (filename.length() == 0)
		return;

	load(filename);
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