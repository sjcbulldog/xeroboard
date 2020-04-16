#pragma once

#include "NTValueWrapper.h"
#include <QObject>
#include <QString>
#include <cstdlib>


class DataSource : public QObject
{
	Q_OBJECT;

public:
	enum class DataType {
		Double,
		Boolean,
		String,
		Unknown
	};

	DataSource() {
	}

	virtual ~DataSource() {
	}

	virtual size_t count() const = 0;
	virtual QString getString(size_t index) const = 0;
	virtual bool getBoolean(size_t index) const = 0;
	virtual double getDouble(size_t index) const = 0;
	virtual DataType getType(size_t index) const = 0;
	virtual const QString& getName(size_t index) const = 0;
	virtual void updateValue() = 0;

signals:
	void valueChanged();

protected:
	void emitValueChanged() {
		emit valueChanged();
	}
};
