#pragma once

#include <networktables/NetworkTableInstance.h>
#include <networktables/NetworkTable.h>
#include <QWidget>
#include <QString>

class XeroDisplayWidget : public QWidget
{
public:
	XeroDisplayWidget(QWidget* parent);
	virtual ~XeroDisplayWidget();

protected:
	virtual void closeEvent(QCloseEvent* ev);
	virtual void paintEvent(QPaintEvent* ev);

	virtual void setTitle(const QString& str) { title_ = str; }

	QRect getClientArea();

	static constexpr int TitleHeight = 12;
	static constexpr int LeftRightBorder = 3;


private:
	QString title_;
	nt::NetworkTableInstance inst_;
};

