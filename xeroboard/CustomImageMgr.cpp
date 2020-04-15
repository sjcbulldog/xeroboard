#include "CustomImageMgr.h"
#include "CustomImage.h"
#include <QCoreApplication>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QRect>
#include <QImage>
#include <QDebug>

CustomImageMgr::CustomImageMgr()
{
}

CustomImageMgr::~CustomImageMgr()
{
}

bool CustomImageMgr::init()
{
	QString exedir = QCoreApplication::applicationDirPath() + "/images";
	QStringList filters;
	filters.push_back("*.json");
	QDirIterator iter(exedir, filters);

	while (iter.hasNext())
	{
		QFile json(iter.next());
		auto im = parse(json) ;
		if (im != nullptr)
		{
			QString imfile = exedir + "/" + im->imageFile();
			QImage image(imfile);
			im->setImage(image);
			images_.insert(im->name(), im);
		}
	}

	return true;
}

std::shared_ptr<CustomImage> CustomImageMgr::parse(QFile& file)
{
	QString text;
	std::shared_ptr<CustomImage> ret;

	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		qWarning() << "Cannot open file '" << file.fileName() << "' for reading";
		return false;
	}

	text = file.readAll();
	QJsonParseError err;
	QJsonDocument doc = QJsonDocument::fromJson(text.toUtf8(), &err);
	if (doc.isNull())
	{
		qWarning() << "Cannot parse file '" << file.fileName() << "' for reading - " << err.errorString();
		return nullptr;
	}

	if (!doc.isObject())
	{
		qWarning() << "JSON file '" << file.fileName() << "' does not hold a JSON object";
		return nullptr;
	}

	QJsonObject obj = doc.object();
	if (!obj.contains(VersionTag))
	{
		qWarning() << "JSON file '" << file.fileName() << " does not have a 'version' field";
		return nullptr;
	}

	if (!obj[VersionTag].isString())
	{
		qWarning() << "JSON file '" << file.fileName() << " has a 'version' field, but it is not a string";
		return nullptr;
	}

	if (!obj.contains(NameTag))
	{
		qWarning() << "JSON file '" << file.fileName() << " does not have a 'name' field";
		return nullptr;
	}

	if (!obj[NameTag].isString())
	{
		qWarning() << "JSON file '" << file.fileName() << " has a 'name' field, but it is not a string";
		return nullptr;
	}

	if (!obj.contains(ImageTag))
	{
		qWarning() << "JSON file '" << file.fileName() << " does not have a 'image' field";
		return nullptr;
	}

	if (!obj[ImageTag].isString())
	{
		qWarning() << "JSON file '" << file.fileName() << " has a 'image' field, but it is not a string";
		return nullptr;
	}

	if (!obj.contains(SlotsTag))
	{
		qWarning() << "JSON file '" << file.fileName() << " does not have a 'slots' field";
		return nullptr;
	}

	if (!obj[SlotsTag].isArray())
	{
		qWarning() << "JSON file '" << file.fileName() << " has a 'images' field, but it is not an array";
		return nullptr;
	}

	ret = std::make_shared<CustomImage>(obj[NameTag].toString(), obj[ImageTag].toString());

	QJsonArray sl = obj[SlotsTag].toArray();

	for (int i = 0; i < sl.count() ; i++)
	{
		if (!sl[i].isObject())
		{
			qWarning() << "JSON file '" << file.fileName() << "' in slots array, index " << i << ", value is not a JSON object";
			return nullptr;
		}

		auto sobj = sl[i].toObject();

		if (!sobj.contains(SlotXTag))
		{
			qWarning() << "JSON file '" << file.fileName() << " in slots array, index " << i << ", does not have 'x' field";
			return nullptr;
		}

		if (!sobj[SlotXTag].isDouble())
		{
			qWarning() << "JSON file '" << file.fileName() << " in slots array, index " << i << ", has 'x' field, but it is not a number";
			return nullptr;
		}

		if (!sobj.contains(SlotYTag))
		{
			qWarning() << "JSON file '" << file.fileName() << " in slots array, index " << i << ", does not have 'y' field";
			return nullptr;
		}

		if (!sobj[SlotYTag].isDouble())
		{
			qWarning() << "JSON file '" << file.fileName() << " in slots array, index " << i << ", has 'y' field, but it is not a number";
			return nullptr;
		}

		if (!sobj.contains(SlotHeightTag))
		{
			qWarning() << "JSON file '" << file.fileName() << " in slots array, index " << i << ", does not have 'height' field";
			return nullptr;
		}

		if (!sobj[SlotHeightTag].isDouble())
		{
			qWarning() << "JSON file '" << file.fileName() << " in slots array, index " << i << ", has 'height' field, but it is not a number";
			return nullptr;
		}

		if (!sobj.contains(SlotWidthTag))
		{
			qWarning() << "JSON file '" << file.fileName() << " in slots array, index " << i << ", does not have 'width' field";
			return nullptr;
		}

		if (!sobj[SlotWidthTag].isDouble())
		{
			qWarning() << "JSON file '" << file.fileName() << " in slots array, index " << i << ", has 'width' field, but it is not a number";
			return nullptr;
		}

		if (!sobj.contains(SlotNameTag))
		{
			qWarning() << "JSON file '" << file.fileName() << " in slots array, index " << i << ", does not have 'name' field";
			return nullptr;
		}

		if (!sobj[SlotNameTag].isString())
		{
			qWarning() << "JSON file '" << file.fileName() << " in slots array, index " << i << ", has 'name' field, but it is not a string";
			return nullptr;
		}

		if (!sobj.contains(SlotTypeTag))
		{
			qWarning() << "JSON file '" << file.fileName() << " in slots array, index " << i << ", does not have 'type' field";
			return nullptr;
		}

		if (!sobj[SlotTypeTag].isString())
		{
			qWarning() << "JSON file '" << file.fileName() << " in slots array, index " << i << ", has 'type' field, but it is not a string";
			return nullptr;
		}

		QString sname = sobj[SlotNameTag].toString();
		QString stype = sobj[SlotTypeTag].toString();
		int x = static_cast<int>(sobj[SlotXTag].toInt());
		int y = static_cast<int>(sobj[SlotYTag].toInt());
		int width = static_cast<int>(sobj[SlotWidthTag].toInt());
		int height = static_cast<int>(sobj[SlotHeightTag].toInt());
		QRect bounds(x, y, width, height);
		ret->addSlot(sname, stype, bounds);
	}

	return ret;
}