#pragma once

#include "XeroBoardWidget.h"
#include "NetworkTableMonitor.h"
#include "NTEntryTracker.h"
#include "XeroNTTreeWidget.h"
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



private:
	static constexpr const char* GeometrySettings = "geometry";
	static constexpr const char* WindowStateSettings = "windowstate";
	static constexpr const char* TopSplitterSettings = "splitter";

private:
	QSplitter* left_right_splitter_;
	XeroNTTreeWidget* nt_tree_;
	XeroBoardWidget* board_;
	NetworkTableMonitor* monitor_;
	std::shared_ptr<NTEntryTracker> top_;
	QTimer* timer_;
	QSettings settings_;

	QMenu* file_;
	QMenu* view_;
	QMenu* help_;
};
