#pragma once
#include "SingleValueDisplay.h"
#include "XeroDisplayWidget.h"
#include "DataSource.h"
#include <QFrame>
#include <string>

class XeroSingleItemWidget : public XeroDisplayWidget
{
public:
	XeroSingleItemWidget(std::shared_ptr<DataSource> src, QWidget* parent);
	virtual ~XeroSingleItemWidget();

	void createJSON(QJsonObject& obj);

protected:
	virtual void resizeEvent(QResizeEvent* ev);
	virtual void dragEnterEvent(QDragEnterEvent* ev);
	virtual void dragMoveEvent(QDragMoveEvent* ev);
	virtual void dragLeaveEvent(QDragLeaveEvent* ev);
	virtual void dropEvent(QDropEvent* ev);

private:
	void valueChanged();

private:
	SingleValueDisplay* display_;
	std::shared_ptr<DataSource> source_;
	QMetaObject::Connection conn_;
};

