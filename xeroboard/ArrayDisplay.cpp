#include "ArrayDisplay.h"

ArrayDisplay::ArrayDisplay(QWidget* parent) : QTreeWidget(parent)
{
	QStringList list;
	list.push_back("Index");
	list.push_back("Value");
	setHeaderLabels(list);
	setColumnCount(2);

	QFontMetrics fm(font());
	setMinimumHeight(fm.lineSpacing() * 3);
}

ArrayDisplay::~ArrayDisplay()
{
}

void ArrayDisplay::setValue(std::shared_ptr<nt::Value> value)
{
	QString txt;
	size_t size = 0;

	clear();

	if (value->type() == NT_BOOLEAN_ARRAY)
		size = value->GetBooleanArray().size();
	else if (value->type() == NT_DOUBLE_ARRAY)
		size = value->GetDoubleArray().size();
	else if (value->type() == NT_STRING_ARRAY)
		size = value->GetStringArray().size();

	for (size_t i = 0; i < size; i++)
	{
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
			txt = (value->GetBooleanArray()[i]) ? "TRUE" : "FALSE";
			break;
		case NT_DOUBLE_ARRAY:
			txt = QString::number(value->GetDoubleArray()[i]);
			break;
		case NT_STRING_ARRAY:
			txt = value->GetStringArray()[i].c_str();
			break;
		case NT_RPC:
			break;
		}

		QTreeWidgetItem* item;
		item = new QTreeWidgetItem();
		QString name = QString::number(i);
		item->setText(0, name);
		addTopLevelItem(item);

		item->setText(1, txt);
		item->setToolTip(1, txt);
	}

	resizeColumnToContents(0);
	resizeColumnToContents(1);
}
