#pragma once

#include "XeroDisplayWidget.h"
#include "SingleDataSource.h"
#include <QTreeWidget>
#include <vector>

class XeroMultiItemWidget : public XeroDisplayWidget
{
public:
	XeroMultiItemWidget(QRect r, QWidget* parent);
	virtual ~XeroMultiItemWidget();

	void addSource(const std::string &name);
	void addSource(SingleDataSource* src);

private:
	void valueChanged();
	void setOne(size_t index);

private:
	std::vector<SingleDataSource*> sources_;
	std::vector<QMetaObject::Connection> connections_;
	std::shared_ptr<QTreeWidget> display_;
};

