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
	virtual ~XeroSingleItemWidget();

	SingleDataSource* takeSource() {
		SingleDataSource* ret = source_;
		source_ = nullptr;
		disconnect(conn_);
		return ret;
	}

private:
	enum class DisplayType
	{
		None,
		Text,
		TextList,
		HorizontalBar,
		VerticalBar,
		ColorCircle,
		ColorSquare,
		ColorSquareList
	};

private:
	void valueChanged();
	DisplayType mapDataToDisplayType(std::shared_ptr<nt::Value> value);

	QRect getSize(QWidget *display);

private:
	std::shared_ptr<IDataDisplay> data_display_;
	SingleDataSource* source_;
	DisplayType display_;
	QPoint initial_loc_;
	QMetaObject::Connection conn_;
};

