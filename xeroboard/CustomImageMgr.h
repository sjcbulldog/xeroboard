#pragma once

#include <memory>
#include <QMap>
#include <QFile>

class CustomImage;

class CustomImageMgr
{
public:
	CustomImageMgr();
	virtual ~CustomImageMgr();

	bool init();

	int count() const { return images_.size();  }

	QStringList imageNames() {
		return images_.keys();
	}

	std::shared_ptr<CustomImage> get(const QString& name) {
		return images_[name];
	}

private:
	static constexpr const char* VersionTag = "version";
	static constexpr const char* NameTag = "name";
	static constexpr const char* ImageTag = "image";
	static constexpr const char* SlotsTag = "slots";
	static constexpr const char* SlotNameTag = "name";
	static constexpr const char* SlotTypeTag = "type";
	static constexpr const char* SlotXTag = "x";
	static constexpr const char* SlotYTag = "y";
	static constexpr const char* SlotWidthTag = "width";
	static constexpr const char* SlotHeightTag = "height";

private:
	std::shared_ptr<CustomImage> parse(QFile& file);

private:
	QMap<QString, std::shared_ptr<CustomImage>> images_;
};

