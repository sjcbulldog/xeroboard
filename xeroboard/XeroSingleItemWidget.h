#pragma once
#include "XeroDisplayWidget.h"
#include "SingleDataSource.h"
#include "IDataDisplay.h"
#include <QFrame>
#include <string>

class XeroSingleItemWidget : public XeroDisplayWidget
{
public:
	XeroSingleItemWidget(const std::string &name, QPoint loc, QWidget* parent);
	XeroSingleItemWidget(SingleDataSource *src, QPoint loc, QWidget* parent);
	virtual ~XeroSingleItemWidget();

	SingleDataSource* takeSource() {
		SingleDataSource* ret = source_;
		source_ = nullptr;
		disconnect(conn_);
		return ret;
	}

	void createJSON(QJsonObject& obj);

private:
	enum class DisplayType
	{
		None,
		Text,
		TextList,
		HorizontalBar,
		VerticalBar,
		Color,
		ColorList
	};

private:
	void valueChanged();
	DisplayType mapDataToDisplayType(std::shared_ptr<nt::Value> value);

	QRect getSize(QWidget *display);
	QString toString(DisplayType dtype);

private:
	std::shared_ptr<IDataDisplay> data_display_;
	SingleDataSource* source_;
	DisplayType display_;
	QPoint initial_loc_;
	QMetaObject::Connection conn_;
};

