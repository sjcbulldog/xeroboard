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
	virtual void mousePressEvent(QMouseEvent* ev);
	virtual void mouseReleaseEvent(QMouseEvent* ev);
	virtual void mouseMoveEvent(QMouseEvent* ev);

	virtual void setTitle(const QString& str) { title_ = str; }

	QRect getClientArea();
	QRect getTitleBarArea();
	QRect getCloseRectArea();

	static constexpr int TitleHeight = 12;
	static constexpr int LeftRightBorder = 3;
	static constexpr int CloseXBorder = 3;

private:
	QString title_;
	nt::NetworkTableInstance inst_;
	bool dragging_;
	QPointF start_drag_point_;
	QPoint start_widget_point_;
};
