#include "XeroPlotItemWidget.h"
#include "Plot.h"
#include "ChartWrapper.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QMessageBox>
#include <QDebug>

using namespace QtCharts;

XeroPlotItemWidget::XeroPlotItemWidget(Plot* plot, QWidget* parent) : XeroDisplayWidget(parent)
{
	left_axis_count_ = 0;
	right_axis_count_ = 0;
	units_ = "in";

	plot_ = plot;
	setTitle(plot_->key().c_str());
	chart_ = new ChartWrapper(this);

	chart_->chart()->setDropShadowEnabled(true);
	chart_->chart()->setAnimationOptions(QChart::AnimationOption::NoAnimation);
	setAcceptDrops(true);
	chart_->setAcceptDrops(true);

	time_ = nullptr;
	legend_ = nullptr;

	populateChart();
}

void XeroPlotItemWidget::resizeEvent(QResizeEvent* ev)
{
	QRect r = getClientArea();
	QRect rc(r.x() + 4, r.y(), r.width() - 8, r.height() - 4);
	chart_->setGeometry(rc);
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

void XeroPlotItemWidget::addVar(const QString& name)
{
	node_names_.push_back(name);
	populateChart();
}

void XeroPlotItemWidget::addVar(const QString& plotname, const QString& varname)
{
	if (plotname.toStdString() != plot_->key()) {
		QMessageBox::warning(this, "Error", "Cannot add this variable to the target graph, it is from a different dataset");
		return;
	}

	addVar(varname);
}

QString XeroPlotItemWidget::nodeToAxisName(const QString& node)
{
	if (node.contains("dist") || node.contains("pos"))
		return "Distance (" + units_ + ")";
	else if (node.contains("vel"))
		return "Velocity (" + units_ + "/s)";
	else if (node.contains("acc"))
		return "Accel (" + units_ + "/s^2)";
	else if (node.contains("jerk"))
		return "Jerk (" + units_ + "/s^3)";
	else if (node.contains("tick"))
		return "Ticks (count)";
	else if (node.contains("out"))
		return "Power (% pwm)";
	else if (node.contains("ang"))
		return "Angle (degrees)";
	else if (node.contains("head"))
		return "Heading (degrees)";

	return node;
}

QValueAxis* XeroPlotItemWidget::createYAxis(const QString& node)
{
	QValueAxis* result = nullptr;
	QString name = nodeToAxisName(node);
	auto it = axes_.find(name);

	if (it == axes_.end())
	{
		result = new QValueAxis();
		axes_[name] = result;

		result->setLabelsVisible(true);
		result->setTickCount(10);
		result->setTitleText(name);
		result->setTitleVisible(true);

		if (left_axis_count_ <= right_axis_count_) {
			chart_->chart()->addAxis(result, Qt::AlignLeft);
			left_axis_count_++;
		}
		else
		{
			chart_->chart()->addAxis(result, Qt::AlignRight);
			right_axis_count_++;
		}
	}
	else {
		result = it->second;
	}

	return result;
}

void XeroPlotItemWidget::createTimeAxis()
{
	time_ = new QValueAxis();
	time_->setLabelsVisible(true);
	time_->setTickCount(10);
	time_->setTitleText("time (s)");
	time_->setTitleVisible(true);
	chart_->chart()->addAxis(time_, Qt::AlignBottom);
}

void XeroPlotItemWidget::createLegend()
{
	legend_ = chart_->chart()->legend();
	legend_->setVisible(true);
	legend_->setAlignment(Qt::AlignBottom);
	legend_->setMarkerShape(QLegend::MarkerShape::MarkerShapeCircle);
	QFont font = legend_->font();
	font.setPointSize(8);
	font.setBold(true);
	legend_->setFont(font);
}

void XeroPlotItemWidget::populateChart()
{
	if (time_ == nullptr)
		createTimeAxis();

	if (legend_ == nullptr)
		createLegend();

	for (auto node : node_names_) {
		auto nodedata = plot_->getNode(node.toStdString());
		if (nodedata == nullptr)
			continue;

		QLineSeries* series;
		auto it = series_.find(node);
		if (it == series_.end()) {
			QValueAxis* yaxis = createYAxis(node);
			series = new QLineSeries();
			chart_->chart()->addSeries(series);
			series_[node] = series;
			series->setName(node);
			series->attachAxis(time_);
			series->attachAxis(yaxis);
		}
		else {
			series = it->second;
		}

		updateData(series, nodedata);
	}

	setMinMaxValues();
	chart_->repaint();
}

void XeroPlotItemWidget::setMinMaxValues()
{
	double minv, maxv;
	bool init = true;

	auto tv = plot_->getNode("time");
	tv->getMinMax(true, minv, maxv);
	time_->setRange(minv, maxv);
	time_->applyNiceNumbers();

	for (auto node : node_names_) {
		auto nodedata = plot_->getNode(node.toStdString());
		if (nodedata == nullptr)
			continue;

		nodedata->getMinMax(true, minv, maxv);
		QString axisname = nodeToAxisName(node);

		auto it = min_max_values_.find(axisname.toStdString());
		if (it == min_max_values_.end()) {
			auto p = std::make_pair(minv, maxv);
			min_max_values_[axisname.toStdString()] = p;
		}
		else {
			auto p = std::make_pair(std::min(minv, it->second.first), std::max(maxv, it->second.second));
			min_max_values_[axisname.toStdString()] = p;
		}
	}

	for (auto p : min_max_values_) {
		QValueAxis* a = createYAxis(p.first.c_str());
		a->setRange(p.second.first, p.second.second);
		a->applyNiceNumbers();
	}
}

void XeroPlotItemWidget::updateData(QLineSeries* series, std::shared_ptr<PlotNode> nodedata)
{
	auto timenode = plot_->getNode("time");
	if (timenode == nullptr)
		return;

	assert(timenode->size() == nodedata->size());

	series->clear();
	for(int i = 0 ; i < nodedata->size() ; i++)
		series->append(timenode->get(i), nodedata->get(i));
}
