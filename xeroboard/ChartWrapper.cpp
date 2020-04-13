#include "ChartWrapper.h"
#include "XeroPlotItemWidget.h"
#include <QMimeData>

ChartWrapper::ChartWrapper(QWidget* parent) : QtCharts::QChartView(parent)
{
	setAcceptDrops(true);
}

ChartWrapper::~ChartWrapper()
{
}

void ChartWrapper::dragEnterEvent(QDragEnterEvent* ev)
{
	setBackgroundRole(QPalette::Highlight);
	ev->setDropAction(Qt::DropAction::CopyAction);
	ev->acceptProposedAction();
}

void ChartWrapper::dragMoveEvent(QDragMoveEvent* ev)
{
	ev->acceptProposedAction();
}

void ChartWrapper::dragLeaveEvent(QDragLeaveEvent* ev)
{
	setBackgroundRole(QPalette::Background);
}

void ChartWrapper::dropEvent(QDropEvent* ev)
{
	QString text;
	static const char* textMimeType = "application/x-text";
	XeroPlotItemWidget* p = dynamic_cast<XeroPlotItemWidget*>(parent());
	if (p == nullptr)
		return;

	const QMimeData* data = ev->mimeData();
	if (data->hasFormat(textMimeType))
	{
		QString key = QString::fromUtf8(data->data(textMimeType));
		if (key.startsWith("plotvar:")) {
			key = key.mid(8);
			QRegExp exp("\\{(.*)\\},\\{(.*)\\}");
			if (exp.exactMatch(key)) {
				QString plotname = exp.cap(1);
				QString varname = exp.cap(2);

				p->addVar(plotname, varname);
			}
		}
	}

	setBackgroundRole(QPalette::Background);
}