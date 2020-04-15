 #include "XeroBoardWidget.h"
#include "XeroSingleItemWidget.h"
#include "XeroMultiItemWidget.h"
#include "XeroPlotItemWidget.h"
#include "XeroBoardMainWindow.h"
#include "Plot.h"
#include "ImageWidget.h"
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QPainter>
#include <QDebug>
#include <QMessageBox>
#include <QJsonObject>
#include <QJsonArray>
#include <QApplication>
#include <algorithm>
#include <cassert>

XeroBoardWidget::XeroBoardWidget(XeroBoardMainWindow* main, QWidget *parent) : QWidget(parent)
{
	setAcceptDrops(true);
	setFocusPolicy(Qt::ClickFocus);
	main_ = main;
	enabled_ = false;
}

XeroBoardWidget::~XeroBoardWidget()
{
}

QRect XeroBoardWidget::parseGeometry(const QJsonObject& obj)
{
	if (!obj.contains("x") || !obj["x"].isDouble()) 
	{
		std::runtime_error err("geometry does not contain 'x' member that is a number");
		throw err;
	}

	if (!obj.contains("y") || !obj["y"].isDouble()) 
	{
		std::runtime_error err("geometry does not contain 'y' member that is a number");
		throw err;
	}

	if (!obj.contains("width") || !obj["width"].isDouble()) 
	{
		std::runtime_error err("geometry does not contain 'width' member that is a number");
		throw err;
	}

	if (!obj.contains("height") || !obj["height"].isDouble()) 
	{
		std::runtime_error err("geometry does not contain 'height' member that is a number");
		throw err;
	}

	double x = obj["x"].toDouble();
	double y = obj["y"].toDouble();
	double width = obj["width"].toDouble();
	double height = obj["height"].toDouble();

	return QRect(x, y, width, height);
}

void XeroBoardWidget::createWidget(const QJsonObject& obj)
{
	if (!obj.contains("geometry") || !obj["geometry"].isObject())
	{
		std::runtime_error err("item descriptor does not contain valid 'geometry' member");
		throw err;
	}

	if (!obj.contains("child") || !obj["child"].isObject())
	{
		std::runtime_error err("item descriptor does not contain valid 'child' member");
		throw err;
	}

	QRect r = parseGeometry(obj["geometry"].toObject());

	QJsonObject child = obj["child"].toObject();
	if (!child.contains("type") || !child["type"].isString())
	{
		std::runtime_error err("item child descriptor does not contain valid 'type' member");
		throw err;
	}

	QString type = child["type"].toString();

	if (type == "single")
		createSingle(child, r);
	else if (type == "multiple")
		createMultiple(child, r);
}

void XeroBoardWidget::createSingle(const QJsonObject& desc, const QRect& r)
{
	if (!desc.contains("source") || !desc["source"].isString())
	{
		std::runtime_error err("item single child descriptor does not contain valid 'source' member");
		throw err;
	}
	QString source = desc["source"].toString();

	XeroSingleItemWidget* item = new XeroSingleItemWidget(source.toStdString(), r.topLeft(), this);
	item->setGeometry(r);
	display_widgets_.push_back(item);
	item->setVisible(true);
}

void XeroBoardWidget::createMultiple(const QJsonObject& desc, const QRect& r)
{
	if (!desc.contains("sources") || !desc["sources"].isArray())
	{
		std::runtime_error err("item single child descriptor does not contain valid 'sources' member");
		throw err;
	}
	QJsonArray sources = desc["sources"].toArray();
	XeroMultiItemWidget *item = new XeroMultiItemWidget(r, this);

	for (int i = 0; i < sources.size(); i++) {
		if (sources[i].isString())
			item->addSource(sources[i].toString().toStdString(), false);
	}

	item->setGeometry(r);
	display_widgets_.push_back(item);
	item->setVisible(true);
}

