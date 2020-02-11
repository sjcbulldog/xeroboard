#include "TextListDisplay.h"

#include "TextDisplay.h"

TextDisplay::TextDisplay(QWidget* parent) : QLabel(parent)
{
	QFontMetrics m(font());
	setMinimumHeight(m.lineSpacing());
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
		break;
	case NT_DOUBLE:
		break;
	case NT_STRING:
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