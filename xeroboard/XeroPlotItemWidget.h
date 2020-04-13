#pragma once
#include "XeroDisplayWidget.h"
#include <QChartView>

class Plot;

class XeroPlotItemWidget : public XeroDisplayWidget
{
public:
	XeroPlotItemWidget(Plot* plot, QWidget* parent = nullptr);

	void createJSON(QJsonObject& obj);

protected:
	void resizeEvent(QResizeEvent* ev) override;
	void populateChart();

private:
	Plot* plot_;
	QtCharts::QChartView* chart_;
	QStringList node_names_;
	QString title_;
};

