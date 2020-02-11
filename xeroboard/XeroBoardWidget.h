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

protected:
	virtual void dragEnterEvent(QDragEnterEvent* ev);
	virtual void dragMoveEvent(QDragMoveEvent* ev);
	virtual void dragLeaveEvent(QDragLeaveEvent* ev);
	virtual void dropEvent(QDropEvent* ev);

	void removeChild(XeroDisplayWidget* obj);
	void replaceSingleWithMulti(XeroDisplayWidget* w, const std::string& newnode);

private:
	std::list<XeroDisplayWidget*> display_widgets_;
};
