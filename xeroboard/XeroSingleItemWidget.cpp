#include "XeroSingleItemWidget.h"
#include "TextDisplay.h"
#include "ColorDisplay.h"
#include "ColorListDisplay.h"
#include "ArrayDisplay.h"
#include <QLabel>
#include <QJsonObject>
#include <memory>

XeroSingleItemWidget::XeroSingleItemWidget(const std::string &name, QPoint loc, QWidget* parent) : XeroDisplayWidget(parent)
{
	display_ = DisplayType::None;
	data_display_ = nullptr;
	initial_loc_ = loc;

	size_t index = name.find_last_of('/');
	if (index == std::string::npos)
		setTitle(name.c_str());
	else
		setTitle(name.substr(index + 1).c_str());

	source_ = new SingleDataSource(name);
	valueChanged();

	conn_ = connect(source_, &SingleDataSource::valueChanged, this, &XeroSingleItemWidget::valueChanged);
}

XeroSingleItemWidget::XeroSingleItemWidget(SingleDataSource *src, QPoint loc, QWidget* parent) : XeroDisplayWidget(parent)
{
	display_ = DisplayType::None;
	data_display_ = nullptr;
	initial_loc_ = loc;

	size_t index = src->name().lastIndexOf('/');
	if (index == -1)
		setTitle(src->name());
	else
		setTitle(src->name().mid(index + 1));

	source_ = src;
	valueChanged();

	conn_ = connect(source_, &SingleDataSource::valueChanged, this, &XeroSingleItemWidget::valueChanged);
}

XeroSingleItemWidget::~XeroSingleItemWidget()
{
	if (source_ != nullptr)
		delete source_;

	disconnect(conn_);
}

XeroSingleItemWidget::DisplayType XeroSingleItemWidget::mapDataToDisplayType(std::shared_ptr<nt::Value> value)
{
	DisplayType dtype = DisplayType::None;

	switch (value->type())
	{
	case NT_UNASSIGNED:
		break;
	case NT_BOOLEAN:
		dtype = DisplayType::Color;
		break;

	case NT_DOUBLE:
		if (display_ == DisplayType::HorizontalBar || display_ == DisplayType::VerticalBar || display_ == DisplayType::Text)
			dtype = display_;
		else
			dtype = DisplayType::Text;
		break;
	case NT_STRING:
		dtype = DisplayType::Text;
		break;
	case NT_RAW:
		break;
	case NT_BOOLEAN_ARRAY:
		dtype = DisplayType::ColorList;
		break;
	case NT_DOUBLE_ARRAY:
		dtype = DisplayType::TextList;
		break;
	case NT_STRING_ARRAY:
		dtype = DisplayType::TextList;
		break;
	case NT_RPC:
		break;
	}

	return dtype;
}

QRect XeroSingleItemWidget::getSize(QWidget *display)
{
	QRect r = getClientArea();
	r.setLeft(r.left() + LeftRightBorder);
	r.setWidth(r.width() - 2 * LeftRightBorder);
	r.setHeight(display->minimumHeight());
	return r;
}

void XeroSingleItemWidget::valueChanged()
{
	QRect r;
	if (source_ == nullptr)
		return;

	auto value = source_->value();
	if (value.isEmpty())
		return;

	DisplayType dtype = mapDataToDisplayType(value.value());

	if (dtype != display_)
	{
		// Replace the display widget
		data_display_ = nullptr;
		display_ = dtype;
	}

	switch (display_)
	{
	case DisplayType::Text:
		if (data_display_ == nullptr)
		{
			auto tdisp = std::make_shared<TextDisplay>(this);
			std::shared_ptr<QWidget> wid = std::dynamic_pointer_cast<QWidget>(tdisp);
			setChild(tdisp);
			data_display_ = tdisp;

			r = geometry();
			int width = r.width();
			r.setLeft(initial_loc_.x());
			r.setTop(initial_loc_.y());
			r.setHeight(TitleHeight + tdisp->height() + 2 * TopBottomBorder);
			r.setWidth(width);
			setGeometry(r);
		}
		data_display_->setValue(value.value());

		break;
	case DisplayType::TextList:
		if (data_display_ == nullptr)
		{
			auto tdisp = std::make_shared<ArrayDisplay>(this);
			std::shared_ptr<QWidget> wid = std::dynamic_pointer_cast<QWidget>(tdisp);
			setChild(tdisp);
			data_display_ = tdisp;

			r = geometry();
			int width = r.width();
			r.setLeft(initial_loc_.x());
			r.setTop(initial_loc_.y());
			r.setHeight(TitleHeight + tdisp->height() + 2 * TopBottomBorder);
			r.setWidth(width);
			setGeometry(r);
		}
		data_display_->setValue(value.value());
		break;
	case DisplayType::HorizontalBar:
		r = geometry();
		r.setHeight(TitleHeight + 48);
		setGeometry(r);
		break;
	case DisplayType::VerticalBar:
		r = geometry();
		r.setHeight(TitleHeight + 48);
		setGeometry(r);
		break;
	case DisplayType::Color:
		if (data_display_ == nullptr)
		{
			auto tdisp = std::make_shared<ColorDisplay>(this);
			std::shared_ptr<QWidget> wid = std::dynamic_pointer_cast<QWidget>(tdisp);
			setChild(tdisp);
			data_display_ = tdisp;

			r = geometry();
			int width = r.width();
			r.setLeft(initial_loc_.x());
			r.setTop(initial_loc_.y());
			r.setHeight(TitleHeight + tdisp->height() + 2 * TopBottomBorder);
			r.setWidth(width);
			setGeometry(r);
		}
		data_display_->setValue(value.value());
		break;
	case DisplayType::ColorList:
		if (data_display_ == nullptr)
		{
			auto tdisp = std::make_shared<ColorListDisplay>(this);
			std::shared_ptr<QWidget> wid = std::dynamic_pointer_cast<QWidget>(tdisp);
			setChild(tdisp);
			data_display_ = tdisp;

			r = geometry();
			int width = r.width();
			r.setLeft(initial_loc_.x());
			r.setTop(initial_loc_.y());
			r.setHeight(TitleHeight + tdisp->height() + 2 * TopBottomBorder);
			r.setWidth(width);
			setGeometry(r);
		}
		data_display_->setValue(value.value());

		break;
	}
}

QString XeroSingleItemWidget::toString(DisplayType dtype)
{
	QString result;

	switch (dtype)
	{
	case DisplayType::None:
		break;

	case DisplayType::Text:
		result = "text";
		break;

	case DisplayType::TextList:
		result = "textlist";
		break;

	case DisplayType::HorizontalBar:
		result = "horizontalbar";
		break;

	case DisplayType::VerticalBar:
		result = "verticalbar";
		break;

	case DisplayType::Color:
		result = "color";
		break;

	case DisplayType::ColorList:
		result = "colorlist";
		break;
	}

	return result;
}

void XeroSingleItemWidget::createJSON(QJsonObject& obj)
{
	obj["title"] = title();
	obj["source"] = source_->name();
	obj["display"] = toString(display_);
	obj["type"] = "single";
}