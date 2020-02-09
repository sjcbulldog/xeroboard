#pragma once

#include <QWidget>
#include "ui_XeroListWidget.h"

class XeroListWidget : public QWidget
{
	Q_OBJECT

public:
	XeroListWidget(QWidget *parent = Q_NULLPTR);
	~XeroListWidget();

private:
	Ui::XeroListWidget ui;
};
