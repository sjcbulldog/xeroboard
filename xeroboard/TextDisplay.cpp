#include "TextDisplay.h"

TextDisplay::TextDisplay(QWidget* parent) : QLabel(parent)
{
	QFontMetrics m(font());
	setMinimumHeight(m.lineSpacing());
	setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

	autosize_font_ = true;
}

TextDisplay::~TextDisplay()
{
}

void TextDisplay::setValue(std::shared_ptr<nt::Value> value)
{
	QString txt;

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

	setText(txt);
	setToolTip(txt);
}

void TextDisplay::resizeEvent(QResizeEvent* ev)
{
	if (autosize_font_)
	{
		QFont newfont(font());
		newfont.setPixelSize(height());
		setFont(newfont);
	}
}