void XeroBoardWidget::init(const QJsonObject& obj)
{
	if (!obj.contains("items")) 
	{
		std::runtime_error err("tab JSON object does not contain items array");
		throw err;
	}

	if (!obj["items"].isArray())
	{
		std::runtime_error err("tab JSON object does not contain items array");
		throw err;
	}

	QJsonArray items = obj["items"].toArray();

	for (int i = 0; i < items.size(); i++) {
		if (!items[i].isObject())
			continue;

		QJsonObject obj = items[i].toObject();
		createWidget(obj);
	}
}

void XeroBoardWidget::setEnabled(bool b)
{
	enabled_ = b;

	for (auto w : display_widgets_)
		w->setEnabled(b);
}

void XeroBoardWidget::dragEnterEvent(QDragEnterEvent* ev)
{
	setBackgroundRole(QPalette::Highlight);
	ev->setDropAction(Qt::DropAction::CopyAction);
	ev->acceptProposedAction();
}

void XeroBoardWidget::dragMoveEvent(QDragMoveEvent* ev)
{
	ev->acceptProposedAction();
}

void XeroBoardWidget::dragLeaveEvent(QDragLeaveEvent* ev)
{
	setBackgroundRole(QPalette::Background);
}

void XeroBoardWidget::dropVariable(QString node, QPoint pt)
{
	for (auto w : display_widgets_)
	{
		QRect r = w->geometry();
		if (r.contains(pt))
		{
			//
			// Replace a single widget with a multi widget
			//
			replaceSingleWithMulti(w, node.toStdString());
			return;
		}
	}

	SingleDataSource* src = new SingleDataSource(node.toStdString());
	if (src->isSubtable())
	{
		QMessageBox::warning(this, "Error", "Cannot display complete table, select a single value.");
		delete src;
	}
	else
	{
		XeroSingleItemWidget* item = new XeroSingleItemWidget(node.toStdString(), pt, this);
		display_widgets_.push_back(item);
		item->setVisible(true);
		main_->setDirty(true);
	}
}

void XeroBoardWidget::dropImage(QString node, QPoint pt)
{
	auto image = main_->getCustomImageManager().get(node);
	if (image != nullptr) {
		ImageWidget* item = new ImageWidget(image, pt, this);
		display_widgets_.push_back(item);
		item->setVisible(true);
		main_->setDirty(true);
	}
}

void XeroBoardWidget::dropPlot(QString key, QPoint pt)
{
	QRect r(pt, QSize(512, 384));
	Plot* plot = new Plot(key.toStdString());
	XeroPlotItemWidget* item = new XeroPlotItemWidget(plot, this);
	item->setGeometry(r);
	display_widgets_.push_back(item);
	item->setVisible(true);
	main_->setDirty(true);
}

void XeroBoardWidget::dropPlotVar(QString key, QPoint pt)
{
	QRegExp exp("\\{(.*)\\},\\{(.*)\\}");
	if (exp.exactMatch(key)) {
		QString plotname = exp.cap(1);
		QString varname = exp.cap(2);

		QRect r(pt, QSize(512, 384));
		Plot* plot = new Plot(plotname.toStdString());
		XeroPlotItemWidget* item = new XeroPlotItemWidget(plot, this);
		item->setGeometry(r);
		display_widgets_.push_back(item);
		item->setVisible(true);
		item->addVar(varname);
		main_->setDirty(true);
	}
}

void XeroBoardWidget::dropEvent(QDropEvent* ev)
{
	QString text;
	static const char* textMimeType = "application/x-text";

	const QMimeData* data = ev->mimeData();
	if (data->hasFormat(textMimeType))
	{
		QString node = QString::fromUtf8(data->data(textMimeType));

		if (node.startsWith("var:"))
		{
			dropVariable(node.mid(4), ev->pos());
		}
		else if (node.startsWith("plot:"))
		{
			dropPlot(node.mid(5), ev->pos());
		}
		else if (node.startsWith("plotvar:"))
		{
			dropPlotVar(node.mid(8), ev->pos());
		}
		else if (node.startsWith("image:"))
		{
			dropImage(node.mid(6), ev->pos());
		}
	}
	setBackgroundRole(QPalette::Background);
}

