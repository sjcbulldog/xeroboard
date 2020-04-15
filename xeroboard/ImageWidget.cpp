#include "ImageWidget.h"
#include <QPainter>

ImageWidget::ImageWidget(std::shared_ptr<CustomImage> image, const QPoint& loc, QWidget* parent) : XeroDisplayWidget(parent)
{
	image_ = image;

	int width = image_->image().width() + LeftRightBorder * 2 + LeftRightImBoundary * 2;
	int height = image_->image().height() + TopBottomBorder * 2 + TopBottomImBoundary * 2;
	QRect r(loc.x(), loc.y(), width, height);
	setGeometry(r);

	setTitle(image->name());

	has_cursor_ = false;
}

ImageWidget::~ImageWidget()
{
}

void ImageWidget::enterEvent(QEvent* ev)
{
	if (!has_cursor_) {
		has_cursor_ = true;
		repaint();
	}
}

void ImageWidget::leaveEvent(QEvent* ev)
{
	if (has_cursor_) {
		has_cursor_ = false;
		repaint();
	}
}

void ImageWidget::paintEvent(QPaintEvent* ev)
{
	XeroDisplayWidget::paintEvent(ev);

	QPainter p(this);
	QRect client = getClientArea();

	int xoff = client.x() + LeftRightImBoundary;
	int yoff = client.y() + TopBottomImBoundary;
	QRect r(xoff, yoff, client.width() - 2 * LeftRightImBoundary, client.height() - 2 * TopBottomImBoundary);
	p.drawImage(r, image_->image());

	if (has_cursor_) {
		QPen pen(QColor(0, 0, 0, 255));
		pen.setStyle(Qt::PenStyle::DashLine);
		p.setPen(pen);

		for (int i = 0; i < image_->slotCount(); i++) {
			auto slot = image_->slot(i);
			QRect s = slot.bounds();
			r = QRect(xoff + s.x(), yoff + s.y(), s.width(), s.height());
			p.drawRect(r);
		}
	}
}

void ImageWidget::createJSON(QJsonObject& obj)
{
}