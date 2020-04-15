#pragma once

#include "XeroBoardWidget.h"
#include "XeroNTTreeWidget.h"
#include "XeroPlotTreeWidget.h"
#include "ImageWidgetList.h"
#include "TabEditName.h"
#include "CustomImageMgr.h"
#include <QMainWindow>
#include <QSplitter>
#include <QTreeWidget>
#include <QTimer>
#include <QSettings>
#include <QStatusBar>
#include <QLabel>
#include <networktables/NetworkTableInstance.h>
#include <networktables/NetworkTable.h>
#include <memory>

class XeroBoardMainWindow : public QMainWindow
{
	Q_OBJECT

public:
	XeroBoardMainWindow(QWidget *parent = Q_NULLPTR);

	void setDirty(bool b) {
		dirty_ = b;
	}

	bool dirty() {
		return dirty_;
	}

	CustomImageMgr& getCustomImageManager() {
		return image_mgr_;
	}

protected:
	void closeEvent(QCloseEvent* ev);

private:
	void createWindows();
	void createMenus();
	void createStatusBar();

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

	QTreeWidgetItem *getTopLevelItem(QTreeWidget *tree, const QString& str);
	QTreeWidgetItem* getItem(QTreeWidgetItem* item, const QString& str);

	void save();
	void createJSONForBoards(QJsonArray& doc);
	void load(QString& str);
	void load(QFile& file);
	void load(const QJsonObject& obj);
	QJsonObject findBoard(QJsonArray boards, const QString& name);

	void clear();

private:
	static constexpr const char* IPAddressSetting = "ipaddr";
	static constexpr const char* GeometrySettings = "geometry";
	static constexpr const char* WindowStateSettings = "windowstate";
	static constexpr const char* LeftSplitterSettings = "leftsplitter";
	static constexpr const char* ContainerPropName = "which";

private:
	QString ipaddr_;
	QSplitter* left_right_splitter_;
	XeroNTTreeWidget* nt_tree_;
	XeroPlotTreeWidget *plot_tree_;
	ImageWidgetList* image_list_;
	QTabWidget* board_tab_;
	QTabWidget* select_tab_;
	nt::NetworkTableInstance inst_;
	QSettings settings_;
	int which_tab_;
	int count_;
	std::map<int, XeroBoardWidget *> containers_;
	TabEditName* editor_;
	CustomImageMgr image_mgr_;
	QLabel* status_text_;
	bool connected_;
	QString filename_;

	QMenu* file_;
	QMenu* edit_;
	QMenu* view_;
	QMenu* align_;
	QMenu* help_;

	bool dirty_;
};
