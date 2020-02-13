#pragma once

#include <networktables/NetworkTableInstance.h>
#include <networktables/NetworkTable.h>
#include <QWidget>
#include <QString>

class XeroBoardWidget;

class XeroDisplayWidget : public QWidget
{
public:
	XeroDisplayWidget(QWidget* parent);
	virtual ~XeroDisplayWidget();

	void setSelected(bool b) {
		if (b != selected_)
		{
			selected_ = b;
			repaint();
		}
	}

	bool selected() {
		return selected_;
	}

protected:
	virtual void closeEvent(QCloseEvent* ev);
	virtual void paintEvent(QPaintEvent* ev);
	virtual void mousePressEvent(QMouseEvent* ev);
	virtual void mouseReleaseEvent(QMouseEvent* ev);
	virtual void mouseMoveEvent(QMouseEvent* ev);
	virtual void resizeEvent(QResizeEvent* ev);

	virtual void setTitle(const QString& str) { title_ = str; }
	virtual void setChild(std::shared_ptr<QWidget> child);

	QRect getClientArea();
	QRect getTitleBarArea();
	QRect getCloseRectArea();

	XeroBoardWidget* parentBoard();

	static constexpr int TitleHeight = 12;
	static constexpr int LeftRightBorder = 5;
	static constexpr int TopBottomBorder = 5;
	static constexpr int CloseXBorder = 3;
	static constexpr int CornerCursorBoundary = 5;
	static constexpr int EdgeCursorBoundary = 5;

private:
	enum class MouseMode
	{
		None,
		Dragging,
		ResizeLLCorner,
		ResizeLRCorner,
		ResizeLeft,
		ResizeRight,
		ResizeBottom,
	};

	enum class MouseCursor
	{
		Arrow,
		ResizeLLCorner,
		ResizeLRCorner,
		ResizeLeft,
		ResizeRight,
		ResizeBottom,
	};

private:
	QString title_;
	nt::NetworkTableInstance inst_;
	MouseMode mouse_mode_;
	MouseCursor mouse_cursor_;
	QPointF start_drag_point_;
	QRect start_widget_geom_;
	std::shared_ptr<QWidget> child_;
	bool selected_;
};
