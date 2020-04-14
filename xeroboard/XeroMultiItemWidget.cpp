#include "XeroMultiItemWidget.h"
#include <QTreeWidget>
#include <QLabel>
#include <QJsonObject>
#include <QJsonArray>
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

	for (auto conn : connections_)
		disconnect(conn);

	connections_.clear();
}

void XeroMultiItemWidget::addSource(const std::string& name, bool resize)
{
	auto src = new SingleDataSource(name);
	addSource(src, resize);
}

void XeroMultiItemWidget::addSource(SingleDataSource* src, bool resize)
{
	sources_.push_back(src);
	valueChanged();

	auto conn = connect(src, &SingleDataSource::valueChanged, this, &XeroMultiItemWidget::valueChanged);
	connections_.push_back(conn);

	if (resize) {
		QSize size = display_->sizeHint();
		QRect geom = geometry();

		int width = size.width();
		if (width < geom.width())
			width = geom.width();

		int height = size.height() + TitleHeight;
		if (height < geom.height())
			height = geom.height();

		QRect newgeom(geom.left(), geom.top(), width, height);
		setGeometry(newgeom);
	}
}

void XeroMultiItemWidget::setOne(size_t index)
{
	QString txt;

	auto value = sources_[index]->value();
	txt = value.toString();



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

void XeroMultiItemWidget::createJSON(QJsonObject& obj)
{
	QJsonArray arr;

	obj["title"] = title();
	for (SingleDataSource *src : sources_)
		arr.push_back(src->name());
	obj["sources"] = arr;
	obj["type"] = "multiple";
}
