#pragma once
#include "XeroDisplayWidget.h"
#include <QChartView>
#include <QValueAxis>
#include <QLineSeries>
#include <map>

class Plot;
class PlotNode;

class XeroPlotItemWidget : public XeroDisplayWidget
{
public:
	XeroPlotItemWidget(Plot* plot, QWidget* parent = nullptr);

	void createJSON(QJsonObject& obj);
	void addVar(const QString &varname);
	void addVar(const QString& plotname, const QString& varname);

protected:
	void resizeEvent(QResizeEvent* ev) override;
	void populateChart();

private:
	void createTimeAxis();
	void createLegend();
	void updateData(QtCharts::QLineSeries* series, std::shared_ptr<PlotNode> nodedata);
	QtCharts::QValueAxis* createYAxis(const QString& node);
	QString nodeToAxisName(const QString& node);
	void setMinMaxValues();

private:
	Plot* plot_;
	QtCharts::QChartView* chart_;
	QStringList node_names_;
	QString title_;

	QtCharts::QValueAxis* time_;
	QtCharts::QLegend* legend_;

	std::map<QString, QtCharts::QValueAxis*> axes_;
	std::map<QString, QtCharts::QLineSeries*> series_;

	int left_axis_count_;
	int right_axis_count_;

	std::map<std::string, std::pair<double, double>> min_max_values_;

	QString units_;
};

