#pragma once

#include "ValueDisplay.h"

class SingleValueDisplay : public ValueDisplay
{
public:
	enum class DisplayType {
		Default,
		Text,
		Color,
		VerticalSlider,
		HorizontalSlider,
	};

	SingleValueDisplay(std::shared_ptr<DataSource> src, QWidget* parent = nullptr);
	virtual ~SingleValueDisplay();

protected:
	void paintEvent(QPaintEvent* ev) override;
	void resizeEvent(QResizeEvent* ev) override;

private:
	void paintColor(QPainter& p, int v);
	void paintText(QPainter& p, const QString& txt);

private:
	DisplayType display_;
	std::vector<QColor> colors_;
	QFont font_;
	bool font_set_;
};

