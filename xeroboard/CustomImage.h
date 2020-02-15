#pragma once

#include <QString>
#include <QImage>
#include <QVector>
#include <memory>

class CustomImage
{
public:

public:
	class CustomImageSlot
	{
	public:
		CustomImageSlot(QString name, QString type, QRect bounds)
		{
			name_ = name;
			type_ = type;
			bounds_ = bounds;
		}

		const QString& name() { return name_; }
		const QString& type() { return type_; }
		const QRect& bounds() { return bounds_; }

	private:
		QString name_;
		QString type_;
		QRect bounds_;
	};

public:
	CustomImage(QString name, QString image)
	{
		name_ = name;
		image_file_ = image;
	}

	QString name() const {
		return name_;
	}

	QString imageFile() const {
		return image_file_;
	}

	QImage image();

	void addSlot(QString name, QString type, QRect bounds)
	{
		CustomImageSlot slot(name, type, bounds);
		slots_.push_back(slot);
	}

	int slotCount() {
		return slots_.size();
	}

	CustomImageSlot& slot(int index) {
		return slots_[index];
	}

private:
	QString name_;
	QString image_file_;
	QImage image_;
	QVector<CustomImageSlot> slots_;
};

