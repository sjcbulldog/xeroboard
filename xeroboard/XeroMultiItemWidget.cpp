#include "XeroMultiItemWidget.h"
#include <QTreeWidget>
#include <QLabel>
#include <memory>

XeroMultiItemWidget::XeroMultiItemWidget(QRect r, QWidget* parent) : XeroDisplayWidget(parent)
{
	setTitle("MultiDisplay");
	display_ = std::make_shared<QTreeWidget>(this);
	display_->setHeaderHidden(true);
	display_->setColumnCount(2);
	display_->show();

	setGeometry(r);
	setChild(display_);
}

XeroMultiItemWidget::~XeroMultiItemWidget()
{
	for (auto s : sources_)
		delete s;

	sources_.clear();
}

void XeroMultiItemWidget::addSource(const std::string& name)
{
	auto src = new SingleDataSource(name);
	sources_.push_back(src);
	valueChanged();
}

void XeroMultiItemWidget::addSource(SingleDataSource* src)
{
	sources_.push_back(src);
	valueChanged();
}

void XeroMultiItemWidget::setOne(size_t index)
{
	QString txt;

	auto value = sources_[index]->value();

	switch (value->type())
	{
	case NT_UNASSIGNED:
		break;
	case NT_BOOLEAN:
		txt = value->GetBoolean() ? "TRUE" : "FALSE";
		break;
	case NT_DOUBLE:
		txt = QString::number(value->GetDouble());
		break;
	case NT_STRING:
		txt = value->GetString().str().c_str();
		break;
	case NT_RAW:
		break;
	case NT_BOOLEAN_ARRAY:
		break;
	case NT_DOUBLE_ARRAY:
		break;
	case NT_STRING_ARRAY:
		break;
	case NT_RPC:
		break;
	}

	QTreeWidgetItem* item;
	if (index >= display_->topLevelItemCount())
	{
		item = new QTreeWidgetItem();
		QString name = sources_[index]->name();

		size_t index = name.lastIndexOf('/');
		if (index == -1)
			item->setText(0, name);
		else
			item->setText(0, name.mid(index + 1));

		display_->addTopLevelItem(item);
	}
	else
	{
		item = display_->topLevelItem(index);
	}

	item->setText(1, txt);
	item->setToolTip(1, txt);
	display_->resizeColumnToContents(0);
	display_->resizeColumnToContents(1);
}

void XeroMultiItemWidget::valueChanged()
{
	for (size_t i = 0; i < sources_.size(); i++)
		setOne(i);
}