void XeroBoardWidget::removeChild(XeroDisplayWidget *widget)
{
	auto it = std::find(display_widgets_.begin(), display_widgets_.end(), static_cast<QWidget*>(widget));
	assert(it != display_widgets_.end());

	display_widgets_.erase(it);
}

void XeroBoardWidget::replaceSingleWithMulti(XeroDisplayWidget* w, const std::string& newnode)
{
	auto multi = dynamic_cast<XeroMultiItemWidget *>(w);
	if (multi != nullptr)
	{
		multi->addSource(newnode, false);
	}
	else 
	{
		auto single = dynamic_cast<XeroSingleItemWidget*>(w);
		if (single != nullptr)
		{
			QRect r = w->geometry();
			XeroMultiItemWidget* neww = new XeroMultiItemWidget(r, this);
			neww->addSource(single->takeSource(), false);
			single->close();

			neww->addSource(newnode, true);
			display_widgets_.push_back(neww);
			neww->setVisible(true);
		}
	}
	main_->setDirty(true);
}

void XeroBoardWidget::selectWidget(XeroDisplayWidget* w, bool replace)
{
	if (replace)
	{
		for (auto* wd : selected_widgets_)
			wd->setSelected(false);
		selected_widgets_.clear();
	}

	w->setSelected(true);

	auto it = std::find(selected_widgets_.begin(), selected_widgets_.end(), w);
	if (it == selected_widgets_.end())
		selected_widgets_.push_back(w);
}

void XeroBoardWidget::mousePressEvent(QMouseEvent* ev)
{
	for (auto* wd : selected_widgets_)
		wd->setSelected(false);
	selected_widgets_.clear();
}

void XeroBoardWidget::startDragging(const QPointF& start)
{
	drag_start_ = start;
}

void XeroBoardWidget::continueDragging(const QPointF &point)
{
	double dx = point.x() - drag_start_.x();
	double dy = point.y() - drag_start_.y();

	for (auto w : selected_widgets_)
	{
		QRect geom = w->geometry();
		QRect newgeom(geom.left() + dx, geom.top() + dy, geom.width(), geom.height());
		w->setGeometry(newgeom);
	}

	drag_start_ = point;
}

void XeroBoardWidget::endDragging()
{
}

void XeroBoardWidget::alignLeftEdge()
{
	if (selected_widgets_.size() < 2)
		return;

	int left = selected_widgets_[0]->geometry().left();
	for (size_t i = 1; i < selected_widgets_.size(); i++)
	{
		QRect geom = selected_widgets_[i]->geometry();
		QRect newgeom(left, geom.top(), geom.width(), geom.height());
		selected_widgets_[i]->setGeometry(newgeom);
	}
}

void XeroBoardWidget::alignRightEdge()
{
	if (selected_widgets_.size() < 2)
		return;

	int right = selected_widgets_[0]->geometry().right();
	for (size_t i = 1; i < selected_widgets_.size(); i++)
	{
		QRect geom = selected_widgets_[i]->geometry();
		QRect newgeom(right - geom.width(), geom.top(), geom.width(), geom.height());
		selected_widgets_[i]->setGeometry(newgeom);
	}
}

void XeroBoardWidget::alignTopEdge()
{
	if (selected_widgets_.size() < 2)
		return;

	int top = selected_widgets_[0]->geometry().top();
	for (size_t i = 1; i < selected_widgets_.size(); i++)
	{
		QRect geom = selected_widgets_[i]->geometry();
		QRect newgeom(geom.left(), top, geom.width(), geom.height());
		selected_widgets_[i]->setGeometry(newgeom);
	}
}

