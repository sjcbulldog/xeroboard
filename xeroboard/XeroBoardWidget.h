#pragma once

#include "XeroDisplayWidget.h"
#include <QWidget>
#include <list>
#include <vector>

class XeroBoardWidget : public QWidget
{
	friend class XeroDisplayWidget;
	Q_OBJECT

public:
	XeroBoardWidget(QWidget *parent = Q_NULLPTR);
	~XeroBoardWidget();

	void selectWidget(XeroDisplayWidget* wid, bool replace);

	void startDragging(const QPointF& pt);
	void continueDragging(const QPointF& pt);
	void endDragging();

	void alignLeftEdge();
	void alignRightEdge();
	void alignTopEdge();
	void alignBottomEdge();
	void alignHorizontalCenter();
	void alignVerticalCenter();
	void makeSameSize();

protected:
	virtual void dragEnterEvent(QDragEnterEvent* ev);
	virtual void dragMoveEvent(QDragMoveEvent* ev);
	virtual void dragLeaveEvent(QDragLeaveEvent* ev);
	virtual void dropEvent(QDropEvent* ev);
	virtual void mousePressEvent(QMouseEvent* ev);
	virtual void keyPressEvent(QKeyEvent* ev);

	void removeChild(XeroDisplayWidget* obj);
	void replaceSingleWithMulti(XeroDisplayWidget* w, const std::string& newnode);

private:
	void dropVariable(QString node, QPoint pt);
	void dropPlot(QString node, QPoint pt);

private:
	std::list<XeroDisplayWidget*> display_widgets_;
	std::vector<XeroDisplayWidget*> selected_widgets_;
	QPointF drag_start_;
};
