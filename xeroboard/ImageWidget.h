#pragma once

#include <CustomImage.h>
#include <XeroDisplayWidget.h>
#include <SingleDataSource.h>
#include <memory>

class ImageWidget : public XeroDisplayWidget
{
public:
	ImageWidget(std::shared_ptr<CustomImage> image, const QPoint &loc, QWidget* parent = nullptr);
	virtual ~ImageWidget();

	void createJSON(QJsonObject& obj);
	void dropNode(std::shared_ptr<SingleDataSource> src, QPoint pt);

protected:
	virtual void paintEvent(QPaintEvent* ev);
	virtual void enterEvent(QEvent* ev);
	virtual void leaveEvent(QEvent* ev);
	virtual void dragEnterEvent(QDragEnterEvent* ev);
	virtual void dragMoveEvent(QDragMoveEvent* ev);
	virtual void dragLeaveEvent(QDragLeaveEvent* ev);
	virtual void dropEvent(QDropEvent* ev);
	virtual void contextMenuEvent(QContextMenuEvent* ev);

private:
	int findSlotByPoint(const QPoint& pt);
	void changeFont();

private:
	static constexpr int LeftRightImBoundary = 4;
	static constexpr int TopBottomImBoundary = 4;

private:
	std::shared_ptr<CustomImage> image_;
	bool has_cursor_;
	std::vector<std::shared_ptr<SingleDataSource>> sources_;
	QFont font_;
};

