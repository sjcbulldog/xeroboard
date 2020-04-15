#pragma once

#include <CustomImage.h>
#include <XeroDisplayWidget.h>
#include <memory>

class ImageWidget : public XeroDisplayWidget
{
public:
	ImageWidget(std::shared_ptr<CustomImage> image, const QPoint &loc, QWidget* parent = nullptr);
	virtual ~ImageWidget();

	void createJSON(QJsonObject& obj);

protected:
	virtual void paintEvent(QPaintEvent* ev);
	virtual void enterEvent(QEvent* ev);
	virtual void leaveEvent(QEvent* ev);

private:
	static constexpr int LeftRightImBoundary = 4;
	static constexpr int TopBottomImBoundary = 4;

private:
	std::shared_ptr<CustomImage> image_;
	bool has_cursor_;
};

