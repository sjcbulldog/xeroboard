#include "TextListDisplay.h"

TextListDisplay::TextListDisplay(QWidget* parent) : QTreeWidget(parent)
{
}

TextListDisplay::~TextListDisplay()
{
}

void TextListDisplay::setValue(std::shared_ptr<nt::Value> value)
{
}