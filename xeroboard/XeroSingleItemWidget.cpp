#include "XeroSingleItemWidget.h"
#include "TextDisplay.h"
#include <QLabel>
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

	(void)connect(source_, &SingleDataSource::valueChanged, this, &XeroSingleItemWidget::valueChanged);
}

XeroSingleItemWidget::~XeroSingleItemWidget()
{
	delete source_;
}

XeroSingleItemWidget::DisplayType XeroSingleItemWidget::mapDataToDisplayType(std::shared_ptr<nt::Value> value)
{
	DisplayType dtype = DisplayType::None;

	switch (value->type())
	{
	case NT_UNASSIGNED:
		break;
	case NT_BOOLEAN:
		if (display_ == DisplayType::ColorCircle || display_ == DisplayType::ColorSquare)
			dtype = display_;
		else
			dtype = DisplayType::ColorCircle;
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
		dtype = DisplayType::ColorSquareList;
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
	auto value = source_->getValue();
	if (value == nullptr)
		return;

	DisplayType dtype = mapDataToDisplayType(value);

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
			tdisp->setGeometry(getSize(wid.get()));
			tdisp->show();
			data_display_ = tdisp;

			r = geometry();
			int width = r.width();
			r.setLeft(initial_loc_.x());
			r.setTop(initial_loc_.y());
			r.setHeight(TitleHeight + tdisp->height());
			r.setWidth(width);
			setGeometry(r);
		}
		data_display_->setValue(value);
		break;
	case DisplayType::TextList:
		r = geometry();
		r.setHeight(TitleHeight + 48);
		setGeometry(r);
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
	case DisplayType::ColorCircle:
		r = geometry();
		r.setHeight(TitleHeight + 48);
		setGeometry(r);
		break;
	case DisplayType::ColorSquare:
		r = geometry();
		r.setHeight(TitleHeight + 48);
		setGeometry(r);
		break;
	case DisplayType::ColorSquareList:
		r = geometry();
		r.setHeight(TitleHeight + 48);
		setGeometry(r);
		break;
	}
}
