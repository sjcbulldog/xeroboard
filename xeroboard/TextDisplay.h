#pragma once

#include "IDataDisplay.h"
#include <QLabel>


class TextDisplay : public QLabel, public IDataDisplay
{
public:
	TextDisplay(QWidget* parent);
	virtual ~TextDisplay();

	void setValue(std::shared_ptr<nt::Value> value) override ;

protected:
	virtual void resizeEvent(QResizeEvent* ev);

private:
	bool autosize_font_;
};