#pragma once

#include "XeroDisplayWidget.h"
#include <QWidget>

class XeroBoardWidget : public QWidget
{
	friend class XeroDisplayWidget;
	Q_OBJECT

public:
	XeroBoardWidget(QWidget *parent = Q_NULLPTR);
	~XeroBoardWidget();

	void selectWidget(XeroDisplayWidget* wid, bool replace);

protected:
	virtual void dragEnterEvent(QDragEnterEvent* ev);
	virtual void dragMoveEvent(QDragMoveEvent* ev);
	virtual void dragLeaveEvent(QDragLeaveEvent* ev);
	virtual void dropEvent(QDropEvent* ev);
	virtual void mousePressEvent(QMouseEvent* ev);

	void removeChild(XeroDisplayWidget* obj);
	void replaceSingleWithMulti(XeroDisplayWidget* w, const std::string& newnode);

private:
	std::list<XeroDisplayWidget*> display_widgets_;
	std::list<XeroDisplayWidget*> selected_widgets_;
};