void XeroBoardWidget::alignBottomEdge()
{
	if (selected_widgets_.size() < 2)
		return;

	int bottom = selected_widgets_[0]->geometry().bottom();
	for (size_t i = 1; i < selected_widgets_.size(); i++)
	{
		QRect geom = selected_widgets_[i]->geometry();
		QRect newgeom(geom.left(), bottom - geom.height(), geom.width(), geom.height());
		selected_widgets_[i]->setGeometry(newgeom);
	}
}

void XeroBoardWidget::alignHorizontalCenter()
{
	if (selected_widgets_.size() < 2)
		return;

	int center = selected_widgets_[0]->geometry().center().y();
	for (size_t i = 1; i < selected_widgets_.size(); i++)
	{
		QRect geom = selected_widgets_[i]->geometry();
		QRect newgeom(geom.left(), center - geom.height() / 2, geom.width(), geom.height());
		selected_widgets_[i]->setGeometry(newgeom);
	}
}

void XeroBoardWidget::alignVerticalCenter()
{
	if (selected_widgets_.size() < 2)
		return;

	int center = selected_widgets_[0]->geometry().center().x();
	for (size_t i = 1; i < selected_widgets_.size(); i++)
	{
		QRect geom = selected_widgets_[i]->geometry();
		QRect newgeom(center - geom.width() / 2, geom.top(),  geom.width(), geom.height());
		selected_widgets_[i]->setGeometry(newgeom);
	}
}

void XeroBoardWidget::makeSameSize()
{
	if (selected_widgets_.size() < 2)
		return;

	int width = selected_widgets_[0]->geometry().width();
	int height = selected_widgets_[0]->geometry().height();
	for (size_t i = 1; i < selected_widgets_.size(); i++)
	{
		QRect geom = selected_widgets_[i]->geometry();
		QRect newgeom(geom.left(), geom.top(), width, height);
		selected_widgets_[i]->setGeometry(newgeom);
	}
}

void XeroBoardWidget::keyPressEvent(QKeyEvent* ev)
{
	bool shift = (ev->modifiers() == Qt::KeyboardModifier::ShiftModifier);
	int delta = shift ? 1 : 10;

	switch (ev->key())
	{
		case Qt::Key::Key_Delete:
			for (auto w : selected_widgets_)
				w->close();
			break;

		case Qt::Key::Key_Left:
			for (auto w : selected_widgets_)
			{
				QRect geom = w->geometry();
				QRect newgeom(geom.left() - delta, geom.top(), geom.width(), geom.height());
				w->setGeometry(newgeom);
			}
			break;
		case Qt::Key::Key_Right:
			for (auto w : selected_widgets_)
			{
				QRect geom = w->geometry();
				QRect newgeom(geom.left() + delta, geom.top(), geom.width(), geom.height());
				w->setGeometry(newgeom);
			}
			break;
		case Qt::Key::Key_Up:
			for (auto w : selected_widgets_)
			{
				QRect geom = w->geometry();
				QRect newgeom(geom.left(), geom.top() - delta, geom.width(), geom.height());
				w->setGeometry(newgeom);
			}
			break;
		case Qt::Key::Key_Down:
			for (auto w : selected_widgets_)
			{
				QRect geom = w->geometry();
				QRect newgeom(geom.left(), geom.top() + delta, geom.width(), geom.height());
				w->setGeometry(newgeom);
			}
			break;
	}
}

void XeroBoardWidget::createJSON(QJsonObject& obj)
{
	QJsonArray arr;

	for (auto item : display_widgets_) {
		QJsonObject itemobj;
		QRect geom = item->geometry();
		QJsonObject geomobj;

		geomobj["x"] = geom.x();
		geomobj["y"] = geom.y();
		geomobj["width"] = geom.width();
		geomobj["height"] = geom.height();
		itemobj["geometry"] = geomobj;

		QJsonObject child;
		item->createJSON(child);
		itemobj["child"] = child;

		arr.push_back(itemobj);
	}

	obj["items"] = arr;
	obj["title"] = title_;
}
