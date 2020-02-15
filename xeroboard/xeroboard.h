#pragma once

#include "XeroBoardWidget.h"
#include "NetworkTableMonitor.h"
#include "NTEntryTracker.h"
#include "XeroNTTreeWidget.h"
#include "XeroPlotTreeWidget.h"
#include "TabEditName.h"
#include "CustomImageMgr.h"
#include <QMainWindow>
#include <QSplitter>
#include <QTreeWidget>
#include <QTimer>
#include <QSettings>
#include <memory>

class xeroboard : public QMainWindow
{
	Q_OBJECT

public:
	xeroboard(QWidget *parent = Q_NULLPTR);

protected:
	void closeEvent(QCloseEvent* ev);

private:
	void createWindows();
	void createMenus();
	void createStatusBar();

	void timerProc();
	void syncDisplay(QTreeWidget *widget, std::shared_ptr<NTEntryTracker> data);
	void syncDisplay(QTreeWidgetItem* item, std::shared_ptr<NTEntryTracker> data);
	void syncPlots(QTreeWidget* widget, std::shared_ptr<NTEntryTracker> data);

	void closeTab(int which);
	void editTab(int which);
	void newTab();
	XeroBoardWidget *newTabWithName(const QString& name);
	void editTabDone();
	void editTabAborted();

	void newLayout();
	void saveLayout();
	void saveAsLayout();
	void loadLayout();
	void exit();

	void alignLeftEdge();
	void alignRightEdge();
	void alignTopEdge();
	void alignBottomEdge();
	void alignHorizontalCenter();
	void alignVerticalCenter();
	void makeSameSize();

	void editDelete();
	void editCreateImage();

	bool treeHasTopLevelItem(QTreeWidget *tree, const QString& str);

private:
	static constexpr const char* GeometrySettings = "geometry";
	static constexpr const char* WindowStateSettings = "windowstate";
	static constexpr const char* TopSplitterSettings = "topsplitter";
	static constexpr const char* LeftSplitterSettings = "leftsplitter";
	static constexpr const char* ContainerPropName = "which";

private:
	QSplitter* left_right_splitter_;
	QSplitter* top_bottom_splitter_;
	XeroNTTreeWidget* nt_tree_;
	XeroPlotTreeWidget *plot_tree_;
	QTabWidget* tab_;
	NetworkTableMonitor* monitor_;
	std::shared_ptr<NTEntryTracker> top_;
	QTimer* timer_;
	QSettings settings_;
	int count_;
	int which_tab_;
	std::map<int, XeroBoardWidget *> containers_;
	TabEditName* editor_;
	CustomImageMgr image_mgr_;

	QMenu* file_;
	QMenu* edit_;
	QMenu* view_;
	QMenu* align_;
	QMenu* help_;
};
