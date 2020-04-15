#include "ImageWidget.h"
#include <QPainter>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDragLeaveEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QMenu>
#include <QAction>
#include <QFontDialog>

ImageWidget::ImageWidget(std::shared_ptr<CustomImage> image, const QPoint& loc, QWidget* parent) : XeroDisplayWidget(parent)
{
	image_ = image;
	setAcceptDrops(true);

	int width = image_->image().width() + LeftRightBorder * 2 + LeftRightImBoundary * 2;
	int height = image_->image().height() + TopBottomBorder * 2 + TopBottomImBoundary * 2;
	QRect r(loc.x(), loc.y(), width, height);
	setGeometry(r);

	setTitle(image->name());

	has_cursor_ = false;

	for (int i = 0; i < image->slotCount(); i++)
		sources_.push_back(nullptr);

	font_ = font();
}

ImageWidget::~ImageWidget()
{
}

void ImageWidget::dragEnterEvent(QDragEnterEvent* ev)
{
	setBackgroundRole(QPalette::Highlight);
	ev->setDropAction(Qt::DropAction::CopyAction);
	ev->acceptProposedAction();
	has_cursor_ = true;
	repaint();
}

void ImageWidget::dragMoveEvent(QDragMoveEvent* ev)
{
	ev->acceptProposedAction();
}

void ImageWidget::dragLeaveEvent(QDragLeaveEvent* ev)
{
	setBackgroundRole(QPalette::Background);
	has_cursor_ = false;
	repaint();
}

void ImageWidget::dropEvent(QDropEvent* ev)
{
	QString text;
	static const char* textMimeType = "application/x-text";

	const QMimeData* data = ev->mimeData();
	if (data->hasFormat(textMimeType))
	{
		QString node = QString::fromUtf8(data->data(textMimeType));
		if (node.startsWith("var:"))
			node = node.mid(4);

		std::shared_ptr<SingleDataSource> src = std::make_shared<SingleDataSource>(node.toStdString());
		dropNode(src, ev->pos());
	}
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

	p.save();
	p.setFont(font_);
	QFontMetrics fm(p.font());

	QBrush txtbrush(QColor(0, 0, 255, 255));
	QBrush emptybrush(QColor(212, 212, 212, 255));

	QPen pen(QColor(0, 0, 255, 255));

	for (int i = 0; i < image_->slotCount(); i++) {
		auto slot = image_->slot(i);
		r = QRect(xoff + slot.bounds().x(), yoff + slot.bounds().y(), slot.bounds().width(), slot.bounds().height());

		if (sources_[i] != nullptr) {
			p.setBrush(txtbrush);
			p.setPen(pen);
			QString txt = sources_[i]->value().toString();
			if (slot.alignment() == CustomImage::CustomImageSlot::Alignment::Right)
				p.drawText(r, Qt::AlignRight | Qt::AlignVCenter, txt);
			else if (slot.alignment() == CustomImage::CustomImageSlot::Alignment::Center)
				p.drawText(r, Qt::AlignHCenter | Qt::AlignVCenter, txt);
			else 
				p.drawText(r, Qt::AlignLeft | Qt::AlignVCenter, txt);
		}
		else {
			p.setBrush(emptybrush);
			p.setPen(Qt::PenStyle::NoPen);
			p.drawRect(r);
		}
	}
	p.restore();

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

int ImageWidget::findSlotByPoint(const QPoint& pt)
{
	for (int i = 0; i < image_->slotCount(); i++)
	{
		auto slot = image_->slot(i);
		if (slot.bounds().contains(pt))
			return i;
	}

	return -1;
}

void ImageWidget::dropNode(std::shared_ptr<SingleDataSource> src, QPoint pt)
{
	QRect client = getClientArea();

	int xoff = client.x() + LeftRightImBoundary;
	int yoff = client.y() + TopBottomImBoundary;

	QPoint qp(pt.x() - xoff, pt.y() - yoff);
	int index = findSlotByPoint(qp);
	if (index != -1) {
		sources_[index] = src;
		repaint();
	}
	else {
	}
}

void ImageWidget::contextMenuEvent(QContextMenuEvent* ev)
{
	QMenu menu(this);
	QAction* act = new QAction("Change Font");
	menu.addAction(act);
	connect(act, &QAction::triggered, this, &ImageWidget::changeFont);
	menu.exec(ev->globalPos());
}

void ImageWidget::changeFont()
{
	bool ok;

	QFont newfont = QFontDialog::getFont(&ok, font(), this);
	if (ok)
		font_ = newfont;
}

void ImageWidget::createJSON(QJsonObject& obj)
{
}
