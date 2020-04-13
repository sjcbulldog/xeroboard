#pragma once

#include <QChartView>

class ChartWrapper : public QtCharts::QChartView
{
public:
	ChartWrapper(QWidget* parent);
	virtual ~ChartWrapper();

protected:
	void dragEnterEvent(QDragEnterEvent* event) override;
	void dragMoveEvent(QDragMoveEvent* event) override;
	void dragLeaveEvent(QDragLeaveEvent* event) override;
	void dropEvent(QDropEvent* ev) override;
};

