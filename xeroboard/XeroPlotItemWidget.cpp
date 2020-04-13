#include "XeroPlotItemWidget.h"
#include "Plot.h"
#include <QJsonArray>
#include <QJsonObject>

XeroPlotItemWidget::XeroPlotItemWidget(Plot* plot, QWidget* parent) : XeroDisplayWidget(parent)
{
	plot_ = plot;
	setTitle(plot_->key().c_str());
	chart_ = new QtCharts::QChartView(this);
	chart_->chart()->setDropShadowEnabled(true);

	populateChart();
}

void XeroPlotItemWidget::resizeEvent(QResizeEvent* ev)
{
	chart_->setGeometry(getClientArea());
}

void XeroPlotItemWidget::createJSON(QJsonObject& obj)
{
	QJsonArray nodes;

	obj["key"] = plot_->key().c_str();
	obj["title"] = title_;

	for (QString node : node_names_)
		nodes.push_back(node);
	obj["nodes"] = nodes;
}

void XeroPlotItemWidget::populateChart()
{

}